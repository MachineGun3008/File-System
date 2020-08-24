#pragma once
#ifndef _Security_
#define _Security_

#include "AttributeHeader.h"
#include <stdlib.h>
#include <time.h>
class Security
{
public:
	Security();
	uint32_t getSize();
	void write(fstream &Vol);
	void read(fstream& vol);
	void encrypt(string password);
	string decrypt();
	bool havePass();
	//bool update(); // ???
private: 
	//AttributeHeader Header; 
	uint32_t Length = 0; // length for what 
	//uint32_t Offset;
	bool HavePass = 0;
	int8_t Pass[200];
};

#endif