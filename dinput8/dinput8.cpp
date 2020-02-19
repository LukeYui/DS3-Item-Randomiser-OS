#include <windows.h>
#include <stdio.h>
#include "Core.h"

HINSTANCE mHinst = 0, mHinstDLL = 0;
extern "C" UINT_PTR mProcs[6] = { 0 };

CCore Core;

LPCSTR mImportNames[6] = {

	"DirectInput8Create",
	"GetdfDIJoystick",
	"DllCanUnloadNow",
	"DllGetClassObject",
	"DllRegisterServer",
	"DllUnregisterServer",

};

DWORD WINAPI Begin(LPVOID lpParam) {

	char dllpath[MAX_PATH];

	GetSystemDirectory(dllpath, MAX_PATH);
	strcat_s(dllpath, "\\dinput8.dll");
	mHinstDLL = LoadLibrary(dllpath);

	if (!mHinstDLL) {
		MessageBoxA(NULL, "Failed to load original DLL", "Error", MB_ICONERROR);
		return FALSE;
	};

	for (int i = 0; i < 6; i++) {
		mProcs[i] = (UINT_PTR)GetProcAddress(mHinstDLL, mImportNames[i]);
	};

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Core.Start, 0, 0, 0);

	return FALSE;
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:

		DisableThreadLibraryCalls(hinstDLL);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Begin, NULL, NULL, NULL);

		break;

	case DLL_PROCESS_DETACH:

		FreeLibrary(mHinstDLL);

		break;
	}

	return TRUE;

}

extern "C" void _DirectInput8Create();