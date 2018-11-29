#include "FirstPersonHack.h"

HMODULE baseModuleRef;

float ExternalFunction(float * a1, float * a2, float * a3)
{
	double v3; // st7
	float *v4; // eax
	double v5; // st6
	double v6; // st5
	double v7; // st4
	double v8; // st3
	double v9; // st2
	float result; // eax
	float v11; // [esp+8h] [ebp+4h]

	v3 = *a2;
	v4 = a3;
	v5 = a2[1];
	v6 = a2[2];
	v7 = a2[3];
	v8 = *a3;
	v11 = a3[2];
	v9 = v4[1];
	result = v4[3];
	*a1 = v8 * v3 - v9 * v5 - v11 * v6 - result * v7;
	a1[1] = result * v6 + v8 * v5 + v9 * v3 - v11 * v7;
	a1[2] = v9 * v7 + v8 * v6 + v11 * v3 - result * v5;
	a1[3] = v8 * v7 + v11 * v5 + result * v3 - v9 * v6;
	return result;
}

float *__fastcall cameraAngelsThinkSo(float *a1, float *a2, float *a3)
{
	/*
	float *v3 = 0
	double v4; // st7
	float *result; // eax
	float v6 = 0; // [esp+4h] [ebp-50h]
	float v7; // [esp+8h] [ebp-4Ch]
	float v8; // [esp+Ch] [ebp-48h]
	float v9; // [esp+10h] [ebp-44h]
	float * v10 = new float[4]{ 0 }; // [esp+14h] [ebp-40h]
	int v11 = 0; // [esp+18h] [ebp-3Ch]
	int v12 = 0; // [esp+1Ch] [ebp-38h]
	float v13 = 1.0f; // [esp+20h] [ebp-34h]
	float v14 = 0; // [esp+24h] [ebp-30h]
	float v15; // [esp+28h] [ebp-2Ch]
	float v16; // [esp+2Ch] [ebp-28h]
	float v17; // [esp+30h] [ebp-24h]
	float * v18; // [esp+34h] [ebp-20h]
	int v19; // [esp+38h] [ebp-1Ch]
	int v20; // [esp+3Ch] [ebp-18h]
	int v21; // [esp+40h] [ebp-14h]
	float * v22 = new float[4]{ 0 }; // [esp+44h] [ebp-10h]
	*/

	float *v3; // esi
	double v4; // st7
	float *result; // eax
	float v6[4]; // [esp+4h] [ebp-50h]
	float v10[4];// [esp+14h] [ebp-40h]
	float v14[4]; // [esp+24h] [ebp-30h]
	float v18[4]; // [esp+34h] [ebp-20h]
	float v22[4]; // [esp+44h] [ebp-10h]
	
	v3 = a1;
	v6[0] = 0;
	v6[1] = a3[0];
	v6[2] = a3[1];
	v6[3] = a3[2];
	v14[0] = a2[0];
	v14[1] = -a2[1];
	v14[2] = -a2[2];
	v14[3] = -a2[3];
	ExternalFunction(v10, a2, v6);
	v18[0] = v10[0];
	v18[1] = v10[1];
	v18[2] = v10[2];
	v18[3] = v10[3];
	ExternalFunction(v22, v18, v14);
	v3[0] = v22[1];
	v3[1] = v22[2];
	v3[2] = v22[3];
	//std::string debugStr = "Result is: " + std::to_string(v3[0]) + ", " + std::to_string(v3[1]) + ", " + std::to_string(v3[2]) + ", " + std::to_string(v3[3]) + ".\r\n";
	//OutputDebugString(debugStr.c_str());
	return v3;
}

FirstPersonHack::FirstPersonHack(HMODULE baseModule)
{
	baseModuleRef = baseModule;
	Hook((DWORD)baseModule + 0x31ED90, cameraAngelsThinkSo, 6);
}


FirstPersonHack::~FirstPersonHack()
{
}
