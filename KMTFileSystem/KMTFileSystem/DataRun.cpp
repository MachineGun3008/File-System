#include "DataRun.h"

void DataRun::create(uint32_t Length, uint32_t Begin)
{
	while (Length)
	{
		uint8_t Next = Length % 256;
		Last.push_back(Next);
		Length /= 256;
	}
	reverse(Last.begin(), Last.end());
	while (Begin)
	{
		uint8_t Next = Begin % 256;
		ClusterBegin.push_back(Next);
		Begin /= 256;
	}
	reverse(ClusterBegin.begin(), ClusterBegin.end());
	Header = ClusterBegin.size() * 16 + Last.size();
}
void DataRun::write(fstream &Vol)
{
	Vol.write((char *)&Header, sizeof(Header));
	Vol.write((char *)&Last[0], sizeof(uint8_t) * Last.size());
	Vol.write((char *)&ClusterBegin, sizeof(uint8_t) * ClusterBegin.size());
}

