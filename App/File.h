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

using CRC = unsigned long;

template <size_t N>
using Buffer = std::unique_ptr<std::array<std::uint8_t, N>>;
using DefaultBuffer = Buffer<BUFFER_SIZE>;


// use raw C fopen/fwrite/fread
// potential to be faster than c++ streams

class File
{
protected:
	unique_file_ptr mFile;

	const std::filesystem::path& mFullPath;

public:
	File(const std::filesystem::path& filepath, std::wstring options) : mFullPath(filepath)
	{
		std::FILE* rawFilePtr;
		if (_wfopen_s(&rawFilePtr, mFullPath.c_str(), options.c_str()))
		{
			// throw?
			return;
		}
		// can be still nullptr?

		unique_file_ptr tmp(rawFilePtr);
		mFile = std::move(tmp);
	}


	template <size_t N>
	size_t read(Buffer<N>& buffer)
	{
		return fread(buffer.get(), sizeof(char), N, mFile.get());
	}


	template <size_t N>
	size_t write(Buffer<N>& buffer)
	{
		return fwrite(buffer.get(), sizeof(char), N, mFile.get());
	}

	template <size_t N>
	size_t write(Buffer<N>& buffer, size_t size)
	{
		return fwrite(buffer.get(), sizeof(char), size, mFile.get());
	}

	const std::filesystem::path& getPath() const { return mFullPath; }
};


class LogFile : protected File
{
	const std::filesystem::path mFolderPath;
	const std::filesystem::path mRelativePath;
	
	CRC mCrc = crc32(0L, Z_NULL, 0);

	size_t mTotalRead = 0;
	size_t mSize = 0;


public:
	LogFile(const std::filesystem::path& filepath, const std::filesystem::path& folderPath) :
		File(filepath, L"rb"), // open binary only
		mFolderPath(folderPath),
		mRelativePath(std::filesystem::relative(mFullPath, mFolderPath))
	{
		mSize = std::filesystem::file_size(filepath);
	}

	template <size_t N>
	size_t read(Buffer<N>& buffer)
	{
		auto read = File::read<N>(buffer);
		if (mTotalRead == 0)
		{
			mCrc = crc32(mCrc, reinterpret_cast<const Bytef*>(buffer.get()), N);
		}

		mTotalRead += read;

		return read;
	}

	CRC getCrc() const { return mCrc; }
	size_t getSize() const { return mSize; }

	const std::filesystem::path& getShortPath() const { return mRelativePath; }
};


class CompressedFile
{
	unique_gzfile_ptr mFile;

public:
	CompressedFile(const std::filesystem::path& path, std::string options) : mFile(gzopen_w(path.c_str(), options.c_str()))
	{
		if (mFile.get() == nullptr)
		{
			// throw?
		}
	}

	template <size_t N>
	int read(Buffer<N>& buffer)
	{
		return gzread(mFile.get(), buffer.get(), N);
	}

	template <typename T>
	int read(T& data)
	{
		return gzread(mFile.get(), &data, sizeof(T));		
	}

	template <size_t N>
	int read(Buffer<N>& buffer, size_t size)
	{
		// get rid of casting (size should not be greater than unsigned int)
		return gzread(mFile.get(), buffer.get(), (unsigned int)size);
	}


	template <size_t N>
	int write(Buffer<N>& buffer, size_t size)
	{
		// get rid of casting (size should not be greater than unsigned int)
		return gzwrite(mFile.get(), buffer.get(), (unsigned int)size);
	}

	template <typename T>
	int write(const T& data)
	{
		return gzwrite(mFile.get(), &data, sizeof(T));
	}
};