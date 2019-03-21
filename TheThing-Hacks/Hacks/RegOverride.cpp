#include "RegOverride.h"
#include <intrin.h>

std::set<std::string> debugSets;
bool * enabledFeatures;

float ToBinaryFloat(int index, float overrideValue = 1.0f, float falseValue = 0)
{
	if (enabledFeatures[index])
		return overrideValue;
	else
		return falseValue;
}

char __fastcall ReadRegistryFloatDetour(float *PtrWhereToWrite, HKEY hKeyLocation, const char *RegistryPath, const char *RegistryProperty)
{
	if(strcmp(RegistryProperty, "DoInGameLoadSave") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::CheatSaveGame);
		return 1;
	}
	else if(strcmp(RegistryProperty, "DoLevelSelect") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::CheatDoLevelSelect);
		return 1;
	}
	else if (strcmp(RegistryProperty, "PlayerInvulnerable") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::CheatPlayerInvulnerable);
		return 1;
	}
	else if (strcmp(RegistryProperty, "NPCInvulnerable") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::CheatNPCInvulnerable);
		return 1;
	}
	else if (strcmp(RegistryProperty, "FullWeaponEquip") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::CheatFullWeaponEquip);
		return 1;
	}
	else if (strcmp(RegistryProperty, "FPS") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::DevShowFPS);
		return 1;
	}
	else if (strcmp(RegistryProperty, "ShowLights") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::DevShowLights);
		return 1;
	}
	else if (strcmp(RegistryProperty, "EnableAntiAliasing") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::GraphicsEnableAA);
		return 1;
	}
	else if (strcmp(RegistryProperty, "SmoothMouse") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::InputMouseSmoothing);
		return 1;
	}
	else if (strcmp(RegistryProperty, "DisableShadows") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::GraphicsDisableShadows);
		return 1;
	}
	else if (strcmp(RegistryProperty, "DisableSomeUI") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::GraphicsDisableShadows);
		return 1;
	}
	else if (strcmp(RegistryProperty, "EnableTrilinear") == 0 || strcmp(RegistryProperty, "BumpEnableTrilinear") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::GraphicsTrilinearFiltering);
		return 1;
	}
	else if (strcmp(RegistryProperty, "EnableAnisotropic") == 0 || strcmp(RegistryProperty, "BumpEnableAnisotropic") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::GraphicsEnableAniso);
		return 1;
	}
	else if (strcmp(RegistryProperty, "BumpEnable") == 0 || strcmp(RegistryProperty, "BumpEnableDynamic") == 0)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::GraphicsEnableBumpMapping);
		return 1;
	}
	else if (strcmp(RegistryProperty, "Console") == 0 )
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::DevDrawConsoleMsg);
		return 1;
	}
	/*
	else if (strcmp(RegistryProperty, "CombatCameraDistance") == 0 || strcmp(RegistryProperty, "ChaseCameraMinDistance") == 0 || strcmp(RegistryProperty, "ChaseCameraMaxDistance") == 0 ||
		strcmp(RegistryProperty, "ChaseOutsideMaxDistance") == 0 ||
		strcmp(RegistryProperty, "ChaseOutsideMinDistance") == 0
		)
	{
		*PtrWhereToWrite = ToBinaryFloat((int)SettingEnum::DevDrawConsoleMsg, 0);
		return 1;
	}*/

#if _DEBUG
	std::string propertyCast = RegistryProperty;
	bool contains = debugSets.find(propertyCast) != debugSets.end();
	if (!contains)
		debugSets.insert(propertyCast);
