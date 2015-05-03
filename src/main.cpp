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

#include <iostream>
#include <string>
#include <fstream>

void PrintUsage()
{
	std::cout << "Usage: wav2raw file1.wav [file2.wav] [file3.wav] ..." << std::endl;
}

void ProcessFile(const std::string &filename)
{
	std::cout << "Processing \"" << filename << "\"..." << std::endl;
	WaveFile wavFile(filename);
	if (wavFile.IsLoaded())
	{
		std::cout <<
			"Meta data:\n"
			" - Audio format : " << wavFile.GetAudioFormatString() << "\n"
			" - Channels     : " << wavFile.GetNumChannels() << "\n"
			" - Sample rate  : " << wavFile.GetSampleRate() << "\n"
			" - Sample size  : " << wavFile.GetBitsPerSample() << " bits" << std::endl;

		std::string filenameSansExt = filename.substr(0, filename.find_last_of('.'));
		std::string filenameRaw = filenameSansExt + ".raw";
		std::fstream rawFile(filenameRaw, std::ios::out | std::ios::trunc | std::ios::binary);

		if (!rawFile.is_open())
		{
			std::cerr << "Error: Failed to open output file \"" << filenameRaw << "\"." << std::endl;
			return;
		}

		const char *rawData = wavFile.GetData();
		std::size_t rawSize = wavFile.GetDataSize();
		rawFile.write(rawData, rawSize);
		std::cout << "Wrote \"" << filenameRaw << "\"." << std::endl;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		PrintUsage();
		return 0;
	}

	for (int i = 1; i < argc; ++i)
	{
		std::string filename = argv[i];
		ProcessFile(filename);
	}

	return 0;
}
