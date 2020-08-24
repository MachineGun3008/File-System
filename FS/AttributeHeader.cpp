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
void AttributeHeader::read(fstream &Vol)
{
	Vol.read((char *)&AttributeType, sizeof(uint32_t));
	Vol.read((char *)&AttributeLength, sizeof(uint32_t));
}
int AttributeHeader::getType()
{
	return AttributeType;
}
int AttributeHeader::getLength()
{
	return AttributeLength;
}

