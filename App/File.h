#pragma once
#include <filesystem>
#include <cstdio>
#include <cstdlib>


// MSDN:
// Just using _wfopen doesn't affect the coded character set that's used in the file stream.


struct deleter {
	void operator()(std::FILE* f) { fclose(f); }
};

using unique_file_ptr = std::unique_ptr<std::FILE, deleter>;

class File
{
	unique_file_ptr mFile;

public:
	File(const std::filesystem::path& path) : mFile(_wfopen(path.c_str(), L"wb")) 
	{
		if (mFile.get() == nullptr)
		{
			// throw?
		}
	}
};