#pragma once
#ifndef _AttributeHeader_
#define _AttributeHeader_
#include "Header.h"
class AttributeHeader
{
public:
	//AttributeHeader();
	void read(fstream &Vol);
	void write(fstream &Vol);
	int getType();
	int getLength();
	//void update();
	//~AttributeHeader();
	void Create(uint32_t Type, uint32_t Length);
private:
	uint32_t AttributeType;
	uint32_t AttributeLength;
	//uint8_t Flag;
	//char Raw[7];
};

#endif