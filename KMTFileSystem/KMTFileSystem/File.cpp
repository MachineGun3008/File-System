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

HuffmanNode* File::getTree()
{
	return root;
}
void File::setTree(HuffmanNode* p)
{
	root = p;
}
void File::setD(string s)
{
	d = s;
}
void File::setAbits(int i)
{
	Abits = i;
}
string File::getD()
{
	return d;
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

void File::Extract()
{
	string path;
	cout << "Extract path: "; cin >> path;

	path += info.getName();

	string s = decode(d, root,Abits);
	ofstream fout(path, ios::out, ios::binary);

	fout << s;

	fout.close();
}
