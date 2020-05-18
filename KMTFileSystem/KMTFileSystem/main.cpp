#include"Volume.h"

int main()
{
	Volume v;
	//v.CreateVol("D:\\test.kmt", 1);
	v.OpenVol("D:\\test.kmt");
	v.AddFile("D:\\audio.mp3", 0);
	//v.Extract();
	//v.CreateFolder("NewFolder", 0);
	//v.AddFileToFolder("D:\\aaaaaaaaaaaaaaaaaaaa.txt", 3);
	//v.AddFolder("D:\\KMT", 0);
	v.CloseVol();

	system("pause");
	return 0;
}