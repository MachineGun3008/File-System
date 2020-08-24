#include "Index.h"

uint32_t Index::getSize()
{
	return sizeof(Length) + sizeof(RemainSize) + Length;
}
void Index::create(uint32_t L, int R)
{
	Length = L, RemainSize = R;
}
void Index::write(fstream &Vol)
{
	Vol.write((char *)&Length, sizeof(Length));
	Vol.write((char *)&RemainSize, sizeof(RemainSize));
}
void Index::read(fstream &Vol)
{
	Vol.read((char *)&Length, sizeof(Length));
	Vol.read((char *)&RemainSize, sizeof(RemainSize));
}
uint32_t Index::getLength()
{
	return Length;
}
int Index::getRemainSize()
{
	return RemainSize;
}
