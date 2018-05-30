#pragma once
#include "hookFunctions.h"


class cheats
{
public:
	cheats();
	~cheats();

	bool bEnableDoInGameSaveLoad;
	void hookAllEnabled(HMODULE moduleReference);
private:
	void enableSaveLoadMenu(HMODULE moduleReference);
};