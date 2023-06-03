#pragma once

#include <string>
#include <filesystem>
#include <rpc.h>
#include <sstream>

#include "IPackaging.h"

class Unpacker : IPackaging
{

public:
	Unpacker(std::string inputFile, std::string outputFolder) : IPackaging(inputFile, outputFolder)
	{
	}

	void run() override
	{
		
	}

	void validate() override
	{
		// check if it is archive
	}
};