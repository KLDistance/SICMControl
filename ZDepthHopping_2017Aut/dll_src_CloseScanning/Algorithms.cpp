#define DLL_IMPLEMENT

#include "Output.h"

DLL_API VOID closeScan(
	DWORD* originalArr,
	DWORD originalRowNum,
	DWORD originalColNum,
	DOUBLE originalXStep,
	DOUBLE originalYStep,
	DWORD* preciseArr,
	DWORD preciseRowNum,
	DWORD preciseColNum,
	DOUBLE preciseXStep,
	DOUBLE preciseYStep,
	DWORD withdrawZIndex,
	DWORD steepWithdrawZIndex
)
{
	//Get the limit change in z direction. Once exceed, shift to steep withdraw condition
	zLimitChange = withdrawZIndex * 2 / 3;

	//Copy the passing in parameters to global variables
	originalXStepNum = originalRowNum;
	originalYStepNum = originalColNum;

	originalXStepLen = originalXStep;
	originalYStepLen = originalYStep;

	preciseXStepNum = preciseRowNum;
	preciseYStepNum = preciseColNum;

	preciseXStepLen = preciseXStep;
	preciseYStepLen = preciseYStep;

	ZWithdrawIndex = withdrawZIndex;
	SteepZWithdrawIndex = steepWithdrawZIndex;

	//Initialize the matrix
	ArrToMatrix(originalArr, originalRowNum, originalColNum, originalZMat);
	CreateMat(flagZMat, originalRowNum, originalColNum);
	CreateMat(withdrawZMat, preciseYStepNum, preciseXStepNum);

	//Find the flags
	findZFlags(*originalZMat, flagZMat);

	//Copy the z withdraw information to the new matrix
	CreateWithdrawMat(*originalZMat, *flagZMat, withdrawZMat);

	//Transfer the withdrawZMat into array
	MatrixToArr(*withdrawZMat, preciseArr);

	//Release all the matrix
	ReleaseMatrix(originalZMat);
	ReleaseMatrix(flagZMat);
	ReleaseMatrix(withdrawZMat);
}

DWORD CreateMat(MATVAR* Mat, DWORD rowNum, DWORD colNum)
{
	DWORD i;
	DWORD j;
	//Assign the new space for matrix
	Mat = new MATVAR(0);

	//Resize the matrix
	(*Mat).resize(rowNum);
	for (i = 0; i < rowNum; i++)
	{
		(*Mat)[i].resize(colNum);
	}

	//Set the matrix initial value to be 0
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			(*Mat)[i][j] = 0;
		}
	}
	return 0;
}

DWORD ArrToMatrix(DWORD* Arr, DWORD rowNum, DWORD colNum, MATVAR* Mat)
{
	DWORD i;
	DWORD j;
	//Assign the new space for matrix
	originalZMat = new MATVAR(0);

	//Resize the matrix
	(*Mat).resize(rowNum);
	for (i = 0; i < rowNum; i++)
	{
		(*Mat)[i].resize(colNum);
	}

	//Copy the value into the matrix
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			(*Mat)[i][j] = Arr[i * rowNum + j];
		}
	}
	return 0;
}

DWORD MatrixToArr(MATVAR Mat, DWORD* Arr)
{
	DWORD i;
	DWORD j;
	DWORD rowNum = Mat.size();
	DWORD colNum = Mat[0].size();

	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			Arr[i * rowNum + j] = Mat[i][j];
		}
	}
	return 0;
}

DWORD ReleaseMatrix(MATVAR* Mat)
{
	DWORD i;
	DWORD yNum = (*Mat).size();
	for (i = 0; i < yNum; i++)
	{
		(*Mat)[i].clear();
	}
	(*Mat).clear();
	return 0;
}

DWORD findZFlags(MATVAR originalZMat, MATVAR* flagMat)
{
	DWORD rowNum = originalZMat.size();
	DWORD colNum = originalZMat[0].size();
	DWORD right = 0;
	DWORD left = 0;
	DWORD down = 0;
	DWORD up = 0;
	DWORD i;
	DWORD j;
	DWORD k;

	//Evaluate the fluctuation of surface
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			//Judge the present point
			if (originalZMat[i][j] > zLimitChange)
			{
				(*flagMat)[i][j] = 1;
				continue;
			}
			for (k = 0; k < confirmPointNum; k++)
			{
				left = j - k - 1;
				right = j + k + 1;
				up = i - k - 1;
				down = i + k + 1;

				//Left column
				if (left >= 0)
				{
					//Left upward vertex
					if (up >= 0)
					{
						if (originalZMat[up][left] > zLimitChange)
						{
							(*flagMat)[up][left] = 1;
							break;
						}
					}
					//Left edge
					if (originalZMat[i][left] > zLimitChange)
					{
						(*flagMat)[i][left] = 1;
						break;
					}
					//Left downward vertex
					if (down <= rowNum)
					{
						if (originalZMat[down][left])
						{
							(*flagMat)[down][left] = 1;
							break;
						}
					}
				}

				//Right column
				if (right <= colNum)
				{
					//Right upward vertex
					if (up >= 0)
					{
						if (originalZMat[up][right] > zLimitChange)
						{
							(*flagMat)[up][right] = 1;
							break;
						}
					}

					//Right edge
					if (originalZMat[i][right] > zLimitChange)
					{
						(*flagMat)[i][right] = 1;
						break;
					}
					//Right downward vertex
					if (down <= rowNum)
					{
						if (originalZMat[down][right] > zLimitChange)
						{
							(*flagMat)[down][right] = 1;
							break;
						}
					}
				}

				//Middle column
				
				//upward vertex
				if (up >= 0)
				{
					if (originalZMat[up][j] > zLimitChange)
					{
						(*flagMat)[up][j] = 1;
						break;
					}
				}

				//downward vertex
				if (down <= rowNum)
				{
					if (originalZMat[down][j] > zLimitChange)
					{
						(*flagMat)[down][j] = 1;
						break;
					}
				}
				
			}
		}
	}
	return 0;
}

DWORD CreateWithdrawMat(MATVAR originalMat, MATVAR flagMat, MATVAR* newZMat)
{
	DOUBLE XStepMultiplier = originalXStepLen / preciseXStepLen;
	DOUBLE YStepMultiplier = originalYStepLen / preciseYStepLen;
	DWORD i;
	DWORD j;
	DWORD xTmpPosIndex = 0;
	DWORD yTmpPosIndex = 0;
	DWORD rowNum = (*newZMat).size();
	DWORD colNum = (*newZMat)[0].size();

	//Assign the depth information to the new matrix
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			xTmpPosIndex = (DWORD)(XStepMultiplier * i);
			yTmpPosIndex = (DWORD)(YStepMultiplier * j);
			if (flagMat[xTmpPosIndex][yTmpPosIndex])
			{
				(*newZMat)[i][j] = originalMat[xTmpPosIndex][yTmpPosIndex] - SteepZWithdrawIndex;
			}
			else
			{
				(*newZMat)[i][j] = originalMat[xTmpPosIndex][yTmpPosIndex] - ZWithdrawIndex;
			}
		}
	}
	return 0;
}