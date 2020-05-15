#include "Volume.h"

void Volume::CreateVol(string path, uint64_t size_of_vol)
{
	Vol.open(path, ios::out | ios::binary);
	Boot.createBootSector(size_of_vol);
	int64_t EmptyCluster[512] = { 0 };
	uint64_t Cluster = Boot.getTotalCluster();
	cout << Cluster << '\n';
	for (uint64_t i = 0; i < Cluster; ++i)
		Vol.write((char *)&EmptyCluster, sizeof(EmptyCluster));
	Vol.seekg(ios_base::beg);
	Boot.write(Vol);
}


