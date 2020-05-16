#include "AttributeHeader.h"

void AttributeHeader::Create(uint32_t Type, uint32_t Length)
{
	AttributeType = Type, AttributeLength = Length;
}
void AttributeHeader::write(fstream &Vol)
{
	Vol.write((char *)&AttributeType, sizeof(uint32_t));
	Vol.write((char *)&AttributeLength, sizeof(uint32_t));
}


