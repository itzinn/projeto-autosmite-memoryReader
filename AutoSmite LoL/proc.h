#pragma once

#include <windows.h>
#include <vector>
#include <TlHelp32.h>

DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* moduleName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t moduleBaseAddress, std::vector<unsigned int> offsets);