#include "dinput8.h"
#define ClassName 0x00A034E8
#define M_PI 3.14159265358979323846
#define USE_MATH_DEFINE


//Global Variables
int bWidth = 1024;
int bHeight = 768;
float bAspectRatio = 1.33333f;
float divChange;
bool bFullscreen;
float fTrue = 1.0f;
float fFalse = 0.0f;
float bDesiredFOV = 90;

cheats * pCheat;
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

DWORD fullscreenOverrideReturn;
void __declspec(naked) fullscreenOverride()
{
	__asm
	{
		mov al,DS:[bFullscreen]
		jmp[fullscreenOverrideReturn]
	}
}

std::string fileExceptionsForaspectCorrectionUI2DDraw[] = { "eff_cctvdarkband", "eff_cctvnoise", "eff_cctvstatic" };
void __fastcall aspectCorrectionUI2DDraw(float *ecx0, int *edx0, signed int a1, signed int a2, signed int a3, signed int a4, char * a5, int a6, int a7)
{
	float *v9 = ecx0;
	int *v10 = edx0;
	float fLeftEdgePrecent = 0;
	float fBottomEdgePrecent = 0;
	float fRightEdgePrecent = 0;
	float fTopEdgePrecent = 0;

	//_DWORD *__thiscall sub_7505E0(_DWORD *this)
	//typedef DWORD*(*probablyInitilizerForCoordinates)(float*);
	//((probablyInitilizerForCoordinates)0x7505E0)(&fLeftEdgePrecent);

	fLeftEdgePrecent = (float)((double)a1 * 0.003125 - 1.0);
	fRightEdgePrecent = (float)((double)a3 * 0.003125 - 1.0);
	fBottomEdgePrecent = (float)((double)a2 * 0.0041666669 - 1.0);
	fTopEdgePrecent = (float)((double)a4 * 0.0041666669 - 1.0);

	
	
	if (std::find(std::begin(fileExceptionsForaspectCorrectionUI2DDraw), std::end(fileExceptionsForaspectCorrectionUI2DDraw), a5) == std::end(fileExceptionsForaspectCorrectionUI2DDraw))
	{
		//fLeftEdgePrecent = fLeftEdgePrecent / divChange;
		//fRightEdgePrecent = fRightEdgePrecent / divChange;
		fBottomEdgePrecent = fBottomEdgePrecent * divChange;
		fTopEdgePrecent = fTopEdgePrecent * divChange;
	}

	//int __fastcall blind_Draw2DElement(float *unknownPointerPRollyStruct, int *unknownPointer, float leftEdgePrecent, float bottomEdgePrecent, float rightEdgePrecent, float topEdgePrecent, int a7, int a8, int a9)
	typedef int(__fastcall *funcDraw2DElement)(float*, int*, float edgeLeft, float edgeBottom, float edgeRight, float edgeTop, char*, int, int);
	((funcDraw2DElement)0x0076E480)(v9, v10, fLeftEdgePrecent, fBottomEdgePrecent, fRightEdgePrecent, fTopEdgePrecent, a5, a6, a7);
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
		bWidth = GetPrivateProfileInt("MAIN", "Width", 0, path);
		bHeight = GetPrivateProfileInt("MAIN", "Height", 0, path);
		if (bWidth == 0 || bHeight == 0)
		{
			bWidth = 1024;
			bHeight = 768;
		}
		bAspectRatio = bWidth * 1.0f / bHeight;
		bFullscreen = GetPrivateProfileInt("MAIN", "Windowed", 0, path) != 1;
		
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
		baseModule = GetModuleHandleA("TheThing.exe");
		UnprotectModule(baseModule);
		Hook((DWORD)baseModule + 0xB1AA, sizeOverride, &sizeOverrideReturn, 0x6);
		Hook((DWORD)baseModule + 0x357D20, fullscreenOverride, &fullscreenOverrideReturn, 0x6);
		*(float*)((DWORD)baseModule + 0x4579AC) = bAspectRatio; //Modify camera aspect ratio

#ifdef _DEBUG
		divChange = bAspectRatio / (4.0f / 3.0f);
		Hook((DWORD)baseModule + 0x36E390, aspectCorrectionUI2DDraw, 0x9F);

		//Crosshair
		/*
		{
		float leftEdge = *(float*)((DWORD)baseModule + 0x97CF6) / divChange;
		float rightEdge = *(float*)((DWORD)baseModule + 0x97D06) / divChange;
		*(float*)((DWORD)baseModule + 0x97CF6) = leftEdge;
		*(float*)((DWORD)baseModule + 0x97D06) = rightEdge;
		}*/
#endif


		//FOV
		*(float*)((DWORD)baseModule + 0x4D3507) = ConvertFOV(bDesiredFOV, bAspectRatio);

		//Cheats
		pCheat->bEnableDoInGameSaveLoad = GetPrivateProfileInt("CHEATS", "EnableSaveLoadGame", 0, path) != 0;
		pCheat->hookAllEnabled(baseModule);

		//*(float*)((DWORD)baseModule + 0x83229)  = 2;//1.3962635; //CombatCameraFOV
		//*(float*)((DWORD)baseModule + 0x29BCC8) = 100;//60; //TorchFOV (Flashlight FOV)
		//*(float*)((DWORD)baseModule + 0x29B8CD) = 100;//50 //TorchFOV1
		//*(float*)((DWORD)baseModule + 0x29B501) = 100;//50 //TorchFOV2



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

float ConvertFOV(float DesiredFOVInDegrees, float AspectRatio)
{
	double horRadian = M_PI * DesiredFOVInDegrees / 180;
	double verRadian = 2 * atan(tan(horRadian / 2) * (3.0 / 4.0));
	return (float)(2 * atan(tan(verRadian / 2) * AspectRatio)); //2 * Math.Atan(Math.Tan(VerticalRadians / 2) * (ResWidth * 1.0 / ResHeight * 1.0));
}
