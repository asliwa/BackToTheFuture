#pragma once
#include <cstdint>
#include <filesystem>
#include <WinDef.h>


struct METADATA_ENTRY_FILE_INFO
{
	// byte offset in RAW file
	std::uint64_t Offset;
	// byte length in RAW file
	std::uint32_t Length;
};

struct METADATA_ENTRY
{
	std::uint16_t Length;

	METADATA_ENTRY_FILE_INFO FileInfo;

	std::uint16_t PathLength;

	WCHAR Path[];
};

struct METADATA_HEADER
{
	GUID Guid;

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