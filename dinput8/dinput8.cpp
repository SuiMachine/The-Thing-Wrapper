#include "dinput8.h"
#define ClassName 0x00A034E8

//Global Variables
int bWidth = 1024;
int bHeight = 768;
float bAspectRatio = 1.33333;
bool bFullscreen;

HMODULE baseModule;

//Detours
DWORD sizeAndFullscreenOverrideReturn;
void __declspec(naked) sizeAndFullscreenOverride()
{
	__asm
	{
		mov edi,DS:[bWidth]
		mov ebp,DS:[bHeight]
		fstp st(0)
		fild DS:[bFullscreen]
		fcomp ds:0x841780
		jmp[sizeAndFullscreenOverrideReturn]
	}
}


//Dll Main
bool WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		//Get module location and its ini file
		char path[MAX_PATH];
		HMODULE hm = NULL;
		GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)"dinput8.dll", &hm);
		GetModuleFileNameA(hm, path, sizeof(path));
		*strrchr(path, '\\') = '\0';
		strcat_s(path, "\\dinput8.ini");

		//Load info from ini
		bWidth = GetPrivateProfileInt("MAIN", "Width", 0, path);
		bHeight = GetPrivateProfileInt("MAIN", "Height", 0, path);
		if (bWidth == 0 || bHeight == 0)
		{
			bWidth = 1024;
			bHeight = 768;
		}
		bAspectRatio = bWidth * 1.0f / bHeight;

		bFullscreen = GetPrivateProfileInt("MAIN", "Windowed", 0, path) != 1;

		//Get dll from Windows directory
		GetSystemDirectory(path, MAX_PATH);
		strcat_s(path, "\\dinput8.dll");

		//Set pointers
		dinput8.dll = LoadLibraryA(path);
		dinput8.DirectInput8Create = (LPWDirectInput8Create)GetProcAddress(dinput8.dll, "DirectInput8Create");
		dinput8.DllCanUnloadNow = (LPWDllCanUnloadNow)GetProcAddress(dinput8.dll, "DllCanUnloadNow");
		dinput8.DllGetClassObject = (LPWDllGetClassObject)GetProcAddress(dinput8.dll, "DllGetClassObject");
		dinput8.DllRegisterServer = (LPWDllRegisterServer)GetProcAddress(dinput8.dll, "DllRegisterServer");
		dinput8.DllUnregisterServer = (LPWDllUnregisterServer)GetProcAddress(dinput8.dll, "DllUnregisterServer");

		//Get base module
		baseModule = GetModuleHandleA("TheThing.exe");
		UnprotectModule(baseModule);
		Hook((DWORD)baseModule + 0xB1AA, sizeAndFullscreenOverride, &sizeAndFullscreenOverrideReturn, 0x6);

		break;
	}
	case DLL_PROCESS_DETACH:
	{
		FreeLibrary(hModule);
		break;
	}
	return true;
	}

	return TRUE;
}

bool Hook(DWORD targetToHook, void * ourFunction, DWORD * returnAddress, int overrideLenght)
{
	if (overrideLenght < 5)
		return false;

	*returnAddress = targetToHook + overrideLenght;

	DWORD curProtectionFlag;
	VirtualProtect((void*)targetToHook, overrideLenght, PAGE_EXECUTE_READWRITE, &curProtectionFlag);
	memset((void*)targetToHook, 0x90, overrideLenght);
	DWORD relativeAddress = ((DWORD)ourFunction - (DWORD)targetToHook) - 5;

	*(BYTE*)targetToHook = 0xE9;
	*(DWORD*)((DWORD)targetToHook + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect((void*)targetToHook, overrideLenght, curProtectionFlag, &temp);
	return true;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
	HRESULT hr = dinput8.DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	return hr;
}

HRESULT WINAPI DllCanUnloadNow()
{
	return dinput8.DllCanUnloadNow();
}

HRESULT WINAPI DllGetClassObject(REFCLSID riidlsid, REFIID riidltf, LPVOID whatever)
{
	return dinput8.DllGetClassObject(riidlsid, riidltf, whatever);
}

HRESULT WINAPI DllRegisterServer()
{
	return dinput8.DllRegisterServer();
}

HRESULT WINAPI DllUnregisterServer()
{
	return dinput8.DllUnregisterServer();
}