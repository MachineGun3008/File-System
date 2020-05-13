#pragma once
#ifndef _RECORDHEADER_
#define _RECORDHEADER_
#include<iostream>
using namespace std;
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

