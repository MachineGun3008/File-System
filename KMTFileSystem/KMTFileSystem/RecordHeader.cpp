#include "RecordHeader.h"

void RecordHeader::create(uint8_t flag, uint32_t entries, uint64_t entryparent, uint64_t entrychild, uint64_t folderparent)
{
	Flag = flag;
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
