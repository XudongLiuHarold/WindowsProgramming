#include"CoordinatesView.h"
#include <windowsx.h>
#include<atlstr.h>
#include"Model.h"


// ===== CONST DEFINITE ==== //
const int Func1ID = 123;
const int Func2ID = 124;
const int Func3ID = 125;
const int Func4ID = 132;

const int PlotButton1 = 126;
const int PlotButton2 = 127;
const int PlotButton3 = 128;
const int PlotButton4 = 131;


const int SAVEBUTTON = 129;
const int IMPORTCSV = 130;

// ===== GLOBAL VAR ==== //
bool changed = true;
bool lButtonDown = false;
short delta;
int offsetX = 0;
int offsetY = 0;
double perGrid = 1.0;


int mouseX1, mouseY1, mouseX2, mouseY2;
int dx = 0, dy = 0;

static wstring needToDrawFunc[4] = {L"",L"",L"",L""};
static vector<POINT> needToDrawPoint;

HWND hwnd_func_1,hwnd_func_2,hwnd_func_3, hwnd_func_4;
COLORREF theColor = RGB(255, 255, 255);
int idFocus;
WNDPROC OldProc[3];
CoordinatesView *  coorView;

// ====FUNC DECLARE ==== //

LRESULT CALLBACK WndProc(_In_  HWND hwnd,_In_  UINT uMsg,_In_  WPARAM wParam,_In_  LPARAM lParam);
LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM);

void draw(HWND hwnd)
{
    coorView = new CoordinatesView(hwnd,0,0,750,750,20,perGrid,5,offsetX,offsetY,theColor);
	coorView->drawCoordiates(0x000000, 0xcccccc);
	for (int  i = 0; i < 4; i++)
	{
		coorView->addFunc(needToDrawFunc[i]);
	}
	coorView->drawAllFunc();
	coorView->drawAllPoint(needToDrawPoint);
	delete(coorView);
}

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	// 类名
	WCHAR* cls_Name = L"Math Plotting Tool";
	// 设计窗口类
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);;
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = ::CreateSolidBrush(RGB(254,240,184));
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = cls_Name;
	wc.hInstance = hInstance;
	// 注册窗口类
	RegisterClass(&wc);

	// 创建窗口
	HWND hwnd = CreateWindow(
		cls_Name,			//类名，要和刚才注册的一致
		L"Math Plotting Tool",	//窗口标题文字
		WS_OVERLAPPED | WS_SYSMENU, //窗口外观样式
		5,				//窗口相对于父级的X坐标
		5,				//窗口相对于父级的Y坐标
		1200,				//窗口的宽度
		750,				//窗口的高度
		NULL,				//没有父窗口，为NULL
		NULL,				//没有菜单，为NULL
		hInstance,			//当前应用程序的实例句柄
		NULL);				//没有附加数据，为NULL
	if (hwnd == NULL) //检查窗口是否创建成功
		return 0;

	ShowWindow(hwnd, SW_SHOW);

	// 更新窗口
	UpdateWindow(hwnd);

	// 消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
