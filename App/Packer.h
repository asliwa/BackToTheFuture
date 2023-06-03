#pragma once

#include <string>
#include <filesystem>
#include <rpc.h>
#include <sstream>

#include "IPackaging.h"

class Packer : IPackaging
{

public:
	Packer(std::string inputFolder, std::string outputFolder) : IPackaging(inputFolder, outputFolder)
	{
	}

	void run() override
	{
		for (const auto& path : std::filesystem::recursive_directory_iterator(mInputPath))
		{
			// TODO: can throw!
			if (!path.is_regular_file())
			{
				// trace it
				continue;
			}
		}
	}

	void validate() override
	{

	}
};