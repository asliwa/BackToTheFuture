#pragma once

#include <string>
#include <filesystem>
#include <rpc.h>
#include <sstream>

#include "IPackaging.h"

#include "File.h"
#include "Metadata.h"


class Unpack : IPackaging
{
	CompressedFile mRawData;

	DefaultBuffer mBuffer = std::make_unique<DefaultBuffer::element_type>();

	MetadataEx readMd()
	{
		size_t mdSize;
		mRawData.read(mdSize);

		// read rest of MD
		mRawData.read(mBuffer, mdSize - sizeof(size_t));

		return MetadataEx(mBuffer, mdSize);
	}

public:
	Unpack(std::string inputFile, std::string outputFolder) : IPackaging(inputFile, outputFolder), mRawData(inputFile, "rb")
	{}

	void execute() override
	{
		METADATA_HEADER header;
		mRawData.read(header);

		if (header.Guid != PACKAGE_GUID)
		{
			std::cout << "UPCK: Guid was not found, wrong file" << std::endl;
			return;
		}

		std::cout << "UPCK: Guid was found" << std::endl;

		size_t entrySize;

		while (mRawData.read(entrySize))
		{
			std::cout << "UPCK: MD entry size: "<< entrySize << std::endl;
			mRawData.read(mBuffer, entrySize - sizeof(size_t));

			auto fileMd = MetadataEx(mBuffer, entrySize);

			auto newFile = File({ mOutputPath / fileMd.getFilePath() }, L"wb");

			std::cout << "UPCK: New file: " << fileMd.getFilePath() << std::endl;

			for (size_t i = 0; i < fileMd.getLength() / BUFFER_SIZE; i++)
			{
				auto bytesRead = mRawData.read(mBuffer);
				newFile.write(mBuffer, bytesRead);
			}

			if (auto remaining = fileMd.getLength() % BUFFER_SIZE)
			{
				auto bytesRead = mRawData.read(mBuffer, remaining);
				newFile.write(mBuffer, bytesRead);
			}
		}
	}

	void validate() override
	{
		// check if it is archive
	}
};