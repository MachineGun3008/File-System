#pragma once
#ifndef _RecordHeader_
#define _RecordHeader_
#include "Header.h"

class RecordHeader
{
public:
	//RecordHeader();
	void read(fstream &Vol);
	//void write();
	//void update();
	//~RecordHeader();
	void create(uint8_t flag, uint32_t entries, uint64_t entryparent, uint64_t entrychild, uint64_t folderparent);
	void write(fstream &Vol);
	uint64_t getEntryChild();
	uint64_t getFolderParent();
	void setEntries(uint32_t e);
private:
	uint8_t Flag;
	const char Label[3] = { 'K', 'M', 'T' };
	uint32_t Entries;
	uint64_t EntryParent;
	uint64_t EntryChild;
	uint64_t FolderParent;
	//uint32_t FirstAttribute;
	//uint32_t Raw;
};
#endif // !_RECORDHEADER_

