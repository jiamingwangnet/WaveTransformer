#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <iostream>

#include <lame.h>
#include <mpg123.h>

#include "WaveFile.h"

namespace algo
{
	struct WavMetadata
	{
		wf::WaveFile::SampleRate sampleRate;
		wf::WaveFile::BitsPerSample bps;
		wf::WaveFile::Channels channels;
		wf::WaveFile::AudioFormat format;
	};

	extern bool convertMp3;
	extern bool mpg124_verbose;

	namespace util
	{
		inline std::vector<std::uint8_t> WavToMp3(const std::vector<std::uint8_t>& wavData, wf::WaveFile::SampleRate sampleRate, wf::WaveFile::BitsPerSample bps, wf::WaveFile::Channels channels, wf::WaveFile::AudioFormat format)
		{
			lame_t lame = lame_init();
			if (!lame)
			{
				std::cerr << "(algo::util::WavToMp3) Error: Unable to initialize LAME encoder" << std::endl;
				throw std::runtime_error("(algo::util::WavToMp3) LAME initialization failed");
			}

			lame_set_in_samplerate(lame, static_cast<int>(sampleRate));
			lame_set_num_channels(lame, static_cast<int>(channels));
			lame_set_VBR(lame, vbr_default);
			//lame_set_quality(lame, 5);
			if (lame_init_params(lame) < 0)
			{
				std::cerr << "(algo::util::WavToMp3) Error: Unable to set LAME parameters" << std::endl;
				lame_close(lame);
				throw std::runtime_error("(algo::util::WavToMp3) LAME parameter initialization failed");
			}

			std::vector<std::uint8_t> mp3Data;

			const int PCM_BUFFER_SIZE = 16384;
			const int MP3_BUFFER_SIZE = 16384;

			std::vector<std::uint8_t> pcmBuffer(PCM_BUFFER_SIZE * static_cast<int>(channels));
			std::vector<std::uint8_t> mp3Buffer( MP3_BUFFER_SIZE );

			int mp3Bytes;

			for (auto pos = wavData.begin(); pos < wavData.end(); pos += PCM_BUFFER_SIZE * static_cast<int>(channels))
			{
				int pcmSize = std::min(static_cast<int>(std::distance(pos, wavData.end())), PCM_BUFFER_SIZE * static_cast<int>(channels));

				std::copy(pos, pos + pcmSize, pcmBuffer.begin());

				if (format == wf::WaveFile::AudioFormat::PCM)
					mp3Bytes = lame_encode_buffer_interleaved(lame,
						reinterpret_cast<short*>(pcmBuffer.data()),
						pcmSize / static_cast<int>(channels) / (static_cast<int>(bps) / 8),
						mp3Buffer.data(),
						MP3_BUFFER_SIZE);
				else if (format == wf::WaveFile::AudioFormat::FLOAT)
					mp3Bytes = lame_encode_buffer_interleaved_ieee_float(lame,
						reinterpret_cast<float*>(pcmBuffer.data()),
						pcmSize / static_cast<int>(channels) / (static_cast<int>(bps) / 8),
						mp3Buffer.data(),
						MP3_BUFFER_SIZE);
				else
				{
					std::cerr << "(algo::util::WavToMp3) Error: Unsupported audio format" << std::endl;
					lame_close(lame);
					throw std::runtime_error("(algo::util::WavToMp3) Unsupported audio format");
				}	

				if (mp3Bytes < 0)
				{
					std::cerr << "(algo::util::WavToMp3) Error: LAME encoding failed" << std::endl;
					lame_close(lame);
					throw std::runtime_error("(algo::util::WavToMp3) LAME encoding error");
				}

				mp3Data.insert(mp3Data.end(), mp3Buffer.begin(), mp3Buffer.begin() + mp3Bytes);
			}

			// Flush LAME buffer
			mp3Bytes = lame_encode_flush(lame, mp3Buffer.data(), MP3_BUFFER_SIZE);
			if (mp3Bytes < 0)
			{
				std::cerr << "(algo::util::WavToMp3) Error: LAME flush failed" << std::endl;
				lame_close(lame);
				throw std::runtime_error("(algo::util::WavToMp3) LAME flush error");
			}
			if(mp3Bytes > 0) mp3Data.insert(mp3Data.end(), mp3Buffer.begin(), mp3Buffer.begin() + mp3Bytes);

			lame_close(lame);

			return mp3Data;
		}

