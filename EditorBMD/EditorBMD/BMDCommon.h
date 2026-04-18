#pragma once

#include <windows.h>
#include <cstddef>
#include <string>
#include <vector>

constexpr WORD BMD_KEY = 0xE2F1;
constexpr int MAX_ITEM_SECTION = 16;
constexpr int MAX_ITEM_TYPE = 512;
constexpr int MAX_CLASS = 15;
constexpr int MAX_CLASS_PLAYER = MAX_CLASS;
constexpr int MAX_RESISTANCE_TYPE = 7;
constexpr int MAX_RESISTANCE = MAX_RESISTANCE_TYPE;

constexpr BYTE BUX_CODE[3] = { 0xFC, 0xCF, 0xAB };

#define GET_ITEM(x, y) (((x) * 512) + (y))

struct Script_Item
{
	int Type;
	short Index;
	short SubIndex;
	char Name[64];
	BYTE Kind1;
	BYTE Kind2;
	BYTE Kind3;
	bool TwoHand;
	WORD Level;
	BYTE m_byItemSlot;
	WORD m_wSkillIndex;
	BYTE Width;
	BYTE Height;
	WORD DamageMin;
	WORD DamageMax;
	BYTE SuccessfulBlocking;
	WORD Defense;
	WORD MagicDefense;
	BYTE WeaponSpeed;
	BYTE WalkSpeed;
	BYTE Durability;
	BYTE MagicDur;
	DWORD MagicPower;
	WORD RequireStrength;
	WORD RequireDexterity;
	WORD RequireEnergy;
	WORD RequireVitality;
	WORD RequireCharisma;
	WORD RequireLevel;
	BYTE Value;
	int iZen;
	BYTE AttType;
	BYTE RequireClass[MAX_CLASS_PLAYER];
	BYTE Resistance[MAX_RESISTANCE];
	bool Drop;
	bool Trade;
	bool StorePersonal;
	bool WhareHouse;
	bool SellNpc;
	bool Expensive;
	bool Repair;
	WORD Overlap;
	WORD PcFlag;
	WORD MuunFlag;
	DWORD PowerATTK;
};

#pragma pack(push, 1)
struct Script_Item_Packed
{
	int Type;
	short Index;
	short SubIndex;
	char Name[64];
	BYTE Kind1;
	BYTE Kind2;
	BYTE Kind3;
	bool TwoHand;
	WORD Level;
	BYTE m_byItemSlot;
	WORD m_wSkillIndex;
	BYTE Width;
	BYTE Height;
	WORD DamageMin;
	WORD DamageMax;
	BYTE SuccessfulBlocking;
	WORD Defense;
	WORD MagicDefense;
	BYTE WeaponSpeed;
	BYTE WalkSpeed;
	BYTE Durability;
	BYTE MagicDur;
	DWORD MagicPower;
	WORD RequireStrength;
	WORD RequireDexterity;
	WORD RequireEnergy;
	WORD RequireVitality;
	WORD RequireCharisma;
	WORD RequireLevel;
	BYTE Value;
	int iZen;
	BYTE AttType;
	BYTE RequireClass[MAX_CLASS_PLAYER];
	BYTE Resistance[MAX_RESISTANCE];
	bool Drop;
	bool Trade;
	bool StorePersonal;
	bool WhareHouse;
	bool SellNpc;
	bool Expensive;
	bool Repair;
	WORD Overlap;
	WORD PcFlag;
	WORD MuunFlag;
	DWORD PowerATTK;
};

struct Script_Item_Legacy
{
	int Type;
	short Index;
	short SubIndex;
	char Name[64];
	BYTE Kind1;
	BYTE Kind2;
	BYTE Kind3;
	bool TwoHand;
	WORD Level;
	BYTE m_byItemSlot;
	WORD m_wSkillIndex;
	BYTE Width;
	BYTE Height;
	WORD DamageMin;
	WORD DamageMax;
	BYTE SuccessfulBlocking;
	WORD Defense;
	WORD MagicDefense;
	BYTE WeaponSpeed;
	BYTE WalkSpeed;
	BYTE Durability;
	BYTE MagicDur;
	DWORD MagicPower;
	WORD RequireStrength;
	WORD RequireDexterity;
	WORD RequireEnergy;
	WORD RequireVitality;
	WORD RequireLevel;
	BYTE Value;
	int iZen;
	BYTE AttType;
	BYTE RequireClass[MAX_CLASS_PLAYER];
	BYTE Resistance[MAX_RESISTANCE];
	bool Drop;
	bool Trade;
	bool StorePersonal;
	bool WhareHouse;
	bool SellNpc;
	bool Expensive;
	bool Repair;
	WORD Overlap;
	WORD PcFlag;
	WORD MuunFlag;
	DWORD PowerATTK;
};
#pragma pack(pop)

enum class BmdItemLayout
{
	Aligned164,
	Packed157,
	Legacy155,
};

static_assert(offsetof(Script_Item, Name) == 8, "Offset invalido para Script_Item::Name.");
static_assert(offsetof(Script_Item, Kind1) == 72, "Offset invalido para Script_Item::Kind1.");
static_assert(offsetof(Script_Item, RequireClass) == 125, "Offset invalido para Script_Item::RequireClass.");
static_assert(offsetof(Script_Item, PowerATTK) == 160, "Offset invalido para Script_Item::PowerATTK.");
static_assert(sizeof(Script_Item) == 164, "Script_Item deve respeitar o layout alinhado do main.");

static_assert(offsetof(Script_Item_Packed, Name) == 8, "Offset invalido para Script_Item_Packed::Name.");
static_assert(offsetof(Script_Item_Packed, Kind1) == 72, "Offset invalido para Script_Item_Packed::Kind1.");
static_assert(offsetof(Script_Item_Packed, RequireClass) == 118, "Offset invalido para Script_Item_Packed::RequireClass.");
static_assert(offsetof(Script_Item_Packed, PowerATTK) == 153, "Offset invalido para Script_Item_Packed::PowerATTK.");
static_assert(sizeof(Script_Item_Packed) == 157, "Script_Item_Packed deve ter 157 bytes.");

static_assert(offsetof(Script_Item_Legacy, Name) == 8, "Offset invalido para Script_Item_Legacy::Name.");
static_assert(offsetof(Script_Item_Legacy, Kind1) == 72, "Offset invalido para Script_Item_Legacy::Kind1.");
static_assert(offsetof(Script_Item_Legacy, RequireClass) == 116, "Offset invalido para Script_Item_Legacy::RequireClass.");
static_assert(offsetof(Script_Item_Legacy, PowerATTK) == 151, "Offset invalido para Script_Item_Legacy::PowerATTK.");
static_assert(sizeof(Script_Item_Legacy) == 155, "Script_Item_Legacy deve ter 155 bytes.");

constexpr int IDC_OPEN = 1001;
constexpr int IDC_SAVE = 1002;
constexpr int IDC_LISTBOX = 1003;
constexpr int IDC_EDIT_NAME = 1004;
constexpr int IDC_EDIT_LEVEL = 1005;
constexpr int IDC_EDIT_DAMAGE = 1006;
constexpr int IDC_EDIT_DEFENSE = 1007;
constexpr int IDC_EDIT_DURABILITY = 1008;
constexpr int IDC_EDIT_REQUIRELEVEL = 1009;
constexpr int IDC_EDIT_VALUE = 10010;
constexpr int IDC_STATIC_INFO = 10011;
