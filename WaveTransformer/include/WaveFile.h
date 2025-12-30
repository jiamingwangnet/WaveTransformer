#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <limits>
#include <fstream>

namespace wf 
{
	class WaveFile
	{
	public:
		enum class SampleRate : std::uint32_t
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

		enum class BitsPerSample : std::uint16_t
		{
			BPS_8bit = 8,
			BPS_16bit = 16,
			BPS_24bit = 24,
			BPS_32bit = 32
		};

		enum class Channels : std::uint16_t
		{
			Mono = 1,
			Stereo = 2
		};

		enum class AudioFormat : std::uint16_t
		{
			PCM = 1,
			FLOAT = 3,
			MP3 = 85
		};
	private:
		struct riffHeader
		{
			std::uint8_t chunkID[4] = { 'R','I','F','F' };      // "RIFF"
			std::uint32_t chunkSize;                     //  	(file size) - 8
			std::uint8_t format[4] = { 'W','A','V','E' };     // "WAVE"
		};

		struct fmtChunk
		{
			std::uint8_t chunkID[4] = { 'f','m','t',' ' };      // "fmt "
			std::uint32_t chunkSize = 16;                // 16 for PCM
			std::uint16_t audioFormat;               // PCM = 1 FLOAT = 3
			std::uint16_t numChannels;              
			std::uint32_t sampleRate;         
			std::uint32_t byteRate;                    // SampleRate * NumChannels * BitsPerSample/8
			std::uint16_t blockAlign;                  // NumChannels * BitsPerSample/8
			std::uint16_t bitsPerSample;
			// std::uint16_t extraFmtBytes = 0;            // unused for PCM
		};

		struct dataChunkHeader 
		{
			std::uint8_t chunkID[4] = { 'd','a','t','a' };      // "data"
			std::uint32_t chunkSize;                    
		};

	public:
		WaveFile(const std::string& path, 
			SampleRate sampleRate = SampleRate::SR_44100Hz,
			BitsPerSample bps = BitsPerSample::BPS_32bit,
			Channels channels = Channels::Mono,
			AudioFormat format = AudioFormat::PCM
		);

		std::string GetPath() const;
		const std::vector<std::uint8_t>& GetData() const;

		// void ReadIn();
		void WriteOut() const;
		void WriteRaw() const;

		void SetData(const std::vector<std::uint8_t>& pcm); // raw pcm data (interlaced if stereo)
		void SetData(std::vector<std::uint8_t>&& pcm); // raw pcm data (interlaced if stereo)

		void SetData(const std::vector<float>& mono); // float pcm data (MONO)
		void SetData(const std::vector<float>& left, const std::vector<float>& right); // float pcm data (STEREO)

		void ClearData();

	private:
		template<typename T>
		T FloatToPCM(float sample, BitsPerSample bps) const;

	private:
		std::string path;
		SampleRate sampleRate;
		BitsPerSample bps;
		Channels channels;
		AudioFormat format;
		std::vector<std::uint8_t> data; // raw pcm data
	};
	template<typename T>
	inline T WaveFile::FloatToPCM(float sample, BitsPerSample bps) const
	{
		float min = 0.0f;
		float max = 0.0f;
		float scale = 0.0f;

		switch (bps)
		{
		case BitsPerSample::BPS_8bit:
			min = 0.0f;
			max = 255.0f;
			scale = 255.0f;

			sample = (sample + 1.0f) * 0.5f; // Convert from [-1.0, 1.0] to [0.0, 1.0]
			sample = sample * scale; // Scale to [0, 255]
			if (sample < 0.0f) sample = 0.0f;
			if (sample > 255.0f) sample = 255.0f;

			return static_cast<T>(sample);
			break;
		case BitsPerSample::BPS_16bit:
			min = -32768.0f;
			max = 32767.0f;
			scale = 32768.0f;
			break;
		case BitsPerSample::BPS_24bit:
			min = -8388608.0f;
			max = 8388607.0f;
			scale = 8388608.0f;
			break;
		case BitsPerSample::BPS_32bit:
			min = -2147483648.0f;
			max = 2147483647.0f;
			scale = 2147483648.0f;
			break;
		default:
			throw std::runtime_error("Unsupported BitsPerSample in FloatToPCM");
		}

		if (sample >= 1.0f)
		{
			return static_cast<T>(std::numeric_limits<T>::max());
		}
		else if (sample <= -1.0f)
		{
			return static_cast<T>(std::numeric_limits<T>::min());
		}
		else
		{
			sample = sample * scale; // Scale to [min, max]
			if (sample < min) sample = min;
			if (sample > max) sample = max;
		}
		return static_cast<T>(sample);
	}
}