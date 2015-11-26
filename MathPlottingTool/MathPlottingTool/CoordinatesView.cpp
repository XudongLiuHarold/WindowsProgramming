#include "CoordinatesView.h"
#include <cstdio>
#include <tchar.h>
#include"Model.h"


CoordinatesView::CoordinatesView(HWND hwnd, int top, int left, int width, int height, int gridLength, double _xPerGrid, int milesGap, int offsetX, int offsetY,COLORREF br)
{

	this->left = left;
	this->top = top;
	this->width = width;
	this->height = height;
	this->gridLength = gridLength;
	this->hwnd = hwnd;
	this->xPerGrid = _xPerGrid;
	this->offsetX = offsetX;
	this->offsetY = offsetY;
	this->milesGap = milesGap;

	drawFuncarray.clear();
	lineColor[0] = RGB(251, 73, 71);
	lineColor[1] = RGB(254,185,37);
	lineColor[2] = RGB(42,204,52);
	lineColor[3] = RGB(0, 64, 139);
	lineColor[4] = RGB(4, 4, 4);

	ps = new PAINTSTRUCT;
	hdc = ::GetDC(hwnd);
	drawPen = ::CreatePen(PS_SOLID, 2, RGB(123, 123, 123));
	
	HPEN transferPen = ::CreatePen(PS_SOLID, 0.1, RGB(255,255,255));
	bgBush = ::CreateSolidBrush(br);

	::SelectObject(hdc, transferPen);
	::Rectangle(hdc, left, top, width + 60, height);
	::DeleteObject(transferPen);
	::SelectObject(hdc,drawPen);
	::SelectObject(hdc,bgBush);
	::Rectangle(hdc,left,top,width,height);
	::DeleteObject(drawPen);
	::DeleteObject(bgBush);

}

CoordinatesView::~CoordinatesView()
{
	::ReleaseDC(hwnd, hdc);
	delete ps;
	ps = nullptr;
}

void CoordinatesView::drawCoordiates(COLORREF axisColor, COLORREF girdColor)
{
	POINT zeroP;
	zeroP.x = (left + width) / 2 + offsetX;
	zeroP.y = (top + height) / 2 + offsetY;

	int gap_x_right = (left + width - zeroP.x) / (gridLength * milesGap);
	int gap_x_left = (zeroP.x - left) / (gridLength * milesGap); 
	int gap_y_up = (zeroP.y - top) / (gridLength * milesGap); 
	int gap_y_down = (top + height - zeroP.y) / (gridLength * milesGap);

	int grid_num_x =  width / gridLength;
	int grid_num_y = height / gridLength;

	//===== Draw the axis ====//

	//HPEN  style 
	HPEN AxisPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	HPEN LightGridPen = CreatePen(PS_DASH, 1.25, RGB(100, 149, 237));
	HPEN BoldGridPen = CreatePen(PS_SOLID, 2, RGB(123, 123, 123));

	//draw Axis
	::SelectObject(hdc,AxisPen);

	// X Axis
	::MoveToEx(hdc,left,zeroP.y,NULL);
	::LineTo(hdc, left + width, zeroP.y);
	// arrows
	::MoveToEx(hdc, left+width, zeroP.y, NULL);
	::LineTo(hdc, left+width - 20,zeroP.y - 20);
	::MoveToEx(hdc, left+width, zeroP.y, NULL);
	::LineTo(hdc,left+width - 20, zeroP.y + 20);
	// Y Axis
	::MoveToEx(hdc, zeroP.x, top, NULL);
	::LineTo(hdc, zeroP.x, top + height);
	//arrows
	::MoveToEx(hdc, zeroP.x, top ,NULL);
	::LineTo(hdc, zeroP.x - 20,  top + 20);
	::MoveToEx(hdc, zeroP.x, top, NULL);
	::LineTo(hdc, zeroP.x + 20, top + 20);

	//=== Draw Grid ===//
	for (int i = 0; i < grid_num_x; i++)
	{
		if (i % 5 == 0 )
		{
			::SelectObject(hdc, BoldGridPen);
		}
		else
		{
			::SelectObject(hdc, LightGridPen);
		}
		::MoveToEx(hdc, left, top + i * gridLength, NULL);
		::LineTo(hdc, left + width, top + i * gridLength);
	}
	for (int i = 0; i < grid_num_y; i++)
	{
		if (i % 5 == 0)
		{
			::SelectObject(hdc, BoldGridPen);
		}
		else
		{
			::SelectObject(hdc, LightGridPen);
		}
		::MoveToEx(hdc, left + i * gridLength, top, NULL);
		::LineTo(hdc, left + i * gridLength, top + height);
	}
	//=== Draw Label ===//
	for (int i = 1; i <= gap_x_right; ++i) {
		::MoveToEx(hdc, zeroP.x + i * (gridLength * milesGap), zeroP.y - 5, NULL);
		::LineTo(hdc, zeroP.x + i * (gridLength * milesGap), zeroP.y + 5);

		TCHAR number_mark[100];
		swprintf_s(number_mark, _T("%g"), i * milesGap * xPerGrid);
		::TextOut(hdc, zeroP.x + i * (gridLength * milesGap) - 2, zeroP.y + 5, number_mark, wcslen(number_mark));
	}
	for (int i = 1; i <= gap_x_right; ++i) {
		::MoveToEx(hdc, zeroP.x - i * (gridLength * milesGap), zeroP.y - 5, NULL);
		::LineTo(hdc, zeroP.x - i * (gridLength * milesGap), zeroP.y + 5);

		TCHAR number_mark[100];
		swprintf_s(number_mark, _T("%g"), -i * milesGap * xPerGrid);
		::TextOut(hdc, zeroP.x - i * (gridLength * milesGap) - 2, zeroP.y + 5, number_mark, wcslen(number_mark));
	}
	
	::DeleteObject(AxisPen);
	::DeleteObject(BoldGridPen);
	::DeleteObject(LightGridPen);
}

