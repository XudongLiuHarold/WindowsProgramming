#pragma once
#include<Windows.h>
#include<vector>

using namespace std;
typedef double(*Func)(double);
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

	void drawFunc(Func func, COLORREF lineColor);
	void drawFunc(Func func, COLORREF lineColor, Range range);
	
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
	vector<double>drawFuncarray;
};