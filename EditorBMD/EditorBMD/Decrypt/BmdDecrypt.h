#pragma once

#include "..\BMDCommon.h"
#include "..\Encrypt\BmdEncryption.h"
#include <string>

class BmdDecrypt
{
public:
	static bool LoadBmd(const char* filePath, std::vector<Script_Item>& outItems, BmdItemLayout& outLayout, std::string& outError);
	static bool SaveBmd(const char* filePath, const std::vector<Script_Item>& items, BmdItemLayout layout, std::string& outError);
};
