/* The MIT License (MIT)

Copyright (c) 2015 Johannes Häggqvist

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/
#include "WaveFile.h"

#include <fstream>
#include <iostream>
#include <cstdint>
#include <cassert>
#include <cstring>

namespace
{
	const std::uint32_t RIFF_ID = 'FFIR';
	const std::uint32_t FMT_ID  = ' tmf';
	const std::uint32_t DATA_ID = 'atad';

	struct RIFFChunk
	{
		static const std::uint32_t FORMAT_WAVE = 'EVAW';

		std::uint32_t chunkID;
		std::uint32_t chunkSize;
		std::uint32_t format;
	};
	struct fmtChunk
	{
		std::uint32_t chunkID;
		std::uint32_t chunkSize;
		std::uint16_t audioFormat;
		std::uint16_t numChannels;
		std::uint32_t sampleRate;
		std::uint32_t byteRate;
		std::uint16_t blockAlign;
		std::uint16_t bitsPerSample;
	};
	struct WaveHeader
	{
		RIFFChunk riff;
		fmtChunk fmt;
	};

	template<typename T>
	inline void ReadField(std::istream &stream, T *field)
	{
		stream.read(reinterpret_cast<char*>(field), sizeof(T));
	}
}

WaveFile::WaveFile() : data(nullptr), size(0)
{
}
WaveFile::WaveFile(const std::string &filename) : data(nullptr), size(0)
{
	Load(filename);
}
WaveFile::~WaveFile()
{
	Unload();
}

bool WaveFile::Load(const std::string &filename)
{
	if (IsLoaded())
	{
		Unload();
	}

	std::fstream file(filename, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file." << std::endl;
		return false;
	}

	WaveHeader header;
	std::memset(&header, 0, sizeof(WaveHeader));

	while (file.peek() != std::char_traits<char>::eof())
	{
		std::uint32_t chunkID;
		std::uint32_t chunkSize;

		ReadField(file, &chunkID);
		ReadField(file, &chunkSize);

		switch (chunkID)
		{
		case RIFF_ID:
			{
				header.riff.chunkID = chunkID;
				header.riff.chunkSize = chunkSize;
				ReadField(file, &header.riff.format);

				if (header.riff.format != RIFFChunk::FORMAT_WAVE)
				{
					std::cerr << "Error: Not a valid WAVE file." << std::endl;
					Unload();
					return false;
				}

				break;
			}
		case FMT_ID:
			{
				header.fmt.chunkID = chunkID;
				header.fmt.chunkSize = chunkSize;
				ReadField(file, &header.fmt.audioFormat);
				ReadField(file, &header.fmt.numChannels);
				ReadField(file, &header.fmt.sampleRate);
				ReadField(file, &header.fmt.byteRate);
				ReadField(file, &header.fmt.blockAlign);
				ReadField(file, &header.fmt.bitsPerSample);

				if (header.fmt.bitsPerSample % 2 != 0)
				{
					std::cerr << "Error: Invalid number of bits per sample" << std::endl;
					Unload();
					return false;
				}
				if (header.fmt.byteRate != (header.fmt.sampleRate * header.fmt.numChannels * header.fmt.bitsPerSample / 8))
				{
					std::cerr << "Error: Invalid byte rate" << std::endl;
					Unload();
					return false;
				}
				if (header.fmt.blockAlign != (header.fmt.numChannels * header.fmt.bitsPerSample / 8))
				{
					std::cerr << "Error: Invalid block align" << std::endl;
					Unload();
					return false;
				}

				break;
			}
		case DATA_ID:
			{
				assert(data == nullptr);
				size = chunkSize;
				data = new char[size];
				file.read(data, chunkSize);

				break;
			}
		default:
			{
				file.ignore(chunkSize);

				break;
			}
		}
	}

	// Check that we got all chunks
	if (header.riff.chunkID != RIFF_ID)
	{
		std::cerr << "Error: Missing RIFF chunk." << std::endl;
		Unload();
		return false;
	}
	if (header.fmt.chunkID != FMT_ID)
	{
		std::cerr << "Error: Missing fmt chunk." << std::endl;
		Unload();
		return false;
	}
	if (data == nullptr)
	{
		std::cerr << "Error: Missing data chunk." << std::endl;
		return false;
	}

	// Fill metadata
	audioFormat   = static_cast<AudioFormat>(header.fmt.audioFormat);
	numChannels   = header.fmt.numChannels;
	sampleRate    = header.fmt.sampleRate;
	bitsPerSample = header.fmt.bitsPerSample;

	return true;
}
void WaveFile::Unload()
{
	delete[] data;
	data = nullptr;
	size = 0;
}

std::string WaveFile::GetAudioFormatString() const
{
	switch (audioFormat)
	{
	case PCM:        return "PCM";
	case IEEE_FLOAT: return "IEEE float";
	default:         return "Unknown";
	}
}
