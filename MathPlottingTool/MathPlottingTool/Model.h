#include<Windows.h>

#include<vector>
#include<string>

using namespace std;

class Model
{
public:
	static Model & GetInstance()
	{
		static Model instance;   //�ֲ���̬����
		return instance;
	}

	int exportImage(HWND hwnd);
    vector<POINT> importData(HWND hwnd);
	double parser(wstring expr, double varible);
private:
	Model()   //���캯����˽�е�
	{}
	void string_replace(string&s1, const string&s2, const string&s3);
};