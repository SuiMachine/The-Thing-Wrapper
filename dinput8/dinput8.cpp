#include "dinput8.h"
#define ClassName 0x00A034E8

//Global Variables
bool bSkipIntro = false;
int bWidth = 1024;
int bHeight = 768;
float bAspectRatio = 1.33333f;
int vPositionX = 0;
int vPositionY = 0;
bool bFullscreen;
bool bBorderless;
float fTrue = 1.0f;
float fFalse = 0.0f;
float bDesiredFOV = 90;

cheats * pCheat;
WidescreenFixes * pWidescreenFixes;
HMODULE baseModule;

//Detours
DWORD sizeOverrideReturn;
void __declspec(naked) sizeOverride()
{
	__asm
	{
		mov edi,DS:[bWidth]
		mov ebp,DS:[bHeight]
		fstp st(0)
		fld DS:[fFalse]
		fcomp ds:0x841780
		jmp[sizeOverrideReturn]
	}
}

DWORD sizeOverrideFullscreenLoadReturn;
void __declspec(naked) sizeOverrideFullscreenLoad()
{
	__asm
	{
		push DS:[bHeight]
		push DS:[bWidth]
		jmp[sizeOverrideFullscreenLoadReturn]
	}
}

DWORD fullscreenOverrideReturn;
void __declspec(naked) fullscreenOverride()
{
	__asm
	{
		mov al, DS:[bFullscreen]
		jmp[fullscreenOverrideReturn]
	}
}

DWORD dwStyleOverride = 0x0;
DWORD styleOverrideReturn;
void __declspec(naked) styleOverride()
{
	__asm
	{
		push    DS:[vPositionY] //Y
		push    DS:[vPositionX] //X
		push    DS:[dwStyleOverride] //dwStyle
		push    eax //lpWindowName
		push    eax //lpClassName
		push    ebx //dwExStyle
		jmp[styleOverrideReturn]
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
		pCheat = new cheats();

		//Load info from ini
		bSkipIntro = GetPrivateProfileInt("MAIN", "SkipIntro", 0, path) != 0;
		bWidth = GetPrivateProfileInt("MAIN", "Width", 0, path);
		bHeight = GetPrivateProfileInt("MAIN", "Height", 0, path);
		if (bWidth == 0 || bHeight == 0)
		{
			bWidth = 1024;
			bHeight = 768;
		}
		bAspectRatio = bWidth * 1.0f / bHeight;

		bFullscreen = GetPrivateProfileInt("MAIN", "Windowed", 0, path) == 0;
		bBorderless = GetPrivateProfileInt("MAIN", "Windowed", 0, path) > 1;
		if (bBorderless)
		{
			dwStyleOverride = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;;
		}
		vPositionX = GetPrivateProfileInt("MAIN", "PositionX", 0, path);
		vPositionY = GetPrivateProfileInt("MAIN", "PositionY", 0, path);
		
		//FOV read as string and parse
		{
			char strValue[255];
			DWORD LenghtWhatever = GetPrivateProfileString("MAIN", "FOV", "60", strValue, 254, path);
			bDesiredFOV = (float)std::atof(strValue);
			if (bDesiredFOV < 10 || bDesiredFOV > 120)
				bDesiredFOV = 60;
		}

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
		baseModule = GetModuleHandle(NULL);
		UnprotectModule(baseModule);

		//Size override detour
		Hook((DWORD)baseModule + 0xB1AA, sizeOverride, &sizeOverrideReturn, 0x6);
		Hook((DWORD)baseModule + 0x35506F, sizeOverrideFullscreenLoad, &sizeOverrideFullscreenLoadReturn, 0x22);

		//Fullscreen, Borderless and Position
		Hook((DWORD)baseModule + 0x357D20, fullscreenOverride, &fullscreenOverrideReturn, 0x6);

		//Borderless mode or position
		if (!bFullscreen && (bBorderless || vPositionX > 0 || vPositionY > 0))
		{
			Hook((DWORD)baseModule + 0x0B1DC, styleOverride, &styleOverrideReturn, 6);
		}
		
		if (bAspectRatio != (4.0f / 3.0f) || bDesiredFOV != 60)
		{
			pWidescreenFixes = new WidescreenFixes(baseModule, bWidth, bHeight);
			pWidescreenFixes->OverrideFOV(bDesiredFOV);
		}

		if (bSkipIntro)
		{
			//This actually makes the game jump to language selector, but it's incomplete, so it skips the intros instead - well in English release at least
			*(short*)((DWORD)baseModule + 0x76D9A) = 0x9090;
		}

		//Cheats
		pCheat->bEnableDoInGameSaveLoad = GetPrivateProfileInt("CHEATS", "EnableSaveLoadGame", 0, path) != 0;
		pCheat->hookAllEnabled(baseModule);

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
