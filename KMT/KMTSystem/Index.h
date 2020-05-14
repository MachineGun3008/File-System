#pragma once
#ifndef _Index_
#define _Index_

#include "AttributeHeader.h"
#include "FileInfo.h"
class Index// folder
{
public:
	void read();
	void write();
	void AddFile();
	void DeleteFile();
private:
	AttributeHeader Header();
	uint8_t Length();
	uint8_t Offset();
	vector<FileInfo> Entry;

};

#endif