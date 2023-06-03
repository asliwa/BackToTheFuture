#pragma once
#include <cstdint>
#include <filesystem>
#include <WinDef.h>
#include <unordered_map>
#include "File.h"


using DuplicatedIndex = size_t;

// move structs

struct METADATA_ENTRY_FILE_INFO
{
	Byte isDuplicate;	
	union 
	{
		// byte length in RAW file
		size_t Length;
		// index of original file in entries container
		size_t DuplicateIndex;
	};

};

struct METADATA_ENTRY
{
	std::uint16_t Length;
	METADATA_ENTRY_FILE_INFO FileInfo;
	std::uint16_t PathLength;
	WCHAR Path[1];
};

struct METADATA_HEADER
{
	GUID Guid;

	std::uint32_t Length;
	
	// version here? crc to ensure it was not modified?

	std::uint32_t EntriesCount;

	METADATA_ENTRY Entries[1];
};


using FileID = std::pair<unsigned long, size_t>;

struct free_delete
{
	void operator()(void* x) { free(x); }
};

using RuntimeMetadataEntry = std::unique_ptr<METADATA_ENTRY, free_delete>;

class Metadata
{
	// keep linked info (CRC of each file with its index in container)
	std::unordered_map<CRC, size_t> mInfo;

	// container where chronological entries are stored
	std::vector<RuntimeMetadataEntry> mEntries;

public:

	void add(const File& file)
	{
		auto iter = mInfo.find(file.getCrc());

		// it means there is a duplicate !
		if (iter != mInfo.end())
		{
			auto entry = static_cast<METADATA_ENTRY*>(malloc(sizeof(METADATA_ENTRY)));
			entry->Length = sizeof(METADATA_ENTRY);
			entry->FileInfo = { true, iter->second };
			entry->PathLength = 0;
			
			mEntries.push_back(std::move(RuntimeMetadataEntry(entry)));
		}

		// it is a new file ! 
		else
		{
			mInfo.emplace(file.getCrc(), mEntries.size());

			auto pathLength = file.getPath().native().length();
			auto entrySize = sizeof(METADATA_ENTRY) + (pathLength * sizeof(wchar_t));
			auto entry = static_cast<METADATA_ENTRY*>(malloc(entrySize));
			// check nbulltpr;

			// todo: cannot be greater than unsigned int? confirm! 
			entry->Length = (unsigned int)entrySize;
			entry->FileInfo = { false, file.getSize() };
			memcpy_s(&entry->Path, entrySize, file.getPath().c_str(), pathLength * sizeof(wchar_t));

			mEntries.push_back(std::move(RuntimeMetadataEntry(entry)));
		}
	}
};