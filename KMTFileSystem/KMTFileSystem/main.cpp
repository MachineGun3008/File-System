#include"Volume.h"

int main()
{
	Volume f;
	f.OpenVol("D:\\test.bin");
	//f.display();
	f.AddFile("D:\\F5.txt");
	f.CloseVol();

	system("pause");
	return 0;
}