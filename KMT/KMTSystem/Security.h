#pragma once
#ifndef _Security_
#define _Security_

#include "AttributeHeader.h"
class Security
{
public:
	uint32_t encrypt();
	// uint32_t encode();
	uint32_t update();
private:
	AttributeHeader Header;
	uint32_t Length;
	uint32_t Offset;
	int8_t HavePass;
	int8_t Pass[200];
};

#endif