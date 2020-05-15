#include "Volume.h"

void Volume::openvol()
{
	Vol.open(path);
	
}

void Volume::closevol()
{
	Vol.close();
}

void Volume::CreateVol(string p, uint64_t size_of_vol)
{
	Vol.open(p, ios::out | ios::binary);
	Boot.createBootSector(size_of_vol);
	int64_t EmptyCluster[512] = { 0 };
	uint64_t Cluster = Boot.getTotalCluster();
	//cout << Cluster << '\n';
	for (uint64_t i = 0; i < Cluster; ++i)
		Vol.write((char *)&EmptyCluster, sizeof(EmptyCluster));
	Vol.seekg(ios_base::beg);
	Boot.write(Vol);
	ClusterManager.create(Cluster);
	uint32_t BeginOfCM = Boot.getBeginOfCM();
	ClusterManager.write(Vol, BeginOfCM);

}

void Volume::AddFile()
{
	ifstream fin(path, ios::binary);

	string path;
	cout << "Enter file path: "; cin >> path;
	File f(fin, path);
	f.Display();

	fin.close();
}
