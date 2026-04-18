// EditorBMD.cpp : Define o ponto de entrada para o aplicativo.
//

#include "..\framework.h"
#include "Main.h"
#include "Resource.h"
#include <commdlg.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "UxTheme.lib")

namespace
{
    constexpr COLORREF kDarkWindowColor = RGB(34, 38, 44);
    constexpr COLORREF kDarkSurfaceColor = RGB(43, 48, 56);
    constexpr COLORREF kDarkControlColor = RGB(52, 58, 67);
    constexpr COLORREF kDarkBorderColor = RGB(82, 89, 99);
    constexpr COLORREF kDarkTextColor = RGB(244, 247, 250);
    constexpr COLORREF kDarkMutedTextColor = RGB(196, 203, 212);
    constexpr COLORREF kDarkAccentColor = RGB(74, 144, 226);

    HBRUSH g_WindowBrush = nullptr;
    HBRUSH g_SurfaceBrush = nullptr;
    HBRUSH g_ControlBrush = nullptr;

    HWND g_hSectionLabel = nullptr;
    HWND g_hSectionCombo = nullptr;
    HWND g_hSearchLabel = nullptr;
    HWND g_hSearchEdit = nullptr;
    HWND g_hAddButton = nullptr;
    HWND g_hDeleteButton = nullptr;
    HWND g_hDuplicateButton = nullptr;
    HWND g_hStatusLabel = nullptr;
    HWND g_hDetailsTable = nullptr;
    HWND g_hInlineEdit = nullptr;
    WNDPROC g_InlineEditProc = nullptr;
    HWND g_MainWindow = nullptr;

    std::vector<int> g_FilteredIndices;
    int g_SelectedRow = -1;
    int g_SelectedColumn = -1;
    int g_EditingRow = -1;
    int g_EditingColumn = -1;

    enum class FieldKind
    {
        Int,
        Word,
        DWord,
        Byte,
        String,
        ByteArray,
        ArrayElement,
    };

    struct FieldDescriptor
    {
        const char* Name;
        FieldKind Kind;
        size_t Offset;
        size_t Length;
        int ArrayIndex;
    };

