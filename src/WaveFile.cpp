/*
The MIT License (MIT)

Copyright (c) 2014 Johannes Häggqvist

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
THE SOFTWARE.
*/
#include "WaveFile.h"

#include <fstream>
#include <iostream>
#include <cstdint>
#include <cassert>
#include <cstring>

namespace
{
	struct RIFFChunk
	{
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

		file.read(reinterpret_cast<char*>(&chunkID), sizeof(std::uint32_t));
		file.read(reinterpret_cast<char*>(&chunkSize), sizeof(std::uint32_t));

		switch (chunkID)
		{
		case 'FFIR':
			{
				header.riff.chunkID = chunkID;
				header.riff.chunkSize = chunkSize;
				file.read(reinterpret_cast<char*>(&header.riff.format), sizeof(std::uint32_t));

				if (header.riff.format != 'EVAW')
				{
					std::cerr << "Error: Not a valid WAVE file." << std::endl;
					return false;
				}

				break;
			}
		case ' tmf':
			{
				header.fmt.chunkID = chunkID;
				header.fmt.chunkSize = chunkSize;
				file.read(reinterpret_cast<char*>(&header.fmt.audioFormat), sizeof(std::uint16_t));
				file.read(reinterpret_cast<char*>(&header.fmt.numChannels), sizeof(std::uint16_t));
				file.read(reinterpret_cast<char*>(&header.fmt.sampleRate), sizeof(std::uint32_t));
				file.read(reinterpret_cast<char*>(&header.fmt.byteRate), sizeof(std::uint32_t));
				file.read(reinterpret_cast<char*>(&header.fmt.blockAlign), sizeof(std::uint16_t));
				file.read(reinterpret_cast<char*>(&header.fmt.bitsPerSample), sizeof(std::uint16_t));

				if (header.fmt.audioFormat != PCM)
				{
					std::cerr << "Error: Not in PCM format" << std::endl;
					return false;
				}
				if (header.fmt.bitsPerSample % 2 != 0)
				{
					std::cerr << "Error: Invalid number of bits per sample" << std::endl;
					return false;
				}
				if (header.fmt.byteRate != (header.fmt.sampleRate * header.fmt.numChannels * header.fmt.bitsPerSample / 8))
				{
					std::cerr << "Error: Invalid byte rate" << std::endl;
					return false;
				}
				if (header.fmt.blockAlign != (header.fmt.numChannels * header.fmt.bitsPerSample / 8))
				{
					std::cerr << "Error: Invalid block align" << std::endl;
					return false;
				}

				break;
			}
		case 'atad':
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
	if (header.riff.chunkID != 'FFIR')
	{
		std::cerr << "Error: Missing RIFF chunk." << std::endl;
		return false;
	}
	if (header.fmt.chunkID != ' tmf')
	{
		std::cerr << "Error: Missing fmt chunk." << std::endl;
		return false;
	}
	if (data == nullptr || size == 0)
	{
		std::cerr << "Error: Missing data chunk." << std::endl;
		return false;
	}

	// Fill meta struct
	meta.audioFormat   = static_cast<AudioFormat>(header.fmt.audioFormat);
	meta.numChannels   = header.fmt.numChannels;
	meta.sampleRate    = header.fmt.sampleRate;
	meta.bitsPerSample = header.fmt.bitsPerSample;

	return true;
}
void WaveFile::Unload()
{
	delete[] data;
	data = nullptr;
	size = 0;
}
