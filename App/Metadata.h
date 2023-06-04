#pragma once
#include <cstdint>
#include <filesystem>
#include <WinDef.h>
#include <unordered_map>
#include "File.h"


using DuplicatedIndex = size_t;

// move structs
// no padding
#pragma pack(push,1)
struct METADATA_ENTRY_HEADER
{
	// Byte isDuplicate;
	union
	{
		// byte length of file content
		size_t Length;
		// index of original file in entries container
		DuplicatedIndex Index;
	};
	size_t PathLength; // in wchars! not in bytes
	WCHAR Path[1];
};

struct METADATA_ENTRY
{
	size_t Size;
	METADATA_ENTRY_HEADER Header;
};

struct METADATA_HEADER
{
	GUID Guid;		
	// version here? crc to ensure it was not modified?
	// do we need anything here?
};

#pragma pack(pop)


struct free_delete
{
	void operator()(void* x) { free(x); }
};


using RuntimeMetadataEntry = std::unique_ptr<METADATA_ENTRY, free_delete>;

class MetadataEx
{
	RuntimeMetadataEntry mEntry;

public:
	MetadataEx(const LogFile& file)
	{
		auto pathLength = file.getShortPath().native().length();
		auto entrySize = sizeof(METADATA_ENTRY) + (pathLength * sizeof(wchar_t));
		mEntry = RuntimeMetadataEntry(static_cast<METADATA_ENTRY*>(malloc(entrySize)));
		
		if (mEntry == nullptr)
		{
			//error
			return;
		}

		// todo: cannot be greater than unsigned int? confirm! 
		mEntry->Size = entrySize;
		mEntry->Header.PathLength = pathLength;
		mEntry->Header.Length = file.getSize();
		memcpy_s(&mEntry->Header.Path, entrySize, file.getShortPath().c_str(), pathLength * sizeof(wchar_t));
	}

	template<size_t N>
	MetadataEx(const Buffer<N>& buffer, size_t size)
	{
		mEntry = RuntimeMetadataEntry(static_cast<METADATA_ENTRY*>(malloc(size)));
		mEntry->Size = size;
		if (mEntry == nullptr)
		{
			//error
			return;
		}
		// buffer should hold only METADATA_ENTRY_HEADER, not METADATA_ENTRY !
		memcpy_s(&mEntry.get()->Header, size, buffer.get(), size - sizeof(mEntry->Size));
	}

	template<size_t N>
	size_t load(Buffer<N>& buffer)
	{
		if (N < mEntry->Size)
		{
			// error
			return 0;
		}

		memcpy_s(buffer.get(), N, mEntry.get(), mEntry->Size);
		return mEntry->Size;
	}

	std::filesystem::path getFilePath() const
	{
		std::wstring path(mEntry->Header.Path, mEntry->Header.PathLength);
		return std::filesystem::path(path);
	}

	size_t getLength() const
	{
		return mEntry->Header.Length;
	}
};

//
//class Metadata
//{
//	// keep linked info (CRC of each file with its index in container)
//	std::unordered_map<CRC, size_t> mInfo;
//
//	// container where chronological entries are stored
//	std::vector<RuntimeMetadataEntry> mEntries;
//
//	void addDuplicate(const LogFile& file, const std::unordered_map<CRC, size_t>::iterator& iter)
//	{
//		std::cout << "MD: Adding duplicate: " << file.getShortPath() << std::endl;
//		auto entry = static_cast<METADATA_ENTRY*>(malloc(sizeof(METADATA_ENTRY)));
//		// check nullptr
//
//		entry->Length = sizeof(METADATA_ENTRY);
//		entry->FileInfo = { true, iter->second };
//		entry->PathLength = 0;
//
//		mEntries.push_back(std::move(RuntimeMetadataEntry(entry)));
//	}
//
//	void addNew(const LogFile& file)
//	{
//		std::cout << "MD: Adding new file: " << file.getShortPath() << std::endl;
//		mInfo.emplace(file.getCrc(), mEntries.size());
//
//		auto pathLength = file.getShortPath().native().length();
//		auto entrySize = sizeof(METADATA_ENTRY) + (pathLength * sizeof(wchar_t));
//		auto entry = static_cast<METADATA_ENTRY*>(malloc(entrySize));
//		// check nbulltpr;
//
//		// todo: cannot be greater than unsigned int? confirm! 
//		entry->Length = (unsigned int)entrySize;
//		entry->FileInfo = { false, file.getSize() };
//		memcpy_s(&entry->Path, entrySize, file.getShortPath().c_str(), pathLength * sizeof(wchar_t));
//
//		mEntries.push_back(std::move(RuntimeMetadataEntry(entry)));
//	}
//
//public:
//
//	void add(const LogFile& file)
//	{
//		/*auto iter = mInfo.find(file.getCrc());
//
//		iter == mInfo.end() ? addNew(file) : addDuplicate(file, iter);*/
//		addNew(file);
//	}
//
//	bool contains(const LogFile& file)
//	{
//		return mInfo.find(file.getCrc()) != mInfo.end();
//	}
//};