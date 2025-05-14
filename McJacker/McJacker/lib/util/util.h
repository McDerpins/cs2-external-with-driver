#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <iostream>

DWORD_PTR GetModuleBaseAddress(DWORD dwPid, const wchar_t* moduleName);

int GetProcessIdByName(const wchar_t* processName);

//void SpawnConsole();