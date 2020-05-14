#pragma once
#ifndef _RecordHeader_
#define _RecordHeader_
#include "Header.h"

class RecordHeader
{
public:
	RecordHeader();
	void read();
	void write();
	void update();
	~RecordHeader();
private:
	uint8_t Flag;
	const char Label[3] = { 'K', 'M', 'T' };
	uint32_t Entries;
	uint64_t EntryParent;
	uint64_t EntryChild;
	uint64_t FolderParent;
	uint32_t FirstAttribute;
	uint32_t Raw;
};
#endif // !_RECORDHEADER_

