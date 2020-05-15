#pragma once
#ifndef _FileInfo_
#define _FileInfo_

#include "AttributeHeader.h"
#include <windows.h>

struct Time
{
	int8_t hour;
	int8_t minute;
	int8_t second;
	long Date; // from 1900
};

class FileInfo
{
public:
	FileInfo();
	FileInfo(ifstream&, string);
	void displayInfo();
	void setBase(uint64_t);
	//void read();
	//void write();
	//void update();
	~FileInfo();
private:
	//AttributeHeader header;
	SYSTEMTIME TimeCreate, TimeAccess, TimeModify; // TimeCreate.wYear...
	uint64_t BaseEntry;
	uint32_t NameLength;
	uint32_t Offset;
	uint32_t FileSize;
	string Name;
};
#endif