    const FieldDescriptor kFields[] = {
        { "Type", FieldKind::Int, offsetof(Script_Item, Type), 0, -1 },
        { "Index", FieldKind::Word, offsetof(Script_Item, Index), 0, -1 },
        { "SubIndex", FieldKind::Word, offsetof(Script_Item, SubIndex), 0, -1 },
        { "Name", FieldKind::String, offsetof(Script_Item, Name), sizeof(((Script_Item*)0)->Name), -1 },
        { "Kind1", FieldKind::Byte, offsetof(Script_Item, Kind1), 0, -1 },
        { "Kind2", FieldKind::Byte, offsetof(Script_Item, Kind2), 0, -1 },
        { "Kind3", FieldKind::Byte, offsetof(Script_Item, Kind3), 0, -1 },
        { "TwoHand", FieldKind::Byte, offsetof(Script_Item, TwoHand), 0, -1 },
        { "Level", FieldKind::Word, offsetof(Script_Item, Level), 0, -1 },
        { "m_byItemSlot", FieldKind::Byte, offsetof(Script_Item, m_byItemSlot), 0, -1 },
        { "m_wSkillIndex", FieldKind::Word, offsetof(Script_Item, m_wSkillIndex), 0, -1 },
        { "Width", FieldKind::Byte, offsetof(Script_Item, Width), 0, -1 },
        { "Height", FieldKind::Byte, offsetof(Script_Item, Height), 0, -1 },
        { "DamageMin", FieldKind::Word, offsetof(Script_Item, DamageMin), 0, -1 },
        { "DamageMax", FieldKind::Word, offsetof(Script_Item, DamageMax), 0, -1 },
        { "SuccessfulBlocking", FieldKind::Byte, offsetof(Script_Item, SuccessfulBlocking), 0, -1 },
        { "Defense", FieldKind::Word, offsetof(Script_Item, Defense), 0, -1 },
        { "MagicDefense", FieldKind::Word, offsetof(Script_Item, MagicDefense), 0, -1 },
        { "WeaponSpeed", FieldKind::Byte, offsetof(Script_Item, WeaponSpeed), 0, -1 },
        { "WalkSpeed", FieldKind::Byte, offsetof(Script_Item, WalkSpeed), 0, -1 },
        { "Durability", FieldKind::Byte, offsetof(Script_Item, Durability), 0, -1 },
        { "MagicDur", FieldKind::Byte, offsetof(Script_Item, MagicDur), 0, -1 },
        { "MagicPower", FieldKind::DWord, offsetof(Script_Item, MagicPower), 0, -1 },
        { "RequireStrength", FieldKind::Word, offsetof(Script_Item, RequireStrength), 0, -1 },
        { "RequireDexterity", FieldKind::Word, offsetof(Script_Item, RequireDexterity), 0, -1 },
        { "RequireEnergy", FieldKind::Word, offsetof(Script_Item, RequireEnergy), 0, -1 },
        { "RequireVitality", FieldKind::Word, offsetof(Script_Item, RequireVitality), 0, -1 },
        { "RequireCharisma", FieldKind::Word, offsetof(Script_Item, RequireCharisma), 0, -1 },
        { "RequireLevel", FieldKind::Word, offsetof(Script_Item, RequireLevel), 0, -1 },
        { "Value", FieldKind::Byte, offsetof(Script_Item, Value), 0, -1 },
        { "iZen", FieldKind::Int, offsetof(Script_Item, iZen), 0, -1 },
        { "AttType", FieldKind::Byte, offsetof(Script_Item, AttType), 0, -1 },
        { "RequireClass[DW]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 0 },
        { "RequireClass[DK]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 1 },
        { "RequireClass[FE]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 2 },
        { "RequireClass[MG]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 3 },
        { "RequireClass[DL]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 4 },
        { "RequireClass[SU]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 5 },
        { "RequireClass[RF]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 6 },
        { "RequireClass[GL]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 7 },
        { "RequireClass[RW]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 8 },
        { "RequireClass[SL]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 9 },
        { "RequireClass[GC]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 10 },
        { "RequireClass[KN]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 11 },
        { "RequireClass[LM]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 12 },
        { "RequireClass[IK]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 13 },
        { "RequireClass[AQ]", FieldKind::Byte, offsetof(Script_Item, RequireClass), 1, 14 },
        { "Resistance[0]", FieldKind::Byte, offsetof(Script_Item, Resistance), 1, 0 },
        { "Resistance[1]", FieldKind::Byte, offsetof(Script_Item, Resistance), 1, 1 },
        { "Resistance[2]", FieldKind::Byte, offsetof(Script_Item, Resistance), 1, 2 },
        { "Resistance[3]", FieldKind::Byte, offsetof(Script_Item, Resistance), 1, 3 },
        { "Resistance[4]", FieldKind::Byte, offsetof(Script_Item, Resistance), 1, 4 },
        { "Resistance[5]", FieldKind::Byte, offsetof(Script_Item, Resistance), 1, 5 },
        { "Resistance[6]", FieldKind::Byte, offsetof(Script_Item, Resistance), 1, 6 },
        { "Drop", FieldKind::Byte, offsetof(Script_Item, Drop), 0, -1 },
        { "Trade", FieldKind::Byte, offsetof(Script_Item, Trade), 0, -1 },
        { "StorePersonal", FieldKind::Byte, offsetof(Script_Item, StorePersonal), 0, -1 },
        { "WhareHouse", FieldKind::Byte, offsetof(Script_Item, WhareHouse), 0, -1 },
        { "SellNpc", FieldKind::Byte, offsetof(Script_Item, SellNpc), 0, -1 },
        { "Expensive", FieldKind::Byte, offsetof(Script_Item, Expensive), 0, -1 },
        { "Repair", FieldKind::Byte, offsetof(Script_Item, Repair), 0, -1 },
        { "Overlap", FieldKind::Word, offsetof(Script_Item, Overlap), 0, -1 },
        { "PcFlag", FieldKind::Word, offsetof(Script_Item, PcFlag), 0, -1 },
        { "MuunFlag", FieldKind::Word, offsetof(Script_Item, MuunFlag), 0, -1 },
        { "PowerATTK", FieldKind::DWord, offsetof(Script_Item, PowerATTK), 0, -1 },
    };

    HMENU ToMenuHandle(int controlId)
    {
        return reinterpret_cast<HMENU>(static_cast<INT_PTR>(controlId));
    }

    constexpr int IDC_SECTION_COMBO = 1201;
    constexpr int IDC_DETAILS_TABLE = 1202;
    constexpr int IDC_SEARCH_EDIT = 1203;
    constexpr int IDC_DELETE_BUTTON = 1204;
    constexpr int IDC_ADD_BUTTON = 1205;
    constexpr int IDC_DUPLICATE_BUTTON = 1206;

    LRESULT CALLBACK InlineEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void ApplyInlineEdit(HWND hWnd, bool cancel);
    void RefreshItemsTable(HWND hWnd, int preferredItemIndex = -1);
    std::string GetGridCellValue(const Script_Item& item, int column);
    bool ApplyCellValue(HWND hWnd, int row, int column, const std::string& value);
    bool DeleteSelectedRow(HWND hWnd);
    bool CopySelectedRow(HWND hWnd);
    bool PasteSelectedRow(HWND hWnd);
    bool AddItemToSelectedSection(HWND hWnd);
    bool DuplicateSelectedRow(HWND hWnd);
    void SelectAllRows(HWND hWnd);
    void UpdateStatusLabel();
    void InitializeDarkThemeResources();
    void ReleaseDarkThemeResources();
    void ApplyDarkModeToWindow(HWND hWnd, bool useWindowColor);
    void ApplyDarkThemeToControls();
    void ApplyDarkThemeToChild(HWND hWnd, const wchar_t* themeClass = nullptr);
    LRESULT HandleListViewCustomDraw(LPARAM lParam);
    void OpenExternalLink(HWND hWnd, int controlId);

    const FieldDescriptor* GetGridFieldDescriptor(int column)
    {
        if (column < 0 || column >= static_cast<int>(_countof(kFields)))
        {
            return nullptr;
        }

        return &kFields[column];
    }

    const char* GetGridColumnTitle(int column)
    {
        const FieldDescriptor* field = GetGridFieldDescriptor(column);
        if (!field)
        {
            return "";
        }

        if (strcmp(field->Name, "Index") == 0)
        {
            return "Secao";
        }

        if (strcmp(field->Name, "SubIndex") == 0)
        {
            return "Index";
        }

        if (strcmp(field->Name, "m_byItemSlot") == 0)
        {
            return "ItemSlot";
        }

        if (strcmp(field->Name, "m_wSkillIndex") == 0)
        {
            return "SkillIndex";
        }

        return field->Name;
    }

    void InitializeDarkThemeResources()
    {
        if (!g_WindowBrush)
        {
            g_WindowBrush = CreateSolidBrush(kDarkWindowColor);
        }

        if (!g_SurfaceBrush)
        {
            g_SurfaceBrush = CreateSolidBrush(kDarkSurfaceColor);
        }

        if (!g_ControlBrush)
        {
            g_ControlBrush = CreateSolidBrush(kDarkControlColor);
        }
    }

    void ReleaseDarkThemeResources()
    {
        if (g_WindowBrush)
        {
            DeleteObject(g_WindowBrush);
            g_WindowBrush = nullptr;
        }

        if (g_SurfaceBrush)
        {
            DeleteObject(g_SurfaceBrush);
            g_SurfaceBrush = nullptr;
        }

        if (g_ControlBrush)
        {
            DeleteObject(g_ControlBrush);
            g_ControlBrush = nullptr;
        }
    }

    void ApplyDarkModeToWindow(HWND hWnd, bool useWindowColor)
    {
        if (!hWnd)
        {
            return;
        }

        InitializeDarkThemeResources();

        const BOOL enableDarkMode = TRUE;
        DwmSetWindowAttribute(hWnd, 20, &enableDarkMode, sizeof(enableDarkMode));
        DwmSetWindowAttribute(hWnd, 19, &enableDarkMode, sizeof(enableDarkMode));

        const COLORREF titleBarColor = useWindowColor ? kDarkWindowColor : kDarkSurfaceColor;
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &titleBarColor, sizeof(titleBarColor));
        DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &kDarkTextColor, sizeof(kDarkTextColor));
        DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &kDarkBorderColor, sizeof(kDarkBorderColor));

        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(useWindowColor ? g_WindowBrush : g_SurfaceBrush));
        SetWindowTheme(hWnd, L"DarkMode_Explorer", nullptr);
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    void ApplyDarkThemeToChild(HWND hWnd, const wchar_t* themeClass)
    {
        if (!hWnd)
        {
            return;
        }

        SetWindowTheme(hWnd, themeClass ? themeClass : L"DarkMode_Explorer", nullptr);
        InvalidateRect(hWnd, nullptr, TRUE);
    }

    void ApplyDarkThemeToControls()
    {
        ApplyDarkThemeToChild(g_hSectionCombo, L"DarkMode_CFD");
        ApplyDarkThemeToChild(g_hSearchEdit, L"DarkMode_CFD");
        ApplyDarkThemeToChild(g_hDetailsTable, L"Explorer");

        if (g_hDetailsTable)
        {
            ListView_SetBkColor(g_hDetailsTable, kDarkSurfaceColor);
            ListView_SetTextBkColor(g_hDetailsTable, kDarkSurfaceColor);
            ListView_SetTextColor(g_hDetailsTable, kDarkTextColor);
            ListView_SetOutlineColor(g_hDetailsTable, kDarkBorderColor);
        }
    }

    LRESULT HandleListViewCustomDraw(LPARAM lParam)
    {
        LPNMLVCUSTOMDRAW customDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(lParam);
        if (!customDraw)
        {
            return CDRF_DODEFAULT;
        }

        switch (customDraw->nmcd.dwDrawStage)
        {
        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
        {
            const bool isSelected = (customDraw->nmcd.uItemState & CDIS_SELECTED) != 0;
            customDraw->clrText = isSelected ? RGB(255, 255, 255) : kDarkTextColor;
            customDraw->clrTextBk = isSelected ? kDarkAccentColor : kDarkSurfaceColor;
            return CDRF_DODEFAULT;
        }
        default:
            return CDRF_DODEFAULT;
        }
    }

    void OpenExternalLink(HWND hWnd, int controlId)
    {
        const wchar_t* url = nullptr;

        switch (controlId)
        {
        case IDC_ABOUT_DISCORD:
            url = L"https://discord.gg/maGp7u7a";
            break;
        case IDC_ABOUT_REPOSITORY:
            url = L"https://github.com/Heyvitor/VGamingMuOnline";
            break;
        default:
            return;
        }

        const HINSTANCE result = ShellExecuteW(hWnd, L"open", url, nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(result) <= 32)
        {
            MessageBoxW(hWnd, L"Nao foi possivel abrir o link no navegador padrao.", L"Erro ao abrir link", MB_OK | MB_ICONWARNING);
        }
    }

    int GetGridColumnWidth(int column)
    {
        const FieldDescriptor* field = GetGridFieldDescriptor(column);
        if (!field)
        {
            return 90;
        }

        if (field->Kind == FieldKind::String)
        {
            return 220;
        }

        if (field->Kind == FieldKind::ByteArray)
        {
            return 140;
        }

        if (strcmp(field->Name, "Type") == 0)
        {
            return 90;
        }

        if (strcmp(field->Name, "Index") == 0 || strcmp(field->Name, "SubIndex") == 0)
        {
            return 80;
        }

        if (strcmp(field->Name, "Name") == 0)
        {
            return 220;
        }

        if (strcmp(field->Name, "RequireLevel") == 0 ||
            strcmp(field->Name, "RequireStrength") == 0 ||
            strcmp(field->Name, "RequireDexterity") == 0 ||
            strcmp(field->Name, "RequireEnergy") == 0 ||
            strcmp(field->Name, "RequireVitality") == 0 ||
            strcmp(field->Name, "RequireCharisma") == 0)
        {
            return 110;
        }

        if (strncmp(field->Name, "RequireClass[", 13) == 0 || strncmp(field->Name, "Resistance[", 11) == 0)
        {
            return 95;
        }

        return 90;
    }

    bool IsGridColumnEditable(int column)
    {
        const FieldDescriptor* field = GetGridFieldDescriptor(column);
        if (!field)
        {
            return false;
        }

        return strcmp(field->Name, "Type") != 0;
    }

    std::string GetWindowTextString(HWND hWnd)
    {
        if (!hWnd)
        {
            return std::string();
        }

        const int length = GetWindowTextLengthA(hWnd);
        if (length <= 0)
        {
            return std::string();
        }

        std::string text(static_cast<size_t>(length) + 1, '\0');
        GetWindowTextA(hWnd, &text[0], length + 1);
        text.resize(static_cast<size_t>(length));
        return text;
    }

    std::string ToLowerAscii(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c)
            {
                return static_cast<char>(std::tolower(c));
            });
        return text;
    }

    bool ItemMatchesSearch(const Script_Item& item)
    {
        std::string searchText = ToLowerAscii(GetWindowTextString(g_hSearchEdit));
        if (searchText.empty())
        {
            return true;
        }

        const std::string nameText = ToLowerAscii(item.Name);
        if (nameText.find(searchText) != std::string::npos)
        {
            return true;
        }

        if (std::to_string(item.Index).find(searchText) != std::string::npos)
        {
            return true;
        }

        if (std::to_string(item.SubIndex).find(searchText) != std::string::npos)
        {
            return true;
        }

        return false;
    }

    void SortItems()
    {
        std::sort(g_Items.begin(), g_Items.end(), [](const Script_Item& a, const Script_Item& b)
            {
                if (a.Index != b.Index)
                {
                    return a.Index < b.Index;
                }

                return a.SubIndex < b.SubIndex;
            });
    }

    bool FieldAffectsVisibleRows(int column)
    {
        const FieldDescriptor* field = GetGridFieldDescriptor(column);
        if (!field)
        {
            return false;
        }

        if (strcmp(field->Name, "Index") == 0)
        {
            return true;
        }

        if (GetWindowTextLengthA(g_hSearchEdit) > 0 &&
            (strcmp(field->Name, "Name") == 0 || strcmp(field->Name, "SubIndex") == 0))
        {
            return true;
        }

        return false;
    }

    std::string GetEmptyValueForColumn(int column)
    {
        const FieldDescriptor* field = GetGridFieldDescriptor(column);
        if (!field)
        {
            return std::string();
        }

        if (field->ArrayIndex >= 0)
        {
            return "0";
        }

        switch (field->Kind)
        {
        case FieldKind::String:
            return std::string();
        case FieldKind::ByteArray:
        {
            std::ostringstream ss;
            for (size_t i = 0; i < field->Length; ++i)
            {
                if (i > 0)
                {
                    ss << ",";
                }
                ss << "0";
            }
            return ss.str();
        }
        default:
            return "0";
        }
    }

    void UpdateVisibleRow(int row)
    {
        if (row < 0 || row >= static_cast<int>(g_FilteredIndices.size()))
        {
            return;
        }

        const Script_Item& item = g_Items[g_FilteredIndices[row]];
        for (int column = 0; column < static_cast<int>(_countof(kFields)); ++column)
        {
            std::string value = GetGridCellValue(item, column);
            LVITEMA itemData = { 0 };
            itemData.iSubItem = column;
            itemData.pszText = const_cast<char*>(value.c_str());
            SendMessageA(g_hDetailsTable, LVM_SETITEMTEXTA, static_cast<WPARAM>(row), reinterpret_cast<LPARAM>(&itemData));
        }
    }

    bool CopyTextToClipboard(HWND hWnd, const std::string& text)
    {
        if (!OpenClipboard(hWnd))
        {
            return false;
        }

        EmptyClipboard();

        HGLOBAL hMemory = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (!hMemory)
        {
            CloseClipboard();
            return false;
        }

        char* memory = static_cast<char*>(GlobalLock(hMemory));
        if (!memory)
        {
            GlobalFree(hMemory);
            CloseClipboard();
            return false;
        }

        memcpy(memory, text.c_str(), text.size() + 1);
        GlobalUnlock(hMemory);

        if (!SetClipboardData(CF_TEXT, hMemory))
        {
            GlobalFree(hMemory);
            CloseClipboard();
            return false;
        }

        CloseClipboard();
        return true;
    }

    bool ReadTextFromClipboard(HWND hWnd, std::string& outText)
    {
        outText.clear();

        if (!OpenClipboard(hWnd))
        {
            return false;
        }

        HANDLE hData = GetClipboardData(CF_TEXT);
        if (!hData)
        {
            CloseClipboard();
            return false;
        }

        const char* text = static_cast<const char*>(GlobalLock(hData));
        if (!text)
        {
            CloseClipboard();
            return false;
        }

        outText = text;
        GlobalUnlock(hData);
        CloseClipboard();
        return true;
    }

    std::string BuildRowClipboardText(int row)
    {
        if (row < 0 || row >= static_cast<int>(g_FilteredIndices.size()))
        {
            return std::string();
        }

        const Script_Item& item = g_Items[g_FilteredIndices[row]];
        std::ostringstream ss;
        for (int column = 0; column < static_cast<int>(_countof(kFields)); ++column)
        {
            if (column > 0)
            {
                ss << "\t";
            }

            ss << GetGridCellValue(item, column);
        }

        return ss.str();
    }

    std::vector<std::string> SplitClipboardRowText(const std::string& text)
    {
        std::vector<std::string> values;
        std::stringstream ss(text);
        std::string token;

        while (std::getline(ss, token, '\t'))
        {
            values.push_back(token);
        }

        if (!values.empty())
        {
            while (!values.back().empty() &&
                (values.back().back() == '\r' || values.back().back() == '\n'))
            {
                values.back().pop_back();
            }
        }

        return values;
    }

    void LayoutControls(HWND hWnd)
    {
        RECT clientRect = { 0 };
        GetClientRect(hWnd, &clientRect);

        const int margin = 10;
        const int top = 10;
        const int labelWidth = 60;
        const int comboWidth = 190;
        const int searchLabelWidth = 45;
        const int searchWidth = 240;
        const int buttonWidth = 100;
        const int rowHeight = 24;
        const int tableTop = 40;
        const int statusHeight = 22;

        if (g_hSectionLabel)
        {
            MoveWindow(g_hSectionLabel, margin, top + 2, labelWidth, rowHeight, TRUE);
        }

        if (g_hSectionCombo)
        {
            MoveWindow(g_hSectionCombo, margin + labelWidth, top, comboWidth, 400, TRUE);
        }

        if (g_hSearchLabel)
        {
            MoveWindow(g_hSearchLabel, margin + labelWidth + comboWidth + 15, top + 2, searchLabelWidth, rowHeight, TRUE);
        }

        if (g_hSearchEdit)
        {
            MoveWindow(g_hSearchEdit, margin + labelWidth + comboWidth + 15 + searchLabelWidth, top, searchWidth, rowHeight, TRUE);
        }

        if (g_hDeleteButton)
        {
            MoveWindow(g_hDeleteButton, margin + labelWidth + comboWidth + 15 + searchLabelWidth + searchWidth + 15, top, buttonWidth, rowHeight, TRUE);
        }

        if (g_hAddButton)
        {
            MoveWindow(g_hAddButton, margin + labelWidth + comboWidth + 15 + searchLabelWidth + searchWidth + 15 + buttonWidth + 10, top, buttonWidth, rowHeight, TRUE);
        }

        if (g_hDuplicateButton)
        {
            MoveWindow(g_hDuplicateButton, margin + labelWidth + comboWidth + 15 + searchLabelWidth + searchWidth + 15 + (buttonWidth + 10) * 2, top, buttonWidth, rowHeight, TRUE);
        }

        if (g_hDetailsTable)
        {
            MoveWindow(g_hDetailsTable,
                margin,
                tableTop,
                clientRect.right - (margin * 2),
                clientRect.bottom - tableTop - statusHeight - margin,
                TRUE);
        }

        if (g_hStatusLabel)
        {
            MoveWindow(g_hStatusLabel,
                margin,
                clientRect.bottom - statusHeight - 4,
                clientRect.right - (margin * 2),
                statusHeight,
                TRUE);
        }
    }

    bool TryParseInt(const std::string& text, long& value)
    {
        char* end = nullptr;
        value = strtol(text.c_str(), &end, 10);
        return end != nullptr && *end == '\0';
    }

    std::string GetFieldValue(const Script_Item& item, const FieldDescriptor& field)
    {
        const BYTE* base = reinterpret_cast<const BYTE*>(&item) + field.Offset;

        if (field.ArrayIndex >= 0)
        {
            return std::to_string(static_cast<int>(base[field.ArrayIndex]));
        }

        switch (field.Kind)
        {
        case FieldKind::Int:
            return std::to_string(*reinterpret_cast<const int*>(base));
        case FieldKind::Word:
            return std::to_string(*reinterpret_cast<const WORD*>(base));
        case FieldKind::DWord:
            return std::to_string(*reinterpret_cast<const DWORD*>(base));
        case FieldKind::Byte:
            return std::to_string(*reinterpret_cast<const BYTE*>(base));
        case FieldKind::String:
            return std::string(reinterpret_cast<const char*>(base));
        case FieldKind::ByteArray:
        {
            std::ostringstream ss;
            for (size_t i = 0; i < field.Length; ++i)
            {
                if (i > 0)
                {
                    ss << ",";
                }
                ss << static_cast<int>(base[i]);
            }
            return ss.str();
        }
        }

        return std::string();
    }

    bool SetFieldValue(Script_Item& item, const FieldDescriptor& field, const std::string& value)
    {
        BYTE* base = reinterpret_cast<BYTE*>(&item) + field.Offset;

        if (field.ArrayIndex >= 0)
        {
            long parsed = 0;
            if (!TryParseInt(value, parsed) || parsed < 0 || parsed > 255)
            {
                return false;
            }
            base[field.ArrayIndex] = static_cast<BYTE>(parsed);
            return true;
        }

        switch (field.Kind)
        {
        case FieldKind::String:
            strcpy_s(reinterpret_cast<char*>(base), field.Length, value.c_str());
            return true;
        case FieldKind::Int:
        {
            long parsed = 0;
            if (!TryParseInt(value, parsed))
            {
                return false;
            }
            *reinterpret_cast<int*>(base) = static_cast<int>(parsed);
            return true;
        }
        case FieldKind::Word:
        {
            long parsed = 0;
            if (!TryParseInt(value, parsed) || parsed < 0 || parsed > 65535)
            {
                return false;
            }
            *reinterpret_cast<WORD*>(base) = static_cast<WORD>(parsed);
            return true;
        }
        case FieldKind::DWord:
        {
            long parsed = 0;
            if (!TryParseInt(value, parsed) || parsed < 0)
            {
                return false;
            }
            *reinterpret_cast<DWORD*>(base) = static_cast<DWORD>(parsed);
            return true;
        }
        case FieldKind::Byte:
        {
            long parsed = 0;
            if (!TryParseInt(value, parsed) || parsed < 0 || parsed > 255)
            {
                return false;
            }
            *reinterpret_cast<BYTE*>(base) = static_cast<BYTE>(parsed);
            return true;
        }
        case FieldKind::ByteArray:
        {
            std::vector<int> values;
            std::string token;
            std::stringstream ss(value);
            while (std::getline(ss, token, ','))
            {
                if (token.empty())
                {
                    return false;
                }

                long parsed = 0;
                if (!TryParseInt(token, parsed) || parsed < 0 || parsed > 255)
                {
                    return false;
                }

                values.push_back(static_cast<int>(parsed));
            }

            if (values.size() != field.Length)
            {
                return false;
            }

            for (size_t i = 0; i < values.size(); ++i)
            {
                base[i] = static_cast<BYTE>(values[i]);
            }
            return true;
        }
        }

        return false;
    }

    std::string GetGridCellValue(const Script_Item& item, int column)
    {
        const FieldDescriptor* field = GetGridFieldDescriptor(column);
        if (!field)
        {
            return std::string();
        }

        return GetFieldValue(item, *field);
    }

    bool SetGridCellValue(Script_Item& item, int column, const std::string& value)
    {
        if (column < 0 || column >= static_cast<int>(_countof(kFields)) || !IsGridColumnEditable(column))
        {
            return false;
        }

        const FieldDescriptor* field = GetGridFieldDescriptor(column);
        if (!field)
        {
            return false;
        }

        return SetFieldValue(item, *field, value);
    }

    bool ApplyCellValue(HWND hWnd, int row, int column, const std::string& value)
    {
        if (row < 0 || row >= static_cast<int>(g_FilteredIndices.size()))
        {
            return false;
        }

        if (column < 0 || column >= static_cast<int>(_countof(kFields)) || !IsGridColumnEditable(column))
        {
            return false;
        }

        const int itemIndex = g_FilteredIndices[row];
        if (!SetGridCellValue(g_Items[itemIndex], column, value))
        {
            MessageBoxA(hWnd, "Valor invalido para o campo selecionado.", "Erro de validacao", MB_OK | MB_ICONWARNING);
            return false;
        }

        if (strcmp(kFields[column].Name, "Index") == 0 || strcmp(kFields[column].Name, "SubIndex") == 0)
        {
            g_Items[itemIndex].Type = GET_ITEM(g_Items[itemIndex].Index, g_Items[itemIndex].SubIndex);
        }

        g_Modified = true;

        if (FieldAffectsVisibleRows(column))
        {
            RefreshItemsTable(hWnd, itemIndex);
        }
        else
        {
            UpdateVisibleRow(row);
            g_SelectedRow = row;
            g_SelectedColumn = column;
        }

        return true;
    }

    bool DeleteSelectedRow(HWND hWnd)
    {
        if (!g_hDetailsTable)
        {
            return false;
        }

        if (g_hInlineEdit)
        {
            ApplyInlineEdit(hWnd, false);
        }

        std::vector<int> selectedIndices;
        int selectedRow = -1;
        
        while ((selectedRow = static_cast<int>(SendMessageA(g_hDetailsTable, LVM_GETNEXTITEM, selectedRow, LVNI_SELECTED))) != -1)
        {
            if (selectedRow >= 0 && selectedRow < static_cast<int>(g_FilteredIndices.size()))
            {
                const int itemIndex = g_FilteredIndices[selectedRow];
                if (itemIndex >= 0 && itemIndex < static_cast<int>(g_Items.size()))
                {
                    selectedIndices.push_back(itemIndex);
                }
            }
        }

        if (selectedIndices.empty())
        {
            return false;
        }

        std::sort(selectedIndices.begin(), selectedIndices.end(), [](int a, int b) { return a > b; });

        for (int itemIndex : selectedIndices)
        {
            if (itemIndex >= 0 && itemIndex < static_cast<int>(g_Items.size()))
            {
                g_Items.erase(g_Items.begin() + itemIndex);
            }
        }

        g_Modified = true;

        int preferredItemIndex = -1;
        if (!g_Items.empty() && !selectedIndices.empty())
        {
            const int firstDeletedIndex = selectedIndices.back();
            const int lastItemIndex = static_cast<int>(g_Items.size()) - 1;
            preferredItemIndex = (firstDeletedIndex < lastItemIndex) ? firstDeletedIndex : lastItemIndex;
        }

        RefreshItemsTable(hWnd, preferredItemIndex);
        return true;
    }

    bool CopySelectedRow(HWND hWnd)
    {
        if (g_SelectedRow < 0 || g_SelectedRow >= static_cast<int>(g_FilteredIndices.size()))
        {
            return false;
        }

        return CopyTextToClipboard(hWnd, BuildRowClipboardText(g_SelectedRow));
    }

    bool PasteSelectedRow(HWND hWnd)
    {
        if (g_SelectedRow < 0 || g_SelectedRow >= static_cast<int>(g_FilteredIndices.size()))
        {
            return false;
        }

        std::string clipboardText;
        if (!ReadTextFromClipboard(hWnd, clipboardText))
        {
            return false;
        }

        std::vector<std::string> values = SplitClipboardRowText(clipboardText);
        if (values.empty())
        {
            return false;
        }

        const bool hasTypeColumn = values.size() == static_cast<int>(_countof(kFields));
        const int startColumn = hasTypeColumn ? 1 : 0;
        const int valueOffset = hasTypeColumn ? 1 : 0;
        const int requiredValues = static_cast<int>(_countof(kFields)) - 1;

        if (static_cast<int>(values.size()) - valueOffset < requiredValues)
        {
            MessageBoxA(hWnd, "Conteudo da area de transferencia invalido para colar a linha.", "Erro de colagem", MB_OK | MB_ICONWARNING);
            return false;
        }

        const int itemIndex = g_FilteredIndices[g_SelectedRow];
        Script_Item updatedItem = g_Items[itemIndex];

        for (int column = startColumn; column < static_cast<int>(_countof(kFields)); ++column)
        {
            if (!IsGridColumnEditable(column))
            {
                continue;
            }

            if (!SetGridCellValue(updatedItem, column, values[column - startColumn + valueOffset]))
            {
                MessageBoxA(hWnd, "Conteudo da area de transferencia invalido para colar a linha.", "Erro de colagem", MB_OK | MB_ICONWARNING);
                return false;
            }
        }

        updatedItem.Type = GET_ITEM(updatedItem.Index, updatedItem.SubIndex);
        g_Items[itemIndex] = updatedItem;
        g_Modified = true;

        if (FieldAffectsVisibleRows(1) || (GetWindowTextLengthA(g_hSearchEdit) > 0))
        {
            RefreshItemsTable(hWnd, itemIndex);
        }
        else
        {
            UpdateVisibleRow(g_SelectedRow);
        }

        return true;
    }

    bool AddItemToSelectedSection(HWND hWnd)
    {
        const int selectedSection = static_cast<int>(SendMessageA(g_hSectionCombo, CB_GETCURSEL, 0, 0)) - 1;
        if (selectedSection < 0)
        {
            MessageBoxA(hWnd, "Selecione uma section especifica para adicionar um item.", "Section obrigatoria", MB_OK | MB_ICONWARNING);
            return false;
        }

        int nextSubIndex = 0;
        for (int i = 0; i < static_cast<int>(g_Items.size()); ++i)
        {
            const Script_Item& item = g_Items[i];
            if (item.Index == selectedSection && item.SubIndex >= nextSubIndex)
            {
                nextSubIndex = item.SubIndex + 1;
            }
        }

        if (nextSubIndex >= MAX_ITEM_TYPE)
        {
            MessageBoxA(hWnd, "A section selecionada ja atingiu o limite maximo de items.", "Limite atingido", MB_OK | MB_ICONWARNING);
            return false;
        }

        Script_Item newItem = {};
        newItem.Index = static_cast<short>(selectedSection);
        newItem.SubIndex = static_cast<short>(nextSubIndex);
        newItem.Type = GET_ITEM(newItem.Index, newItem.SubIndex);
        strcpy_s(newItem.Name, "New Item");
        newItem.m_byItemSlot = 0xFF;

        g_Items.push_back(newItem);
        SortItems();
        g_Modified = true;

        int newItemIndex = -1;
        for (int i = 0; i < static_cast<int>(g_Items.size()); ++i)
        {
            if (g_Items[i].Index == selectedSection && g_Items[i].SubIndex == nextSubIndex)
            {
                newItemIndex = i;
                break;
            }
        }

        RefreshItemsTable(hWnd, newItemIndex);
        return true;
    }

    bool DuplicateSelectedRow(HWND hWnd)
    {
        if (g_SelectedRow < 0 || g_SelectedRow >= static_cast<int>(g_FilteredIndices.size()))
        {
            MessageBoxA(hWnd, "Selecione uma linha para duplicar.", "Linha nao selecionada", MB_OK | MB_ICONWARNING);
            return false;
        }

        if (g_hInlineEdit)
        {
            ApplyInlineEdit(hWnd, false);
        }

        const int itemIndex = g_FilteredIndices[g_SelectedRow];
        if (itemIndex < 0 || itemIndex >= static_cast<int>(g_Items.size()))
        {
            return false;
        }

        const Script_Item& sourceItem = g_Items[itemIndex];
        const int sourceSection = sourceItem.Index;

        int nextSubIndex = 0;
        for (int i = 0; i < static_cast<int>(g_Items.size()); ++i)
        {
            const Script_Item& item = g_Items[i];
            if (item.Index == sourceSection && item.SubIndex >= nextSubIndex)
            {
                nextSubIndex = item.SubIndex + 1;
            }
        }

        if (nextSubIndex >= MAX_ITEM_TYPE)
        {
            MessageBoxA(hWnd, "A section desta linha ja atingiu o limite maximo de items.", "Limite atingido", MB_OK | MB_ICONWARNING);
            return false;
        }

        Script_Item duplicatedItem = sourceItem;
        duplicatedItem.SubIndex = static_cast<short>(nextSubIndex);
        duplicatedItem.Type = GET_ITEM(duplicatedItem.Index, duplicatedItem.SubIndex);

        char newName[64] = { 0 };
        sprintf_s(newName, _countof(newName), "%.56s (Copy)", sourceItem.Name);
        strcpy_s(duplicatedItem.Name, newName);

        g_Items.push_back(duplicatedItem);
        SortItems();
        g_Modified = true;

        int newItemIndex = -1;
        for (int i = 0; i < static_cast<int>(g_Items.size()); ++i)
        {
            if (g_Items[i].Index == sourceSection && g_Items[i].SubIndex == nextSubIndex)
            {
                newItemIndex = i;
                break;
            }
        }

        RefreshItemsTable(hWnd, newItemIndex);
        return true;
    }

    void SelectAllRows(HWND hWnd)
    {
        if (!g_hDetailsTable)
        {
            return;
        }

        if (g_hInlineEdit)
        {
            ApplyInlineEdit(hWnd, false);
        }

        const int itemCount = static_cast<int>(SendMessageA(g_hDetailsTable, LVM_GETITEMCOUNT, 0, 0));
        
        if (itemCount <= 0)
        {
            return;
        }

        SendMessageA(g_hDetailsTable, LVM_SETITEMSTATE, -1, 0);

        for (int i = 0; i < itemCount; ++i)
        {
            LVITEMA itemState = { 0 };
            itemState.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
            itemState.state = LVIS_SELECTED;
            SendMessageA(g_hDetailsTable, LVM_SETITEMSTATE, static_cast<WPARAM>(i), reinterpret_cast<LPARAM>(&itemState));
        }

        if (itemCount > 0)
        {
            LVITEMA focusState = { 0 };
            focusState.stateMask = LVIS_FOCUSED;
            focusState.state = LVIS_FOCUSED;
            SendMessageA(g_hDetailsTable, LVM_SETITEMSTATE, 0, reinterpret_cast<LPARAM>(&focusState));
            g_SelectedRow = 0;
        }
    }

    void UpdateStatusLabel()
    {
        if (!g_hStatusLabel)
        {
            return;
        }

        int sectionCounts[MAX_ITEM_SECTION] = { 0 };
        for (int i = 0; i < static_cast<int>(g_Items.size()); ++i)
        {
            const int section = g_Items[i].Index;
            if (section >= 0 && section < MAX_ITEM_SECTION)
            {
                ++sectionCounts[section];
            }
        }

        std::ostringstream status;
        status << "Linhas por secao";
        for (int section = 0; section < MAX_ITEM_SECTION; ++section)
        {
            status << " | S" << section << ": " << sectionCounts[section];
        }

        const std::string text = status.str();
        SetWindowTextA(g_hStatusLabel, text.c_str());
    }

    void RefreshItemsTable(HWND hWnd, int preferredItemIndex)
    {
        UNREFERENCED_PARAMETER(hWnd);

        if (!g_hDetailsTable)
        {
            return;
        }

        const int selectedSection = static_cast<int>(SendMessageA(g_hSectionCombo, CB_GETCURSEL, 0, 0)) - 1;
        SendMessageA(g_hDetailsTable, LVM_DELETEALLITEMS, 0, 0);
        g_FilteredIndices.clear();

        int preferredRow = -1;

        for (int i = 0; i < static_cast<int>(g_Items.size()); ++i)
        {
            const Script_Item& item = g_Items[i];
            if (selectedSection >= 0 && item.Index != selectedSection)
            {
                continue;
            }

            if (!ItemMatchesSearch(item))
            {
                continue;
            }

            const int row = static_cast<int>(g_FilteredIndices.size());
            g_FilteredIndices.push_back(i);

            LVITEMA rowData = { 0 };
            rowData.mask = LVIF_TEXT;
            rowData.iItem = row;
            std::string firstValue = GetGridCellValue(item, 0);
            rowData.pszText = const_cast<char*>(firstValue.c_str());
            SendMessageA(g_hDetailsTable, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&rowData));

            for (int column = 1; column < static_cast<int>(_countof(kFields)); ++column)
            {
                std::string value = GetGridCellValue(item, column);

                LVITEMA itemData = { 0 };
                itemData.mask = LVIF_TEXT;
                itemData.iItem = row;
                itemData.iSubItem = column;
                itemData.pszText = const_cast<char*>(value.c_str());
                SendMessageA(g_hDetailsTable, LVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&itemData));
            }

            if (i == preferredItemIndex)
            {
                preferredRow = row;
            }
        }

        if (preferredRow < 0 && !g_FilteredIndices.empty())
        {
            preferredRow = 0;
        }

        if (preferredRow >= 0)
        {
            LVITEMA selectedItem = { 0 };
            selectedItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
            selectedItem.state = LVIS_SELECTED | LVIS_FOCUSED;
            SendMessageA(g_hDetailsTable, LVM_SETITEMSTATE,
                static_cast<WPARAM>(preferredRow),
                reinterpret_cast<LPARAM>(&selectedItem));
            SendMessageA(g_hDetailsTable, LVM_ENSUREVISIBLE, static_cast<WPARAM>(preferredRow), FALSE);
        }

        g_SelectedRow = preferredRow;
        if (preferredRow < 0)
        {
            g_SelectedColumn = -1;
        }

        UpdateStatusLabel();
    }

    void BeginInlineEdit(int row, int column)
    {
        if (row < 0 || row >= static_cast<int>(g_FilteredIndices.size()))
        {
            return;
        }

        if (column < 0 || column >= static_cast<int>(_countof(kFields)) || !IsGridColumnEditable(column))
        {
            return;
        }

        if (g_hInlineEdit)
        {
            if (g_EditingRow == row && g_EditingColumn == column)
            {
                return;
            }

            ApplyInlineEdit(g_MainWindow, false);
        }

        RECT cellRect = { 0 };
        cellRect.top = column;
        cellRect.left = LVIR_BOUNDS;
        if (!SendMessageA(g_hDetailsTable, LVM_GETSUBITEMRECT, static_cast<WPARAM>(row), reinterpret_cast<LPARAM>(&cellRect)))
        {
            return;
        }

        const Script_Item& item = g_Items[g_FilteredIndices[row]];
        std::string currentValue = GetGridCellValue(item, column);

        g_EditingRow = row;
        g_EditingColumn = column;
        g_hInlineEdit = CreateWindowExA(0, "EDIT", currentValue.c_str(),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            cellRect.left + 1, cellRect.top + 1,
            cellRect.right - cellRect.left - 2,
            cellRect.bottom - cellRect.top - 2,
            g_hDetailsTable, nullptr, hInst, nullptr);

        SendMessage(g_hInlineEdit, WM_SETFONT, SendMessage(g_hDetailsTable, WM_GETFONT, 0, 0), TRUE);
        g_InlineEditProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(g_hInlineEdit, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(InlineEditProc)));
        SetFocus(g_hInlineEdit);
        SendMessageA(g_hInlineEdit, EM_SETSEL, 0, -1);
    }

    void ApplyInlineEdit(HWND hWnd, bool cancel)
    {
        if (!g_hInlineEdit)
        {
            return;
        }

        bool refreshTable = false;
        int editedItemIndex = -1;
        int editedRow = g_EditingRow;

        if (!cancel &&
            g_EditingRow >= 0 &&
            g_EditingRow < static_cast<int>(g_FilteredIndices.size()) &&
            g_EditingColumn >= 0 &&
            g_EditingColumn < static_cast<int>(_countof(kFields)))
        {
            char buffer[512];
            GetWindowTextA(g_hInlineEdit, buffer, static_cast<int>(sizeof(buffer)));

            editedItemIndex = g_FilteredIndices[g_EditingRow];
            refreshTable = FieldAffectsVisibleRows(g_EditingColumn);
            ApplyCellValue(hWnd, g_EditingRow, g_EditingColumn, buffer);
        }

        SetWindowLongPtrA(g_hInlineEdit, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_InlineEditProc));
        DestroyWindow(g_hInlineEdit);
        g_hInlineEdit = nullptr;
        g_InlineEditProc = nullptr;
        g_EditingRow = -1;
        g_EditingColumn = -1;

        UNREFERENCED_PARAMETER(refreshTable);
        UNREFERENCED_PARAMETER(editedItemIndex);
        UNREFERENCED_PARAMETER(editedRow);
    }

    LRESULT CALLBACK InlineEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_KEYDOWN)
        {
            if (wParam == VK_RETURN)
            {
                ApplyInlineEdit(g_MainWindow, false);
                return 0;
            }
            if (wParam == VK_ESCAPE)
            {
                ApplyInlineEdit(g_MainWindow, true);
                return 0;
            }
        }
        else if (message == WM_KILLFOCUS)
        {
            ApplyInlineEdit(g_MainWindow, false);
            return 0;
        }

        return CallWindowProcA(g_InlineEditProc, hWnd, message, wParam, lParam);
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    INITCOMMONCONTROLSEX commonControls = { 0 };
    commonControls.dwSize = sizeof(commonControls);
    commonControls.dwICC = ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&commonControls);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EDITORBMD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDITORBMD));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDITORBMD));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    InitializeDarkThemeResources();
    wcex.hbrBackground = g_WindowBrush;
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_EDITORBMD);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    g_MainWindow = nullptr;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 1200, 780, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    g_MainWindow = hWnd;

    g_hSectionLabel = CreateWindowExA(0, "STATIC", "SECAO:",
        WS_CHILD | WS_VISIBLE, 10, 12, 60, 24, hWnd, nullptr, hInstance, nullptr);

    g_hSectionCombo = CreateWindowExA(0, "COMBOBOX", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        70, 10, 190, 600, hWnd, ToMenuHandle(IDC_SECTION_COMBO), hInstance, nullptr);

    g_hSearchLabel = CreateWindowExA(0, "STATIC", "BUSCA:",
        WS_CHILD | WS_VISIBLE, 275, 12, 45, 24, hWnd, nullptr, hInstance, nullptr);

    g_hSearchEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        325, 10, 240, 24, hWnd, ToMenuHandle(IDC_SEARCH_EDIT), hInstance, nullptr);

    g_hDeleteButton = CreateWindowExA(0, "BUTTON", "Apagar Linha",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        580, 10, 100, 24, hWnd, ToMenuHandle(IDC_DELETE_BUTTON), hInstance, nullptr);

    g_hAddButton = CreateWindowExA(0, "BUTTON", "Adicionar Item",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        690, 10, 100, 24, hWnd, ToMenuHandle(IDC_ADD_BUTTON), hInstance, nullptr);

    g_hDuplicateButton = CreateWindowExA(0, "BUTTON", "Duplicar Item",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        800, 10, 100, 24, hWnd, ToMenuHandle(IDC_DUPLICATE_BUTTON), hInstance, nullptr);

    g_hDetailsTable = CreateWindowExA(WS_EX_CLIENTEDGE, WC_LISTVIEWA, "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS,
        10, 40, 1160, 670, hWnd, ToMenuHandle(IDC_DETAILS_TABLE), hInstance, nullptr);

    g_hStatusLabel = CreateWindowExA(0, "STATIC", "",
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
        10, 742, 1160, 20, hWnd, nullptr, hInstance, nullptr);

    SendMessageA(g_hDetailsTable, LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    for (int column = 0; column < static_cast<int>(_countof(kFields)); ++column)
    {
        LVCOLUMNA columnData = { 0 };
        columnData.mask = LVCF_TEXT | LVCF_WIDTH;
        columnData.cx = GetGridColumnWidth(column);
        columnData.pszText = const_cast<char*>(GetGridColumnTitle(column));
        SendMessageA(g_hDetailsTable, LVM_INSERTCOLUMNA, column, reinterpret_cast<LPARAM>(&columnData));
    }

    SendMessageA(g_hSectionCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Todas as secoes"));
    for (int section = 0; section < MAX_ITEM_SECTION; ++section)
    {
        char text[64];
        sprintf_s(text, "Secao %d", section);
        SendMessageA(g_hSectionCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text));
    }
    SendMessageA(g_hSectionCombo, CB_SETCURSEL, 0, 0);

    HFONT hFont = reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    SendMessage(g_hSectionLabel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hSectionCombo, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hSearchLabel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hSearchEdit, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hDeleteButton, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hAddButton, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hDuplicateButton, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hDetailsTable, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
    SendMessage(g_hStatusLabel, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

    ApplyDarkModeToWindow(hWnd, true);
    ApplyDarkThemeToControls();
    LayoutControls(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void OpenBmdFile(HWND hWnd)
{
    OPENFILENAMEA ofn = { 0 };
    char filePath[MAX_PATH] = { 0 };

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "BMD Files (*.bmd)\0*.bmd\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = "Abrir Item.bmd";

    if (GetOpenFileNameA(&ofn) == FALSE)
    {
        return;
    }

    std::string error;
    if (BmdDecrypt::LoadBmd(filePath, g_Items, g_CurrentItemLayout, error) == false)
    {
        MessageBoxA(hWnd, error.c_str(), "Erro ao abrir BMD", MB_ICONERROR | MB_OK);
        return;
    }

    strcpy_s(g_CurrentFilePath, filePath);
    g_Modified = false;

    RefreshItemsTable(hWnd);

    char titleText[MAX_PATH + 50];
    sprintf_s(titleText, "Editor BMD - %s", filePath);
    SetWindowTextA(hWnd, titleText);

    MessageBoxA(hWnd, "Arquivo BMD carregado com sucesso!", "Sucesso", MB_ICONINFORMATION | MB_OK);
}

void SaveBmdFile(HWND hWnd)
{
    if (g_CurrentFilePath[0] == '\0')
    {
        SaveAsBmdFile(hWnd);
        return;
    }

    std::string error;
    if (BmdDecrypt::SaveBmd(g_CurrentFilePath, g_Items, g_CurrentItemLayout, error) == false)
    {
        MessageBoxA(hWnd, error.c_str(), "Erro ao salvar BMD", MB_ICONERROR | MB_OK);
        return;
    }

    g_Modified = false;
    MessageBoxA(hWnd, "Arquivo BMD salvo com sucesso!", "Sucesso", MB_ICONINFORMATION | MB_OK);
}

void SaveAsBmdFile(HWND hWnd)
{
    OPENFILENAMEA ofn = { 0 };
    char filePath[MAX_PATH] = { 0 };

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "BMD Files (*.bmd)\0*.bmd\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrTitle = "Salvar Item.bmd";

    if (GetSaveFileNameA(&ofn) == FALSE)
    {
        return;
    }

    std::string error;
    if (BmdDecrypt::SaveBmd(filePath, g_Items, g_CurrentItemLayout, error) == false)
    {
        MessageBoxA(hWnd, error.c_str(), "Erro ao salvar BMD", MB_ICONERROR | MB_OK);
        return;
    }

    strcpy_s(g_CurrentFilePath, filePath);
    g_Modified = false;

    char titleText[MAX_PATH + 50];
    sprintf_s(titleText, "Editor BMD - %s", filePath);
    SetWindowTextA(hWnd, titleText);

    MessageBoxA(hWnd, "Arquivo BMD salvo com sucesso!", "Sucesso", MB_ICONINFORMATION | MB_OK);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        if (wmId == IDC_SECTION_COMBO && wmEvent == CBN_SELCHANGE)
        {
            RefreshItemsTable(hWnd);
            return 0;
        }

        if (wmId == IDC_SEARCH_EDIT && wmEvent == EN_CHANGE)
        {
            RefreshItemsTable(hWnd);
            return 0;
        }

        switch (wmId)
        {
        case IDC_DELETE_BUTTON:
            DeleteSelectedRow(hWnd);
            break;
        case IDC_ADD_BUTTON:
            AddItemToSelectedSection(hWnd);
            break;
        case IDC_DUPLICATE_BUTTON:
            DuplicateSelectedRow(hWnd);
            break;
        case IDM_OPEN:
            OpenBmdFile(hWnd);
            break;
        case IDM_SAVE:
            SaveBmdFile(hWnd);
            break;
        case IDM_SAVEAS:
            SaveAsBmdFile(hWnd);
            break;
        case IDM_ABOUT:
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About) == -1)
            {
                MessageBoxA(hWnd, "Nao foi possivel abrir a tela Ajuda/Sobre.", "Erro ao abrir ajuda", MB_OK | MB_ICONERROR);
            }
            break;
        case IDM_EXIT:
            if (g_Modified)
            {
                int result = MessageBoxA(hWnd, "Deseja salvar as alteracoes antes de sair?", "Confirmar Saida", MB_YESNOCANCEL | MB_ICONQUESTION);
                if (result == IDYES)
                {
                    SaveBmdFile(hWnd);
                }
                else if (result == IDCANCEL)
                {
                    return 0;
                }
            }
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
        LayoutControls(hWnd);
        break;
    case WM_NOTIFY:
    {
        LPNMHDR notification = reinterpret_cast<LPNMHDR>(lParam);
        if (notification->idFrom == IDC_DETAILS_TABLE && notification->code == NM_CUSTOMDRAW)
        {
            return HandleListViewCustomDraw(lParam);
        }

        if (notification->idFrom == IDC_DETAILS_TABLE && notification->code == NM_CLICK)
        {
            LPNMITEMACTIVATE itemActivate = reinterpret_cast<LPNMITEMACTIVATE>(lParam);
            if (itemActivate->iItem >= 0 && itemActivate->iSubItem >= 0)
            {
                g_SelectedRow = itemActivate->iItem;
                g_SelectedColumn = itemActivate->iSubItem;
                BeginInlineEdit(itemActivate->iItem, itemActivate->iSubItem);
            }
            return 0;
        }

        if (notification->idFrom == IDC_DETAILS_TABLE && notification->code == LVN_KEYDOWN)
        {
            NMLVKEYDOWN* keyData = reinterpret_cast<NMLVKEYDOWN*>(lParam);
            const bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

            if (keyData->wVKey == VK_DELETE)
            {
                DeleteSelectedRow(hWnd);
                return 0;
            }

            if (ctrlPressed && keyData->wVKey == 'C')
            {
                if (g_hInlineEdit)
                {
                    return 0;
                }

                CopySelectedRow(hWnd);
                return 0;
            }

            if (ctrlPressed && keyData->wVKey == 'V')
            {
                if (g_hInlineEdit)
                {
                    return 0;
                }

                PasteSelectedRow(hWnd);
                return 0;
            }

            if (ctrlPressed && keyData->wVKey == 'A')
            {
                if (g_hInlineEdit)
                {
                    return 0;
                }

                SelectAllRows(hWnd);
                return 0;
            }
        }
    }
    break;
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        HWND hControl = reinterpret_cast<HWND>(lParam);
        const bool isStatusLabel = hControl == g_hStatusLabel;
        const bool isHeaderLabel = hControl == g_hSectionLabel || hControl == g_hSearchLabel;

        SetTextColor(hdc, (isStatusLabel || isHeaderLabel) ? kDarkMutedTextColor : kDarkTextColor);

        if (isStatusLabel || isHeaderLabel)
        {
            SetBkColor(hdc, kDarkWindowColor);
            return reinterpret_cast<INT_PTR>(g_WindowBrush);
        }

        SetBkColor(hdc, kDarkSurfaceColor);
        return reinterpret_cast<INT_PTR>(g_SurfaceBrush);
    }
    case WM_CTLCOLOREDIT:
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, kDarkTextColor);
        SetBkColor(hdc, kDarkControlColor);
        return reinterpret_cast<INT_PTR>(g_ControlBrush);
    }
    case WM_CTLCOLORLISTBOX:
    {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetTextColor(hdc, kDarkTextColor);
        SetBkColor(hdc, kDarkControlColor);
        return reinterpret_cast<INT_PTR>(g_ControlBrush);
    }
    case WM_ERASEBKGND:
    {
        RECT clientRect = { 0 };
        GetClientRect(hWnd, &clientRect);
        FillRect(reinterpret_cast<HDC>(wParam), &clientRect, g_WindowBrush);
        return 1;
    }
    case WM_DESTROY:
        ReleaseDarkThemeResources();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        ApplyDarkModeToWindow(hDlg, false);
        ApplyDarkThemeToChild(GetDlgItem(hDlg, IDOK));
        return static_cast<INT_PTR>(TRUE);

    case WM_CTLCOLORDLG:
        SetTextColor(reinterpret_cast<HDC>(wParam), kDarkTextColor);
        SetBkColor(reinterpret_cast<HDC>(wParam), kDarkSurfaceColor);
        return reinterpret_cast<INT_PTR>(g_SurfaceBrush);
    case WM_CTLCOLORSTATIC:
    {
        HWND hControl = reinterpret_cast<HWND>(lParam);
        const bool isCreditLabel = hControl == GetDlgItem(hDlg, IDC_ABOUT_CREDITS);
        SetTextColor(reinterpret_cast<HDC>(wParam), isCreditLabel ? kDarkMutedTextColor : kDarkTextColor);
        SetBkColor(reinterpret_cast<HDC>(wParam), kDarkSurfaceColor);
        return reinterpret_cast<INT_PTR>(g_SurfaceBrush);
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_ABOUT_DISCORD || LOWORD(wParam) == IDC_ABOUT_REPOSITORY)
        {
            OpenExternalLink(hDlg, static_cast<int>(LOWORD(wParam)));
            return static_cast<INT_PTR>(TRUE);
        }

        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return static_cast<INT_PTR>(TRUE);
        }
        break;
    }
    return static_cast<INT_PTR>(FALSE);
}
