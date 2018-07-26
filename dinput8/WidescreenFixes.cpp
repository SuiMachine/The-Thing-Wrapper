#include "WidescreenFixes.h"
#define M_PI 3.14159265358979323846
#define USE_MATH_DEFINE

int StretchedAspectTop = 0;
int StretchedAspectLeft= 0;
int StretchedAspectWidth = 1024;
int StretchedAspectHeight = 768;
float divChange = 1;

#pragma region StaticInjectionFunctions
#pragma region CutscenesAspectCorrectionFix
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
#pragma endregion
#pragma region 2dIconsScaling
//Some images are better left stretched (usually the ones used as overlays like camera noise, so he is a list of them
std::string filesExcludedFromAspectCorrection[] = { "eff_cctvdarkband", 
	"eff_cctvnoise",
	"eff_cctvstatic",
	"gui_scrollbarkit",
	"gui_ringframekit",
	"gui_outerbar",
	"eff_cctvreclight",
};

void __fastcall aspectCorrectionUI2DDraw(float *ecx0, int *edx0, signed int a1, signed int a2, signed int a3, signed int a4, char * a5, int a6, int a7)
{
	float *v9 = ecx0;
	int *v10 = edx0;
	float fLeftEdgePrecent = 0;
	float fBottomEdgePrecent = 0;
	float fRightEdgePrecent = 0;
	float fTopEdgePrecent = 0;

	//This is basically precent coordinates where -1, -1 is left top, 0,0 is middle 1,1 is bottom right etc.
	fLeftEdgePrecent = (float)((double)a1 * 0.003125 - 1.0);
	fRightEdgePrecent = (float)((double)a3 * 0.003125 - 1.0);
	fBottomEdgePrecent = (float)((double)a2 * 0.0041666669 - 1.0);
	fTopEdgePrecent = (float)((double)a4 * 0.0041666669 - 1.0);
	
	if (std::find(std::begin(filesExcludedFromAspectCorrection), std::end(filesExcludedFromAspectCorrection), a5) != std::end(filesExcludedFromAspectCorrection))
	{
	}
	else
	{
		//calculate vector for repositioning
		float centerVectorX = 0.0f - fLeftEdgePrecent;
		float centerVectorY = 0.0f - fBottomEdgePrecent;

		//check if image is inverted
		bool isInvertedXAxis = (fLeftEdgePrecent > fRightEdgePrecent);
		bool isInvertedYAxis = (fTopEdgePrecent > fBottomEdgePrecent);

		//invert on X axis
		if (isInvertedXAxis)
		{
			fLeftEdgePrecent *= -1.0f;
			fRightEdgePrecent *= -1.0f;
		}

		//invert on Y axis
		if (isInvertedYAxis)
		{
			fTopEdgePrecent *= -1.0f;
			fBottomEdgePrecent *= -1.0f;
		}

		//reposition image so that we get edges with coordinates 0,0, Width, Height
		fLeftEdgePrecent += centerVectorX;
		fTopEdgePrecent += centerVectorY;
		fRightEdgePrecent += centerVectorX;
		fBottomEdgePrecent += centerVectorY;

		//Calculate half size
		float halfSizeX = (fRightEdgePrecent - fLeftEdgePrecent) / 2;
		float halfSizeY = (fTopEdgePrecent - fBottomEdgePrecent) / 2;

		//Reposition iamge so we have it right in the middle of X,Y axis
		fLeftEdgePrecent -= halfSizeX;
		fTopEdgePrecent -= halfSizeY;
		fRightEdgePrecent -= halfSizeX;
		fBottomEdgePrecent -= halfSizeY;

		//Scale left and right edges on X and Y axis
		fLeftEdgePrecent /= divChange;
		fRightEdgePrecent /= divChange;

		//Move image back to original position
		fLeftEdgePrecent = fLeftEdgePrecent - centerVectorX + halfSizeX;
		fTopEdgePrecent = fTopEdgePrecent - centerVectorY + halfSizeY;
		fRightEdgePrecent = fRightEdgePrecent - centerVectorX + halfSizeX;
		fBottomEdgePrecent = fBottomEdgePrecent - centerVectorY + halfSizeY;

		//Invert on X axis if it was inverted
		if (isInvertedXAxis)
		{
			fLeftEdgePrecent *= -1.0f;
			fRightEdgePrecent *= -1.0f;
		}

		//Invert on Y axis if it was inverted
		if (isInvertedYAxis)
		{
			fTopEdgePrecent *= -1.0f;
			fBottomEdgePrecent *= -1.0f;
		}
	}

	//int __fastcall blind_Draw2DElement(float *unknownPointerPRollyStruct, int *unknownPointer, float leftEdgePrecent, float bottomEdgePrecent, float rightEdgePrecent, float topEdgePrecent, int a7, int a8, int a9)
	typedef int(__fastcall *funcDraw2DElement)(float*, int*, float edgeLeft, float edgeBottom, float edgeRight, float edgeTop, char*, int, int);
	((funcDraw2DElement)0x0076E480)(v9, v10, fLeftEdgePrecent, fBottomEdgePrecent, fRightEdgePrecent, fTopEdgePrecent, a5, a6, a7);
}
#pragma endregion

