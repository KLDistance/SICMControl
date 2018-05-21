#pragma once
#include <Windows.h>
#include <stdio.h>
#include <vector>

//Define the dll export

#ifdef DLL_IMPLEMENT 
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//Define the thread exit codes

#define SHOWWIN_EXIT 0x100

//Define the macros of frame parameters

#define WINDOW_WIDTH 1250
#define WINDOW_HEIGHT 800

#define PLOTAREA_UP 70
#define PLOTAREA_DOWN 670
#define PLOTAREA_LEFT 70
#define PLOTAREA_RIGHT 670

#define PLOTLEN_X (PLOTAREA_RIGHT - PLOTAREA_LEFT)
#define PLOTLEN_Y (PLOTAREA_DOWN - PLOTAREA_UP)

#define EDGE 2

#define INTERVAL_TO_SCALEBAR 50
#define SCALEBAR_WIDTH 30
#define SCALEBAR_PIXEL 50

//Functions

/*
DllMain is used to define the overall entry of the dll.

*/
BOOL __stdcall DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

/*
WinStart is used to start the window process

This is a method to pass in parameters ,pointers and handles

P1: Original data array of depth data (only in the form of 1D array)

P2: Row number of array of depth data
P3: Col number of array of depth data

P4: Undecided row number of precise scan row number
P5: undecided column number of precise scan column number

P6: X step length in the unit of micrometer
P7: Y step length in the unit of micrometer
When the dll is attached to the labview, the ul_reason_for_call is 0x1.
This allows the initialization of the hInstance of the window

return value: Simulation of the dll attachment.

P8: X initial position
P9: Y initial position
*/
DLL_API VOID WinStart(
	DWORD* dataArr,
	DWORD* arrRow,
	DWORD* arrCol,
	DWORD* preciseRow,
	DWORD* preciseCol,
	DOUBLE* XStepLen,
	DOUBLE* YStepLen,
	DOUBLE XInitPos,
	DOUBLE YInitPos
);

/*
ShowWin is used to load the interface onto the screen and loop for the interface message

P1 is the pointer to the parameters

Thread exit code is 0x10
*/
DWORD __stdcall ShowWin(LPVOID lpParameter);

/*
WinProc is used to loop for the interface message

P1 is the instance of the window frame.
P2 is the message type
P3 is a supplement message to P2 parameter
P4 is an another supplement message to P2 parameter

Default return is the leftover message to the operating system
*/
LRESULT __stdcall WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*
InitWindow is used to attach all the necessary instance onto the HWND type
For instance, HDC, set up matrix and so on.
*/
VOID InitWindow(HWND hwnd);

/*
Prepaint is used to draw the intensity plot onto the screen
*/
VOID PrePaint(MATVAR Mat);

/*
DrawWindowRect is used to draw a region chosen when lbutton is down, onto the window to indicate the desired part of intensity of the depth
*/
VOID DrawWindowRect();

/*
DrawScaleBar is used to provide a scale indicator of depth
*/
VOID DrawScaleBar();

/*
DrawFont is used to draw all the texts in the window
*/
VOID DrawFont();

//Variables

//The global hInstance of the window process (Image Base)
HINSTANCE hAppInstance = NULL;

//The global components on the window frame
HWND XStepInputBox;
HWND YStepInputBox;

//The global DC for the screen
HDC baseDC = NULL;

//The global handles of sub threads
//Window running thread
HANDLE hWinThread;

//The global event structure
//The WinStart block
HANDLE hMainEvent;

//Basic info of the movement
struct
{
	DWORD isDown = 0;
	SHORT xInitPos = 0;
	SHORT xPresPos = 0;
	SHORT yInitPos = 0;
	SHORT yPresPos = 0;
} info;

//Arrays, positions and steps passing in

//Vector containing Z height
MATVAR originalZMat;
MATVAR subZMat;

//Row and column numbers of Array passing in
DWORD originalColNum = 0;
DWORD originalRowNum = 0;

//Step length passing in
DOUBLE ScanXStep = 0;
DOUBLE ScanYStep = 0;

//Scan X initial position passing in
DOUBLE ScanXInitPos = 0;
DOUBLE ScanYInitPos = 0;

//Input max and min Z height
DWORD minZHeight = 0;
DWORD maxZHeight = 0;

//Position and steps renewed

//New step length after zooming in
DOUBLE NewXStepLen = 0;
DOUBLE NewYStepLen = 0;

//New step number
DWORD NewXStepNum = 0;
DWORD NewYStepNum = 0;

//New initial and ending position
DOUBLE NewXInit = 0;
DOUBLE NewYInit = 0;
DOUBLE NewXEnd = 0;
DOUBLE NewYEnd = 0;

//Chosen row and column number
DWORD chosenRowNum = 0;
DWORD chosenColNum = 0;

//Variables for drawing the frame contents
DWORD XSlotSize = 1;
DWORD YSlotSize = 1;

DWORD XSumSize = 1;
DWORD YSumSize = 1;

DWORD ScaleBar_XPos = 1;

DWORD ScaleBar_Font_XPos = 1;

DWORD ScanInfo_XPos = 1;


//Temp buffer tanks
CHAR SizeInfo[256] = { 0 };
CHAR ZoomInfo[256] = { 0 };
CHAR NewLenInfo[16] = { 0 };

BOOL inPlace = 0;

//Get the return of messagebox choice
INT MessageBoxRet = 0;