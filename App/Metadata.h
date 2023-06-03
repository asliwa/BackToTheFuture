#pragma once
#include <cstdint>
#include <filesystem>


struct METADATA_ENTRY_FILE_INFO
{
	// byte offset in RAW file
	std::uint64_t Offset;
	// byte length in RAW file
	std::uint32_t Length;
};

struct METADATA_ENTRY
{
	METADATA_ENTRY_FILE_INFO FileInfo;

	std::uint16_t PathLength;

	wchar_t Path[];
};

struct METADATA_HEADER
{
	std::uint32_t Length;
	
	// version here? crc to ensure it was not modified?

	std::uint32_t EntriesCount;

	METADATA_ENTRY Entries[];
};



class Metadata
{

public:
	Metadata(const std::filesystem::path& path)
	{

	}
};