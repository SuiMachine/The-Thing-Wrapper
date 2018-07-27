#pragma once
#include "hookFunctions.h"
#include <string>
#include <iostream>
#include <functional>
#include <set>



enum class SettingEnum
{
	InputMouseSmoothing,
	GraphicsEnableAA,
	GraphicsEnableAniso,
	GraphicsTrilinearFiltering,
	GraphicsEnableBumpMapping,
	GraphicsDisableShadows,
	DevShowFPS,
	DevShowLights,
	DevDrawConsoleMsg,
	CheatSaveGame,
	CheatDoLevelSelect,
	CheatPlayerInvulnerable,
	CheatNPCInvulnerable,
	CheatFullWeaponEquip,
	LastEntry
};

class RegOverride
{
public:
	bool isRegOverrideEnabled = false;
	RegOverride();
	~RegOverride();
	void SetSetting(SettingEnum setting);
	void LoadValuesFromIni(char * iniPath);
	void HookRegistry();
};