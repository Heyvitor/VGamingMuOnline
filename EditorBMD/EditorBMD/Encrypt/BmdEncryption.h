#pragma once

#include "..\BMDCommon.h"

class BmdEncryption
{
public:
	static void BuxConvert(BYTE* buffer, int size);
	static DWORD GenerateCheckSum(BYTE* buffer, int size, WORD key);
	static bool EncryptItem(BYTE* buffer, int size);
	static bool DecryptItem(BYTE* buffer, int size);
};