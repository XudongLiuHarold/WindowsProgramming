#pragma once
#include<Windows.h>
#include<vector>

using namespace std;
struct Range
{
	double begin;
	double end;
};

class  CoordinatesView
{
public:
	CoordinatesView(HWND hwnd, 
		int top,
		int left, 
		int width, 
		int height, 
		int gridLength,
		double _xPerGrid,
		int milesGap,
		int offsetX,
		int offsetY);
	~CoordinatesView();

	void drawCoordiates(COLORREF axisColor, COLORREF girdColor);
	void addFunc(wstring expr);
	void drawAllFunc();
	
private:
	HWND hwnd;
	int top;
	int left;
	int width;
	int height;
	int gridLength;
	int offsetX;
	int offsetY;
	int milesGap;
	double xPerGrid;

	HDC hdc;
	HPEN drawPen;
	HBRUSH bgBush;
	PAINTSTRUCT *ps;
    vector<wstring>drawFuncarray;
	COLORREF lineColor[3];
	void drawFunc(wstring func, COLORREF lineColor);
	void drawFunc(wstring func, COLORREF lineColor, Range range);
};