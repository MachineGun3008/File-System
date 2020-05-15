#include "BitMap.h"
void BitMap::create(uint32_t s)
{
	Size = s / 8 + (s % 8 != 0);
	Bit.resize(Size);
	int cnt = 7;
	while (s % 8)
	{
		Bit.back() |= (1 << cnt);
		--cnt;
		++s;
	}
	cnt = 7 - cnt;
	s -= cnt;
	int SystemArea = s * 13 / 100;
	int i = 0;
	for (; i < SystemArea / 8; ++i)
		Bit[i] = ~Bit[i];
	cnt = 7;
	SystemArea %= 8;
	if (SystemArea)
	{
		Bit[i] |= ((1 << SystemArea) - 1);
	}
}
void BitMap::read(fstream &Vol, uint32_t Position, uint32_t s)
{
	Vol.seekg(ios_base::beg, uint64_t(Position) * 512);
	Size = s / 8 + (s % 8 != 0);
	Bit.resize(Size);
	Vol.read((char *)&Bit[0], sizeof(uint8_t) * Size);
}

void BitMap::write(fstream &Vol, uint32_t Position)
{
	Vol.seekg(uint64_t(Position) * 512, ios_base::beg);
	Vol.write((char *)&Bit[0], sizeof(uint8_t) * Size);
}

void BitMap::GetFreeCluster()
{
	uint32_t length = 0, ClusterBegin = 0, pre = 1, cur, Cluster = 0;
	for (int i = 0; i < Size; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (Bit[i] & (1 << j))
				cur = 1;
			else
				cur = 0;
			if (cur != pre)
			{
				if (pre == 0)
					pq.push({ length, ClusterBegin });
				else
				{
					ClusterBegin = Cluster;
				}
				length = 0;
				pre = cur;
			}
			else
				++length;
			++Cluster;

		}
	}
	if (pre == 0)
		pq.push({ length, ClusterBegin });
}
