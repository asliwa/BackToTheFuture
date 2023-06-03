#pragma once

#include <string>
#include <filesystem>
#include <rpc.h>
#include <sstream>

#include "IPackaging.h"

#include "File.h"
#include "Metadata.h"

class Packer : IPackaging
{
	CompressedFile mRawData;
	Metadata mMetadata;

public:
	Packer(std::string inputFolder, std::string outputFolder) : IPackaging(inputFolder, outputFolder), mRawData(std::filesystem::path(mWorkingPath) / "Raw")
	{
	}

	void run() override
	{
		for (const auto& iter : std::filesystem::recursive_directory_iterator(mInputPath))
		{
			// TODO: can throw!
			if (!iter.is_regular_file())
			{
				// trace it
				continue;
			}

			auto file = File{ iter.path() };
			
			READ_WRITE_BUFFER buffer;

			while (file.read(buffer))
			{
				mRawData.write(buffer);
			}

			mMetadata.add(file);

		}
	}

	void validate() override
	{
		// check folder?
	}
};