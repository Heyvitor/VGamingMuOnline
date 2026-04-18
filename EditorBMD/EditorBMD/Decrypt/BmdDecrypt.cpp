#include "BmdDecrypt.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace
{
	Script_Item ConvertPackedToAligned(const Script_Item_Packed& source)
	{
		Script_Item target = {};
		target.Type = source.Type;
		target.Index = source.Index;
		target.SubIndex = source.SubIndex;
		memcpy(target.Name, source.Name, sizeof(target.Name));
		target.Kind1 = source.Kind1;
		target.Kind2 = source.Kind2;
		target.Kind3 = source.Kind3;
		target.TwoHand = source.TwoHand;
		target.Level = source.Level;
		target.m_byItemSlot = source.m_byItemSlot;
		target.m_wSkillIndex = source.m_wSkillIndex;
		target.Width = source.Width;
		target.Height = source.Height;
		target.DamageMin = source.DamageMin;
		target.DamageMax = source.DamageMax;
		target.SuccessfulBlocking = source.SuccessfulBlocking;
		target.Defense = source.Defense;
		target.MagicDefense = source.MagicDefense;
		target.WeaponSpeed = source.WeaponSpeed;
		target.WalkSpeed = source.WalkSpeed;
		target.Durability = source.Durability;
		target.MagicDur = source.MagicDur;
		target.MagicPower = source.MagicPower;
		target.RequireStrength = source.RequireStrength;
		target.RequireDexterity = source.RequireDexterity;
		target.RequireEnergy = source.RequireEnergy;
		target.RequireVitality = source.RequireVitality;
		target.RequireCharisma = source.RequireCharisma;
		target.RequireLevel = source.RequireLevel;
		target.Value = source.Value;
		target.iZen = source.iZen;
		target.AttType = source.AttType;
		memcpy(target.RequireClass, source.RequireClass, sizeof(target.RequireClass));
		memcpy(target.Resistance, source.Resistance, sizeof(target.Resistance));
		target.Drop = source.Drop;
		target.Trade = source.Trade;
		target.StorePersonal = source.StorePersonal;
		target.WhareHouse = source.WhareHouse;
		target.SellNpc = source.SellNpc;
		target.Expensive = source.Expensive;
		target.Repair = source.Repair;
		target.Overlap = source.Overlap;
		target.PcFlag = source.PcFlag;
		target.MuunFlag = source.MuunFlag;
		target.PowerATTK = source.PowerATTK;
		return target;
	}

	Script_Item ConvertLegacyToAligned(const Script_Item_Legacy& source)
	{
		Script_Item target = {};
		target.Type = source.Type;
		target.Index = source.Index;
		target.SubIndex = source.SubIndex;
		memcpy(target.Name, source.Name, sizeof(target.Name));
		target.Kind1 = source.Kind1;
		target.Kind2 = source.Kind2;
		target.Kind3 = source.Kind3;
		target.TwoHand = source.TwoHand;
		target.Level = source.Level;
		target.m_byItemSlot = source.m_byItemSlot;
		target.m_wSkillIndex = source.m_wSkillIndex;
		target.Width = source.Width;
		target.Height = source.Height;
		target.DamageMin = source.DamageMin;
		target.DamageMax = source.DamageMax;
		target.SuccessfulBlocking = source.SuccessfulBlocking;
		target.Defense = source.Defense;
		target.MagicDefense = source.MagicDefense;
		target.WeaponSpeed = source.WeaponSpeed;
		target.WalkSpeed = source.WalkSpeed;
		target.Durability = source.Durability;
		target.MagicDur = source.MagicDur;
		target.MagicPower = source.MagicPower;
		target.RequireStrength = source.RequireStrength;
		target.RequireDexterity = source.RequireDexterity;
		target.RequireEnergy = source.RequireEnergy;
		target.RequireVitality = source.RequireVitality;
		target.RequireCharisma = 0;
		target.RequireLevel = source.RequireLevel;
		target.Value = source.Value;
		target.iZen = source.iZen;
		target.AttType = source.AttType;
		memcpy(target.RequireClass, source.RequireClass, sizeof(target.RequireClass));
		memcpy(target.Resistance, source.Resistance, sizeof(target.Resistance));
		target.Drop = source.Drop;
		target.Trade = source.Trade;
		target.StorePersonal = source.StorePersonal;
		target.WhareHouse = source.WhareHouse;
		target.SellNpc = source.SellNpc;
		target.Expensive = source.Expensive;
		target.Repair = source.Repair;
		target.Overlap = source.Overlap;
		target.PcFlag = source.PcFlag;
		target.MuunFlag = source.MuunFlag;
		target.PowerATTK = source.PowerATTK;
		return target;
	}

	Script_Item_Packed ConvertAlignedToPacked(const Script_Item& source)
	{
		Script_Item_Packed target = {};
		target.Type = source.Type;
		target.Index = source.Index;
		target.SubIndex = source.SubIndex;
		memcpy(target.Name, source.Name, sizeof(target.Name));
		target.Kind1 = source.Kind1;
		target.Kind2 = source.Kind2;
		target.Kind3 = source.Kind3;
		target.TwoHand = source.TwoHand;
		target.Level = source.Level;
		target.m_byItemSlot = source.m_byItemSlot;
		target.m_wSkillIndex = source.m_wSkillIndex;
		target.Width = source.Width;
		target.Height = source.Height;
		target.DamageMin = source.DamageMin;
		target.DamageMax = source.DamageMax;
		target.SuccessfulBlocking = source.SuccessfulBlocking;
		target.Defense = source.Defense;
		target.MagicDefense = source.MagicDefense;
		target.WeaponSpeed = source.WeaponSpeed;
		target.WalkSpeed = source.WalkSpeed;
		target.Durability = source.Durability;
		target.MagicDur = source.MagicDur;
		target.MagicPower = source.MagicPower;
		target.RequireStrength = source.RequireStrength;
		target.RequireDexterity = source.RequireDexterity;
		target.RequireEnergy = source.RequireEnergy;
		target.RequireVitality = source.RequireVitality;
		target.RequireCharisma = source.RequireCharisma;
		target.RequireLevel = source.RequireLevel;
		target.Value = source.Value;
		target.iZen = source.iZen;
		target.AttType = source.AttType;
		memcpy(target.RequireClass, source.RequireClass, sizeof(target.RequireClass));
		memcpy(target.Resistance, source.Resistance, sizeof(target.Resistance));
		target.Drop = source.Drop;
		target.Trade = source.Trade;
		target.StorePersonal = source.StorePersonal;
		target.WhareHouse = source.WhareHouse;
		target.SellNpc = source.SellNpc;
		target.Expensive = source.Expensive;
		target.Repair = source.Repair;
		target.Overlap = source.Overlap;
		target.PcFlag = source.PcFlag;
		target.MuunFlag = source.MuunFlag;
		target.PowerATTK = source.PowerATTK;
		return target;
	}

	const char* GetLayoutName(BmdItemLayout layout)
	{
		switch (layout)
		{
		case BmdItemLayout::Legacy155:
			return "Legacy155";
		case BmdItemLayout::Packed157:
			return "Packed157";
		case BmdItemLayout::Aligned164:
		default:
			return "Aligned164";
		}
	}
}

