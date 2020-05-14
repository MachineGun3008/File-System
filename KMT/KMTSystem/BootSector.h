#pragma once
#ifndef _BOOTSECTOR_
#define _BOOTSECTOR_
#include "Header.h"

class BootSector
{
public:
	void createBootSector(uint64_t size_of_vol);
private:
	const string label = " AKCMTT ";
	uint32_t Byte_Per_Sector;
	uint32_t Sector_Per_Cluster;
	uint64_t Total_Sector;
	uint32_t Sector_Begin_Of_CM;
	uint32_t Cluster_Begin_RDET;
	const string cr = "Copyright belong to Anh Khoa vs Cong Minh vs Thanh Tu";
	const uint16_t End_Marker = 43605;
};
#endif // !_BOOTSECTOR_
