#include "BmdEncryption.h"

void BmdEncryption::BuxConvert(BYTE* buffer, int size)
{
	for (int i = 0; i < size; ++i)
	{
		buffer[i] ^= BUX_CODE[i % 3];
	}
}

DWORD BmdEncryption::GenerateCheckSum(BYTE* buffer, int size, WORD key)
{
	DWORD result = static_cast<DWORD>(key << 9);
	DWORD dwKey = static_cast<DWORD>(key);

	for (int checked = 0; checked <= size - 4; checked += 4)
	{
		DWORD temp = 0;
		memcpy(&temp, buffer + checked, sizeof(DWORD));

		if (((key + (checked >> 2)) % 2) == 0)
		{
			result ^= temp;
		}
		else
		{
			result += temp;
		}

		if ((checked % 0x10) == 0)
		{
			result ^= static_cast<unsigned int>(result + dwKey) >> ((checked >> 2) % 8 + 1);
		}
	}

	return result;
}

bool BmdEncryption::EncryptItem(BYTE* buffer, int size)
{
	if (buffer == nullptr || size <= 0)
	{
		return false;
	}

	BuxConvert(buffer, size);
	return true;
}

bool BmdEncryption::DecryptItem(BYTE* buffer, int size)
{
	if (buffer == nullptr || size <= 0)
	{
		return false;
	}

	BuxConvert(buffer, size);
	return true;
}