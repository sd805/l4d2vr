#pragma once
#include <Windows.h>
#include <psapi.h>
#include <vector>
#include <sstream>

class SigScanner
{
public:
	// Returns 0 if current offset matches, -1 if no matches found.
	// A value > 0 is the new offset.
	static int VerifyOffset(std::string moduleName, int currentOffset, std::string signature, int sigOffset = 0)
	{
		HMODULE hModule = GetModuleHandle(moduleName.c_str());
		MODULEINFO moduleInfo;
		GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo));

		uint8_t *bytes = (uint8_t *)moduleInfo.lpBaseOfDll;

		std::vector<int> pattern;

		std::stringstream ss(signature);
		std::string sigByte;
		while (ss >> sigByte)
		{
			if (sigByte == "?" || sigByte == "??")
				pattern.push_back(-1);
			else
				pattern.push_back(strtoul(sigByte.c_str(), NULL, 16));
		}

		int patternLen = pattern.size();

		// Check if current offset is good
		bool offsetMatchesSig = true;
		for (int i = 0; i < patternLen; ++i)
		{
			if ( (bytes[currentOffset - sigOffset + i] != pattern[i]) && (pattern[i] != -1) )
			{
				offsetMatchesSig = false;
				break;
			}
		}

		if (offsetMatchesSig)
			return 0;

		// Scan the dll for new offset
		for (int i = 0; i < moduleInfo.SizeOfImage; ++i)
		{
			bool found = true;
			for (int j = 0; j < patternLen; ++j)
			{
				if ((bytes[i + j] != pattern[j]) && (pattern[j] != -1))
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				return i + sigOffset;
			}
		}
		return -1;

	}
};