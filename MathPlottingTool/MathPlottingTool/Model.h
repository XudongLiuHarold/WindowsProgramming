#include<Windows.h>

#include<vector>
#include<string>

using namespace std;

class Model
{
public:
	static Model & GetInstance()
	{
		static Model instance;   //局部静态变量
		return instance;
	}

	int exportImage(HWND hwnd);
    vector<POINT> importData(HWND hwnd);
	double parser(wstring expr, double varible);
private:
	Model()   //构造函数是私有的
	{}
	void string_replace(string&s1, const string&s2, const string&s3);
};