#pragma once
#ifndef _BITMAP_
#define _BITMAP_
#include<iostream>
#include<vector>
using namespace std;

class BitMap
{
public:
	BitMap();
	void read();
	void write();
	void update();
	~BitMap();
private:
	uint32_t Size;
	vector<char> Bit;
};
#endif // !_BITMAP_

