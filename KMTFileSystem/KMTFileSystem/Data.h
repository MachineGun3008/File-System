#pragma once
#ifndef _Data_
#define _Data_

#include "AttributeHeader.h"
#include "DataRun.h"
#include "BitMap.h"
#include<list>
class Data
{
public:
	//void read();
	//void write();
	//BitMap GetCluster(); // ???
	void addRun(pair<int32_t, uint32_t> Run);
	void getBlockData(list<DataRun> &ans, int &Size, int &pos);
	//void update();
private:
	//AttributeHeader header;
	uint32_t Length = 0;
	uint32_t Offset;
	vector<DataRun> data;
};

#endif