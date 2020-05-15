#pragma once
#ifndef _FileInfo_
#define _FileInfo_

#include "AttributeHeader.h"

class FileInfo
{
public:
	//FileInfo();
	//void read();
	//void write();
	//void update();
	//~FileInfo();
private:
	AttributeHeader header;
	int64_t TimeCreate;
	int64_t TimeUpdate;
	uint64_t BaseEntry;
	uint32_t NameLength;
	uint32_t Offset;
	uint32_t FileSize;
	string Name;
};
#endif
