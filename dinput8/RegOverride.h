#pragma once
#include "hookFunctions.h"
#include <string>
#include <iostream>
#include <functional>



enum class SettingEnum
{
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
	RegOverride();
	~RegOverride();
	void SetSetting(SettingEnum setting);
	void HookRegistry();
};