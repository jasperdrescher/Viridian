#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace FileUtility
{
	static std::string ReadFile(const char* aFilepath)
	{
		if (!std::filesystem::exists(aFilepath))
		{
			printf("Failed to open %s\n", aFilepath);
			return "";
		}

		std::ifstream filestream(aFilepath, std::ifstream::in);
		if (!filestream.is_open())
		{
			printf("Failed to open %s\n", aFilepath);
			return "";
		}

		std::stringstream sourceStringStream;
		sourceStringStream << filestream.rdbuf();

		filestream.close();

		return sourceStringStream.str();
	}
}