		inline std::vector<std::uint8_t> Mp3ToWav(const std::vector<std::uint8_t>& mp3Data, wf::WaveFile::SampleRate sampleRate, wf::WaveFile::BitsPerSample bps, wf::WaveFile::Channels channels, wf::WaveFile::AudioFormat format)
		{
			mpg123_init();

			mpg123_handle* mh = mpg123_new(nullptr, nullptr);
			if (!mh)
			{
				std::cerr << "(algo::util::Mp3ToWav) Error: Unable to initialize mpg123 decoder" << std::endl;
				throw std::runtime_error("(algo::util::Mp3ToWav) mpg123 initialization failed");
			}

			long rate = static_cast<long>(sampleRate);
			int ch = static_cast<int>(channels);

			int encoding;
			if (format == wf::WaveFile::AudioFormat::PCM && bps == wf::WaveFile::BitsPerSample::BPS_16bit)
				encoding = MPG123_ENC_SIGNED_16;
			else if (format == wf::WaveFile::AudioFormat::FLOAT && bps == wf::WaveFile::BitsPerSample::BPS_32bit)
				encoding = MPG123_ENC_FLOAT_32;
			else
			{
				std::cerr << "(algo::util::Mp3ToWav) Error: Unsupported audio format or bits per sample" << std::endl;
				mpg123_delete(mh);
				throw std::runtime_error("(algo::util::Mp3ToWav) Unsupported audio format or bits per sample");
			}

			mpg123_format_none(mh);
			mpg123_format(mh, rate, ch, encoding);
			if(!mpg124_verbose) mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_QUIET, 0);

			if (mpg123_open_feed(mh) != MPG123_OK)
			{
				std::cerr << "(algo::util::Mp3ToWav) Error: Unable to open mpg123 feed" << std::endl;
				mpg123_delete(mh);
				throw std::runtime_error("(algo::util::Mp3ToWav) mpg123 open feed failed");
			}

			std::vector<std::uint8_t> pcmData;
			std::size_t bytesDone = 0;
			const std::size_t bufferSize = 16384;
			std::vector<std::uint8_t> buffer(bufferSize);

			if (mpg123_feed(mh, mp3Data.data(), mp3Data.size()) != MPG123_OK)
			{
				std::cerr << "(algo::util::Mp3ToWav) Error: Unable to feed mp3 data to mpg123" << std::endl;
				mpg123_close(mh);
				mpg123_delete(mh);
				throw std::runtime_error("(algo::util::Mp3ToWav) mpg123 feed failed");
			}

			int err;
			while (true)
			{
				err = mpg123_read(mh, buffer.data(), bufferSize, &bytesDone);

				if (err == MPG123_NEW_FORMAT)
				{
					long rate;
					int ch, enc;
					mpg123_getformat(mh, &rate, &ch, &enc);

					std::cout << "(algo::util::Mp3ToWav) Warning: New format - Rate: " << rate << ", Channels: " << ch << ", Encoding: " << enc << std::endl;

					continue;
				}

				if (err == MPG123_OK)
				{
					pcmData.insert(pcmData.end(), buffer.begin(), buffer.begin() + bytesDone);
					continue;
				}

				if (err == MPG123_DONE || err == MPG123_NEED_MORE)
				{
					if (bytesDone > 0)
						pcmData.insert(pcmData.end(), buffer.begin(), buffer.begin() + bytesDone);
					break;
				}

				//error
				const char* msg = mpg123_plain_strerror(err);

				std::cerr << "(algo::util::Mp3ToWav) Error: mpg123 read failed (" << err << ')' << std::endl;
				std::cerr << "mpg123 error: " << msg << std::endl;
				mpg123_close(mh);
				mpg123_delete(mh);
				throw std::runtime_error("(algo::util::Mp3ToWav) mpg123 read error");
			}
			

			mpg123_close(mh);
			mpg123_delete(mh);
			mpg123_exit();

			return pcmData;
		}

