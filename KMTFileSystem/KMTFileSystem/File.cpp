#include "File.h"

void File::getInfo(string path, uint64_t Size)
{
	info.getInfo(path);
	info.setFileSize(Size);
}
void File::addRun(pair<int32_t, uint32_t> Run)
{
	data.addRun(Run);
}
void File::setBase(uint64_t Entry)
{
	info.BaseEntry = Entry;
}
uint32_t File::getSizeInfo()
{
	uint32_t Size = info.Name.size() + 20;
	return Size;
}
void File::writeInfo(fstream &Vol)
{
	info.write(Vol);
}

uint32_t File::getSizeSecurity()
{
	return pass.getSize();
}
void File::writeSecurity(fstream &Vol)
{
	pass.write(Vol);
}
void File::getBlockData(list<DataRun> &Run, int &Size, int &pos)
{
	data.getBlockData(Run, Size, pos);
}
