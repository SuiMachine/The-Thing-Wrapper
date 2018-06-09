#pragma once
#include "hookFunctions.h"
#include <string>

class WidescreenFixes
{
public:
	WidescreenFixes(HMODULE moduleReference, int Width, int Height);
	void OverrideFOV(float HorizontalFOV);
	float ConvertFOV(float DesiredFOVInDegrees, float AspectRatio);
	~WidescreenFixes();
private:
	void CorrectCameraAspectRatio(float aspect);
	void TestingStuff();

	int Width = 1024;
	int Height = 768;
	float AspectRatio = (4.0f / 3.0f);
	float divChange;
	HMODULE baseModuleRef;
};