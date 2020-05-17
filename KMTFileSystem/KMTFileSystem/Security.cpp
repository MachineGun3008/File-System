#include "Security.h"

Security::Security()
{
	Length = 0;
	HavePass = 0;
	fill(Pass, Pass + 200, 0);
}
uint32_t Security::getSize()
{
	return 205;
}
void Security::write(fstream &Vol)
{
	Vol.write((char *)&Length, sizeof(Length));
	Vol.write((char *)&HavePass, sizeof(HavePass));
	Vol.write((char *)&Pass, sizeof(char) * 200);
}
