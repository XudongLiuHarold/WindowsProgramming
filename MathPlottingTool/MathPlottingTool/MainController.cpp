#include"CoordinatesView.h"
#include <windowsx.h>
#include<atlstr.h>


#define PI 3.1415


const static struct Const 
{
	bool changed = true;
    double perGrid = 1.0;
	short delta;
	int offsetX = 0;
	int offsetY = 0;
	int fun_input_id1 = 123;
	int fun_input_id2 = 124;
	int fun_input_id3 = 125;
	int plot_button_id1 = 126;
	int plot_button_id2 = 127;
	int plot_button_id3 = 128;
	int save_bitmap_id = 129;
	int save_png_id = 130;
	int save_svg_id = 131;
	int import_cvs_id = 132;
};

Const con;
bool lButtonDown = false;
int mouseX1, mouseY1, mouseX2, mouseY2;
int dx = 0, dy = 0;
WCHAR debug_buf[4096];

HWND hwnd_func_1,hwnd_func_2,hwnd_func_3;

LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);




void draw(HWND hwnd,wstring func)
{
	CoordinatesView * coorView = new CoordinatesView(hwnd,0,0,750,750,20,con.perGrid,5,con.offsetX,con.offsetY);
	coorView->drawCoordiates(0x000000, 0xcccccc);
	//wstring func = L"Log(x)";
	coorView->addFunc(func);
	coorView->drawAllFunc();
	delete coorView;
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
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpfnWndProc = WindowProc;
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
LRESULT CALLBACK WindowProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		// func input 
		hwnd_func_1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"edit", L"F(x)",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			800, 100, 200, 24,	// x, y, w, h
			hwnd, (HMENU)(con.fun_input_id1),
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		 hwnd_func_2 = CreateWindowEx(WS_EX_CLIENTEDGE, L"edit", L"G(x)",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			800, 150, 200, 24,	// x, y, w, h
			hwnd, (HMENU)(con.fun_input_id2),
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		 hwnd_func_3 = CreateWindowEx(WS_EX_CLIENTEDGE, L"edit", L"H(x)",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT,
			800, 200, 200, 24,	// x, y, w, h
			hwnd, (HMENU)(con.fun_input_id3),
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

		// plot button
		HWND plotButton1 = CreateWindowW(L"BUTTON", L"Plot",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			100, 100, 24, hwnd, (HMENU)(con.plot_button_id1),
			GetModuleHandle(NULL), NULL);
		HWND plotButton2 = CreateWindowW(L"BUTTON", L"Plot",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			150, 100, 24, hwnd, (HMENU)(con.plot_button_id2),
			GetModuleHandle(NULL), NULL);
		HWND plotButton3 = CreateWindowW(L"BUTTON", L"Plot",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			200, 100, 24, hwnd, (HMENU)(con.plot_button_id3),
			GetModuleHandle(NULL), NULL);

		//export button

		HWND saveButton1 = CreateWindowW(L"BUTTON", L"Save as bitmap",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			300, 100, 24, hwnd, (HMENU)(con.save_bitmap_id),
			GetModuleHandle(NULL), NULL);
		HWND saveButton2 = CreateWindowW(L"BUTTON", L"Save as png",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			350, 100, 24, hwnd, (HMENU)(con.save_png_id),
			GetModuleHandle(NULL), NULL);
		HWND saveButton3 = CreateWindowW(L"BUTTON", L"Save as svg",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			400, 100, 24, hwnd, (HMENU)(con.save_svg_id),
			GetModuleHandle(NULL), NULL);

		// import button 

		HWND import = CreateWindowW(L"BUTTON", L"Import from file",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 1030,
			500, 100, 24, hwnd, (HMENU)(con.import_cvs_id),
			GetModuleHandle(NULL), NULL);
	}
	break;

	case WM_PAINT:
		if (con.changed) {
			draw(hwnd,L"");
			con.changed = false;
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
			con.offsetX += dx;
			con.offsetY += dy;
			draw(hwnd,L"");
		}
		break;

	case WM_LBUTTONUP:
		lButtonDown = false;
		break;
	case WM_MOUSEWHEEL:
		con.delta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (con.delta < 0) {
			con.perGrid *= 1.25;
			if (con.perGrid > 0xFFFFFFFF) {
				con.perGrid /= 2;
			}
		}
		else {
			con.perGrid /= 1.25;
			if (con.perGrid <  0.00000000000000005) {
				con.perGrid *= 1.25;
			}
		}
		con.changed = true;
		draw(hwnd,L"");
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 126:
		{	
			TCHAR buff [] = TEXT("A#4");
			GetWindowText(hwnd_func_1,buff, GetWindowTextLength(hwnd_func_1)+1);
			wstring funcString(buff);
			//redraw !
			draw(hwnd,funcString);
		}
			break;
		case 127:
		{
			TCHAR buff[] = TEXT("A#4");
			GetWindowText(hwnd_func_2, buff, GetWindowTextLength(hwnd_func_2) + 1);
			wstring funcString(buff);
			draw(hwnd, funcString);
			MessageBox(hwnd, buff, L"提示", MB_OK | MB_ICONINFORMATION);
		
		}
			break;
		case 128:
		{
			TCHAR buff[] = TEXT("A#4");
			GetWindowText(hwnd_func_3, buff, GetWindowTextLength(hwnd_func_3) + 1);
			wstring funcString(buff);
			draw(hwnd, funcString);
			MessageBox(hwnd, buff, L"提示", MB_OK | MB_ICONINFORMATION);	
		}
			break;
		case 129:
			MessageBox(hwnd, L"Save bitmap", L"提示", MB_OK | MB_ICONINFORMATION);
			break;
		case 130:
			MessageBox(hwnd, L"Save Png", L"提示", MB_OK | MB_ICONINFORMATION);
			break;
		case 131:
			MessageBox(hwnd, L"Save svg", L"提示", MB_OK | MB_ICONINFORMATION);
			break;
		case 132:
			MessageBox(hwnd, L"Import from file", L"提示", MB_OK | MB_ICONINFORMATION);
			break;
		
		default:
			break;
		}
		break;



	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;

	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
