#define DLL_IMPLEMENT

#include "BackAlgorithm.h"
#include "DllOutput.h"

BOOL __stdcall DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		//If the dll is attached onto the LabView, the following codes will automatically run to the end
	case DLL_PROCESS_ATTACH:
		//Import the hModule to the hInstance
		hAppInstance = (HINSTANCE)hModule;
		//Declare the window processing thread but suspended in a moment
		hWinThread = ::CreateThread(NULL, NULL, ShowWin, NULL, CREATE_SUSPENDED, NULL);
		break;
	}
	return TRUE;
}

DLL_API VOID WinStart(
	DWORD* dataArr,
	DWORD* arrRow,
	DWORD* arrCol,
	DWORD* preciseRow,
	DWORD* preciseCol,
	DOUBLE* XStepLen,
	DOUBLE* YStepLen,
	DOUBLE* XInitPos,
	DOUBLE* YInitPos
)
{
	//Pass the input parameters into global variables' position

	//Alter the input array to matrix (2D vector)
	ArrToMatrix(dataArr, *arrRow, *arrCol, &originalZMat);

	//Pass the step and position parameters
	originalRowNum = originalZMat.size();
	originalColNum = originalZMat[0].size();

	ScanXStep = *XStepLen;
	ScanYStep = *YStepLen;

	ScanXInitPos = *XInitPos;
	ScanYInitPos = *YInitPos;

	//Seek the max and min Z height of the matrix
	DWORD i;
	DWORD j;
	DWORD iLoopNum = originalZMat.size();
	DWORD jLoopNum = originalZMat[0].size();

	maxZHeight = originalZMat[0][0];
	minZHeight = originalZMat[0][0];

	for (i = 0; i < iLoopNum; i++)
	{
		for (j = 0; j < jLoopNum; j++)
		{
			if (originalZMat[i][j] > maxZHeight)
			{
				maxZHeight = originalZMat[i][j];
			}
			if (originalZMat[i][j] < minZHeight)
			{
				minZHeight = originalZMat[i][j];
			}
		}
	}

	//Continue the process of window process
	::ResumeThread(hWinThread);

	//Set block to the WinStart
	hMainEvent = ::CreateEventA(NULL, FALSE, FALSE, "MainEvent");
	
	//Set short interval for value return and err check
	Sleep(50);

	//Terminate the Main window thread (This is a security of dead thread)
	::TerminateThread(hWinThread, SHOWWIN_EXIT);
	::WaitForSingleObject(hWinThread, INFINITE);

	//Extract the sub matrix from the original matrix
	DWORD xBase = (DWORD)(((DOUBLE)(info.xInitPos - PLOTAREA_LEFT)) / PLOTLEN_X * originalColNum);
	DWORD yBase = (DWORD)(((DOUBLE)(info.yInitPos - PLOTAREA_UP)) / PLOTLEN_Y * originalRowNum);
	CreateSubMat(originalZMat, &subZMat, xBase, yBase, chosenColNum, chosenRowNum);

	//Copy the value to the in passing ptr type of parameters
	*arrRow = chosenRowNum;
	*arrCol = chosenColNum;
	*preciseRow = NewYStepNum;
	*preciseCol = NewXStepNum;
	*XStepLen = NewXStepLen;
	*YStepLen = NewYStepLen;
	*XInitPos = NewXInit;
	*YInitPos = NewYInit;
	MatrixToArr(subZMat, &dataArr);
	ReleaseMatrix(&originalZMat);
	ReleaseMatrix(&subZMat);

}

DWORD __stdcall ShowWin(LPVOID lpParameter)
{
	//Declare the class of window instance
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WinProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hAppInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("DeepScan");

	//Register the class
	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	//Sign the pattern for the window
	HWND hwnd = CreateWindowEx(
		NULL,
		TEXT("DeepScan"),
		TEXT("Mouse Drag Frame"),
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
		100,
		50,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hAppInstance,
		NULL
	);

	//Move show, and update window
	MoveWindow(hwnd, 200, 50, WINDOW_WIDTH, WINDOW_HEIGHT, TRUE);
	ShowWindow(hwnd, 0x1);
	UpdateWindow(hwnd);

	//Initialize the fundamental information of window and parameters
	InitWindow(hwnd);

	//Create a message structure to contain the information
	MSG msg = { 0 };
	//Information loop for obtaining message
	while (msg.message != WM_QUIT)
	{
		//If there is any operation
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			//Send the message back to the operating system
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//If the message is 0 or user does not give any operation
		else
		{
			//Draw the intensity plot
			DrawWindowRect();
		}
	}

	//Release the DC from HWND
	ReleaseDC(hwnd, baseDC);

	//Unregister the class from operating system
	UnregisterClass(TEXT("DeepScan"), wndClass.hInstance);

	//Resume the event block in the main method
	::SetEvent(hMainEvent);

	return 0;
}

