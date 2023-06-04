#pragma once

#include "IPackaging.h"

#include "File.h"
#include "Metadata.h"

class Pack : IPackaging
{
	CompressedFile mRawData;

public:
	Pack(std::string inputFolder, std::string outputFolder) : IPackaging(inputFolder, outputFolder), mRawData(std::filesystem::path(mOutputPath) / "Raw", "wb")
	{
	}

	void execute() override
	{
		std::cout << "PCK: starting packing in folder: " << mInputPath << std::endl;

		METADATA_HEADER header { PACKAGE_GUID };		
		mRawData.write(header);

		std::cout << "PCK: global header written " << mInputPath << std::endl;
		
		for (const auto& iter : std::filesystem::recursive_directory_iterator(mInputPath))
		{
			std::cout << "PCK: File: " << iter.path() << std::endl;
			// TODO: can throw!
			if (!iter.is_regular_file())
			{
				// trace it
				std::cout << "PCK: Skipping this file as it is not regular!" << std::endl;
				continue;
			}

			// open: does file extension matter? assumption here that it does not, we are reading *all* files in given directory, not .bin & .txt only

			pack(iter.path());

		}

		std::cout << "PCK: finished packing in folder: " << mInputPath << std::endl;
	}

	void validate() override
	{
		// check folder?
	}

	void pack(const std::filesystem::path& path)
	{
		auto file = LogFile{ path, mInputPath };

		auto buffer = std::make_unique<DefaultBuffer::element_type>();
		
		// get MD
		auto bytesLoaded = MetadataEx(file).load(buffer);
		// write MD
		auto bytesWritten = mRawData.write(buffer, bytesLoaded);
		
		// read from file
		while (auto bytesRead = file.read(buffer))
		{
			bytesWritten = mRawData.write(buffer, bytesRead);
		}
	}
};