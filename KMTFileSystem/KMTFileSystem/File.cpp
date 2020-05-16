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