bool BmdDecrypt::LoadBmd(const char* filePath, std::vector<Script_Item>& outItems, BmdItemLayout& outLayout, std::string& outError)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file)
	{
		outError = "Falha ao abrir o arquivo BMD.";
		return false;
	}

	file.seekg(0, std::ios::end);
	const size_t fileSize = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	if (fileSize < sizeof(DWORD) * 2)
	{
		outError = "Arquivo muito pequeno para ser um BMD valido.";
		return false;
	}

	DWORD maxLine = 0;
	file.read(reinterpret_cast<char*>(&maxLine), sizeof(DWORD));
	if (!file || maxLine == 0)
	{
		char errorMsg[256];
		sprintf_s(errorMsg, "Cabecalho invalido no Item.bmd (maxLine: %u, fileSize: %u).", maxLine, static_cast<unsigned int>(fileSize));
		outError = errorMsg;
		return false;
	}

	const size_t rawSize = fileSize - (sizeof(DWORD) * 2);
	if (rawSize % maxLine != 0)
	{
		outError = "O bloco de dados do Item.bmd nao e divisivel pela quantidade de linhas.";
		return false;
	}

	const size_t itemSize = rawSize / maxLine;
	if (itemSize == sizeof(Script_Item))
	{
		outLayout = BmdItemLayout::Aligned164;
	}
	else if (itemSize == sizeof(Script_Item_Packed))
	{
		outLayout = BmdItemLayout::Packed157;
	}
	else if (itemSize == sizeof(Script_Item_Legacy))
	{
		outLayout = BmdItemLayout::Legacy155;
	}
	else
	{
		char errorMsg[256];
		sprintf_s(errorMsg, "Tamanho do arquivo invalido. Tamanho real do item: %u bytes, esperado: %u, %u ou %u bytes, maxLine: %u.",
			static_cast<unsigned int>(itemSize),
			static_cast<unsigned int>(sizeof(Script_Item_Legacy)),
			static_cast<unsigned int>(sizeof(Script_Item_Packed)),
			static_cast<unsigned int>(sizeof(Script_Item)),
			maxLine);
		outError = errorMsg;
		return false;
	}

	std::vector<BYTE> rawBuffer(rawSize);
	file.read(reinterpret_cast<char*>(rawBuffer.data()), static_cast<std::streamsize>(rawSize));
	if (!file)
	{
		outError = "Falha ao ler dados criptografados do Item.bmd.";
		return false;
	}

	DWORD checksum = 0;
	file.read(reinterpret_cast<char*>(&checksum), sizeof(DWORD));
	if (!file)
	{
		outError = "Falha ao ler checksum do Item.bmd.";
		return false;
	}

	const DWORD expectedChecksum = BmdEncryption::GenerateCheckSum(rawBuffer.data(), static_cast<int>(rawBuffer.size()), BMD_KEY);
	if (checksum != expectedChecksum)
	{
		outError = "Checksum invalido: arquivo BMD corrompido ou chave incorreta.";
		return false;
	}

	outItems.clear();
	outItems.resize(maxLine);

	for (DWORD i = 0; i < maxLine; ++i)
	{
		if (outLayout == BmdItemLayout::Legacy155)
		{
			BYTE* current = rawBuffer.data() + (i * sizeof(Script_Item_Legacy));
			BmdEncryption::BuxConvert(current, static_cast<int>(sizeof(Script_Item_Legacy)));

			Script_Item_Legacy legacyItem = {};
			memcpy(&legacyItem, current, sizeof(legacyItem));
			outItems[i] = ConvertLegacyToAligned(legacyItem);
		}
		else if (outLayout == BmdItemLayout::Packed157)
		{
			BYTE* current = rawBuffer.data() + (i * sizeof(Script_Item_Packed));
			BmdEncryption::BuxConvert(current, static_cast<int>(sizeof(Script_Item_Packed)));

			Script_Item_Packed packedItem = {};
			memcpy(&packedItem, current, sizeof(packedItem));
			outItems[i] = ConvertPackedToAligned(packedItem);
		}
		else
		{
			BYTE* current = rawBuffer.data() + (i * sizeof(Script_Item));
			BmdEncryption::BuxConvert(current, static_cast<int>(sizeof(Script_Item)));
			memcpy(&outItems[i], current, sizeof(Script_Item));
		}
	}

	std::sort(outItems.begin(), outItems.end(), [](const Script_Item& a, const Script_Item& b)
		{
			if (a.Index != b.Index)
			{
				return a.Index < b.Index;
			}

			return a.SubIndex < b.SubIndex;
		});

	return true;
}

