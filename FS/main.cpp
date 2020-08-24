#include"Volume.h"
#include"Menu.h"
int main()
{
	Volume f;
	//f.CreateVol("D:\\test.bin", 1);
	//f.OpenVol("D:\\test.bin");
	//f.AddFile("D:\\F3.txt", 0);
	//f.AddFolder("D:\\NewFolder", 2);
	//f.MoveFolder(3, "D:\\KMT\\");
	//f.CloseVol();


	srand(time(NULL));
	Menu M;
	int choice = 0;
	while (true)
	{
		choice = M.StartMenu();
		if (choice == 3)	// Exit
		{
			system("cls");
			cout << "Thank you !!!" << endl;
			Sleep(1000);
			return 0;
		}
		if (choice == 2)	// Opening
		{
			while (choice != 5)
			{
				choice = M.OperatorForFolder();
			}
		}
	}
	system("pause");
	return 0;
}