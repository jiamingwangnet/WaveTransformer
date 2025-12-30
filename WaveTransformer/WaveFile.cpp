#include "include/WaveFile.h"

namespace wf
{
	wf::WaveFile::WaveFile(const std::string& path, SampleRate sampleRate, BitsPerSample bps, Channels channels, AudioFormat format)
		: path(path), sampleRate(sampleRate), bps(bps), channels(channels), format(format)
	{}

	std::string wf::WaveFile::GetPath() const
	{
		return path;
	}

	const std::vector<std::uint8_t>& wf::WaveFile::GetData() const
	{
		return data;
	}

	void wf::WaveFile::WriteOut() const
	{
		riffHeader riff;
		fmtChunk fmt;
		dataChunkHeader dataHeader;

		riff.chunkSize = sizeof(riffHeader) + sizeof(fmtChunk) + sizeof(dataChunkHeader) + static_cast<std::uint32_t>(data.size()) - 8;
		fmt.audioFormat = static_cast<std::uint16_t>(format);
		fmt.numChannels = static_cast<std::uint16_t>(channels);
		fmt.sampleRate = static_cast<std::uint32_t>(sampleRate);
		fmt.bitsPerSample = static_cast<std::uint16_t>(bps);
		fmt.byteRate = fmt.sampleRate * fmt.numChannels * fmt.bitsPerSample / 8;
		fmt.blockAlign = fmt.numChannels * fmt.bitsPerSample / 8;
		dataHeader.chunkSize = static_cast<std::uint32_t>(data.size());

		std::ofstream file{ path, std::ios::binary | std::ofstream::trunc };
		if (!file)
		{
			throw std::runtime_error("Failed to open file for writing: " + path);
		}
		//std::cout << data.size() << " bytes written to " << path << std::endl;
		file.write(reinterpret_cast<const char*>(&riff), sizeof(riffHeader));
		file.write(reinterpret_cast<const char*>(&fmt), sizeof(fmtChunk));
		file.write(reinterpret_cast<const char*>(&dataHeader), sizeof(dataChunkHeader));
		file.write(reinterpret_cast<const char*>(data.data()), data.size());

		file.close();
	}

	void wf::WaveFile::WriteRaw() const
	{
		std::ofstream file{ path, std::ios::binary | std::ofstream::trunc };
		if (!file)
		{
			throw std::runtime_error("Failed to open file for writing: " + path);
		}

		file.write(reinterpret_cast<const char*>(data.data()), data.size());

		file.close();
	}

	void wf::WaveFile::SetData(const std::vector<std::uint8_t>& pcm)
	{
		data = pcm;
	}

	void wf::WaveFile::SetData(const std::vector<float>& pcm_mono)
	{
		if (channels != Channels::Mono)
		{
			throw std::runtime_error("SetData with mono PCM called on non-mono WaveFile");
		}
		if (bps != BitsPerSample::BPS_32bit && format == AudioFormat::FLOAT)
		{
			throw std::runtime_error("SetData with float PCM called on non-32bit WaveFile");
		}

		switch (format)
		{
		case AudioFormat::FLOAT:
			data.resize(pcm_mono.size() * sizeof(float));
			std::memcpy(data.data(), pcm_mono.data(), data.size());
			break;
		case AudioFormat::PCM:
		{
			switch (bps)
			{
			case BitsPerSample::BPS_8bit:
				data.resize(pcm_mono.size() * sizeof(std::uint8_t));
				for (size_t i = 0; i < pcm_mono.size(); ++i)
				{
					float sample = pcm_mono[i];
					data[i] = FloatToPCM<std::uint8_t>(sample, bps);
				}
				break;
			case BitsPerSample::BPS_16bit:
				data.resize(pcm_mono.size() * sizeof(std::int16_t));
				for (size_t i = 0; i < pcm_mono.size(); ++i)
				{
					float sample = pcm_mono[i];
					std::int16_t intSample = FloatToPCM<std::int16_t>(sample, bps);
					std::memcpy(&data[i * sizeof(std::int16_t)], &intSample, sizeof(std::int16_t));
				}
				break;
			case BitsPerSample::BPS_24bit:
				data.resize(pcm_mono.size() * 3); // 3 bytes per sample
				for (size_t i = 0; i < pcm_mono.size(); ++i)
				{
					float sample = pcm_mono[i];
					std::int32_t intSample = FloatToPCM<std::int32_t>(sample, bps);
					data[i * 3 + 0] = static_cast<std::uint8_t>(intSample & 0xFF);
					data[i * 3 + 1] = static_cast<std::uint8_t>((intSample >> 8) & 0xFF);
					data[i * 3 + 2] = static_cast<std::uint8_t>((intSample >> 16) & 0xFF);
				}
				break;
			case BitsPerSample::BPS_32bit:
				data.resize(pcm_mono.size() * sizeof(std::int32_t));
				for (size_t i = 0; i < pcm_mono.size(); ++i)
				{
					float sample = pcm_mono[i];
					std::int32_t intSample = FloatToPCM<std::int32_t>(sample, bps);
					std::memcpy(&data[i * sizeof(std::int32_t)], &intSample, sizeof(std::int32_t));
				}
				break;
				break;
			default:
				throw std::runtime_error("Unsupported BitsPerSample in SetData with float PCM");
			}
		}
		}
	}

