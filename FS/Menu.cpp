#include "Menu.h"
int Menu::StartMenu() 
{
	int choice = 8;
	while (choice > 3) 
	{
		system("cls");
		cout << "Chose your action:" << endl;
		cout << "1. Create Volume" << endl;
		cout << "2. Open Volume" << endl;
		cout << "3. Exit\n";
		cout << "Your choice: ";
		cin >> choice;
		switch (choice)
		{
		case 1:
		{
			if (!CreateVol())
			{
				cout << "Can't create Volume\n";
				system("pause");
			}
			else
			{
				cout << "Create succefully\n";
				system("pause");
			}
			return 0;
		}
		case 2:
		{
			system("cls");
			string path;
			cout << "Path of volume: ";
			cin >> path;
			Volum.OpenVol(path);
			if (!Volum.IsOpen())
			{
				cout << "Can't open Volume\n";
				system("pause");
				return 0;
			}
			else
			{
				Folder.push_back(list<pair<string, uint64_t>>());
				BaseFolder.push_back(0);
				Volum.GetFileInfoAndEntry(Folder.back(), 0);
				cout << "Open succefully\n";
				system("pause");
			}
			return 2;
		}
		case 3:
		{
			Volum.CloseVol();
			return 3;
		}
		default:
			cout << "Invalid choice. Try again!";
			break;
		}
	}
}

int Menu::CreateVol() {
	string path;
	uint64_t size;

	system("cls");
	cout << "Enter your volume's path: ";
	cin >> path;
	cout << "Enter your volume's size (MB): ";
	cin >> size;

	if (Volum.CreateVol(path, size))
		return 1;
	else
		return 0;
}

//int Menu::OpenVol() {
//	string path;
//	uint64_t size;
//
//	system("cls");
//	cout << "Enter your volume's path: ";
//	getline(cin, path);
//
//	if (Vol.CreateVol(path, size)) {
//		return 1;
//	}
//	else
//		return 0;
//}

int Menu::OperatorForFolder() 
{
	int choice = 8;
	while (choice != 0) 
	{
		system("cls");
		cout << "Chose your action:" << endl;
		cout << "1. List" << endl;
		cout << "2. Add File" << endl;
		cout << "3. Add Folder" << endl;
		cout << "4. Extract Folder" << endl;
		cout << "5. Go back" << endl;
		cout << "Your choice: \n"; cin >> choice;
		switch (choice)
		{
		case 1:
		{
			ListFile();
			return 1;
		}
		case 2:
		{
			// chon duong dan
			system("cls");
			string Filepath;
			cout << "Path of file you want to add: ";
			cin >> Filepath;

			uint64_t ans = Volum.AddFile(Filepath, BaseFolder.back());
			if (!ans)
			{
				// ko du bo nho
				cout << "Don't have enough space to add\n";
				system("pause");
			}
			else
			{
				// add thanh cong
				// get Name
				int begin = Filepath.size() - 1;
				while (begin >= 0 && Filepath[begin] != '\\')
					--begin;
				++begin;

				Folder.back().push_back({ Filepath.substr(begin, Filepath.size() - begin), ans });
				cout << "Add successfully\n";
				system("pause");
			}
			return 2;
		}
		case 3:
		{
			system("cls");
			string path;
			cout << "Path of folder you want to add: ";
			cin >> path;
			uint64_t ans = Volum.AddFolder(path, BaseFolder.back());
			if (!ans)
			{
				cout << "Don't have enought space to add\n";
				system("pause");
			}
			else
			{
				int begin = path.size() - 1;
				while (begin >= 0 && path[begin] != '\\')
					--begin;
				++begin;
				Folder.back().push_back({ path.substr(begin, path.size() - begin), ans });
				cout << "Add successfully\n";
				system("pause");
			}
			return 3;
		}
		case 4:
		{
			system("cls");
			if (BaseFolder.back() == 0)
			{
				cout << "This is root folder\n";
				system("pause");
				return 4;
			}
			string path;
			cout << "Where you want to move: ";
			cin >> path;
			Volum.MoveFolder(BaseFolder.back(), path);
			Folder.pop_back();
			BaseFolder.pop_back();
			return 4;
		}
		case 5:
		{
			Folder.pop_back();
			BaseFolder.pop_back();
			if (!BaseFolder.empty())
			{
				return 4;
			}
			else
				return 5;
		}
		default:
			cout << "Invalid choice. Try again!";
			break;
		}
	}
}

int Menu::ListFile() 
{
	int no = 1, choice = 0, lastchoice = Folder.back().size();
	choice = lastchoice + 1;
	while (choice > lastchoice) 
	{
		system("cls");
		cout << "Chose your action:" << endl;
		for (auto i = Folder.back().begin(); i != Folder.back().end(); ++i)
		{
			cout << no << ". " << i->first << endl;
			no++;
		}
		cout << no << ". " << "Go back" << endl;
		cin >> choice;
		if (choice == Folder.back().size() + 1)	// Go back
			return 0;
	}
	auto it = Folder.back().begin();
	choice -= 1;

	// dich chuyen it toi folder hay thu muc hien tai
	while (choice)
	{
		++it;
		--choice;
	}

	if (Volum.isDirectory(it->second))
	{
		//neu user chon mo folder
		Folder.push_back(list<pair<string, uint64_t>>());
		Volum.GetFileInfoAndEntry(Folder.back(), it->second);
		BaseFolder.push_back(it->second);
		return 0;
	}
	else
	{
		system("cls");
		pair<bool, string> pass = Volum.HasPass(it->second);
		if (pass.first)
		{
			string pass_user;
			// bat nguoi dung nhap
			choice = 3;
			while (choice != 1)
			{
				system("cls");
				bool CorrectPass = 1;
				choice = 3;
				cout << "Password of file: ";
				cin >> pass_user;
				for (int i = 0; i < 8; ++i)
					if (pass_user[i] != pass.second[i])
					{
						// wrong password
						CorrectPass = 0;
						return 0;
					}
				// blabla....
				if (OperatorForFile(it->second) == 2)
					return 0;

				if (OperatorForFile(it->second))
					Folder.back().erase(it);
			}
		}
		if (OperatorForFile(it->second))
			Folder.back().erase(it);
		// xi xu ly tiep // ???????? 
	}
	return 0;
}
int Menu::OperatorForFile(uint64_t Entry)
{
	int choice = -1;
	while (choice != 0) 
	{
		system("cls");
		cout << "Chose your action:" << endl;
		cout << "1. Delete File" << endl;
		cout << "2. Move File" << endl;
		cout << "3. Create password" << endl;
		cout << "4. Go back" << endl;
		cout << "Your choice: "; cin >> choice;
		switch (choice)
		{
		case 1:
		{
			Volum.DelFile(Entry);
			return 1;
		}
		case 2:
		{
			system("cls");
			string path;
			cout << "Where you want to move: ";
			cin >> path;
			Volum.MovFile(Entry, path);
			return 1;
		}
		case 3:
		{
			system("cls");
			cout << "Password is sequence of number at most 8 digits: ";
			string password;
			cin >> password;
			Volum.AddPassword(password, Entry);
			return 0;
		}
		case 4:	// go back
		{
			return 2;
		}
		default:
			cout << "Invalid choice. Try again!";
			break;
		}
	}

}
