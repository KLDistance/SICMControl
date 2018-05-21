#pragma once
#include <Windows.h>
#include <vector>

typedef std::vector<std::vector<DWORD>> MATVAR;

/*
ArrToMatrix is used to transfer an 1DArray (which is confined during passing from labview to dll source code) to a 2D matrix

P1: Array to be transfered
P2: Row number of the array
P3: Column number of the array
P4: Handle of the matrix
*/
DWORD ArrToMatrix(IN DWORD* Arr, IN DWORD rowNum, IN DWORD colNum, OUT MATVAR* Mat);

/*
MatrixToArr is used to transfer a matrix into an 1DArray

P1: Pointer of matrix
P2: Handle of array
*/
DWORD MatrixToArr(MATVAR Mat, DWORD** Arr);

/*
ReleaseMatrix is used to release the space of a matrix in case of memory leak

P1: Handle of the matrix
*/
DWORD ReleaseMatrix(MATVAR* Mat);

/*
CreateSubMat is used to extract a sub matrix from the original matrix

P1: Original matrix to be extracted
P2: The handle of sub matrix extracted from the original matrix
P3: The beginning column the sub matrix starts to extract from the original matrix
P4: The beginning row the sub matrix starts to extract from the original matrix
P5: The extracting column number
P6: The extracting row number
*/
DWORD CreateSubMat(MATVAR Mat, MATVAR* subMat, DWORD xBase, DWORD yBase, DWORD xNum, DWORD yNum);