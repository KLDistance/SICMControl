#pragma once

#include <Windows.h>

#ifdef DLL_IMPLEMENT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

DLL_API INT ModifyNextPos(
	DOUBLE xMaxMov,
	DOUBLE yMaxMov,
	DOUBLE xInitPt,
	INT xPreviousNum,
	DOUBLE* calXPos,
	DOUBLE* calYPos,
	INT* xCounting,
	INT* yCounting,
	INT* xVel,
	INT lastZPos,
	INT preZPos,
	DOUBLE* xStepLen,
	DOUBLE* yStepLen,
	DOUBLE xInitStepLen,
	DOUBLE yInitStepLen,
	DOUBLE zRisingLimitLen
);