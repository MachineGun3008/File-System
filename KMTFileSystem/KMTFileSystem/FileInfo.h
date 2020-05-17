#pragma once
#ifndef _FileInfo_
#define _FileInfo_

#include "AttributeHeader.h"
//#include <windows.h>

//struct Time
//{
//	int8_t hour;
//	int8_t minute;
//	int8_t second;
//	long Date; // from 1900
//};

class FileInfo
{
public:
	FileInfo();
	//FileInfo(ifstream&, string);
	//void displayInfo();
	void setBase(uint64_t);
	void getInfo(string path);
	void setFileSize(uint64_t Size);
	void read(fstream &Vol);
	void write(fstream &Vol);
	uint32_t getLength();
	//void update();
	//~FileInfo();
private:
	//AttributeHeader header;
	
	// give up with attribute time
	//SYSTEMTIME TimeCreate, TimeAccess, TimeModify; // TimeCreate.wYear...
	uint64_t BaseEntry;
	uint32_t NameLength;
	//uint32_t Offset;
	uint64_t FileSize;
	string Name;
	friend class File;
};
#endif