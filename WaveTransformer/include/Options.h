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

	constexpr const char* BLOCK_SIZE_SHORT = "-s";
	constexpr const char* BLOCK_SIZE_LONG = "--blocksize";
	namespace block_size
	{
		constexpr const char* DESCRIPTION = "Block size for required operations (in bytes)";
		constexpr std::size_t DEFAULT = 256;
	} // namespace block_size

	constexpr const char* PROBABILITY_SHORT = "-p";
	constexpr const char* PROBABILITY_LONG = "--probability";
	namespace probability
	{
		constexpr const char* DESCRIPTION = "Probability/Percentage for required operations (0.0 - 1.0)";
		constexpr double DEFAULT = 0.1;
	} // namespace probability

	constexpr const char* BLOCK_RANGE_SHORT = "-x";
	constexpr const char* BLOCK_RANGE_LONG = "--blockrange";
	namespace block_range
	{
		constexpr const char* DESCRIPTION = "Block range for required operations (min-max)";
		constexpr std::size_t DEFAULT_MIN = 256;
		constexpr std::size_t DEFAULT_MAX = 1024;
	} // namespace block_range

	constexpr const char* BLOCK_BYTE_ALIGN_SHORT = "-a";
	constexpr const char* BLOCK_BYTE_ALIGN_LONG = "--bytealign";
	namespace byte_align
	{
		constexpr const char* DESCRIPTION = "Align bytes to multiples of the bit depth/8 when dealing with blocks.";
		constexpr bool DEFAULT = false;
	}

	constexpr const char* NTH_BYTE_SHORT = "-n";
	constexpr const char* NTH_BYTE_LONG = "--nthbyte";
	namespace nth_byte
	{
		constexpr const char* DESCRIPTION = "N-th byte to process in each block (1-based index).";
		constexpr std::size_t DEFAULT = 1;
	}

	constexpr const char* CONVERT_MP3_SHORT = "-m";
	constexpr const char* CONVERT_MP3_LONG = "--convertmp3";
	namespace convert_mp3
	{
		constexpr const char* DESCRIPTION = "Convert to mp3 before operting on the raw data.";
		constexpr bool DEFAULT = false;
	} // namespace convert_mp3

	constexpr const char* VERBOSE_MPG123_SHORT = "-v";
	constexpr const char* VERBOSE_MPG123_LONG = "--verbosempg123";
	namespace verbose_mpg123
	{
		constexpr const char* DESCRIPTION = "Enable verbose output from mpg123 library during mp3 conversion.";
		constexpr bool DEFAULT = false;
	} // namespace verbose_mpg123

	namespace operation
	{
		constexpr const char* REINTERPRET   = "reint";
		constexpr const char* INTERLACE	    = "inter";
		constexpr const char* SHUFFLE       = "shuff";
		constexpr const char* BYTE_MIRROR   = "bymir";
		constexpr const char* BIT_FLIP      = "bitfl";
		constexpr const char* CASCADE_SWAP  = "caswp";
		constexpr const char* RANGE_SHUFFLE = "rngsh";
		constexpr const char* DROPOUT       = "dropt";
		constexpr const char* STUTTER       = "stutr";
		constexpr const char* ENCODE_MP3    = "enmp3";
		constexpr const char* DECODE_MP3    = "demp3";

		enum OPERATIONS
		{
			OP_REINTERPRET,
			OP_INTERLACE,
			OP_SHUFFLE,
			OP_BYTE_MIRROR,
			OP_BIT_FLIP,
			OP_CASCADE_SWAP,
			OP_RANGE_SHUFFLE,
			OP_DROPOUT,
			OP_STUTTER,
			OP_ENCODE_MP3,
			OP_DECODE_MP3
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
		tag += (format == wf::WaveFile::AudioFormat::PCM) ? "pcm" : (format == wf::WaveFile::AudioFormat::FLOAT ? "float" : "mp3");

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
		std::cout << "  " << operation::SHUFFLE << ": Shuffle the audio data of the input file based on the block size.\n";
		std::cout << "  " << operation::BYTE_MIRROR << ": Mirror the bytes in each block of the input file.\n";
		std::cout << "  " << operation::BIT_FLIP << ": Flip bits in each byte of the input file based on the specified probability.\n";
		std::cout << "  " << operation::CASCADE_SWAP << ": Perform cascade byte swapping in each block of the input file.\n";
		std::cout << "  " << operation::RANGE_SHUFFLE << ": Shuffle blocks of data of size within a specified range in the input file.\n";
		std::cout << "  " << operation::DROPOUT << ": Randomly drop out bytes in the input file based on the specified probability.\n";
		std::cout << "  " << operation::STUTTER << ": Set every nth byte to zero.\n";
		std::cout << "  " << operation::ENCODE_MP3 << ": Encode the input wave file to MP3 format.\n";
		std::cout << "  " << operation::DECODE_MP3 << ": Decode the input MP3 file to wave format.\n";
		std::cout << "Options:\n";
		std::cout << HELP_SHORT << ", " << HELP_LONG << ": " << HELP_DESCRIPTION << "\n";
		std::cout << CHANNELS_SHORT << ", " << CHANNELS_LONG << ": " << channels::DESCRIPTION << " (Default: " << (channels::DEFAULT == 1 ? "mono" : "stereo") << ")\n";
		std::cout << SAMPLE_RATE_SHORT << ", " << SAMPLE_RATE_LONG << ": " << sample_rate::DESCRIPTION << " (Default: " << sample_rate::DEFAULT << ")\n";
		std::cout << BIT_DEPTH_SHORT << ", " << BIT_DEPTH_LONG << ": " << bit_depth::DESCRIPTION << " (Default: " << bit_depth::DEFAULT << ")\n";
		std::cout << FORMAT_SHORT << ", " << FORMAT_LONG << ": " << format::DESCRIPTION << " (Default: " << (format::DEFAULT == 1 ? "PCM" : (format::DEFAULT == 3 ? "FLOAT" : "INVALID")) << ")\n";
		std::cout << OUT_SHORT << ", " << OUT_LONG << ": " << output::DESCRIPTION << " (Default: " << output::DEFAULT << ")\n";
		std::cout << TAG_SHORT << ", " << TAG_LONG << ": " << tag::DESCRIPTION << std::endl;
		std::cout << BLOCK_SIZE_SHORT << ", " << BLOCK_SIZE_LONG << ": " << block_size::DESCRIPTION << " (Default: " << block_size::DEFAULT << ")\n";
		std::cout << PROBABILITY_SHORT << ", " << PROBABILITY_LONG << ": " << probability::DESCRIPTION << " (Default: " << probability::DEFAULT << ")\n";
		std::cout << BLOCK_RANGE_SHORT << ", " << BLOCK_RANGE_LONG << ": " << block_range::DESCRIPTION << " (Default: " << block_range::DEFAULT_MIN << ' - ' << block_range::DEFAULT_MAX << ")\n";
		std::cout << BLOCK_BYTE_ALIGN_SHORT << ", " << BLOCK_BYTE_ALIGN_LONG << ": " << byte_align::DESCRIPTION << " (Default: " << (byte_align::DEFAULT ? "true" : "false") << ")\n";
		std::cout << NTH_BYTE_SHORT << ", " << NTH_BYTE_LONG << ": " << nth_byte::DESCRIPTION << " (Default: " << nth_byte::DEFAULT << ")\n";
		std::cout << CONVERT_MP3_SHORT << ", " << CONVERT_MP3_LONG << ": " << convert_mp3::DESCRIPTION << " (Default: " << (convert_mp3::DEFAULT ? "true" : "false") << ")\n";
		std::cout << VERBOSE_MPG123_SHORT << ", " << VERBOSE_MPG123_LONG << ": " << verbose_mpg123::DESCRIPTION << " (Default: " << (verbose_mpg123::DEFAULT ? "true" : "false") << ")\n";
	}
}