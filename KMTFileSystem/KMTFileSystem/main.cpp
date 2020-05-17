#include"Volume.h"

int main()
{
	Volume f;
	//f.CreateVol("D:\\test.bin", 1);
	f.OpenVol("D:\\test.bin");
	//f.AddFile("D:\\F3.txt", 0);
	//f.CreateFolder("NewFolder", 0);
	//f.AddFileToFolder("D:\\aaaaaaaaaaaaaaaaaaaa.txt", 3);
	f.AddFolder("D:\\KMT", 0);
	f.CloseVol();

	system("pause");
	return 0;
}