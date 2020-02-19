#include "Core.h"

CCore* Core;
CItemRandomiser *ItemRandomiser;
CAutoEquip *AutoEquip;
SCore* CoreStruct;


int pOffsetList[1605];
DWORD64 qItemEquipComms = 0;

DWORD64 rItemRandomiser = 0;
DWORD64 rAutoEquip = 0;
DWORD64 rNoWeaponRequirements = 0;

VOID CCore::Start() {

	Core = new CCore();
	ItemRandomiser = new CItemRandomiser();
	AutoEquip = new CAutoEquip();
	CoreStruct = new SCore();

	Core->DebugInit();

	if (!Core->Initialise()){
		Core->Panic("Failed to initialise", "...\\Randomiser\\Core\\Core.cpp", FE_InitFailed, 1);
		int3
	};

	while (true) {
	
		Core->Run();
		Sleep(500);
	};

	delete CoreStruct;
	delete Core;

	return;
};

VOID CCore::Run() {


	return;
};

BOOL CCore::Initialise() {

	int i = 0;
	char pBuffer[MAX_PATH];
	BOOL bReturn = true;
	INIReader reader("RandomiserPreferences.ini");

	if (MH_Initialize() != MH_OK) return false;

	CoreStruct->dIsAutoSave = reader.GetBoolean("Randomiser", "SaveProgress", true);
	CoreStruct->dRandomsieHealItems = reader.GetBoolean("Randomiser", "RandomiseHeals", true);
	CoreStruct->dRandomiseKeyItems = reader.GetBoolean("Randomiser", "RandomiseKeys ", false);
	CoreStruct->dIsMessageActive = reader.GetBoolean("Randomiser", "RandomsierMessage", true);
	CoreStruct->dIsAutoEquip = reader.GetBoolean("AutoEquip", "AutoEquipToggle", true);
	CoreStruct->dIsNoWeaponRequirements = reader.GetBoolean("AutoEquip", "NoWeaponRequirements", true);

#ifdef DEBUG
	sprintf_s(pBuffer, "[Randomiser] - SaveProgress = %i\n", CoreStruct->dIsAutoSave);
	printf_s(pBuffer);
	sprintf_s(pBuffer, "[Randomiser] - RandomiseHeals = %i\n", CoreStruct->dRandomsieHealItems);
	printf_s(pBuffer);
	sprintf_s(pBuffer, "[Randomiser] - RandomiseKeys = %i\n", CoreStruct->dRandomiseKeyItems);
	printf_s(pBuffer);
	sprintf_s(pBuffer, "[Randomiser] - RandomsierMessage = %i\n", CoreStruct->dIsMessageActive);
	printf_s(pBuffer);
	sprintf_s(pBuffer, "[AutoEquip] - AutoEquipToggle = %i\n", CoreStruct->dIsAutoEquip);
	printf_s(pBuffer);
	sprintf_s(pBuffer, "[AutoEquip] - NoWeaponRequirements = %i\n", CoreStruct->dIsNoWeaponRequirements);
	printf_s(pBuffer);
#endif

	GetArrayList();

	while (!pOffsetList[i]) {
		pItemArray[0]++;
		i++;
	}; 

	bReturn &= Hook(0x1407BBA80, (DWORD64)&tItemRandomiser, &rItemRandomiser, 5);

	if (CoreStruct->dIsAutoEquip) bReturn &= Hook(0x1407BBE92, (DWORD64)&tAutoEquip, &rAutoEquip, 6);
	if (CoreStruct->dIsNoWeaponRequirements) bReturn &= Hook(0x140C073B9, (DWORD64)&tNoWeaponRequirements, &rNoWeaponRequirements, 7);

	AutoEquip->EquipItem = (fEquipItem*)0x140AFBBB0;

	return bReturn;
};

BOOL CCore::GetArrayList() {

	int i = 0;

	std::ifstream readfile("DS3RandomAoB.txt");

	if (readfile.is_open()) {

		while (i < (MAX_LIST_ITEMS + 1)) {
			readfile >> pOffsetList[i];
			i++;
		};
		readfile.close();
		return true;
	};

	MessageBoxA(NULL, "The randomiser failed to find DS3RandomAoB.txt in the game directory. Please use the .exe that came with the mod to generate a list and then move it into your DarkSoulsIII.exe folder.", "Load Error", MB_ICONWARNING);

	return false;
};

BOOL CCore::SaveArrayList() {

	int i = 0;

	std::ofstream outfile("DS3RandomAoB.txt");

	if (outfile.is_open()) {

		while (i < (MAX_LIST_ITEMS + 1)) {
			outfile << pOffsetList[i] << std::endl;
			i++;
		};
		outfile.close();
		return true;
	};

	MessageBoxA(NULL, "The randomiser failed to find DS3RandomAoB.txt in the game directory, please restore this file. If this persists please inform LukeYui via the modpage.", "Save Error", MB_ICONWARNING);

	return false;

};

BOOL CCore::Hook(DWORD64 qAddress, DWORD64 qDetour, DWORD64* pReturn, DWORD dByteLen) {

	if (MH_CreateHook((LPVOID)qAddress, (LPVOID)qDetour, 0) != MH_OK) return false;
	if (MH_EnableHook((LPVOID)qAddress) != MH_OK) return false;

	*pReturn = (qAddress + dByteLen);

	return true;
};

VOID CCore::Panic(char* pMessage, char* pSort, DWORD dError, DWORD dIsFatalError) {

	char pOutput[MAX_PATH];
	char pTitle[MAX_PATH];

	sprintf_s(pOutput, "%s -> %s (%i)", pSort, pMessage, dError);

	if (IsDebuggerPresent()) {
		OutputDebugStringA(pOutput);
	};

	if (CoreStruct->dIsDebug) {
		printf_s("CCore::Panic is outputting debug-mode error information\n");
		sprintf_s(pOutput, "%s\n", pOutput);
		printf_s(pOutput);
	}
	else {
		if (dIsFatalError){
			sprintf_s(pTitle, "[Item Randomiser - Fatal Error]");
			sprintf_s(pOutput, "A fatal error occured: %s\n", pOutput);
		} 
		else {
			sprintf_s(pTitle, "[Item Randomiser - Error]");
			sprintf_s(pOutput, "An exception has occured: %s\n", pOutput);
		}; 
		
		MessageBoxA(NULL, pOutput, pTitle, MB_ICONERROR);
	};

	if (dIsFatalError) *(int*)0 = 0;

	return;
};

VOID CCore::DebugInit() {
	CoreStruct->dIsDebug = 0;
#ifdef DEBUG
	FILE* fp;

	AllocConsole();
	SetConsoleTitleA("Dark Souls III - Item Randomiser Debug Console");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	printf_s("Starting...\n");
	CoreStruct->dIsDebug = 1;
#endif

	return;
};