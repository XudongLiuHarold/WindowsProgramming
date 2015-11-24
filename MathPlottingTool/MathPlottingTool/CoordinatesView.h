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
		int offsetY,
		COLORREF br);
	~CoordinatesView();

	int drawFunc(wstring func, COLORREF lineColor);
	int  drawFunc(wstring func, COLORREF lineColor, Range range);

	void drawCoordiates(COLORREF axisColor, COLORREF girdColor);
	void addFunc(wstring expr);
	void drawAllFunc();
	void drawAllPoint(vector<POINT>allPointToDraw);
	void setBush(HBRUSH br);

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
	COLORREF lineColor[5];

	
};