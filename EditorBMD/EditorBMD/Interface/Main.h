#pragma once

#include "..\BMDCommon.h"
#include "..\Decrypt\BmdDecrypt.h"
#include "Resource.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

std::vector<Script_Item> g_Items;
bool g_Modified = false;
char g_CurrentFilePath[MAX_PATH] = { 0 };
BmdItemLayout g_CurrentItemLayout = BmdItemLayout::Aligned164;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void OpenBmdFile(HWND hWnd);
void SaveBmdFile(HWND hWnd);
void SaveAsBmdFile(HWND hWnd);
void UpdateListBox(HWND hWnd);
