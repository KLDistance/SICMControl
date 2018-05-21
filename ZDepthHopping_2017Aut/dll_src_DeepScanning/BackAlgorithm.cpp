#include "BackAlgorithm.h"

DWORD ArrToMatrix(IN DWORD* Arr, IN DWORD rowNum, IN DWORD colNum, OUT MATVAR* Mat)
{
	DWORD i;
	DWORD j;

	//Resize the matrix to contain the contents from array
	(*Mat).resize(rowNum);
	for (i = 0; i < rowNum; i++)
	{
		(*Mat)[i].resize(colNum);
	}

	//Copy the value to the matrix
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			(*Mat)[i][j] = Arr[i * rowNum + j];
		}
	}
	return 0;
}

DWORD MatrixToArr(MATVAR Mat, DWORD** Arr)
{
	DWORD rowNum = Mat.size();
	DWORD colNum = Mat[0].size();
	DWORD i;
	DWORD j;
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			(*Arr)[i * rowNum + j] = Mat[i][j];
		}
	}
	return 0;
}

DWORD ReleaseMatrix(MATVAR* Mat)
{
	DWORD rowNum = (*Mat).size();
	DWORD i;

	//Clear the contents in each row
	for (i = 0; i < rowNum; i++)
	{
		(*Mat)[i].clear();
	}

	//Clear the row contents
	(*Mat).clear();

	return 0;
}

DWORD CreateSubMat(MATVAR Mat, MATVAR* subMat, DWORD xBase, DWORD yBase, DWORD xNum, DWORD yNum)
{
	DWORD i;
	DWORD j;
	//Assign the space for the matrix
	subMat = new MATVAR(0);

	//Resize for the sub matrix
	(*subMat).resize(xNum);
	for (i = 0; i < xNum; i++)
	{
		(*subMat)[i].resize(yNum);
	}

	//Extract the sub matrix
	for (i = 0; i < xNum; i++)
	{
		for (j = 0; j < yNum; j++)
		{
			(*subMat)[i][j] = Mat[xBase + i][yBase + j];
		}
	}

	return 0;
}