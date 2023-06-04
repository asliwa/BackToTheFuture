#pragma once
#include <cstdint>
#include <filesystem>
#include <WinDef.h>
#include "File.h"


// move structs
// no padding

#pragma pack(push,1)
struct METADATA_ENTRY_HEADER
{
	size_t Length;
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