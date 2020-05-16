#pragma once
#ifndef _Security_
#define _Security_

#include "AttributeHeader.h"
class Security
{
public:
	Security();
	//uint32_t encrypt();
	//// uint32_t encode();
	//uint32_t update();
private:
	//AttributeHeader Header;
	uint32_t Length = sizeof(uint32_t) + sizeof(bool) + sizeof (int8_t) * 200;
	//uint32_t Offset;
	bool HavePass = 0;
	int8_t Pass[200];
};

#endif