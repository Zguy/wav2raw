#ifndef WaveFile_h__
#define WaveFile_h__

#include <string>

class WaveFile
{
public:
	struct Meta
	{
		enum AudioFormat
		{
			PCM = 1
		} audioFormat;
		unsigned int numChannels;
		unsigned int sampleRate;
		unsigned int bitsPerSample;
	};

	WaveFile();
	WaveFile(const std::string &filename);
	~WaveFile();

	bool Load(const std::string &filename);
	void Unload();

	inline bool IsLoaded() const
	{
		return (data != nullptr && size != 0);
	}

	inline const Meta &GetMeta() const
	{
		return meta;
	}
	inline const char *GetData() const
	{
		return data;
	}
	inline std::size_t GetDataSize() const
	{
		return size;
	}

private:
	Meta meta;
	char *data;
	std::size_t size;
};

#endif // WaveFile_h__
