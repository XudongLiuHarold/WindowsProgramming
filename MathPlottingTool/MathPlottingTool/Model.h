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

	double parser();
private:
	Model()   //���캯����˽�е�
	{
	}
	
};