#include "Volume.h"

bool Volume::OpenVol(string path)
{
	Vol.open(path, ios::binary | ios::in | ios::out);
	if (!Vol.is_open())
		return false;
	Boot.read(Vol);
	ClusterManager.read(Vol, Boot.getBeginOfCM(), Boot.getTotalCluster());
	ClusterManager.GetFreeCluster();
	return true;
}

void Volume::CloseVol()
{
	Vol.close();
}

bool Volume::CreateVol(string p, uint64_t size_of_vol)
{
	Vol.open(p, ios::out | ios::binary);
	if (!Vol.is_open())
		return false;
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
	return true;
}
void Volume::display()
{
	Boot.display();
	ClusterManager.display();
}
void Volume::SeekToSector(uint64_t position)
{
	Vol.seekg(position * 512, ios_base::beg);
}
void Volume::GetListFreeEntry()
{
	uint64_t Begin = Boot.getBeginRDET(), End = Boot.getTotalCluster();
	End = End * 13 / 100;
	End = End * 8;
	SeekToSector(Begin);
	char Sector[512];
	while (Begin < End)
	{
		Vol.read((char *)&Sector, 512);
		if (Sector[0] == 0)
			FreeEntry.push_back(Begin);
		++Begin;
	}
}
pair<bool, uint64_t> Volume::GetFreeEntry()
{
	if (!FreeEntry.empty())
		return { -1, 0 };
	pair<bool, uint64_t> Entry = { 1, FreeEntry.front };
	FreeEntry.pop_front();
	return Entry;
}
void Volume::PushBackEntry(uint64_t Entry)
{
	FreeEntry.push_back(Entry);
}
bool Volume::AddFile(string path, uint64_t RootParent)
{
	File f;
	pair<bool, uint64_t> PushDataToVol = GetDataFromFile(f, path);
	if (PushDataToVol.first == 0)
		return 0;
	f.getInfo(path, PushDataToVol.second);
	
	// count how many entry
	int cnt = 0;
	list<uint64_t> UseEntry; // Entry use to store file

	//Remain space in entry to store attribute
	uint32_t Size = 468;

	pair<bool, uint64_t> Entry = GetFreeEntry();
	if (Entry.first == 0)
		return false;
	char Sector[512];
	int pos = 0;
	Sector[0] = 1, Sector[1] = 'K', Sector[2] = 'M', Sector[3] = 'T';
	
	SeekToSector(Entry.second);
	return true;
}
void Volume::SeekToCluster(uint32_t position)
{
	Vol.seekg((uint64_t)position * 4096, ios_base::beg);
}
pair<bool, uint64_t> Volume::GetDataFromFile(File &f, string path)
{
	ifstream fin(path, ios::binary | ios::in);
	if (!fin.is_open())
	{
		return { 0, 0 };
	}
	char Cluster[4096];
	pair<int32_t, uint32_t> FreeCluster = ClusterManager.GetCluster(), Run;
	if (FreeCluster.first == -1)
	{
		fin.close();
		return { 0, 0 };
	}
	uint64_t Size = 0;
	uint32_t CurrentCluster = FreeCluster.second;
	Run = FreeCluster;
	Run.first = 0;
	SeekToCluster(CurrentCluster);
	while (!fin.eof())
	{
		if (!FreeCluster.first)
		{
			FreeCluster = ClusterManager.GetCluster();
			if (FreeCluster.first == -1)
			{
				fin.close();
				return { 0, 0 };
			}
			f.addRun(Run);
			Run = FreeCluster;
			Run.first = 0;
			CurrentCluster = FreeCluster.second;
			SeekToCluster(CurrentCluster);
		}
		
		// muon compress data thi code trong block nay lam sao tra ve Cluster da nen data
		// bat dau cho nen
		fin.read((char *)&Cluster, sizeof(char) * 4096);
		
		// ket thuc nen
		uint32_t ByteCount = fin.gcount();
		Size += ByteCount;
		if (ByteCount < 4096)
			fill(Cluster + ByteCount, Cluster + 4096, 0);
		Vol.write((char *)&Cluster, sizeof(char) * 4096);
		++Run.first;
		--FreeCluster.first;
		++FreeCluster.second;
	}
	if (FreeCluster.first)
	{
		ClusterManager.pushBackFreeCluster(FreeCluster);
		f.addRun(Run);
	}
	fin.close();
	return { 1, Size };
}
