#include "DataRun.h"

int DataRun::create(uint32_t Length, uint32_t Begin)
{
	while (Length)
	{
		uint8_t Next = Length % 256;
		Last.push_back(Next);
		Length /= 256;
	}
	while (Begin)
	{
		uint8_t Next = Begin % 256;
		ClusterBegin.push_back(Next);
		Begin /= 256;
	}
	Header = ClusterBegin.size() * 16 + Last.size();
	return ClusterBegin.size() + Last.size() + 1;
}
void DataRun::write(fstream &Vol)
{
	Vol.write((char *)&Header, sizeof(Header));
	Vol.write((char *)&Last[0], sizeof(uint8_t) * Last.size());
	Vol.write((char *)&ClusterBegin[0], sizeof(uint8_t) * ClusterBegin.size());
}
int DataRun::getSize()
{
	return Last.size() + ClusterBegin.size() + 1;
}
pair<uint32_t, uint32_t> DataRun::convert()
{
	pair<uint32_t, uint32_t> ans = { 0, 0 };
	for (int i = 0; i < Last.size(); ++i)
		ans.first = ans.first * 8 + Last[i];
	for (int i = 0; i < ClusterBegin.size(); ++i)
		ans.second = ans.second * 8 + ClusterBegin[i];
	return ans;
}
void DataRun::read(fstream &Vol)
{
	Vol.read((char *)&Header, sizeof(uint8_t));
	Last.resize(Header % 16);
	Vol.read((char *)&Last[0], sizeof(uint8_t) * Last.size());
	ClusterBegin.resize(Header / 16);
	Vol.read((char *)&ClusterBegin[0], sizeof(uint8_t) * ClusterBegin.size());
}

