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


class File
{
	unique_file_ptr mFile;

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
	}


	void read();

	void write();


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