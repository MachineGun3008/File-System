#pragma once
#ifndef _File_
#define _File_

#include "FileInfo.h"
#include "Security.h"
#include "Data.h"
class File
{
public:
	File();
	File(ifstream&, string);
	void Display();
	//void Read();
	//void Write();
	//void Delete();
private:
	FileInfo info;
	Security pass;
	Data data;
};

#endif