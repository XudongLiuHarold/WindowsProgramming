
#include<Windows.h>
class Model
{
public:
	static Model* singleton();
	~Model();

	int exportImage(HWND hwnd);

	int importData();

	double parser();
private:
	Model();
	static Model* instance;

};