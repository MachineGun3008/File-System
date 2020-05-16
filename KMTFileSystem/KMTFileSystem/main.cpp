#include"Volume.h"

int main()
{
	Volume f;
	//f.CreateVol("D:\\test.bin", 1);
	f.OpenVol("D:\\test.bin");
	//f.display();
	f.AddFile("D:\\F2.txt", 0);
	f.CloseVol();

	system("pause");
	return 0;
}