#endif

	//Need to clean this shit up

	HKEY v4 = hKeyLocation; // ebp
	DWORD v8; // edi
	char v9; // bl
	int v10; // eax
	char v11; // cl
	int v12; // eax
	char v13; // cl
	int v14; // ebp
	BYTE *v15; // eax
	byte v17 = 0; // [esp+13h] [ebp-1059h]
	HKEY phkResult = 0; // [esp+14h] [ebp-1058h]
	DWORD Type; // [esp+18h] [ebp-1054h]
	char *v20; // [esp+1Ch] [ebp-1050h]
	__int64 v22; // [esp+24h] [ebp-1048h]
	unsigned int Data[16]; // [esp+2Ch] [ebp-1040h]
	char v25[2048]; // [esp+86Ch] [ebp-800h]

	CHAR SubKey[2048];
	strcpy_s(SubKey, RegistryPath);
	*(WORD*)&SubKey[strlen(SubKey)] = *(WORD*)0x8D95F0;
	strcat_s(SubKey, RegistryProperty);
	char *v6 = strchr(SubKey, 92);
	const char *v7 = v6 + 1;
	v20 = v6 + 1;

	v8 = RegOpenKeyExA(v4, RegistryPath, 0, KEY_READ, &phkResult);
	if (v8)
	{
		v9 = 0;
		if (v8 != 2)
		{
			if (v4 == HKEY_LOCAL_MACHINE)
			{
				v10 = 0;
				do
				{
					v11 = ((byte*)0x8DC094)[v10];
					v25[v10++] = v11;
				} while (v11);
			}
			else if (v4 == HKEY_CURRENT_USER)
			{
				v12 = 0;
				do
				{
					v13 = ((byte*)0x8DC080)[v12];
					v25[v12++] = v13;
				} while (v13);
			}
			else
			{
				sprintf_s(v25, 2048, "[08x%x]", v4);
			}
			v14 = *(DWORD*)(__readfsdword(0x2C) + 4 * (*(DWORD*)0x00944A50));
			*(byte*)(v14 + 4872) = 0;
			if (v8 == -1)
				v8 = GetLastError();
			if (!FormatMessageA(0x1000, 0, v8, 0, (LPSTR)(v14 + 4872), 0x100, 0))
				sprintf_s((char*)(v14 + 4872), 2048, "System Error %d (0x%x)", v8, v8);

			v15 = (byte*)(strlen((const char*)(v14 + 4872)), +v14 + 4872);
			if (*v15 == 10)
				*v15 = 0;
		}
		return v9;
	}

	DWORD cbData = 64; // [esp+20h] [ebp-104Ch]

	v8 = RegQueryValueExA(phkResult, RegistryProperty, NULL, &Type, (LPBYTE)Data, &cbData);
	if (!v8)
	{
		switch (Type)
		{
		case 1:
		case 2:
		case 7:
			if (LOBYTE(Data[0]))
			{
				const char* ReadData = (const char*)Data;
				*PtrWhereToWrite = (float)atof(ReadData);
				v17 = 1;
			}
			break;
		case 4:
			v22 = Data[0];
			*PtrWhereToWrite = (float)Data[0];
			v9 = 1;
			break;
		default:
			break;
		}
		if(v9 != 1)
			v9 = v17;

		RegCloseKey(phkResult);
		return v9;
	}

	return 0;
}


RegOverride::RegOverride()
{
	enabledFeatures = new bool[(int)SettingEnum::LastEntry]{ false };
}


RegOverride::~RegOverride()
{
}

void RegOverride::SetSetting(SettingEnum element)
{
	enabledFeatures[(int)element] = true;
}

void RegOverride::LoadValuesFromIni(CIniReader iniReader)
{
	isRegOverrideEnabled = true;

	//Graphics
	if (iniReader.ReadInteger("GRAPHICS", "AntiAliasing", 0) != 0) SetSetting(SettingEnum::GraphicsEnableAA);
	if (iniReader.ReadInteger("GRAPHICS", "TrilinearFiltering", 0) != 0) SetSetting(SettingEnum::GraphicsTrilinearFiltering);
	if (iniReader.ReadInteger("GRAPHICS", "DisableShadows", 0) != 0) SetSetting(SettingEnum::GraphicsDisableShadows);
	if (iniReader.ReadInteger("GRAPHICS", "BumpMapping", 0) != 0) SetSetting(SettingEnum::GraphicsEnableBumpMapping);
	if (iniReader.ReadInteger("GRAPHICS", "AnisotropicFiltering", 0) != 0) SetSetting(SettingEnum::GraphicsEnableAniso);

	//Input
	if (iniReader.ReadInteger("INPUT", "MouseSmoothing", 0) != 0) SetSetting(SettingEnum::InputMouseSmoothing);

	//Cheats
	if (iniReader.ReadInteger("CHEATS", "EnableSaveLoadGame", 0) != 0) SetSetting(SettingEnum::CheatSaveGame);
	if (iniReader.ReadInteger("CHEATS", "EnableLevelSelect", 0) != 0) SetSetting(SettingEnum::CheatDoLevelSelect);
	if (iniReader.ReadInteger("CHEATS", "PlayerInvulnerable ", 0) != 0) SetSetting(SettingEnum::CheatPlayerInvulnerable);
	if (iniReader.ReadInteger("CHEATS", "TeamInvulnerable", 0) != 0) SetSetting(SettingEnum::CheatNPCInvulnerable);
	if (iniReader.ReadInteger("CHEATS", "AllWeapons", 0) != 0) SetSetting(SettingEnum::CheatFullWeaponEquip);

	//Developer
	if (iniReader.ReadInteger("DEVELOPER", "ShowFPS", 0) != 0) SetSetting(SettingEnum::DevShowFPS);
	if (iniReader.ReadInteger("DEVELOPER", "ShowLights", 0) != 0) SetSetting(SettingEnum::DevShowLights);
	if (iniReader.ReadInteger("DEVELOPER", "DrawConsoleMsg", 0) != 0) SetSetting(SettingEnum::DevDrawConsoleMsg);
}

void RegOverride::HookRegistry()
{
	Hook(0x682240, ReadRegistryFloatDetour, 0x5);

	//Hook(0x48D770, cameraPosDetour, 0x5);
}


