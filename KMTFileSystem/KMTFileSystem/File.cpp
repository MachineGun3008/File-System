#include "File.h"

File::File(ifstream &in, string path)
{
	FileInfo i(in,path);
	info = i;

	char choice = 'N';
	string p;
	cout << "Do you want to create password? [Y/N]: "; cin >> choice;
	if (choice == 'y')
	{
		cout << "Enter password: "; cin >> p;
		cout << "Your password is: " << p << endl;
	}
	//Security s(p); // 'Y' 
	//Security s;	 // 'N'
	//pass = p;
}

void File::Display()
{
	info.displayInfo();
}
