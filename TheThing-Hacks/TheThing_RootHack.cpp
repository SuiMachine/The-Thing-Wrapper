#include "TheThing_RootHack.h"


//Global Variables
bool bSkipIntro = false;
bool bEnableHudCorrection = false;
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

//Detours
DWORD sizeOverrideReturn;
void __declspec(naked) sizeOverride()
{
	__asm
	{
		mov edi, DS:[bWidth]
		mov ebp, DS : [bHeight]
		fstp st(0)
		fld DS : [fFalse]
		fcomp ds : 0x841780
		jmp[sizeOverrideReturn]
	}
}

DWORD sizeOverrideFullscreenLoadReturn;
void __declspec(naked) sizeOverrideFullscreenLoad()
{
	__asm
	{
		push DS : [bHeight]
		push DS : [bWidth]
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
		push    DS : [vPositionY] //Y
		push    DS : [vPositionX] //X
		push    DS : [dwStyleOverride] //dwStyle
		push    eax //lpWindowName
		push    eax //lpClassName
		push    ebx //dwExStyle
		jmp[styleOverrideReturn]
	}
}


TheThings_RootHack::TheThings_RootHack(std::string DllName)
{
	if (SuiString_EndsWith(DllName, "d3d8.ini"))
		IsD3D8Hook = true;

	const char * path = DllName.c_str();

	//Get base module
	baseModule = GetModuleHandle(NULL);
	char baseModuleName[255];
	DWORD whatever = GetModuleFileName(baseModule, baseModuleName, 255);
	std::string cast = (std::string)baseModuleName;
	std::transform(cast.begin(), cast.end(), cast.begin(), ::tolower);
	bool IsTheThingExe = SuiString_EndsWith(cast, "thething.exe");
	if (!IsTheThingExe)
		return;
	UnprotectModule(baseModule);

	//Cheats
	regOverride = new RegOverride();

	CIniReader configReader(path);

	//Load info from ini
	bSkipIntro = configReader.ReadInteger("MAIN", "SkipIntro", 0) != 0;
	if (bSkipIntro)
	{
		//This actually makes the game jump to language selector, but it's incomplete, so it skips the intros instead - well in English release at least
		*(short*)((DWORD)baseModule + 0x76D9A) = (short)0x9090;
	}

	bEnableHudCorrection = configReader.ReadInteger("MAIN", "EnableHUDCorrection", 0) != 0;
	bWidth = configReader.ReadInteger("MAIN", "Width", 0);
	bHeight = configReader.ReadInteger("MAIN", "Height", 0);
	if (bWidth == 0 || bHeight == 0)
	{
		bWidth = 1024;
		bHeight = 768;
	}
	bAspectRatio = bWidth * 1.0f / bHeight;

	bFullscreen = configReader.ReadInteger("MAIN", "Windowed", 0) == 0;
	bBorderless = configReader.ReadInteger("MAIN", "Windowed", 0) > 1;
	if (bBorderless)
	{
		dwStyleOverride = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	vPositionX = configReader.ReadInteger("MAIN", "PositionX", 0);
	vPositionY = configReader.ReadInteger("MAIN", "PositionY", 0);

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

	//FOV read as string and parse
	{
		bDesiredFOV = configReader.ReadFloat("MAIN", "FOV", 60.0f);
		if (bDesiredFOV < 10 || bDesiredFOV > 120)
			bDesiredFOV = 60;
	}

	if (bAspectRatio != (4.0f / 3.0f) || bDesiredFOV != 60)
	{
		pWidescreenFixes = new WidescreenFixes(baseModule, bWidth, bHeight);
		pWidescreenFixes->OverrideFOV(bDesiredFOV);
		if (bEnableHudCorrection)
			pWidescreenFixes->EnableHudCorrection();
	}

	if (configReader.ReadInteger("MAIN", "OverrideRegistryValues", 0) != 0)
	{
		regOverride->LoadValuesFromIni(configReader);
	}

	if (regOverride->isRegOverrideEnabled)
		regOverride->HookRegistry();

#if false
	FirstPersonHack * fph = new FirstPersonHack(baseModule);
#endif

	std::string loadAdditionalDLLName = configReader.ReadString("MAIN", "LoadDll", "");

	//LoadLibary
	if (SuiString_EndsWith(loadAdditionalDLLName, ".dll"))
	{
		LoadLibraryA(loadAdditionalDLLName.c_str());
	}
}


TheThings_RootHack::~TheThings_RootHack()
{
}
