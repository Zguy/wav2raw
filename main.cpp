/* The MIT License (MIT)

Copyright (c) 2022 Johannes Häggqvist

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
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static const uint32_t RIFF_ID = 'FFIR';
static const uint32_t FMT_ID  = ' tmf';
static const uint32_t DATA_ID = 'atad';

static const uint32_t FORMAT_WAVE = 'EVAW';

static void print_usage()
{
	printf("Usage: wav2raw file1.wav [file2.wav] [file3.wav] ...\n");
}

static void path_set_extension(char *result, size_t result_size, const char *path, size_t path_len, const char *new_extension)
{
	size_t dir_sep_index = path_len;
	size_t ext_index = path_len;
	while (dir_sep_index-- > 0 && path[dir_sep_index] != '/' && path[dir_sep_index] != '\\');
	while (ext_index-- > 0 && path[ext_index] != '.');
	if (ext_index < dir_sep_index + 1)
		ext_index = path_len;
	size_t ext_len = strlen(new_extension);
	memcpy(result, path, ext_index);
	memcpy(result + ext_index, new_extension, ext_len);
	result[ext_index + ext_len] = '\0';
}

static bool extract_data(FILE *in_file, size_t in_file_size, char *data, size_t *out_len)
{
	if (fseek(in_file, 0, SEEK_SET) != 0)
		return false;
	while (ftell(in_file) < in_file_size)
	{
		uint32_t chunk_id;
		if (fread(&chunk_id, sizeof(chunk_id), 1, in_file) != 1)
			return false;
		switch (chunk_id)
		{
			case RIFF_ID:
			{
				if (fseek(in_file, 4, SEEK_CUR) != 0)
					return false;
				uint32_t format_id;
				if (fread(&format_id, sizeof(format_id), 1, in_file) != 1)
					return false;
				if (format_id != FORMAT_WAVE)
					return false;
			} break;
			case FMT_ID:
			{
				if (fseek(in_file, 20, SEEK_CUR) != 0)
					return false;
			} break;
			case DATA_ID:
			{
				uint32_t chunk_size;
				if (fread(&chunk_size, sizeof(chunk_size), 1, in_file) != 1)
					return false;
				if (fread(data, chunk_size, 1, in_file) != 1)
					return false;
				*out_len = chunk_size;
				return true;
			} break;
			default:
			{
				uint32_t chunk_size;
				if (fread(&chunk_size, sizeof(chunk_size), 1, in_file) != 1)
					return false;
				if (fseek(in_file, chunk_size, SEEK_CUR) != 0)
					return false;
			} break;
		}
	}

	return false;
}

static void process_file(const char *path)
{
	FILE *wav_file = nullptr;
	FILE *raw_file = nullptr;
	char *path_raw = nullptr;
	char *raw_data = nullptr;

	printf("Extracting \"%s\"...\n", path);
	wav_file = fopen(path, "rb");
	if (wav_file == nullptr)
	{
		fprintf(stderr, "Error: Could not open file.\n");
		goto cleanup;
	}

	if (fseek(wav_file, 0, SEEK_END) != 0)
	{
		fprintf(stderr, "Error: Failed to seek to end of file.\n");
		goto cleanup;
	}
	size_t wav_file_size = ftell(wav_file);

	raw_data = (char*)malloc(wav_file_size);
	if (raw_data == nullptr)
	{
		fprintf(stderr, "Error: Out of memory.\n");
		goto cleanup;
	}

	size_t path_len = strlen(path);
	path_raw = (char*)malloc(path_len + 5); // +5 for length of ".raw" and null terminator.
	if (path_raw == nullptr)
	{
		fprintf(stderr, "Error: Out of memory.\n");
		goto cleanup;
	}
	path_set_extension(path_raw, path_len + 5, path, path_len, ".raw");

	raw_file = fopen(path_raw, "wb");
	if (raw_file == nullptr)
	{
		fprintf(stderr, "Error: Failed to open output file \"%s\".\n", path_raw);
		goto cleanup;
	}

	size_t raw_size;
	if (!extract_data(wav_file, wav_file_size, raw_data, &raw_size))
	{
		fprintf(stderr, "Error: Failed to extract data.\n");
		goto cleanup;
	}

	if (fwrite(raw_data, raw_size, 1, raw_file) != 1)
	{
		fprintf(stderr, "Error: Failed to write data to \"%s\".\n", path_raw);
		goto cleanup;
	}
	printf("Wrote \"%s\".\n", path_raw);

	cleanup:
	if (wav_file)
		fclose(wav_file);
	if (raw_file)
		fclose(raw_file);
	free(path_raw);
	free(raw_data);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		print_usage();
		return 0;
	}

	for (int i = 1; i < argc; ++i)
		process_file(argv[i]);

	return 0;
}
