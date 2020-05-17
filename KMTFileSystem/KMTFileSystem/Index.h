#pragma once
#ifndef _Index_
#define _Index_

#include "AttributeHeader.h"
#include "FileInfo.h"
class Index// folder
{
public:
	//void read();
	void write(fstream &Vol);
	void read(fstream &Vol);
	//void AddFile();
	//void DeleteFile();
	uint32_t getSize();
	void create(uint32_t L, int R);
	uint32_t getLength();
	int getRemainSize();
private:
	//AttributeHeader Header();
	uint32_t Length = 0;
	int RemainSize = 0;
	//uint8_t Offset;
	//vector<FileInfo> Entry;

};

#endif