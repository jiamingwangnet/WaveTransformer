#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

namespace algo
{
	void Reinterpret(const std::string& inputFile, std::vector<std::uint8_t>& audioData)
	{
		std::cout << "Input: " << inputFile << std::endl;

		std::ifstream inputStream{ inputFile, std::ios::binary };
		if (!inputStream)
		{
			std::cerr << "(algo::Reinterpret) Error: Unable to open input file: " << inputFile << std::endl;
			throw std::runtime_error("(algo::Reinterpret) Failed to open input file");
		}
		// read input file data to audioData
		audioData.assign((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());
	}

	void Interlace(const std::vector<std::string>& inputFiles, std::vector<std::uint8_t>& audioData)
	{
		std::cout << "Inputs: \n";
		for (const auto& file : inputFiles)
		{
			std::cout << file << "\n";
		}
		std::cout << std::endl;

		// interlace the data from multiple input files into audioData
		// append 0s if files are of unequal length
		std::vector<std::ifstream> inputStreams;
		std::vector<std::vector<std::uint8_t>> fileData(inputFiles.size());
		size_t maxSize = 0;
		// Open all input files and read their data
		for (size_t i = 0; i < inputFiles.size(); ++i)
		{
			inputStreams.emplace_back(inputFiles[i], std::ios::binary);
			if (!inputStreams.back())
			{
				std::cerr << "(algo::Interlace) Error: Unable to open input file: " << inputFiles[i] << std::endl;
				throw std::runtime_error("(algo::Interlace) Failed to open input file");
			}
			fileData[i].assign((std::istreambuf_iterator<char>(inputStreams.back())), std::istreambuf_iterator<char>());
			if (fileData[i].size() > maxSize)
			{
				maxSize = fileData[i].size();
			}
		}
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
	}
}