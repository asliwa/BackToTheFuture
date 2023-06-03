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


static constexpr size_t BUFFER_SIZE = 8 * 1024; // 8KB buffer size, prolly the best? maybe tune this

struct READ_WRITE_BUFFER
{
	// raw buffer for read/write content, max defined
	char RawBuffer[BUFFER_SIZE];
	// actual valid size of content in buffer
	unsigned Size;

	READ_WRITE_BUFFER()
	{
		memset(&RawBuffer, 0, BUFFER_SIZE);
	}
};


// use raw C fopen/fwrite/fread
// potential to be faster than c++ streams

class File
{
	unique_file_ptr mFile;
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

		// use seek (should be safe up to 2GB?) 
		fseek(mFile.get(), 0, SEEK_END); 
		mSize = ftell(mFile.get());
		fseek(mFile.get(), 0, SEEK_SET);
	}


	bool read(READ_WRITE_BUFFER& buffer)
	{
		// we can safely cast as read will never be greater than unsigned
		buffer.Size = (unsigned)fread(&buffer.RawBuffer, sizeof(char), BUFFER_SIZE, mFile.get());
		mTotalRead += buffer.Size;
		return buffer.Size;
	}

	

	void write(READ_WRITE_BUFFER& buffer)
	{
		auto count = fwrite(&buffer.RawBuffer, sizeof(char), buffer.Size, mFile.get());
		if (count != buffer.Size)
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

	void write(READ_WRITE_BUFFER& buffer)
	{
		auto count = gzwrite(mFile.get(), &buffer.RawBuffer, buffer.Size);
		if (count <= 0)
		{
			// throw?
		}
	}
};