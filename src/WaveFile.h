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
#ifndef WAVEFILE_H__
#define WAVEFILE_H__

#include <string>

///\note All meta data is undefined if IsLoaded() == false
class WaveFile
{
public:
	enum AudioFormat
	{
		PCM        = 1,
		IEEE_FLOAT = 3
	};

	WaveFile();
	WaveFile(const std::string &filename);
	~WaveFile();

	bool Load(const std::string &filename);
	void Unload();

	inline bool IsLoaded() const { return (data != nullptr); }

	inline AudioFormat GetAudioFormat() const { return audioFormat; }
	std::string GetAudioFormatString() const;

	inline unsigned int GetNumChannels() const { return numChannels; }
	inline unsigned int GetSampleRate() const { return sampleRate; }
	inline unsigned int GetBitsPerSample() const { return bitsPerSample; }

	inline const char *GetData() const { return data; }
	inline std::size_t GetDataSize() const { return size; }

private:
	AudioFormat audioFormat;
	unsigned int numChannels;
	unsigned int sampleRate;
	unsigned int bitsPerSample;

	char *data;
	std::size_t size;
};

#endif // WAVEFILE_H__