	void wf::WaveFile::SetData(std::vector<std::uint8_t>&& pcm)
	{
		data = std::move(pcm);
	}

	void wf::WaveFile::SetData(const std::vector<float>& pcm_left, const std::vector<float>& pcm_right)
	{
		if (channels != Channels::Stereo)
		{
			throw std::runtime_error("SetData with stereo PCM called on non-stereo WaveFile");
		}
		if (bps != BitsPerSample::BPS_32bit && format == AudioFormat::FLOAT)
		{
			throw std::runtime_error("SetData with float PCM called on non-32bit WaveFile");
		}
		if (pcm_left.size() != pcm_right.size())
		{
			throw std::runtime_error("Left and right PCM data size mismatch");
		}

		switch (format)
		{
		case AudioFormat::FLOAT:
			data.resize(pcm_left.size() * 2 * sizeof(float));
			for (size_t i = 0; i < pcm_left.size(); ++i)
			{
				std::memcpy(&data[(i * 2) * sizeof(float)], &pcm_left[i], sizeof(float));
				std::memcpy(&data[(i * 2 + 1) * sizeof(float)], &pcm_right[i], sizeof(float));
			}
			break;
		case AudioFormat::PCM:
		{
			switch (bps)
			{
			case BitsPerSample::BPS_8bit:
				data.resize(pcm_left.size() * 2 * sizeof(std::uint8_t));
				for (size_t i = 0; i < pcm_left.size(); ++i)
				{
					// Left channel
					float sampleL = pcm_left[i];
					data[i * 2] = FloatToPCM<std::uint8_t>(sampleL, bps);

					// Right channel
					float sampleR = pcm_right[i];
					data[i * 2 + 1] = FloatToPCM<std::uint8_t>(sampleR, bps);
				}
				break;
			case BitsPerSample::BPS_16bit:
				data.resize(pcm_left.size() * 2 * sizeof(std::int16_t));
				for (size_t i = 0; i < pcm_left.size(); ++i)
				{
					// Left channel
					float sampleL = pcm_left[i];
					std::int16_t intSampleL = FloatToPCM<std::int16_t>(sampleL, bps);
					std::memcpy(&data[(i * 2) * sizeof(std::int16_t)], &intSampleL, sizeof(std::int16_t));

					// Right channel
					float sampleR = pcm_right[i];
					std::int16_t intSampleR = FloatToPCM<std::int16_t>(sampleR, bps);
					std::memcpy(&data[(i * 2 + 1) * sizeof(std::int16_t)], &intSampleR, sizeof(std::int16_t));
				}
				break;
			case BitsPerSample::BPS_24bit:
				data.resize(pcm_left.size() * 2 * 3); // 3 bytes per sample
				for (size_t i = 0; i < pcm_left.size(); ++i)
				{
					// Left channel
					float sampleL = pcm_left[i];
					std::int32_t intSampleL = FloatToPCM<std::int32_t>(sampleL, bps);
					data[(i * 2) * 3 + 0] = static_cast<std::uint8_t>(intSampleL & 0xFF);
					data[(i * 2) * 3 + 1] = static_cast<std::uint8_t>((intSampleL >> 8) & 0xFF);
					data[(i * 2) * 3 + 2] = static_cast<std::uint8_t>((intSampleL >> 16) & 0xFF);

					// Right channel
					float sampleR = pcm_right[i];
					std::int32_t intSampleR = FloatToPCM<std::int32_t>(sampleR, bps);
					data[(i * 2 + 1) * 3 + 0] = static_cast<std::uint8_t>(intSampleR & 0xFF);
					data[(i * 2 + 1) * 3 + 1] = static_cast<std::uint8_t>((intSampleR >> 8) & 0xFF);
					data[(i * 2 + 1) * 3 + 2] = static_cast<std::uint8_t>((intSampleR >> 16) & 0xFF);
				}
				break;
			case BitsPerSample::BPS_32bit:
				data.resize(pcm_left.size() * 2 * sizeof(std::int32_t));
				for (size_t i = 0; i < pcm_left.size(); ++i)
				{
					// Left channel
					float sampleL = pcm_left[i];
					std::int32_t intSampleL = FloatToPCM<std::int32_t>(sampleL, bps);
					std::memcpy(&data[(i * 2) * sizeof(std::int32_t)], &intSampleL, sizeof(std::int32_t));

					// Right channel
					float sampleR = pcm_right[i];
					std::int32_t intSampleR = FloatToPCM<std::int32_t>(sampleR, bps);
					std::memcpy(&data[(i * 2 + 1) * sizeof(std::int32_t)], &intSampleR, sizeof(std::int32_t));
				}
				break;
			default:
				throw std::runtime_error("Unsupported BitsPerSample in SetData with float PCM for stereo");
			}
			break;
		}
		}
	}

	void wf::WaveFile::ClearData()
	{
		data.clear();
	}
}