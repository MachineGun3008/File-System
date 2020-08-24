#pragma once
#ifndef _File_
#define _File_

#include "FileInfo.h"
#include "Security.h"
#include "Data.h"
#include<fstream>
#include<list>
class File
{
public:
	//File();
	//File(ifstream&, string);
	//void Create(string path);

	// Info
	void getInfo(string path, uint64_t Size);
	void addRun(pair<int32_t, uint32_t> Run);
	void setBase(uint64_t Entry);
	void writeInfo(fstream &Vol);
	uint32_t getSizeInfo();

	// Security
	uint32_t getSizeSecurity();
	void writeSecurity(fstream &Vol);
	void getBlockData(list<DataRun> &Run, int &Size, int &pos);
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