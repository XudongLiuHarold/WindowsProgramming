#include "Model.h"
#include"parser.h"


int Model::exportImage(HWND hwnd)
{
	WCHAR filename[MAX_PATH] = L"";

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = 0;
	ofn.lpstrFilter = L"BMP (*.bmp)\0*.bmp\0JPEG (*.jpg)\0*.jpg\0PNG (*.png)\0*.png\0All\0*.*\0";
	ofn.nFilterIndex = 3;
	ofn.lpstrTitle = L"Export viewport";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = L"bmp";

	BOOL bResult = GetSaveFileNameW(&ofn);

	HDC hScreenDC = GetDC(hwnd);
	// and a device context to put it in
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	int x = 760;
	int y = 720;


	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, x, y);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
	BitBlt(hMemoryDC, 0, 0, 760, 720, hScreenDC, 0, 0, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

	// clean up
	DeleteDC(hMemoryDC);
	DeleteDC(hScreenDC);

	HDC hDC;
	int iBits; 
	WORD wBitCount; 
	DWORD dwPaletteSize = 0,  
		dwBmBitsSize,
		dwDIBSize, dwWritten;
	BITMAP Bitmap; 
	BITMAPFILEHEADER bmfHdr; 
	BITMAPINFOHEADER bi; 
	LPBITMAPINFOHEADER lpbi; 

	HANDLE fh, hDib, hPal, hOldPal = NULL; 

	HDC hWndDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	hDC = ::CreateCompatibleDC(hWndDC);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);

	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else
		wBitCount = 24;

	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);
 
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize,
		(LPBITMAPINFO)
		lpbi, DIB_RGB_COLORS);
 
	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(ofn.lpstrFile, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return 0;

	bmfHdr.bfType = 0x4D42; // "BM"  
	dwDIBSize = sizeof(BITMAPFILEHEADER)
		+ sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)
		+ (DWORD)sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize,
		&dwWritten, NULL);
  
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return 1;
}

vector<POINT> Model::importData(HWND hwnd)
{

	WCHAR filename[MAX_PATH] = L"";

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = 0;
	ofn.lpstrFilter = L"CSV (*.csv)\0All\0*.*\0";
	ofn.nFilterIndex = 3;
	ofn.lpstrTitle = L"Import viewport";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = L"png";
	BOOL bResult = GetSaveFileNameW(&ofn);


	vector<POINT>fileData;
	HANDLE hFile;
	hFile = CreateFile(ofn.lpstrFile,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	DWORD dwDataLen;
	char FileBuffer[1000];
	ReadFile(hFile, FileBuffer, 100, &dwDataLen, NULL);
	FileBuffer[dwDataLen] = 0;
	CloseHandle(hFile);
	int i = 0;
	std::string temp = "";
	float x, y;
	bool isY = false;
	POINT point;
	while (FileBuffer[i] != '\0')
	{
		if (FileBuffer[i] == '\r' || FileBuffer[i] == '\n')
		{
			y = atof(temp.data());
			i++;
			temp = "";
			point.x = x;
			point.y = y;
			fileData.push_back(point);
		}
		else if (FileBuffer[i] != ',') {
			temp += FileBuffer[i];

		}
		else {
			x = atof(temp.data());
			temp = "";

		}
		i++;
	}

	return fileData;
}

double Model::parser(wstring expr, double varible)
{
	Parser prs;
	string expression(expr.length(), ' ');

	copy(expr.begin(), expr.end(), expression.begin());
	string var;
	char buff[20];
	sprintf_s(buff, "%f", varible);
	var = buff;
	string x = "x";
	string_replace(expression, x, var);
	if (strcmp(expression.c_str(), "") != 0)
	{
		// evaluate the expression

		return prs.parse(expression.c_str());

	
	}


	return 0.0;
}

void Model::string_replace(string & s1, const string & s2, const string & s3)
{
	{
		string::size_type pos = 0;
		string::size_type a = s2.size();
		string::size_type b = s3.size();
		while ((pos = s1.find(s2, pos)) != string::npos)
		{
			s1.replace(pos, a, s3);
			pos += b;
		}
	}
}

