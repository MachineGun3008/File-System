#include "BootSector.h"



void BootSector::createBootSector(uint64_t size_of_vol) {
	ofstream fout("output.bin", ios::binary);
	if (!fout.is_open()) {
		cout << "Error!";
	}
	fout.write(label.c_str(), 8);

	Byte_Per_Sector = 512;
	fout.write((char*)&Byte_Per_Sector, sizeof(uint32_t));

	Sector_Per_Cluster = 8;
	fout.write((char*)&Sector_Per_Cluster, sizeof(uint32_t));

	Total_Sector = (size_of_vol * 1024 * 1024) / 512;
	fout.write((char*)&Total_Sector, sizeof(uint64_t));

	Sector_Begin_Of_CM = 7;
	fout.write((char*)&Sector_Begin_Of_CM, sizeof(uint32_t));

	Cluster_Begin_RDET = 7 + Total_Sector / (512 * 8);
	fout.write((char*)&Cluster_Begin_RDET, sizeof(uint32_t));

	fout.write(cr.c_str(), 48);

	fout.write((char*)&End_Marker, sizeof(uint16_t));
}