bool BmdDecrypt::SaveBmd(const char* filePath, const std::vector<Script_Item>& items, BmdItemLayout layout, std::string& outError)
{
	if (items.empty())
	{
		outError = "Nao e possivel salvar um Item.bmd vazio.";
		return false;
	}

	if (layout == BmdItemLayout::Legacy155)
	{
		layout = BmdItemLayout::Packed157;
	}

	const size_t itemSize = (layout == BmdItemLayout::Packed157) ? sizeof(Script_Item_Packed) : sizeof(Script_Item);
	const DWORD maxLine = static_cast<DWORD>(items.size());
	const size_t rawSize = static_cast<size_t>(maxLine) * itemSize;
	std::vector<BYTE> rawBuffer(rawSize);

	for (DWORD i = 0; i < maxLine; ++i)
	{
		Script_Item normalizedItem = items[i];
		normalizedItem.Type = GET_ITEM(normalizedItem.Index, normalizedItem.SubIndex);

		BYTE* current = rawBuffer.data() + (i * itemSize);
		if (layout == BmdItemLayout::Packed157)
		{
			const Script_Item_Packed packedItem = ConvertAlignedToPacked(normalizedItem);
			memcpy(current, &packedItem, sizeof(packedItem));
			BmdEncryption::BuxConvert(current, static_cast<int>(sizeof(packedItem)));
		}
		else
		{
			memcpy(current, &normalizedItem, sizeof(normalizedItem));
			BmdEncryption::BuxConvert(current, static_cast<int>(sizeof(normalizedItem)));
		}
	}

	const DWORD checksum = BmdEncryption::GenerateCheckSum(rawBuffer.data(), static_cast<int>(rawBuffer.size()), BMD_KEY);

	std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
	if (!file)
	{
		outError = "Falha ao abrir Item.bmd para escrita.";
		return false;
	}

	file.write(reinterpret_cast<const char*>(&maxLine), sizeof(DWORD));
	file.write(reinterpret_cast<const char*>(rawBuffer.data()), static_cast<std::streamsize>(rawSize));
	file.write(reinterpret_cast<const char*>(&checksum), sizeof(DWORD));

	if (!file)
	{
		outError = "Falha ao gravar Item.bmd criptografado.";
		return false;
	}

	std::string txtPath = filePath;
	const size_t lastDot = txtPath.find_last_of('.');
	if (lastDot != std::string::npos)
	{
		txtPath = txtPath.substr(0, lastDot) + ".txt";
	}
	else
	{
		txtPath += ".txt";
	}

	std::ofstream txtFile(txtPath);
	if (txtFile)
	{
		txtFile << "Item.bmd Configuration\n";
		txtFile << "=======================\n\n";
		txtFile << "MaxLines: " << maxLine << "\n";
		txtFile << "ItemSize: " << itemSize << " bytes\n";
		txtFile << "Layout: " << GetLayoutName(layout) << "\n";
		txtFile << "TotalItems: " << items.size() << "\n";
		txtFile << "File Path: " << filePath << "\n";
		txtFile << "Checksum: 0x" << std::hex << std::uppercase << checksum << std::dec << "\n\n";

		std::vector<int> sectionCounts(MAX_ITEM_SECTION, 0);
		for (const auto& item : items)
		{
			if (item.Index >= 0 && item.Index < MAX_ITEM_SECTION)
			{
				sectionCounts[item.Index]++;
			}
		}

		txtFile << "Items per Section:\n";
		txtFile << "------------------\n";
		for (int i = 0; i < MAX_ITEM_SECTION; ++i)
		{
			if (sectionCounts[i] > 0)
			{
				txtFile << "Section " << i << ": " << sectionCounts[i] << " items\n";
			}
		}

		txtFile << "\nFile Structure:\n";
		txtFile << "---------------\n";
		txtFile << "Header: 4 bytes (maxLine)\n";
		txtFile << "Data: " << rawSize << " bytes (" << maxLine << " items x " << itemSize << " bytes)\n";
		txtFile << "Checksum: 4 bytes\n";
		txtFile << "Total File Size: " << (sizeof(DWORD) + rawSize + sizeof(DWORD)) << " bytes\n\n";

		txtFile << "Created: " << __DATE__ << " " << __TIME__ << "\n";
	}

	return true;
}
