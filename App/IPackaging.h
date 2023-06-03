#pragma once

#include <filesystem>

class IPackaging
{
protected:
	const std::filesystem::path mInputPath;
	const std::filesystem::path mOutputPath;
	const std::filesystem::path mWorkingPath;

	enum class Status { Success, Fail } mStatus = Status::Success;

	IPackaging(std::string input, std::string output) : mInputPath(input), mOutputPath(output), mWorkingPath(mOutputPath / "XDD")
	{
		std::error_code err;

		// check status
		// assumption there is no 'XDD' folder in destination folder
		std::filesystem::create_directories(mWorkingPath, err);
	}


	virtual void clean()
	{
		if (mStatus == Status::Fail)
		{
			// clean dir
		}
	}

	virtual void validate() = 0;
	virtual void run() = 0;
};