LRESULT CALLBACK WndProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{

	COLORREF			rcPrim[3] = { RGB(255,0,0),RGB(0,255,0),RGB(0,0,255) };			
	int					 i;
	TCHAR				szBuffer[10];
	static HINSTANCE	hInstance;
	static RECT			rect;															
	static HWND			hScroll[3], hLabel[3], hValue[3], hStatic;								
	static HBRUSH		hBrushScroll[3], hBrushStatic;										
	static int			cyChar, iColor[3];
	TCHAR				*szColorName[3] = { L"Red",L"Green",L"Blue" };
	Model model = Model::GetInstance();


	switch (uMsg)
	{
	case WM_CREATE:
	{


		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		for (i = 0; i<3; i++)
		{
			hScroll[i] = CreateWindow(L"scrollbar",										
				NULL,
				WS_CHILD | WS_VISIBLE | SBS_VERT,
				920+i*50, 550, 20, 100,
				hwnd,
				(HMENU)i,											
				hInstance,
				NULL);

			SetScrollRange(hScroll[i], SB_CTL, 0, 255, FALSE);								
			SetScrollPos(hScroll[i], SB_CTL, 0, FALSE);										
			hLabel[i] = CreateWindow(L"static",											
				szColorName[i],
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				910+i*50, 550, 50, 15,
				hwnd,
				(HMENU)(i + 3),							
				hInstance,
				NULL);

			hValue[i] = CreateWindow(L"static",										
				L"0",
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				910 + i * 50, 650, 50, 15,
				hwnd,
				(HMENU)(i + 6),									
				hInstance,
				NULL);

			hBrushScroll[i] = CreateSolidBrush(rcPrim[i]);									

			OldProc[i] = (WNDPROC)SetWindowLong(hScroll[i], GWL_WNDPROC, (LONG)ScrollProc);			
		}

		cyChar = HIWORD(GetDialogBaseUnits());
		hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));

		// func input 
		
		hwnd_func_1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			900, 100, 160, 24,	// x, y, w, h
			hwnd, (HMENU)(Func1ID),
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		 hwnd_func_2 = CreateWindowEx(WS_EX_CLIENTEDGE, L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			900, 150, 160, 24,	// x, y, w, h
			hwnd, (HMENU)(Func2ID),
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		 hwnd_func_3 = CreateWindowEx(WS_EX_CLIENTEDGE, L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			900, 200, 160, 24,	// x, y, w, h
			hwnd, (HMENU)(Func3ID),
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

		 hwnd_func_4 = CreateWindowEx(WS_EX_CLIENTEDGE, L"edit", L"",
			 WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			 900, 250, 160, 24,	// x, y, w, h
			 hwnd, (HMENU)(Func4ID),
			 (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		// plot button
		HWND plotButton1 = CreateWindowW(L"BUTTON", L"Plot",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1070,
			100, 80, 24, hwnd, (HMENU)(PlotButton1),
			GetModuleHandle(NULL), NULL);
		HWND plotButton2 = CreateWindowW(L"BUTTON", L"Plot",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1070,
			150, 80, 24, hwnd, (HMENU)(PlotButton2),
			GetModuleHandle(NULL), NULL);
		HWND plotButton3 = CreateWindowW(L"BUTTON", L"Plot",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1070,
			200, 80, 24, hwnd, (HMENU)(PlotButton3),
			GetModuleHandle(NULL), NULL);

		HWND plotButton4 = CreateWindowW(L"BUTTON", L"Plot",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1070,
			250, 80, 24, hwnd, (HMENU)(PlotButton4),
			GetModuleHandle(NULL), NULL);

		//export button
		HWND saveButton1 = CreateWindowW(L"BUTTON", L"Save Picture",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 850,
			340, 96, 96, hwnd, (HMENU)(SAVEBUTTON),
			GetModuleHandle(NULL), NULL);
		// import button 
		HWND import = CreateWindowW(L"BUTTON", L"ImportCSV",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			340, 96, 96, hwnd, (HMENU)(IMPORTCSV),
			GetModuleHandle(NULL), NULL);
	}
	break;

	case WM_PAINT:
	{
		if (changed) {
			SetRect(&rect, 0, 0, 750, 750);
			draw(hwnd);
			changed = false;
		}
		// ===== UI CONFIGURE ==== //
		HBRUSH rectBS1 = ::CreateSolidBrush(RGB(251, 73, 71));
		HBRUSH rectBS2 = ::CreateSolidBrush(RGB(254, 185, 37));
		HBRUSH rectBS3 = ::CreateSolidBrush(RGB(42, 204, 52));
		HBRUSH rectBS4 = ::CreateSolidBrush(RGB(0, 64, 139));
		HPEN linePen = ::CreatePen(PS_SOLID, 1.25, RGB(0, 0, 0));
		HDC hdc = GetDC(hwnd);

		::TextOut(::GetDC(hwnd), 820, 75, L"Plot Function Line", 18);
		::TextOut(::GetDC(hwnd), 820, 290, L"Import & Export", 15);
		::TextOut(::GetDC(hwnd), 820, 490, L"Change Coordinates Color", 24);
		SelectObject(hdc, linePen);
		::MoveToEx(hdc, 940, 80, NULL);
		::LineTo(hdc, 1170, 80);
		::MoveToEx(hdc, 925, 295, NULL);
		::LineTo(hdc, 1170, 295);
		::MoveToEx(hdc, 1000, 495, NULL);
		::LineTo(hdc, 1170, 495);
		DeleteObject(linePen);


		::SelectObject(hdc, rectBS1);
		::Rectangle(hdc, 820, 105, 840, 125);
		::DeleteObject(rectBS1);
		::SelectObject(hdc, rectBS2);
		::Rectangle(hdc, 820, 155, 840, 175);
		::DeleteObject(rectBS2);
		::SelectObject(hdc, rectBS3);
		::Rectangle(hdc, 820, 205, 840, 225);
		::DeleteObject(rectBS3);
		::SelectObject(hdc, rectBS4);
		::Rectangle(hdc, 820, 255, 840, 275);
		::DeleteObject(rectBS4);
		::TextOut(::GetDC(hwnd), 850, 105, L"F(x) =", 6);
		::TextOut(::GetDC(hwnd), 850, 155, L"G(x) =", 6);
		::TextOut(::GetDC(hwnd), 850, 205, L"H(x) =", 6);
		::TextOut(::GetDC(hwnd), 850, 255, L"Z(x) =", 6);

	}
		break;
	case WM_LBUTTONDOWN:
		mouseX1 = GET_X_LPARAM(lParam);
		mouseY1 = GET_Y_LPARAM(lParam);
		lButtonDown = true;
		break;
	case WM_MOUSEMOVE:
		if (lButtonDown) {
			mouseX2 = GET_X_LPARAM(lParam);
			mouseY2 = GET_Y_LPARAM(lParam);
			dx = mouseX2 - mouseX1;
			dy = mouseY2 - mouseY1;
			mouseX1 = mouseX2;
			mouseY1 = mouseY2;
			offsetX += dx;
			offsetY += dy;
			draw(hwnd);
		}
		break;

	case WM_LBUTTONUP:
		lButtonDown = false;
		break;
	case WM_MOUSEWHEEL:
		delta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (delta < 0) {
			perGrid *= 1.25;
			if (perGrid > 0xFFFFFFFF) {
				perGrid /= 2;
			}
		}
		else {
			perGrid /= 1.25;
			if (perGrid <  0.00000000000000005) {
				perGrid *= 1.25;
			}
		}
		changed = true;
		draw(hwnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case PlotButton1:
		{	
		
			TCHAR buff [256] = TEXT("A#4");
			GetWindowText(hwnd_func_1,buff, GetWindowTextLength(hwnd_func_1)+1);
			wstring funcString(buff);
			if (-1024 == model.parser(funcString,0))
			{
				::MessageBox(hwnd, L"输入的表达式错误, 请您重新输入", L"提示", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				needToDrawFunc[0] = funcString;
				draw(hwnd);
			}

		}
			break;
		case PlotButton2:
		{
	
			TCHAR buff[] = TEXT("A#4");
			GetWindowText(hwnd_func_2, buff, GetWindowTextLength(hwnd_func_2) + 1);
			wstring funcString(buff);

			if (-1024  == model.parser(funcString, 0))
			{
				::MessageBox(hwnd, L"输入的表达式错误,请您重新输入", L"提示", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				needToDrawFunc[1] = funcString;
				draw(hwnd);
			}

		}
			break;
		case PlotButton3:
		{
			TCHAR buff[] = TEXT("A#4");
			GetWindowText(hwnd_func_3, buff, GetWindowTextLength(hwnd_func_3) + 1);
			wstring funcString(buff);
		
			if (-1024  == model.parser(funcString, 0))
			{
				::MessageBox(hwnd, L"输入的表达式错误", L"提示", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				needToDrawFunc[2] = funcString;
				draw(hwnd);
			}
		}
			break;
		case PlotButton4:
		{
			TCHAR buff[] = TEXT("A#4");
			GetWindowText(hwnd_func_4, buff, GetWindowTextLength(hwnd_func_4) + 1);
			wstring funcString(buff);

			if (-1024 == model.parser(funcString, 0))
			{
				::MessageBox(hwnd, L"输入的表达式错误", L"提示", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				needToDrawFunc[3] = funcString;
				draw(hwnd);
			}
		}
		break;
		case SAVEBUTTON:
		{	
			model.exportImage(hwnd);
		}
		break;
		case IMPORTCSV:
		{
			needToDrawPoint = model.importData(hwnd);
			draw(hwnd);
		}	
			break;
		default:
			break;
	}
		break;

	case WM_SETFOCUS:
		SetFocus(hScroll[idFocus]);
		return 0;

	case WM_VSCROLL:
	{
		i = GetWindowLong((HWND)lParam, GWL_ID);
		SetFocus(hScroll[i]);

		switch (LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			iColor[i] = min(255, iColor[i] + 15);
			break;
		case SB_LINEDOWN:
			iColor[i] = min(255, iColor[i] + 1);
			break;
		case SB_PAGEUP:
			iColor[i] = max(0, iColor[i] - 15);
			break;
		case SB_LINEUP:
			iColor[i] = max(0, iColor[i] - 1);
			break;
		case SB_TOP:
			iColor[i] = 0;
			break;
		case SB_BOTTOM:
			iColor[i] = 255;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			iColor[i] = HIWORD(wParam);														
			break;
		}

		SetScrollPos(hScroll[i], SB_CTL, iColor[i], TRUE);
		wsprintf(szBuffer, L"%d", iColor[i]);
		SetWindowText(hValue[i], szBuffer);
	 theColor = RGB(abs(iColor[0]-255),abs(iColor[1]-255), abs(iColor[2]-255));
		 draw(hwnd);
	}
		return 0;

	case WM_CTLCOLORSCROLLBAR:
		i = GetWindowLong((HWND)lParam, GWL_ID);
		return (LRESULT)hBrushScroll[i];

	case WM_CTLCOLORSTATIC:
		i = GetWindowLong((HWND)lParam, GWL_ID);
		if (i >= 3 && i <= 8)
		{
			SetTextColor((HDC)wParam, rcPrim[i % 3]);
			SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNHIGHLIGHT));
			return (LRESULT)hBrushStatic;
		}

		break;

	case WM_SYSCOLORCHANGE:
		DeleteObject(hBrushStatic);
		hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
		return 0;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
	{
		DeleteObject((HBRUSH)SetClassLong(hwnd, GCL_HBRBACKGROUND, GetSysColor(WHITE_BRUSH)));
		DeleteObject(hBrushStatic);
		for (i = 0; i<3; i++)
			DeleteObject(hBrushScroll[i]);
		PostQuitMessage(0);
		return 0;
	}
	break;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ScrollProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong(hwnd, GWL_ID);

	switch (message)
	{
	case WM_KEYDOWN:
		if (LOWORD(wParam) == VK_TAB)
			SetFocus(GetDlgItem(GetParent(hwnd), (3 + id + (GetKeyState(VK_SHIFT) < 0 ? -1 : 1)) % 3));			
		break;
	case WM_SETFOCUS:
		idFocus = id;
		break;
	}

	return CallWindowProc(OldProc[id], hwnd, message, wParam, lParam);
}

