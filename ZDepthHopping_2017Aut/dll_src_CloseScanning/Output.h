#pragma once
#include <Windows.h>
#include <stdio.h>
#include <vector>

#ifdef DLL_IMPLEMENT 
#define DLL_API __declspec(dllexport)
#else 
#define DLL_API __declspec(dllimport)
#endif

typedef std::vector<std::vector<DWORD>> MATVAR;

/*
closeScan is an export method to get the parameters from labview

P1: The chosen array from DeepScanning
P2: Row number of the original array
P3: Column number of the original array
P4: X step length of the original array
P5: Y step length of the original array

P6: The precise array (empty)
P7: Row number of the precised array
P8: Column number of the precised array
P9: X step length of the precised array
P10: Y step length of the precised array

P11: Withdrawn distance from surface under normal state or plain
P12: Withdrawn distance from surface under steep condition
*/
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
);

/*
CreateMat is used to generate a matrix with certain number of columns and rows
Initial value is 0

P1: Handle of the matrix
P2: The row number desired 
P3: The column number desired
*/
DWORD CreateMat(MATVAR* Mat, DWORD rowNum, DWORD colNum);

/*
ArrToMatrix is used to transfer an 1D array into 2D matrix

P1: Original array
P2: Row number of array
P3: Column number of array
P4: Input handle of the matrix
*/
DWORD ArrToMatrix(DWORD* Arr, DWORD rowNum, DWORD colNum, MATVAR* Mat);

/*
findZFlags is used to fine the withdraw point 

P1: Original matrix containing Z depth information
P2: Matrix containing flags to let the Z axis withdraw in case of collision
*/
DWORD findZFlags(MATVAR originalZMat, MATVAR* flagMat);

/*
CreateWithdrawMat is used to create a matrix containing the withdraw step information of tip

P1: Original Z depth matrix
P2: Flag matrix
P3: New Z withdraw matrix
*/
DWORD CreateWithdrawMat(MATVAR originalMat, MATVAR flagMat, MATVAR* newZMat);

/*
ReleaseMatrix is used to release an unused matrix

P1: The matrix wanting to be aborted
*/
DWORD ReleaseMatrix(MATVAR* Mat);

/*
MatrixToArr is used to transfer a matrix into an array

P1: The matrix
P2: The array
*/
DWORD MatrixToArr(MATVAR Mat, DWORD* Arr);

//Global variables

//The limit of Z hight changed
DWORD zLimitChange = 100;

//Confirm points
DWORD confirmPointNum = 3;

//Withdraw reserve steps
DWORD ZWithdrawIndex = 0;
DWORD SteepZWithdrawIndex = 0;

//Parameters passing in
DWORD originalXStepNum = 0;
DWORD originalYStepNum = 0;

DOUBLE originalXStepLen = 0;
DOUBLE originalYStepLen = 0;

DWORD preciseXStepNum = 0;
DWORD preciseYStepNum = 0;

DOUBLE preciseXStepLen = 0;
DOUBLE preciseYStepLen = 0;

//Matrices

//Matrix containing the original Z depth
MATVAR* originalZMat = NULL;

//Matrix containing flags of steep withdraw
MATVAR* flagZMat = NULL;

//Matrix containing withdraw distance in Z direction
MATVAR* withdrawZMat = NULL;
