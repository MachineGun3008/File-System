#pragma once
#ifndef _File_
#define _File_

#include "FileInfo.h"
#include "Security.h"
#include "Data.h"
class File
{
public:
	//File();
	//File(ifstream&, string);
	//void Create(string path);
	void getInfo(string path, uint64_t Size);
	void addRun(pair<int32_t, uint32_t> Run);
	//void Display();
	//void Read();
	//void Write();
	//void Delete();
private:
	FileInfo info;
	Security pass;
	Data data;
};

#endif