		std::vector<std::uint8_t> GetAudioData(const std::string& inputFile, const std::string& algoName, const WavMetadata* wavm, bool ignoreMp3 = false);
		std::vector<std::vector<std::uint8_t>> GetAudioData(const std::vector<std::string>& inputFiles, const std::string& algoName, const WavMetadata* wavm, bool ignoreMp3 = false);
		void ReturnAudioData(std::vector<uint8_t>& audioData, const WavMetadata* wavm);
	}

	inline void Reinterpret(const std::string& inputFile, std::vector<std::uint8_t>& audioData)
	{
		std::cout << "Input: " << inputFile << std::endl;

		audioData = util::GetAudioData(inputFile, "Reinterpret", nullptr, true);
	}

	inline void Interlace(const std::vector<std::string>& inputFiles, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm)
	{
		std::cout << "Inputs: \n";
		for (const auto& file : inputFiles)
		{
			std::cout << file << "\n";
		}
		std::cout << std::endl;

		// interlace the data from multiple input files into audioData
		// append 0s if files are of unequal length
		std::vector<std::vector<std::uint8_t>> fileData = util::GetAudioData(inputFiles, "Interlace", wavm);
		size_t maxSize = std::max_element(fileData.begin(), fileData.end(), [](const auto& a, const auto& b) {return a.size() < b.size(); })->size();
		
		// Interlace data
		for (size_t pos = 0; pos < maxSize; ++pos)
		{
			for (size_t fileIdx = 0; fileIdx < fileData.size(); ++fileIdx)
			{
				if (pos < fileData[fileIdx].size())
				{
					audioData.push_back(fileData[fileIdx][pos]);
				}
				else
				{
					audioData.push_back(0); // padding with 0 if this file is shorter
				}
			}
		}

		util::ReturnAudioData(audioData, wavm);
	}

	// Byte Block Shuffling: Divide data into blocks and randomly shuffle their order
	inline void ByteBlockShuffle(const std::string& inputFile, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, std::size_t blockSize = 256, bool align = false)
	{
		std::cout << "Input: " << inputFile << std::endl;

		audioData = util::GetAudioData(inputFile, "ByteBlockShuffle", wavm);

		if (blockSize == 0 || audioData.empty()) return;

		if (align)
		{
			// round block size to nearest multiple of  bps/8
			std::size_t byteAlign = static_cast<std::size_t>(wavm->bps) / 8;
			if (byteAlign > 0)
			{
				blockSize = ((blockSize + byteAlign - 1) / byteAlign) * byteAlign;
			}
		}

		std::size_t numBlocks = (audioData.size() + blockSize - 1) / blockSize;
		std::vector<std::vector<std::uint8_t>> blocks(numBlocks);

		// Split data into blocks
		for (std::size_t i = 0; i < numBlocks; ++i)
		{
			std::size_t start = i * blockSize;
			std::size_t end = std::min(start + blockSize, audioData.size());
			blocks[i].assign(audioData.begin() + start, audioData.begin() + end);
		}

		// Shuffle blocks
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(blocks.begin(), blocks.end(), g);

		// Reassemble audioData
		audioData.clear();
		for (const auto& block : blocks)
		{
			audioData.insert(audioData.end(), block.begin(), block.end());
		}

		util::ReturnAudioData(audioData, wavm);
	}

	inline void ShuffleRange(const std::string& inputFile, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, std::size_t minSize = 256, std::size_t maxSize = 1024, bool align = false)
	{
		std::cout << "Input: " << inputFile << std::endl;

		audioData = util::GetAudioData(inputFile, "ShuffleRange", wavm);

		if (maxSize == 0 || minSize == 0 || audioData.empty()) return;
		if (minSize > maxSize)
		{
			std::cerr << "(algo::ShuffleRange) Error: min greater than max" << std::endl;
			throw std::runtime_error{ "(algo::ShuffleRange) Error: min greater than max" };
		}

		std::random_device rd;
		std::mt19937 g(rd());
		std::uniform_int_distribution<std::size_t> distrib{ minSize, maxSize };

		std::vector<std::vector<std::uint8_t>> blocks;

		// Split data into blocks
		std::size_t start = 0;
		while ( start < audioData.size() )
		{
			std::size_t blockSize = distrib(g);

			if (align)
			{
				// round block size to nearest multiple of  bps/8
				std::size_t byteAlign = static_cast<std::size_t>(wavm->bps) / 8;
				if (byteAlign > 0)
				{
					blockSize = ((blockSize + byteAlign - 1) / byteAlign) * byteAlign;
				}
			}

			std::size_t end = std::min(start + blockSize, audioData.size());
			blocks.emplace_back().assign(audioData.begin() + start, audioData.begin() + end);
			start += blockSize;
		}
		
		// Shuffle blocks
		std::shuffle(blocks.begin(), blocks.end(), g);

		// Reassemble audioData
		audioData.clear();
		for (const auto& block : blocks)
		{
			audioData.insert(audioData.end(), block.begin(), block.end());
		}

		util::ReturnAudioData(audioData, wavm);
	}

	// Byte Mirror: For each block of blockSize, reverse the order of bytes within the block
	inline void ByteMirror(const std::string& inputFile, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, std::size_t blockSize = 256, bool align = false)
	{
		std::vector<std::uint8_t> buffer = util::GetAudioData(inputFile, "ByteMirror", wavm);
		audioData.clear();
		audioData.reserve(buffer.size());

		if (align)
		{
			// round block size to nearest multiple of  bps/8
			std::size_t byteAlign = static_cast<std::size_t>(wavm->bps) / 8;
			if (byteAlign > 0)
			{
				blockSize = ((blockSize + byteAlign - 1) / byteAlign) * byteAlign;
			}
		}

		std::size_t total = buffer.size();
		for (std::size_t i = 0; i < total; i += blockSize)
		{
			std::size_t end = std::min(i + blockSize, total);
			// Copy and reverse the block
			for (std::size_t j = 0; j < end - i; ++j)
			{
				audioData.push_back(buffer[end - 1 - j]);
			}
		}

		util::ReturnAudioData(audioData, wavm);
	}

	inline void ByteBitFlip(const std::string& inputFile, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, double flipProbability = 0.1)
	{
		audioData = util::GetAudioData(inputFile, "ByteBitFlip", wavm);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> prob(0.0, 1.0);
		std::uniform_int_distribution<> bit(0, 7);

		for (auto& b : audioData)
		{
			if (prob(gen) < flipProbability)
			{
				b ^= (1 << bit(gen));
			}
		}

		util::ReturnAudioData(audioData, wavm);
	}

	inline void ByteCascadeSwap(const std::string& inputFile, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, std::size_t blockSize = 256)
	{
		std::vector<std::uint8_t> buffer = util::GetAudioData(inputFile, "ByteCascadeSwap", wavm);
		audioData.clear();
		audioData.reserve(buffer.size());

		std::size_t total = buffer.size();
		for (std::size_t i = 0; i < total; i += blockSize)
		{
			std::size_t end = std::min(i + blockSize, total);
			if (end - i > 1)
			{
				// Shift block right by one
				audioData.push_back(buffer[end - 1]);
				for (std::size_t j = i; j < end - 1; ++j)
					audioData.push_back(buffer[j]);
			}
			else if (end - i == 1)
			{
				audioData.push_back(buffer[i]);
			}
		}

		util::ReturnAudioData(audioData, wavm);
	}

	inline void PerlinNoise(std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, std::size_t length, double scale = 0.1)
	{}

	// uniformly drop bytes from the audio data
	inline void Dropout(const std::string& inputFile, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, double dropPercentage = 0.5)
	{
		audioData = util::GetAudioData(inputFile, "Dropout", wavm);

		if (dropPercentage <= 0.0 || dropPercentage >= 1.0)
		{
			std::cerr << "(algo::Dropout) Error: dropPercentage must be between 0 and 1" << std::endl;
			throw std::runtime_error("(algo::Dropout) Invalid dropPercentage value");
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> prob(0.0, 1.0);

		std::vector<std::uint8_t> modifiedData;
		modifiedData.reserve(audioData.size());

		for (const auto& byte : audioData)
		{
			if (prob(gen) >= dropPercentage)
			{
				modifiedData.push_back(byte);
			}
		}

		audioData = std::move(modifiedData);

		util::ReturnAudioData(audioData, wavm);
	}

	// set every nth byte to zero
	inline void Stutter(const std::string& inputFile, std::vector<std::uint8_t>& audioData, const WavMetadata* wavm, std::size_t n = 10)
	{
		audioData = util::GetAudioData(inputFile, "Stutter", wavm);

		if (n == 0)
		{
			std::cerr << "(algo::Stutter) Error: n must be greater than 0" << std::endl;
			throw std::runtime_error("(algo::Stutter) Invalid n value");
		}

		for (std::size_t i = n - 1; i < audioData.size(); i += n)
		{
			audioData[i] = 0;
		}

		util::ReturnAudioData(audioData, wavm);
	}
}