#include "RecordHeader.h"

void RecordHeader::create(uint8_t flag, uint32_t entries, uint64_t entryparent, uint64_t entrychild, uint64_t folderparent)
{
	Flag = flag;
	if (entrychild == 0)
		entrychild = ~entrychild;
	Entries = entries, EntryParent = entryparent, EntryChild = entrychild, FolderParent = folderparent;
}
void RecordHeader::write(fstream &Vol)
{
	Vol.write((char *)&Flag, 1);
	Vol.write((char *)&Label, 3);
	Vol.write((char *)&Entries, sizeof(Entries));
	Vol.write((char *)&EntryParent, sizeof(EntryParent));
	Vol.write((char *)&EntryChild, sizeof(EntryChild));
	Vol.write((char *)&FolderParent, sizeof(FolderParent));
}
void RecordHeader::read(fstream &Vol)
{
	Vol.read((char *)&Flag, 1);
	Vol.read((char *)&Label, 3);
	Vol.read((char *)&Entries, sizeof(Entries));
	Vol.read((char *)&EntryParent, sizeof(EntryParent));
	Vol.read((char *)&EntryChild, sizeof(EntryChild));
	Vol.read((char *)&FolderParent, sizeof(FolderParent));
}
uint64_t RecordHeader::getEntryChild()
{
	return EntryChild;
}
uint64_t RecordHeader::getFolderParent()
{
	return FolderParent;
}
void RecordHeader::setEntries(uint32_t e)
{
	Entries = e;
}


