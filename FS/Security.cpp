#include "Security.h"

// Permmutate password
void SetUpPer(int* a)
{
	int t[10];
	for (int i = 0; i < 8; i++)
		t[i] = i;

	for (int i = 0; i < 8; i++)
	{
		//srand(time(NULL));
		int x = rand() % 8;
		while (*(a + x) != -1)
		{
			x = rand() % 8;
		}
		a[x] = t[i];
	}
}

void Security::encrypt(string password)
{
	int pass[8];
	for (int i = 0; i < 8; i++)
		pass[i] = password[i] - 48;

	int per[10]{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	SetUpPer(per);

	int sec[8];
	for (int i = 0; i < 8; i++)
		sec[i] = per[pass[i]];

	//sec[8] = -1;
	//sec[9] = -1;
	//swap(sec[8], sec[per[8]]);
	//swap(sec[9], sec[per[9]]);

	string t;
	for (int i = 0; i < 10; i++)
	{
		string block;
		//part1
		block.push_back(char(per[i]));

		//srand(time(NULL));
		int Length = rand() % 6 + 4;

		//part2
		int NextAddress = t.length() + Length;

		int temp = NextAddress;
		while (temp >= 10)
		{
			block.push_back(char(temp % 10));
			temp /= 10;
		}
		block.push_back(char(temp));

		//part3

		int cursize = block.length();
		for (int i = 0; i < Length - cursize; i++)
		{
			block.push_back(rand() % 10); // trash
		}

		t += block;
	}

	for (int i = 0; i < 8; i++)
	{
		string block;
		//part1
		block.push_back(char(sec[i]));

		//srand(time(NULL));
		int Length = rand() % 6 + 4;

		//part2
		int NextAddress = t.length() + Length;

		int temp = NextAddress;
		while (temp >= 10)
		{
			block.push_back(char(temp % 10));
			temp /= 10;
		}

		block.push_back(char(temp));

		//part3

		int cursize = block.length();
		for (int i = 0; i < Length - cursize; i++)
		{
			block.push_back(rand() % 10); // trash
		}

		t += block;
	}

	//Length = s.length();

	Length = t.length();
	while (t.length() < 200)
	{
		t.push_back(rand() % 10);
	}

	//HavePass = 1;


	for (int i = 0; i < 200; i++)
		Pass[i] = t[i];
	HavePass = 1;

}

string Security::decrypt()
{
	int per[10], p[8], sec[8];
	int curIndex = 0;

	for (int i = 0; i < 10; i++)
	{
		//string block;
		//part1
		int data;
		data = Pass[curIndex++];
		per[i] = data;

		//part2
		int NextIndex[3];
		for (int i = 0; i < 3; i++)
			NextIndex[i] = Pass[curIndex + i];

		int temp = NextIndex[0];
		if (temp < curIndex)	// 2 chu so
			temp += NextIndex[1] * 10;
		if (temp < curIndex)	// 3 chu so
			temp += NextIndex[2] * 100;

		curIndex = temp;
	}

	for (int i = 0; i < 8; i++)
	{
		//string block;
		//part1
		int data;
		data = int(Pass[curIndex++]);
		sec[i] = data;

		//part2
		int NextIndex[3];
		for (int i = 0; i < 3; i++)
			NextIndex[i] = Pass[curIndex + i];

		int temp = NextIndex[0];
		if (temp < curIndex)	// 2 chu so
			temp += NextIndex[1] * 10;
		if (temp < curIndex)	// 3 chu so
			temp += NextIndex[2] * 100;

		curIndex = temp;
	}

	//swap(sec[9], sec[per[9]]);
	//swap(sec[8], sec[per[8]]);

	for (int i = 0; i < 8; i++)
	{
		int index = 0;
		for (index = 0; index < 10; index++)
			if (per[index] == sec[i])
				break;
		p[i] = index;
	}
	string ans(8, '0');
	for (int i = 0; i < 8; ++i)
		ans[i] = char(p[i] + '0');
	return ans;
}

//bool Security::update()
//{
//	string p;
//	cout << "Enter your password [8]: "; cin >> p;
//	if (!decrypt(p))
//		return false;
//
//	cout << "Enter your new password [8]: "; cin >> p;
//
//	encrypt(p);
//
//	cout << "Retype your new password [8]: "; cin >> p;
//
//	return decrypt(p);
//}

Security::Security()
{
	Length = 0;
	HavePass = 0;
	fill(Pass, Pass + 200, 0);
}

bool Security::havePass()
{
	return HavePass;
}

uint32_t Security::getSize()
{
	return 205;
}

void Security::write(fstream &Vol)
{
	Vol.write((char *)&Length, sizeof(Length));
	Vol.write((char *)&HavePass, sizeof(HavePass));
	Vol.write((char *)&Pass, sizeof(char) * 200);
}

void Security::read(fstream& Vol)
{
	Vol.read((char*)&Length, sizeof(Length));
	Vol.read((char*)&HavePass, sizeof(HavePass));
	Vol.read((char*)&Pass, sizeof(char) * 200);
}
