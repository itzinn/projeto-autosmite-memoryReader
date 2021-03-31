#include "proc.h"
#include <iostream>

DWORD GetProcId(const wchar_t* procName) {

	DWORD procId = 0;

	HANDLE hSnaps = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //pega snapshot de todos os processos

	if (hSnaps != INVALID_HANDLE_VALUE) {

		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnaps, &procEntry)) {
			do {

				if (!_wcsicmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}

			} while (Process32Next(hSnaps, &procEntry));
		}

	}

	CloseHandle(hSnaps);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* moduleName) {

	uintptr_t moduleBaseAddr = 0;

	HANDLE hSnaps = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	if (hSnaps != INVALID_HANDLE_VALUE) {

		MODULEENTRY32 moduleEntry;
		moduleEntry.dwSize = sizeof(moduleEntry);

		if (Module32First(hSnaps, &moduleEntry)) {

			do {

				if (!_wcsicmp(moduleEntry.szModule, moduleName)) {
					moduleBaseAddr = (uintptr_t)moduleEntry.modBaseAddr;
					break;
				}

			} while (Module32Next(hSnaps, &moduleEntry));

		}
	}

	CloseHandle(hSnaps);
	return moduleBaseAddr; //return uintptr_t pointer

}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t moduleBaseAddress, std::vector<unsigned int> offsets) {

	uintptr_t addr = moduleBaseAddress;

	uintptr_t addr2;

	for (int i = 0; i < offsets.size(); ++i) {
		ReadProcessMemory(hProc, (BYTE*)addr, &addr2, sizeof(addr), nullptr); // cast to BYTE?
		//std::cout << "addr: " << std::hex << addr2 << std::endl;
		if (addr2 == 0x0) {
			--i;
			continue;
		}
		addr2 += offsets[i];
	}

	return addr2;

}