LRESULT __stdcall WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//If the mouse's left button is down
	case WM_LBUTTONDOWN:

		//Set the button down state to 1
		info.isDown = 1;

		//Extract the x initial position and y initial position from LPARAM
		info.xInitPos = (SHORT)LOWORD(lParam);
		info.yInitPos = (SHORT)HIWORD(lParam);

		//Confine the position of cursor
		if (info.xInitPos > PLOTAREA_LEFT + XSumSize)
		{
			info.xInitPos = PLOTAREA_LEFT + XSumSize;
		}
		else if (info.yInitPos > PLOTAREA_UP + YSumSize)
		{
			info.yInitPos = PLOTAREA_UP + YSumSize;
		}
		else if (info.xInitPos < PLOTAREA_LEFT)
		{
			info.xInitPos = PLOTAREA_LEFT;
		}
		else if (info.xInitPos < PLOTAREA_UP)
		{
			info.yInitPos = PLOTAREA_UP;
		}
		else
		{
			inPlace = 1;
		}

		//Copy the value to the present position
		info.xPresPos = info.xInitPos;
		info.yPresPos = info.yInitPos;

		break;
	case WM_LBUTTONUP:

		//If the cursor is released within the intensity plot
		if (inPlace)
		{
			//Gather the info of the EditBox
			memset(NewLenInfo, 0, 16);
			GetWindowTextA(XStepInputBox, NewLenInfo, 16);
			sscanf(NewLenInfo, "%u", &NewXStepNum);

			memset(NewLenInfo, 0, 16);
			GetWindowTextA(XStepInputBox, NewLenInfo, 16);
			sscanf(NewLenInfo, "%u", &NewYStepNum);

			//Calculate the new initial position
			NewXInit = ((DOUBLE)(info.xInitPos - PLOTAREA_LEFT)) / PLOTLEN_X * originalColNum * ScanXStep + ScanXInitPos;
			NewYInit = ((DOUBLE)(info.yInitPos - PLOTAREA_UP)) / PLOTLEN_Y * originalRowNum * ScanYStep + ScanYInitPos;
			NewXEnd = ((DOUBLE)(info.xPresPos - PLOTAREA_LEFT)) / PLOTLEN_X * originalColNum * ScanXStep + ScanXInitPos;
			NewYEnd = ((DOUBLE)(info.yPresPos - PLOTAREA_UP)) / PLOTLEN_X * originalRowNum * ScanYStep + ScanYInitPos;

			//Give restriction to the minimum step
			if (NewXStepNum == 0)
				NewXStepNum = 1;
			if (NewYStepNum == 0)
				NewYStepNum = 1;

			//Calculate the entry numbers chosen base on original matrix
			chosenRowNum = (NewYEnd - NewYInit) / ScanYStep;
			chosenColNum = (NewXEnd - NewXInit) / ScanXStep;

			//Calculate the new step length
			NewXStepLen = (NewXEnd - NewXInit) / NewXStepNum;
			NewYStepLen = (NewYEnd - NewYInit) / NewYStepNum;

			//Indicating DialogBox
			memset(ZoomInfo, 0, 256);
			sprintf(ZoomInfo, "The choosen area from (%lf um, %lf um) to (%lf um, %lf um), with the X step length = %lf um\nAre you sure to zoom into this new scan area?", 
				NewXInit, NewYInit, NewXEnd, NewYEnd, NewXStepLen, NewYStepLen);
			MessageBoxRet = MessageBoxA(hwnd, ZoomInfo, "Comfirm information", MB_OKCANCEL);
			switch (MessageBoxRet)
			{
			case 1:
				//Quit the thread with a chosen area
				PostQuitMessage(0);
				break;
			case 2:
				//Clear the info
				info.isDown = 0;
				info.xInitPos = 0;
				info.yInitPos = 0;
				info.xPresPos = 0;
				info.yPresPos = 0;

				//Restore the map
				PrePaint(originalZMat);
				break;
			}
			inPlace = 0;
			break;
		}
		else
		{
			info.isDown = 0;
			info.xInitPos = 0;
			info.yInitPos = 0;
			info.xPresPos = 0;
			info.yPresPos = 0;

			//Restore the map
			PrePaint(originalZMat);
		}
		break;
	case WM_MOUSEMOVE:
		//If the mouse is in the down start, execute the following codes
		if (info.isDown)
		{
			info.xPresPos = (SHORT)LOWORD(lParam);
			info.yPresPos = (SHORT)HIWORD(lParam);
			if (info.xPresPos > PLOTAREA_LEFT + XSumSize)
			{
				info.xPresPos = PLOTAREA_LEFT + XSumSize;
			}
			if (info.yPresPos > PLOTAREA_UP + YSumSize)
			{
				info.yPresPos = PLOTAREA_UP + YSumSize;
			}
			if (info.xPresPos < PLOTAREA_LEFT)
			{
				info.xPresPos = PLOTAREA_LEFT;
			}
			if (info.yPresPos < PLOTAREA_UP)
			{
				info.yPresPos = PLOTAREA_UP;
			}
		}
		break;
	case WM_DESTROY:
		//Quit the thread without giving a chosen area
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

