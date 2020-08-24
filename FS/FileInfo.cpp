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
	int Begin = path.size() - 1;
	while (Begin >= 0 && path[Begin] != '\\')
		--Begin;
	++Begin;
	NameLength = path.size() - Begin;
	Name = path.substr(Begin, path.size() - Begin);
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
uint32_t FileInfo::getLength()
{
	return sizeof(BaseEntry) + sizeof(NameLength) + sizeof(FileSize) + Name.size();
}
void FileInfo::read(fstream &Vol)
{
	Vol.read((char *)&BaseEntry, sizeof(BaseEntry));
	Vol.read((char *)&NameLength, sizeof(NameLength));
	Vol.read((char *)&FileSize, sizeof(FileSize));
	Name.resize(NameLength);
	Vol.read((char *)&Name[0], sizeof(char) * NameLength);
}
uint64_t FileInfo::getBase()
{
	return BaseEntry;
}
string FileInfo::getName()
{
	return Name;
}
uint64_t FileInfo::getFileSize()
{
	return FileSize;
}
