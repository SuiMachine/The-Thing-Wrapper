#pragma once
#include "hookFunctions.h"
#include <string>
#include <set>

class WidescreenFixes
{
public:
	WidescreenFixes(HMODULE moduleReference, int Width, int Height);
	void OverrideFOV(float HorizontalFOV);
	float ConvertFOV(float DesiredFOVInDegrees, float AspectRatio);
	void EnableHudCorrection();
	~WidescreenFixes();
private:
	void CorrectCameraAspectRatio(float aspect);
	void TestingStuff();
	void CorrectCrosshair();

	int Width = 1024;
	int Height = 768;
	float AspectRatio = (4.0f / 3.0f);
	//float divChange;
	HMODULE baseModuleRef;
};