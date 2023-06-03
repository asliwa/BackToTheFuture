#pragma once

#include <string>
#include <filesystem>
#include <rpc.h>
#include <sstream>

#include "IPackaging.h"

#include "File.h"

class Packer : IPackaging
{


public:
	Packer(std::string inputFolder, std::string outputFolder) : IPackaging(inputFolder, outputFolder)
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

			
		
		}
	}

	void validate() override
	{
		// check folder?
	}
};