#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <windows.h>



class IPackaging
{
protected:
	const std::filesystem::path mInputPath;
	const std::filesystem::path mOutputPath;
	
	//const std::filesystem::path mWorkingPath;

	// {405E4CBA-A516-42D2-8110-E2C9FB345C64}
	static constexpr GUID PACKAGE_GUID{ 0x405e4cba, 0xa516, 0x42d2, { 0x81, 0x10, 0xe2, 0xc9, 0xfb, 0x34, 0x5c, 0x64 } };

	enum class Status { Success, Fail } mStatus = Status::Success;

	IPackaging(std::string input, std::string output) : mInputPath(input), mOutputPath(output)
	{
	}

	// command interface
	virtual void validate() = 0;
	virtual void execute() = 0;
};