#include "include/Algo.h"

namespace algo
{
	bool convertMp3 = false;
	bool mpg124_verbose = false;

	namespace util
	{
		std::vector<std::uint8_t> GetAudioData(const std::string& inputFile, const std::string& algoName, const WavMetadata* wavm, bool ignoreMp3)
		{
			std::ifstream inputStream{ inputFile, std::ios::binary };
			if (!inputStream)
			{
				std::cerr << "(algo::" << algoName << ") Error: Unable to open input file: " << inputFile << std::endl;
				throw std::runtime_error("(algo::" + algoName + ") Failed to open input file");
			}
			// read input file data to audioData
			std::vector<std::uint8_t> output{ (std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>() };

			if (convertMp3 && !ignoreMp3)
				output = WavToMp3(output, wavm->sampleRate, wavm->bps, wavm->channels, wavm->format);

			return output;
		}

		std::vector<std::vector<std::uint8_t>> GetAudioData(const std::vector<std::string>& inputFiles, const std::string& algoName, const WavMetadata* wavm, bool ignoreMp3)
		{
			std::vector<std::vector<std::uint8_t>> allData;
			for (const auto& file : inputFiles)
			{
				std::ifstream inputStream{ file, std::ios::binary };
				if (!inputStream)
				{
					std::cerr << "(algo::" << algoName << ") Error: Unable to open input file: " << file << std::endl;
					throw std::runtime_error("(algo::" + algoName + ") Failed to open input file");
				}

				std::vector<std::uint8_t> output{ (std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>() };

				if (convertMp3 && !ignoreMp3)
					output = WavToMp3(output, wavm->sampleRate, wavm->bps, wavm->channels, wavm->format);

				// read input file data to audioData
				allData.push_back(std::move(output));
			}
			return allData;
		}

		void ReturnAudioData(std::vector<uint8_t>& audioData, const WavMetadata* wavm)
		{
			if (convertMp3)
			{
				audioData = Mp3ToWav(audioData, wavm->sampleRate, wavm->bps, wavm->channels, wavm->format);
			}
		}
	}
}