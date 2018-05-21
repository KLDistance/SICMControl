#define DLL_IMPLEMENT

#include "NextXYPos.h"

#define LOWER_LIMIT (DOUBLE)5
#define ROUGHXYZ_SMALLEST_STEP_UM 0.001
#define Y_PRECISE_NUM_LEVEL1 (DOUBLE)1.5
#define Y_PRECISE_NUM_LEVEL2 (DOUBLE)2.0
#define Y_PRECISE_NUM_LEVEL3 (DOUBLE)2.5
#define Y_PRECISE_NUM_LEVEL4 (DOUBLE)3.0
#define Y_PRECISE_NUM_LEVEL5 (DOUBLE)3.5
#define Y_PRECISE_NUM_LEVEL6 (DOUBLE)4.0
#define Y_PRECISE_NUM_LEVEL7 (DOUBLE)4.5

extern DOUBLE errorDel = 1E-10;

DOUBLE Abs(DOUBLE num)
{
	if (num >= 0)
		return num;
	else
		return -num;
}

INT Abs(INT num)
{
	if (num >= 0)
		return num;
	else
		return -num;
}

DOUBLE findMax(DOUBLE num1, DOUBLE num2)
{
	if (num1 >= num2)
		return num1;
	else
		return num2;
}

DOUBLE XMovRedefine(DOUBLE xDelta, DOUBLE zDel, DOUBLE zRisingLimitLen, DOUBLE xInitStepLen)
{
	DOUBLE newXMovDis = xDelta * zRisingLimitLen / zDel;
	if (xDelta > errorDel)
	{
		if (zDel > zRisingLimitLen)
		{
			if (newXMovDis <= ROUGHXYZ_SMALLEST_STEP_UM)
			{
				return newXMovDis;
			}
			else
			{
				return ROUGHXYZ_SMALLEST_STEP_UM;
			}
		}
		else if (zDel < zRisingLimitLen / LOWER_LIMIT)
		{
			if (newXMovDis <= xInitStepLen)
			{
				return newXMovDis;
			}
			else
			{
				return xInitStepLen;
			}
		}
		else
			return xDelta;
	}
	else
	{
		return xDelta;
	}
}

DOUBLE YMovRedefine(INT xCounting, INT xPreviousNum, DOUBLE yInitStepLen)
{
	if (xCounting <= xPreviousNum * Y_PRECISE_NUM_LEVEL1)
	{
		return yInitStepLen;
	}
	else if (xCounting > xPreviousNum * Y_PRECISE_NUM_LEVEL1 && xCounting <= Y_PRECISE_NUM_LEVEL2)
	{
		return findMax(yInitStepLen / Y_PRECISE_NUM_LEVEL1, ROUGHXYZ_SMALLEST_STEP_UM);
	}
	else if (xCounting > xPreviousNum * Y_PRECISE_NUM_LEVEL2 && xCounting <= Y_PRECISE_NUM_LEVEL3)
	{
		return findMax(yInitStepLen / Y_PRECISE_NUM_LEVEL2, ROUGHXYZ_SMALLEST_STEP_UM);
	}
	else if (xCounting > xPreviousNum * Y_PRECISE_NUM_LEVEL3 && xCounting <= Y_PRECISE_NUM_LEVEL4)
	{
		return findMax(yInitStepLen / Y_PRECISE_NUM_LEVEL3, ROUGHXYZ_SMALLEST_STEP_UM);
	}
	else if (xCounting > xPreviousNum * Y_PRECISE_NUM_LEVEL4 && xCounting <= Y_PRECISE_NUM_LEVEL5)
	{
		return findMax(yInitStepLen / Y_PRECISE_NUM_LEVEL4, ROUGHXYZ_SMALLEST_STEP_UM);
	}
	else if (xCounting > xPreviousNum * Y_PRECISE_NUM_LEVEL5 && xCounting <= Y_PRECISE_NUM_LEVEL6)
	{
		return findMax(yInitStepLen / Y_PRECISE_NUM_LEVEL5, ROUGHXYZ_SMALLEST_STEP_UM);
	}
	else if (xCounting > xPreviousNum * Y_PRECISE_NUM_LEVEL6 && xCounting <= Y_PRECISE_NUM_LEVEL7)
	{
		return findMax(yInitStepLen / Y_PRECISE_NUM_LEVEL6, ROUGHXYZ_SMALLEST_STEP_UM);
	}
	else
	{
		return findMax(yInitStepLen / Y_PRECISE_NUM_LEVEL7, ROUGHXYZ_SMALLEST_STEP_UM);
	}
}

INT ModifyNextPos(
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
	DOUBLE zRisingLimitLen)
{
	if (!(*xCounting == 0 && yCounting == 0))
	{
		*xStepLen = XMovRedefine(*xStepLen, Abs((lastZPos - preZPos)*3.0518*1E-4), zRisingLimitLen, xInitStepLen);
		if (*xVel == 2 || *xVel == -2)
		{
			*yStepLen = YMovRedefine(*xCounting, xPreviousNum, yInitStepLen);
		}
	}

	if (*calXPos + (*xStepLen) * (*xVel) > xMaxMov && *xVel == 1)
	{
		*calXPos = xMaxMov;
		*xVel = 2;
		*xCounting++;
		return 2;
	}
	else if (*calXPos + (*xStepLen) * (*xVel) < xInitPt && *xVel == -1)
	{
		*calXPos = xInitPt;
		*xVel = -2;
		*xCounting++;
		return 2;
	}
	else if (*xVel == 2)
	{
		*xCounting = 1;
		*calYPos += *yStepLen;
		*xVel = -1;
		*yCounting++;
		return 0;
	}
	else if (*xVel == -2)
	{
		*xCounting = 1;
		*calYPos += *yStepLen;
		*xVel = 1;
		*yCounting++;
		return 0;
	}
	else if (*calYPos + *yStepLen > yMaxMov && (*xVel == -2 || *xVel == 2))
	{
		return 1;
	}
	else
	{
		*calXPos += *xStepLen * (*xVel);
		*xCounting++;
	}
}