float *__fastcall menuButtonsScaling(float *a1, float *a2, float *a3)
{
	//Matrix * Matrix?
	float divFix = 1.3333333f;
	a2[1] = a2[1] / 2;
	float *result = a3;
	*a1 = (a3[8] * a2[2] + a2[1] * a3[4] + *a2 * *a3 + a2[3] * a3[12]); //35
	a1[1] = result[9] * a2[2] + a2[1] * result[5] + *a2 * result[1] + result[13] * a2[3];
	a1[2] = result[10] * a2[2] + a2[1] * result[6] + result[2] * *a2 + result[14] * a2[3];
	a1[3] = a2[3] * result[15] + a2[1] * result[7] + result[11] * a2[2] + result[3] * *a2;
	a1[4] = a2[5] * result[4] + a2[7] * result[12] + result[8] * a2[6] + a2[4] * *result;
	a1[5] = (result[5] * a2[5] + a2[4] * result[1] + result[13] * a2[7] + result[9] * a2[6]); //35
	a1[6] = a2[5] * result[6] + result[14] * a2[7] + result[10] * a2[6] + a2[4] * result[2];
	a1[7] = a2[4] * result[3] + a2[7] * result[15] + a2[6] * result[11] + a2[5] * result[7];
	a1[8] = a2[9] * result[4] + a2[11] * result[12] + result[8] * a2[10] + a2[8] * *result;
	a1[9] = result[5] * a2[9] + a2[8] * result[1] + result[13] * a2[11] + result[9] * a2[10];
	a1[10] = (a2[9] * result[6] + result[14] * a2[11] + result[10] * a2[10] + a2[8] * result[2]); //35
	a1[11] = a2[8] * result[3] + a2[11] * result[15] + a2[10] * result[11] + a2[9] * result[7];
	a1[12] = (a2[13] * result[4] + a2[15] * result[12] + result[8] * a2[14] + a2[12] * *result); //PositionX
	a1[13] = result[5] * a2[13] + a2[12] * result[1] + result[13] * a2[15] + result[9] * a2[14]; //PositionY
	a1[14] = a2[13] * result[6] + result[14] * a2[15] + result[10] * a2[14] + a2[12] * result[2]; //0
	a1[15] = a2[12] * result[3] + a2[15] * result[15] + a2[14] * result[11] + a2[13] * a3[7]; //1
	return result;
}
#pragma endregion

//Contructor
WidescreenFixes::WidescreenFixes(HMODULE moduleReference, int Width, int Height)
{
	this->baseModuleRef = moduleReference;
	this->Width = Width;
	this->Height = Height;
	this->AspectRatio = Width * 1.0f / Height;
	divChange = this->AspectRatio / (4.0f / 3.0f);

	StretchedAspectHeight = Height;
	StretchedAspectWidth = (int)(Height * (4.0f / 3.0f));
	StretchedAspectTop = 0;
	StretchedAspectLeft = (int)((Width / 2.0f)-(StretchedAspectWidth/2.0f));
	Hook((DWORD)(this->baseModuleRef) + 0x3B20C9, cinematicsRectangleOverride, &cinematicsRectangleOverrideReturn, 0x11);

	CorrectCameraAspectRatio(this->AspectRatio);
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

void WidescreenFixes::CorrectCrosshair()
{

	float leftEdge = *(float*)((DWORD)this->baseModuleRef + 0x97CF6) / divChange;
	float rightEdge = *(float*)((DWORD)this->baseModuleRef + 0x97D06) / divChange;
	*(float*)((DWORD)this->baseModuleRef + 0x97CF6) = leftEdge;
	*(float*)((DWORD)this->baseModuleRef + 0x97D06) = rightEdge;

}

//function to calculate hor+ FOV
float WidescreenFixes::ConvertFOV(float DesiredFOVInDegrees, float AspectRatio)
{
	double horRadian = M_PI * DesiredFOVInDegrees / 180;
	double verRadian = 2 * atan(tan(horRadian / 2) * (3.0 / 4.0));
	return (float)(2 * atan(tan(verRadian / 2) * AspectRatio)); //2 * Math.Atan(Math.Tan(VerticalRadians / 2) * (ResWidth * 1.0 / ResHeight * 1.0));
}

void WidescreenFixes::EnableHudCorrection()
{
	Hook((DWORD)(this->baseModuleRef) + 0x36E390, aspectCorrectionUI2DDraw, 0x9F);
	//Hook((DWORD)(this->baseModuleRef) + 0x1B7F80, menuButtonsScaling, 0x5);


	//*(float*)((DWORD)baseModule + 0x83229)  = 2;//1.3962635; //CombatCameraFOV
	//*(float*)((DWORD)baseModule + 0x29BCC8) = 100;//60; //TorchFOV (Flashlight FOV)
	//*(float*)((DWORD)baseModule + 0x29B8CD) = 100;//50 //TorchFOV1
	//*(float*)((DWORD)baseModule + 0x29B501) = 100;//50 //TorchFOV2
	CorrectCrosshair();
}
