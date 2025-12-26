#pragma once

#include <iostream>
#include "WaveFile.h"

namespace opt
{
	constexpr const char* CHANNELS_SHORT = "-c";
	constexpr const char* CHANNELS_LONG = "--channels";
	namespace channels
	{
		constexpr const char* DESCRIPTION = "Number of audio channels (mono, stereo)";
		enum CHANNELS
		{
			MONO = 1,
			STEREO = 2
		};
		constexpr const char* CHAN_MONO = "mono";
		constexpr const char* CHAN_STEREO = "stereo";

		constexpr int DEFAULT = CHANNELS::MONO;
	
	} // namespace channels
	constexpr const char* SAMPLE_RATE_SHORT = "-r";
	constexpr const char* SAMPLE_RATE_LONG = "--rate";
	namespace sample_rate
	{
		constexpr const char* DESCRIPTION = "Sample rate in Hz (8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000, 192000, etc.)";
		enum SAMPLE_RATE
		{
			SR_8000Hz = 8000,
			SR_11025Hz = 11025,
			SR_16000Hz = 16000,
			SR_22050Hz = 22050,
			SR_32000Hz = 32000,
			SR_44100Hz = 44100,
			SR_48000Hz = 48000,
			SR_96000Hz = 96000,
			SR_192000Hz = 192000
		};
		constexpr int DEFAULT = SAMPLE_RATE::SR_44100Hz;
	} // namespace sample_rate
	constexpr const char* BIT_DEPTH_SHORT = "-b";
	constexpr const char* BIT_DEPTH_LONG = "--bitdepth";
	namespace bit_depth
	{
		constexpr const char* DESCRIPTION = "Bit depth per sample (8, 16, 24, 32)";
		enum BIT_DEPTH
		{
			BD_8bit = 8,
			BD_16bit = 16,
			BD_24bit = 24,
			BD_32bit = 32
		};
		constexpr int DEFAULT = BIT_DEPTH::BD_16bit;
	} // namespace bit_depth
	constexpr const char* FORMAT_SHORT = "-f";
	constexpr const char* FORMAT_LONG = "--format";
	namespace format
	{
		constexpr const char* DESCRIPTION = "Audio format (pcm, float)";
		enum FORMAT
		{
			PCM = 1,
			IEEE_FLOAT = 3
		};
		constexpr const char* FMT_PCM = "pcm";
		constexpr const char* FMT_IEEE_FLOAT = "float";
		constexpr int DEFAULT = FORMAT::PCM;
	} // namespace format
	constexpr const char* HELP_SHORT = "-h";
	constexpr const char* HELP_LONG = "--help";
	constexpr const char* HELP_DESCRIPTION = "Display this help message";

	constexpr const char* OUT_SHORT = "-o";
	constexpr const char* OUT_LONG = "--output";
	namespace output
	{
		constexpr const char* DESCRIPTION = "Output file path";
		constexpr const char* DEFAULT = "output.wav";
	} // namespace output

	// -t/--tag (tag) appends the settings to the file name
	constexpr const char* TAG_SHORT = "-t";
	constexpr const char* TAG_LONG = "--tag";
	namespace tag
	{
		constexpr const char* DESCRIPTION = "Append settings to output file name";
	} // namespace tag

	namespace operation
	{
		constexpr const char* REINTERPRET = "reint";
		constexpr const char* INTERLACE	  = "inter";

		enum OPERATIONS
		{
			OP_REINTERPRET,
			OP_INTERLACE
		};
	}

	std::string TagFile(const std::string& filename, const std::string& operation, wf::WaveFile::Channels channels, wf::WaveFile::SampleRate sampleRate, wf::WaveFile::BitsPerSample bitDepth, wf::WaveFile::AudioFormat format)
	{
		std::string taggedName = filename;

		// Insert before file extension
		size_t dotPos = taggedName.find_last_of('.');
		if (dotPos == std::string::npos)
		{
			dotPos = taggedName.length();
		}

		std::string tag = "_";
		tag += operation + "_";
		tag += (channels == wf::WaveFile::Channels::Mono ? "mono_" : "stereo_");
		tag += std::to_string(static_cast<int>(sampleRate)) + "Hz_";
		tag += std::to_string(static_cast<int>(bitDepth)) + "bit_";
		tag += (format == wf::WaveFile::AudioFormat::PCM) ? "pcm" : "float";

		taggedName.insert(dotPos, tag);
		return taggedName;
	}

	void DisplayHelp()
	{
		std::cout << "WaveTransformer Help:\n";
		std::cout << "Format: <operation> [inputs...] <options> [value]...\n";
		std::cout << "Operations:\n";
		std::cout << "  " << operation::REINTERPRET << ": Reinterpret the data of the input file as an wave file with specified parameters.\n";
		std::cout << "  " << operation::INTERLACE << ": Interlace multiple files into a single wave file.\n";
		std::cout << "Options:\n";
		std::cout << HELP_SHORT << ", " << HELP_LONG << ": " << HELP_DESCRIPTION << "\n";
		std::cout << CHANNELS_SHORT << ", " << CHANNELS_LONG << ": " << channels::DESCRIPTION << " (Default: " << (channels::DEFAULT == 1 ? "mono" : "stereo") << ")\n";
		std::cout << SAMPLE_RATE_SHORT << ", " << SAMPLE_RATE_LONG << ": " << sample_rate::DESCRIPTION << " (Default: " << sample_rate::DEFAULT << ")\n";
		std::cout << BIT_DEPTH_SHORT << ", " << BIT_DEPTH_LONG << ": " << bit_depth::DESCRIPTION << " (Default: " << bit_depth::DEFAULT << ")\n";
		std::cout << FORMAT_SHORT << ", " << FORMAT_LONG << ": " << format::DESCRIPTION << " (Default: " << (format::DEFAULT == 1 ? "PCM" : (format::DEFAULT == 3 ? "FLOAT" : "INVALID")) << ")\n";
		std::cout << OUT_SHORT << ", " << OUT_LONG << ": " << output::DESCRIPTION << " (Default: " << output::DEFAULT << ")\n";
		std::cout << TAG_SHORT << ", " << TAG_LONG << ": " << tag::DESCRIPTION << std::endl;
	}
}