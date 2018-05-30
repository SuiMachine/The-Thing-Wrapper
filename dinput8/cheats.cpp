#include "cheats.h"

cheats::cheats()
{
}

cheats::~cheats()
{
}


void cheats::hookAllEnabled(HMODULE moduleReference)
{
	if (bEnableDoInGameSaveLoad)
		enableSaveLoadMenu(moduleReference);
}

#pragma region EnableSaveLoadMenu
DWORD enableSaveLoadMenuReturn;
bool bCEnableDoInGameSaveLoad;
void __declspec(naked) enableSaveLoadNaked()
{
	__asm
	{
		fstp st(0)
		fild bCEnableDoInGameSaveLoad
		fcomp   ds:0x841780
		jmp[enableSaveLoadMenuReturn]
	}
}

void cheats::enableSaveLoadMenu(HMODULE moduleReference)
{
	Hook((DWORD)moduleReference + 0x00212D47, enableSaveLoadNaked, &enableSaveLoadMenuReturn, 0x6);
	bCEnableDoInGameSaveLoad = true;
}
#pragma endregion


