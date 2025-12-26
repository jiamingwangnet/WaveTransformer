#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "include/InputParser.h"
#include "include/WaveFile.h"
#include "include/Options.h"

int main(int argc, char** argv)
{
	InputParser parser{ argc, argv };

	wf::WaveFile::SampleRate sampleRate = static_cast<wf::WaveFile::SampleRate>(opt::sample_rate::DEFAULT);
	wf::WaveFile::BitsPerSample bitDepth = static_cast<wf::WaveFile::BitsPerSample>(opt::bit_depth::DEFAULT);
	wf::WaveFile::Channels channels = static_cast<wf::WaveFile::Channels>(opt::channels::DEFAULT);
	wf::WaveFile::AudioFormat format = static_cast<wf::WaveFile::AudioFormat>(opt::format::DEFAULT);
	std::string outputFile = opt::output::DEFAULT;

	if (parser.cmdOptionExists(opt::HELP_SHORT) || parser.cmdOptionExists(opt::HELP_LONG))
	{
		opt::DisplayHelp();
		return 0;
	}

	if (parser.cmdOptionExists(opt::SAMPLE_RATE_SHORT) || parser.cmdOptionExists(opt::SAMPLE_RATE_LONG))
	{
		std::string rateStr = parser.getCmdOption(parser.cmdOptionExists(opt::SAMPLE_RATE_SHORT) ? opt::SAMPLE_RATE_SHORT : opt::SAMPLE_RATE_LONG);
		int rate = std::stoi(rateStr);
		sampleRate = static_cast<wf::WaveFile::SampleRate>(rate);
	}

	if (parser.cmdOptionExists(opt::BIT_DEPTH_SHORT) || parser.cmdOptionExists(opt::BIT_DEPTH_LONG))
	{
		std::string depthStr = parser.getCmdOption(parser.cmdOptionExists(opt::BIT_DEPTH_SHORT) ? opt::BIT_DEPTH_SHORT : opt::BIT_DEPTH_LONG);
		int depth = std::stoi(depthStr);
		bitDepth = static_cast<wf::WaveFile::BitsPerSample>(depth);
	}

	if (parser.cmdOptionExists(opt::CHANNELS_SHORT) || parser.cmdOptionExists(opt::CHANNELS_LONG))
	{
		std::string channelsStr = parser.getCmdOption(parser.cmdOptionExists(opt::CHANNELS_SHORT) ? opt::CHANNELS_SHORT : opt::CHANNELS_LONG);
		int ch;

		if (std::strcmp(channelsStr.c_str(), opt::channels::CHAN_MONO) == 0)
			ch = opt::channels::MONO;
		else if (std::strcmp(channelsStr.c_str(), opt::channels::CHAN_STEREO) == 0)
			ch = opt::channels::STEREO;
		else
		{
			std::cerr << "Error: Invalid channels option: " << channelsStr << std::endl;
			return 1;
		}

		channels = static_cast<wf::WaveFile::Channels>(ch);
	}

	if (parser.cmdOptionExists(opt::FORMAT_SHORT) || parser.cmdOptionExists(opt::FORMAT_LONG))
	{
		std::string formatStr = parser.getCmdOption(parser.cmdOptionExists(opt::FORMAT_SHORT) ? opt::FORMAT_SHORT : opt::FORMAT_LONG);
		int fmt;

		if (std::strcmp(formatStr.c_str(), opt::format::FMT_PCM) == 0)
			fmt = opt::format::PCM;
		else if (std::strcmp(formatStr.c_str(), opt::format::FMT_IEEE_FLOAT) == 0)
			fmt = opt::format::IEEE_FLOAT;
		else
		{
			std::cerr << "Error: Invalid format option: " << formatStr << std::endl;
			return 1;
		}

		format = static_cast<wf::WaveFile::AudioFormat>(fmt);
	}

	if (parser.cmdOptionExists(opt::OUT_SHORT) || parser.cmdOptionExists(opt::OUT_LONG))
	{
		outputFile = parser.getCmdOption(parser.cmdOptionExists(opt::OUT_SHORT) ? opt::OUT_SHORT : opt::OUT_LONG);
	}

	if (parser.cmdOptionExists(opt::TAG_SHORT) || parser.cmdOptionExists(opt::TAG_LONG))
	{
		outputFile = opt::TagFile(outputFile, channels, sampleRate, bitDepth, format);
	}

	std::string inputFile;
	// Assume the first non-option argument is the input file
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg[0] != '-')
		{
			inputFile = arg;
			break;
		}
	}

	std::cout << "Configured Wave File Parameters:" << std::endl;
	std::cout << "Sample Rate: " << static_cast<int>(sampleRate) << " Hz" << std::endl;
	std::cout << "Bit Depth: " << static_cast<int>(bitDepth) << " bits" << std::endl;
	std::cout << "Channels: " << static_cast<int>(channels) << std::endl;
	std::cout << "Format: " << (format == wf::WaveFile::AudioFormat::PCM ? "PCM" : "FLOAT") << std::endl;

	wf::WaveFile waveFile{outputFile, sampleRate, bitDepth, channels, format };

	std::vector<uint8_t> audioData;
	std::ifstream inputStream{ inputFile, std::ios::binary };
	if (!inputStream)
	{
		std::cerr << "Error: Unable to open input file: " << inputFile << std::endl;
		return 1;
	}
	// read input file data to audioData
	audioData.assign((std::istreambuf_iterator<char>(inputStream)), std::istreambuf_iterator<char>());
	waveFile.SetData(std::move(audioData));
	waveFile.WriteOut();

	std::cout << "Wave file written to " << outputFile << std::endl;

	return 0;
}
