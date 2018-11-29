#pragma once
#include <string>
#include <dinput.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>  
#include <algorithm>

#include "hookFunctions.h"
#include "sui_stringOperationsSimplification.h"

#include "Hacks\WidescreenFixes.h"
#include "Hacks\RegOverride.h"
#include "Hacks\FirstPersonHack.h"


class TheThings_RootHack
{
public:
	TheThings_RootHack(std::string DllName);
	bool IsD3D8Hook = false;
	~TheThings_RootHack();
private:
	RegOverride * regOverride;
	WidescreenFixes * pWidescreenFixes;
	HMODULE baseModule;
};

