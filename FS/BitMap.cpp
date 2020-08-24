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
	Vol.seekg(uint64_t(Position) * 512, ios_base::beg);
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
	uint32_t ClusterBegin = 0, pre = 1, cur, Cluster = 0;
	int32_t length = 0;
	while (!pq.empty())
		pq.pop();
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
				length = 1;
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
pair<int32_t, uint32_t> BitMap::GetCluster()
{
	if (pq.empty())
		return { -1, 0 };
	pair<int32_t, uint32_t> BlockCluster = pq.top();
	pq.pop();
	return BlockCluster;
}
void BitMap::display()
{
	cout << Size << '\n';
	for (int i = 0; i < Size; ++i)
		cout << Bit[i] << ' ';
	cout << '\n';
}
void BitMap::pushBackFreeCluster(pair<int32_t, uint32_t> FreeCluster)
{
	pq.push(FreeCluster);
}
void BitMap::update(uint32_t Last, uint32_t Begin)
{
	int i = Begin / 8, j = Begin % 8;
	while (Last)
	{
		Bit[i] ^= (1 << j);
		++j;
		if (j == 8)
			++i, j = 0;
		--Last;
	}
}
