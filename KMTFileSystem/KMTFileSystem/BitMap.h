#pragma once
#ifndef _BITMAP_
#define _BITMAP_
#include "Header.h"
#include<queue>
class BitMap
{
public:
	//BitMap();
	void read(fstream &Vol, uint32_t Position, uint32_t s); // position is sector begin of cluster manager
	void write(fstream &Vol, uint32_t Position);
	void GetFreeCluster();
	//void update();
	//~BitMap();
	void create(uint32_t s); // s is total cluster
private:
	uint32_t Size;
	vector<uint8_t> Bit;
	priority_queue<pair<uint32_t, uint32_t>> pq; // <length of free cluster, cluster begin>
};
#endif // !_BITMAP_

