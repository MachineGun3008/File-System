#include "FileInfo.h"

FileInfo::FileInfo()
{
	BaseEntry = 0;
	NameLength = 0;
	FileSize = 0;
}


void FileInfo::setBase(uint64_t base)
{
    BaseEntry = base;
}
void FileInfo::getInfo(string path)
{
	NameLength = path.size();
	Name = path;
}
void FileInfo::setFileSize(uint64_t Size)
{
	FileSize = Size;
}
void FileInfo::write(fstream &Vol)
{
	Vol.write((char *)&BaseEntry, sizeof(BaseEntry));
	Vol.write((char *)&NameLength, sizeof(NameLength));
	Vol.write((char *)&FileSize, sizeof(FileSize));
	Vol.write((char *)&Name[0], sizeof(char) * NameLength);
}