VOID InitWindow(HWND hwnd)
{
	//Initialize necessary variables of window frames
	XSlotSize = max((PLOTAREA_RIGHT - PLOTAREA_LEFT) / originalColNum, 1);
	YSlotSize = max((PLOTAREA_DOWN - PLOTAREA_UP) / originalRowNum, 1);

	XSumSize = XSlotSize * originalColNum;
	YSumSize = YSlotSize * originalRowNum;

	ScaleBar_XPos = PLOTAREA_LEFT + XSumSize + INTERVAL_TO_SCALEBAR;
	
	ScaleBar_Font_XPos = ScaleBar_XPos + SCALEBAR_WIDTH + EDGE * 5;

	ScanInfo_XPos = ScaleBar_Font_XPos + 80;

	//Get the window DC
	baseDC = GetDC(hwnd);

	//Prepaint the center intensity diagram onto the screen
	PrePaint(originalZMat);

	//Draw the Scale bar as a depth indicator
	DrawScaleBar();

	//Draw the font
	DrawFont();

	//Draw the two input boxes
	XStepInputBox = CreateWindow
	(
		TEXT("EDIT"),
		NULL,
		WS_VISIBLE | WS_CHILDWINDOW | WS_BORDER, 
		ScanInfo_XPos,
		PLOTAREA_UP + 180,
		250,
		30,
		hwnd,
		NULL,
		hAppInstance,
		NULL
	);

	YStepInputBox = CreateWindow
	(
		TEXT("EDIT2"),
		NULL,
		WS_VISIBLE | WS_CHILDWINDOW | WS_BORDER,
		ScanInfo_XPos,
		PLOTAREA_UP + 260,
		250,
		30,
		hwnd,
		NULL,
		hAppInstance,
		NULL
	);

}

VOID PrePaint(MATVAR Mat)
{
	//Set the border of the intensity plot
	RECT border;
	HBRUSH hBorderBrush = CreateSolidBrush(RGB(0, 0, 0));
	SetRect(&border, PLOTAREA_LEFT - EDGE, PLOTAREA_UP - EDGE, PLOTAREA_LEFT + XSumSize + EDGE, PLOTAREA_UP + YSumSize + EDGE);
	FrameRect(baseDC, &border, hBorderBrush);
	DeleteObject(hBorderBrush);

	//Draw the contents of the Mat onto the intensity plot
	DWORD i;
	DWORD j;
	DWORD iLoopNum = Mat.size();
	DWORD jLoopNum = Mat[0].size();
	RECT rect;
	UINT R = 0;
	UINT G = 0;
	UINT B = 0;
	UINT tmpVar = 0;
	for (i = 0; i < iLoopNum; i++)
	{
		for (j = 0; j < jLoopNum; j++)
		{
			HBRUSH rectBrush;
			tmpVar = (UINT)(((DOUBLE)Mat[i][j] - minZHeight) / (maxZHeight - minZHeight) * 255);
			R = 255;
			G = tmpVar;
			B = tmpVar;

			//Draw the intensity plot region
			SetRect(&rect, XSlotSize * j + PLOTAREA_LEFT, YSlotSize * i + PLOTAREA_UP,
				XSlotSize * (j + 1) + PLOTAREA_LEFT, YSlotSize * (i + 1) + PLOTAREA_UP);
			rectBrush = CreateSolidBrush(RGB(R, G, B));
			FillRect(baseDC, &rect, rectBrush);
			DeleteObject(rectBrush);
		}
	}

}

