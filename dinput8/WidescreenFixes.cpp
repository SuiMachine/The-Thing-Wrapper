#include "WidescreenFixes.h"
#define M_PI 3.14159265358979323846
#define USE_MATH_DEFINE

#pragma region StaticInjectionFunctions
int StretchedAspectTop = 0;
int StretchedAspectLeft= 0;
int StretchedAspectWidth = 1024;
int StretchedAspectHeight = 768;

DWORD cinematicsRectangleOverrideReturn;
void __declspec(naked) cinematicsRectangleOverride()
{
	__asm
	{
		mov edx,[StretchedAspectHeight]
		mov eax,[ebp+0x10]
		mov ecx,[eax]
		push edx
		mov edx,[StretchedAspectWidth]
		push edx
		mov edx,[StretchedAspectTop]
		push edx
		mov edx,[StretchedAspectLeft]
		push edx
		jmp[cinematicsRectangleOverrideReturn]
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
		//fBottomEdgePrecent = fBottomEdgePrecent / divChange;
		//fTopEdgePrecent = fTopEdgePrecent / divChange;
	}

	//int __fastcall blind_Draw2DElement(float *unknownPointerPRollyStruct, int *unknownPointer, float leftEdgePrecent, float bottomEdgePrecent, float rightEdgePrecent, float topEdgePrecent, int a7, int a8, int a9)
	typedef int(__fastcall *funcDraw2DElement)(float*, int*, float edgeLeft, float edgeBottom, float edgeRight, float edgeTop, char*, int, int);
	((funcDraw2DElement)0x0076E480)(v9, v10, fLeftEdgePrecent, fBottomEdgePrecent, fRightEdgePrecent, fTopEdgePrecent, a5, a6, a7);
}
#pragma endregion

//Contructor
WidescreenFixes::WidescreenFixes(HMODULE moduleReference, int Width, int Height)
{
	this->baseModuleRef = moduleReference;
	this->Width = Width;
	this->Height = Height;
	this->AspectRatio = Width * 1.0f / Height;
	this->divChange = this->AspectRatio / (4.0f / 3.0f);

	StretchedAspectHeight = Height;
	StretchedAspectWidth = (int)(Height * (4.0f / 3.0f));
	StretchedAspectTop = 0;
	StretchedAspectLeft = (int)((Width / 2.0f)-(StretchedAspectWidth/2.0f));
	//Hook((DWORD)(this->baseModuleRef) + 0x3B20C9, cinematicsRectangleOverride, &cinematicsRectangleOverrideReturn, 0x11);

	CorrectCameraAspectRatio(this->AspectRatio);


	//Crosshair
	/*
	{
	float leftEdge = *(float*)((DWORD)baseModule + 0x97CF6) / divChange;
	float rightEdge = *(float*)((DWORD)baseModule + 0x97D06) / divChange;
	*(float*)((DWORD)baseModule + 0x97CF6) = leftEdge;
	*(float*)((DWORD)baseModule + 0x97D06) = rightEdge;
	}*/
}

//Function used to override FOV
void WidescreenFixes::OverrideFOV(float HorizontalFOV)
{
	*(float*)((DWORD)(this->baseModuleRef) + 0x4D3507) = ConvertFOV(HorizontalFOV, this->AspectRatio);
}

//Destructor (not used?)
WidescreenFixes::~WidescreenFixes()
{
}

//Function used to override camera aspect ratio
void WidescreenFixes::CorrectCameraAspectRatio(float aspect)
{
	*(float*)((DWORD)(this->baseModuleRef) + 0x4579AC) = aspect; //Modify camera aspect ratio
}

//Function used for testing stuff
void WidescreenFixes::TestingStuff()
{
	Hook((DWORD)(this->baseModuleRef) + 0x36E390, aspectCorrectionUI2DDraw, 0x9F);

	//*(float*)((DWORD)baseModule + 0x83229)  = 2;//1.3962635; //CombatCameraFOV
	//*(float*)((DWORD)baseModule + 0x29BCC8) = 100;//60; //TorchFOV (Flashlight FOV)
	//*(float*)((DWORD)baseModule + 0x29B8CD) = 100;//50 //TorchFOV1
	//*(float*)((DWORD)baseModule + 0x29B501) = 100;//50 //TorchFOV2
}

//function to calculate hor+ FOV
float WidescreenFixes::ConvertFOV(float DesiredFOVInDegrees, float AspectRatio)
{
	double horRadian = M_PI * DesiredFOVInDegrees / 180;
	double verRadian = 2 * atan(tan(horRadian / 2) * (3.0 / 4.0));
	return (float)(2 * atan(tan(verRadian / 2) * AspectRatio)); //2 * Math.Atan(Math.Tan(VerticalRadians / 2) * (ResWidth * 1.0 / ResHeight * 1.0));
}