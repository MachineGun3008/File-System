#pragma once
#ifndef _AttributeHeader_
#define _AttributeHeader_
#include "Header.h"
class AttributeHeader
{
public:
	//AttributeHeader();
	//void read();
	//void write();
	//void update();
	//~AttributeHeader();
private:
	uint32_t AttributeType;
	uint32_t AttributeLength;
	uint8_t Flag;
	char Raw[7];
};

#endif