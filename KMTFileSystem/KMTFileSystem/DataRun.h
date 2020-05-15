#pragma once
#ifndef _DataRun_
#define _DataRun_

#include "Header.h"
class DataRun
{
public:
	//DataRun();
	//vector<uint8_t> GetClusterBegin(); // offset begin
	//vector< uint8_t> GetLength();
	void create(uint32_t Length, uint32_t Begin);
	void write(fstream &Vol);
private:
	uint8_t Header;
	vector<uint8_t> Last;
	vector<uint8_t>ClusterBegin; // offset
};

#endif