void CoordinatesView::addFunc(wstring expr)
{
	drawFuncarray.push_back(expr);
}

int CoordinatesView::drawFunc(wstring func, COLORREF lineColor)
{
	Range range = {};
	range.begin = -9999999;
	range.end = +9999999;
	return  drawFunc(func, lineColor, range);
}

int  CoordinatesView::drawFunc(wstring func, COLORREF lineColor, Range range)
{
	Model model = Model::GetInstance();

	if (range.end < range.begin) {
		std::fprintf(stderr, "ERROR! can't draw function in that range\n");
	}

	HPEN drawFuncpen = ::CreatePen(PS_SOLID, 2, lineColor);
	
	
	::SelectObject(hdc, drawFuncpen);
	int zeroPointX = ((left + width) / 2) + offsetX;
	int zeroPointY = ((top + height) / 2) + offsetY;
	double x_per_pixel = static_cast<double>(this->xPerGrid) / static_cast<double>(this->gridLength);

	int drawing_pixel_x;
	double minimum_visible_x = static_cast<double>(-(zeroPointX - left) * x_per_pixel);
	if (minimum_visible_x > range.begin) {
		range.begin = minimum_visible_x;
		drawing_pixel_x = left - 1;
	}
	else {
		drawing_pixel_x = zeroPointX + static_cast<int>(range.begin / x_per_pixel);
	}
	double maximum_visible_x = static_cast<double>((left + width - zeroPointX) * x_per_pixel);
	if (maximum_visible_x < range.end)
	{
		range.end = maximum_visible_x;
	}

	int startY = static_cast<int>(model.parser(func,range.begin));

	::MoveToEx(hdc, drawing_pixel_x, startY, NULL);

	for (double x = range.begin; x < range.end; x += x_per_pixel) {
		drawing_pixel_x += 1;
		double y = model.parser(func,x);
		int drawing_y = static_cast<int>(-(y / x_per_pixel) + zeroPointY);
		::LineTo(hdc, drawing_pixel_x, drawing_y);
	::DeleteObject(drawFuncpen);
	}
	return 0;
}

void CoordinatesView::drawAllFunc()
{
	for (int i = 0; i < drawFuncarray.size(); i++)
	{
		if (drawFuncarray[i].length() != 0 )
		{
			drawFunc(drawFuncarray[i], lineColor[i]);
		}	
	}
}

void CoordinatesView::drawAllPoint(vector<POINT>allPointToDraw)
{
	if (allPointToDraw.size()>0)
	{

		POINT zeroP;
		zeroP.x = (left + width) / 2 + offsetX;
		zeroP.y = (top + height) / 2 + offsetY;

		double x_per_pixel = static_cast<double>(this->xPerGrid) / static_cast<double>(this->gridLength);
		POINT x[256];

		// it's hard ,need to improve
		for (int i = 0; i < allPointToDraw.size(); i++)
		{
			x[i].x =zeroP.x+allPointToDraw[i].x;
			x[i].y = zeroP.y + allPointToDraw[i].y;
		}
		HPEN drawFuncpen = ::CreatePen(PS_SOLID, 2, lineColor[4]);
		::SelectObject(hdc, drawFuncpen);
		::Polyline(hdc, x, allPointToDraw.size());
		::DeleteObject(drawFuncpen);
	}
}

void CoordinatesView::setBush(HBRUSH br)
{
	bgBush = br;
}
