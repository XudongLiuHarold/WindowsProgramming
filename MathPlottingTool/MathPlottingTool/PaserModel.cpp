
#include <cstdlib>
#include <cstdio>
#include "parser.h"
#include<windowsx.h>
#include"PaserModel.h"

using namespace std;
void string_replace(string&s1, const string&s2, const string&s3);
double parserFunc(wstring expr, double varible)
{

	// create a parser object
	Parser prs;
	string expression(expr.length(), ' ');
	copy(expr.begin(), expr.end(), expression.begin());

	string var;
	char buff[20];
	sprintf_s(buff, "%f",varible);
	var = buff;

	string x = "x";
	
	string_replace(expression, x, var);


		if (strcmp(expression.c_str(), "") != 0)
		{
				// evaluate the expression
				double result;
				result = prs.parse(expression.c_str());
				return result;
		}
		
	return 0.0;
}
void string_replace(string&s1, const string&s2, const string&s3)
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