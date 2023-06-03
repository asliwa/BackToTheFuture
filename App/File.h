#pragma once
#include <filesystem>
#include <cstdio>
#include <cstdlib>

#include "zlib.h"


// MSDN:
// Just using _wfopen doesn't affect the coded character set that's used in the file stream.


struct file_deleter {
	void operator()(std::FILE* f) { fclose(f); }
};

struct gzfile_deleter {
	void operator()(gzFile f) { gzclose(f); }
};

using unique_file_ptr = std::unique_ptr<std::FILE, file_deleter>;
using unique_gzfile_ptr = std::unique_ptr<gzFile_s, gzfile_deleter>;


// use raw C fopen/fwrite/fread
// potential to be faster than c++ streams

class File
{
	unique_file_ptr mFile;
	static constexpr size_t BUFFER_SIZE = 8 * 1024; // 8KB buffer size, prolly the best? maybe tune this
	char mBuffer[BUFFER_SIZE];
	size_t mTotalRead = 0;
	long mSize;

public:
	File(const std::filesystem::path& path)
	{
		std::FILE* rawFilePtr;
		if (_wfopen_s(&rawFilePtr, path.c_str(), L"wb"))
		{
			// throw?
			return;
		}
		// can be still nullptr?

		unique_file_ptr tmp(rawFilePtr);
		mFile = std::move(tmp);

		memset(mBuffer, 0, BUFFER_SIZE);

		// use seek (should be safe up to 2GB?) 
		fseek(mFile.get(), 0, SEEK_END); 
		mSize = ftell(mFile.get());
		fseek(mFile.get(), 0, SEEK_SET);
	}


	size_t read()
	{
		auto count = fread(mBuffer, sizeof(char), BUFFER_SIZE, mFile.get());
		mTotalRead += count;
		return count;
	}

	void write(std::array<char,BUFFER_SIZE> content, long size)
	{
		size_t count = fwrite(&content, sizeof(char), size, mFile.get());
		if (count != size)
		{
			// throw?
		}
	}

	long getSize() const { return mSize; }
};

class CompressedFile
{
	unique_gzfile_ptr mFile;

public:
	CompressedFile(const std::filesystem::path& path) : mFile(gzopen_w(path.c_str(), "wb"))
	{
		if (mFile.get() == nullptr)
		{
			// throw?
		}
	}

	void read();

	void write();
};