VOID DrawWindowRect()
{
	RECT rect;

	//Create the brush of a color
	HBRUSH hBrush = CreateSolidBrush(RGB(140, 140, 255));

	//Seek the max and min of the present rectangle in case of an inversed select
	DWORD xMin = min(info.xInitPos, info.xPresPos);
	DWORD xMax = max(info.xInitPos, info.xPresPos);
	DWORD yMin = min(info.yInitPos, info.yPresPos);
	DWORD yMax = max(info.yInitPos, info.yPresPos);
	DWORD xLen = abs(info.xPresPos - info.xInitPos);
	DWORD yLen = abs(info.yPresPos - info.yInitPos);

	//Set the shape of the rectangle
	SetRect(&rect, xMin, yMin, xMax, yMax);

	//Fill the region of the rectangle
	FillRect(baseDC, &rect, hBrush);

	//Delete the brush instance in case of memory leak
	DeleteObject(hBrush);
}

VOID DrawScaleBar()
{
	//Draw the contents of the scale bar
	RECT rect;
	DWORD i;
	FLOAT stepLen = (FLOAT)YSumSize / SCALEBAR_PIXEL;
	FLOAT colorStep = (FLOAT)256 / SCALEBAR_PIXEL;
	for (i = 0; i < SCALEBAR_PIXEL; i++)
	{
		//Set the position of each scale bar pixels
		SetRect(&rect, ScaleBar_XPos, PLOTAREA_UP + i * stepLen, ScaleBar_XPos + SCALEBAR_WIDTH, PLOTAREA_UP + (i + 1) * stepLen);
		HBRUSH hBrush = CreateSolidBrush(RGB(255, 255 - colorStep * i, 255 - colorStep * i));
		FillRect(baseDC, &rect, hBrush);
		DeleteObject(hBrush);
	}
}

VOID DrawFont()
{
	CHAR ScaleUp[16] = { 0 };
	CHAR ScaleDown[16] = { 0 };

	//Clear the memory stub in the string
	memset(ScaleUp, 0, 16);
	memset(ScaleDown, 0, 16);

	//Transfer the value into the string
	sprintf(ScaleUp, "%u", maxZHeight);
	sprintf(ScaleDown, "%u", minZHeight);

	//Print the text out onto the window
	TextOutA(baseDC, ScaleBar_Font_XPos, PLOTAREA_UP, ScaleUp, strlen(ScaleUp));
	TextOutA(baseDC, ScaleBar_Font_XPos, PLOTAREA_UP + YSumSize, ScaleDown, strlen(ScaleDown));

	//Next is the rest of the fonts

	memset(SizeInfo, 0, 256);
	sprintf(SizeInfo, "X scan range: %lf um -> %lf um",
		ScanXInitPos,
		ScanXInitPos + ScanXStep * originalColNum
	);
	TextOutA(baseDC, ScanInfo_XPos, PLOTAREA_UP, SizeInfo, strlen(SizeInfo));

	memset(SizeInfo, 0, 256);
	sprintf(SizeInfo, "X step length: %lf um", ScanXStep);
	TextOutA(baseDC, ScanInfo_XPos, PLOTAREA_UP + 25, SizeInfo, strlen(SizeInfo));

	memset(SizeInfo, 0, 256);
	sprintf(SizeInfo, "Y scan range: %lf um -> %lf um",
		ScanYInitPos,
		ScanYInitPos + ScanYStep * originalRowNum
		);
	TextOutA(baseDC, ScanInfo_XPos, PLOTAREA_UP + 50, SizeInfo, strlen(SizeInfo));

	memset(SizeInfo, 0, 256);
	sprintf(SizeInfo, "Y step length: %lf um", ScanYStep);
	TextOutA(baseDC, ScanInfo_XPos, PLOTAREA_UP + 75, SizeInfo, strlen(SizeInfo));

	memset(SizeInfo, 0, 256);
	sprintf(SizeInfo, "New X step number: ");
	TextOutA(baseDC, ScanInfo_XPos, PLOTAREA_UP + 150, SizeInfo, strlen(SizeInfo));

	memset(SizeInfo, 0, 256);
	sprintf(SizeInfo, "New Y step number: ");
	TextOutA(baseDC, ScanInfo_XPos, PLOTAREA_UP + 230, SizeInfo, strlen(SizeInfo));
}