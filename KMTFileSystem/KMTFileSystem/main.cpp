#include"Volume.h"

int main()
{
	Volume f;
	f.CreateVol("D:\\test.bin", 1);
	f.closevol();

	system("pause");
	return 0;
}