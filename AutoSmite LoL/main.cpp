/*

programa que pega valores da memória do processo (vida, e time do objeto undermouse e o level do player)

offsets da versão 11.7

*/

#include "proc.h"
#include <iostream>
#include <thread>

HANDLE hProc = 0;

int smiteDmg = 390, level = 1, oldlevel = 1;
int team;
float life = 0;

bool bAutoSmite = 0;

uintptr_t underMouseBaseAddr, playerBaseAddr;
uintptr_t lifeAddr, levelAddr, teamAddr;

int dmgArray[18] = {390, 410, 430, 450, 480, 510, 540, 570, 600, 640, 680, 720, 760, 800, 850, 900, 950, 1000};

void startaPausa() {
	while (true) {
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			bAutoSmite = !bAutoSmite;

			if (bAutoSmite) {
				std::cout << "Iniciado." << std::endl;
				MessageBeep(MB_OK);
			}
			else {
				std::cout << "Pausado." << std::endl;
				MessageBeep(MB_ICONSTOP);
			}
		}
		Sleep(30);
	}
}

void pegaValorSmite() {
	while (true) {
		Sleep(1000);
		ReadProcessMemory(hProc, (BYTE*)levelAddr, &level, sizeof(level), nullptr);
		if (level != oldlevel) {
			smiteDmg = dmgArray[level - 1];
			std::cout << std::dec << "Dano do smite: " << smiteDmg << std::endl;
			if (level == 18) {
				break;
			}
			oldlevel = level;
		}
	}
}

int main(void) {

	std::vector<unsigned int> lifeOffsets = { 0xD98 }; //ObjHealth
	std::vector<unsigned int> levelOffsets = { 0x36DC }; //ObjLvl
	std::vector<unsigned int> teamOffsets = { 0x4C }; //ObjTeam

	//get procId
	DWORD procId = GetProcId(L"League of Legends.exe");

	if (procId) {

		//get module base address
		uintptr_t moduleBaseAddress = GetModuleBaseAddress(procId, L"League of Legends.exe");

		std::cout << "Endereco base do modulo do lol: " << std::hex << moduleBaseAddress << std::endl;

		//get a handle to the proc
		hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		//resolve base adress
		underMouseBaseAddr = moduleBaseAddress + 0x2326780; //0x2346840;
		playerBaseAddr = moduleBaseAddress + 0x2F7513C; //0x02f9512c;

		std::cout << "Endereco base do player: " << std::hex << playerBaseAddr << std::endl;

		//resolve level address (constante)
		levelAddr = FindDMAAddy(hProc, playerBaseAddr, levelOffsets);

	}
	else {
		std::cout << "O processo nao foi encontrado." << std::endl;
		std::getchar();
		return 0;
	}

	std::thread valorSmite(pegaValorSmite);
	std::thread controlador(startaPausa);

	DWORD dwExitCode = 0;

	std::cout << "Aperte 1 (NUMPAD) para iniciar o AutoSmite." << std::endl;

	while (GetExitCodeProcess(hProc, &dwExitCode) && dwExitCode == STILL_ACTIVE) {

		if (bAutoSmite) {
			lifeAddr = FindDMAAddy(hProc, underMouseBaseAddr, lifeOffsets); //diferente para cada monstro under mouse
			teamAddr = FindDMAAddy(hProc, underMouseBaseAddr, teamOffsets); //diferente para cada monstro under mouse

			ReadProcessMemory(hProc, (BYTE*)lifeAddr, &life, sizeof(life), nullptr);
			ReadProcessMemory(hProc, (BYTE*)teamAddr, &team, sizeof(team), nullptr);

			if (life <= smiteDmg) {
				if (team == 300) {

					INPUT Input = { 0 };
					Input.type = INPUT_KEYBOARD;
					Input.ki.wVk = NULL;
					Input.ki.wScan = 33; //scanCode 'F'
					Input.ki.dwFlags = KEYEVENTF_SCANCODE;
					SendInput(1, &Input, sizeof(Input));
					ZeroMemory(&Input, sizeof(Input));
					Input.ki.dwFlags = KEYEVENTF_KEYUP;
					SendInput(1, &Input, sizeof(Input));

					Sleep(15);
					
					//release
					INPUT Input2 = { 0 };
					Input2.type = INPUT_KEYBOARD;
					Input2.ki.wVk = NULL;
					Input2.ki.wScan = 33; //scanCode 'F'
					Input2.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					SendInput(1, &Input2, sizeof(INPUT));
				}
			}

		}
	}

}