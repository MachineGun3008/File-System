#include "Volume.h"

bool Volume::OpenVol(string path)
{
	Vol.open(path, ios::binary | ios::in | ios::out);
	if (!Vol.is_open())
		return false;
	Boot.read(Vol);
	ClusterManager.read(Vol, Boot.getBeginOfCM(), Boot.getTotalCluster());
	ClusterManager.GetFreeCluster();
	GetListFreeEntry();
	return true;
}

void Volume::CloseVol()
{
	ClusterManager.write(Vol, Boot.getBeginOfCM());
	Vol.close();
}

bool Volume::CreateVol(string p, uint64_t size_of_vol)
{
	Vol.open(p, ios::binary | ios::out);
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
	Vol.close();
	Vol.open(p, ios::binary | ios::in | ios::out);
	if (!Vol.is_open())
		return false;
	GetListFreeEntry();
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
	if (FreeEntry.empty())
		return { 0, 0 };
	pair<bool, uint64_t> Entry = { 1, FreeEntry.front() };
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
	int cnt = 1;
	list<uint64_t> UseEntry; // Entry use to store file

	//Remain space in entry to store attribute
	int Size = 476;

	pair<bool, uint64_t> Entry = GetFreeEntry();
	if (Entry.first == 0)
	{// Need to handle in here
		auto it = UseEntry.begin();
		--it;
		while (true)
		{
			FreeEntry.push_front(*it);
			if (it == UseEntry.begin())
				break;
			else
				--it;
		}
		ClusterManager.GetFreeCluster();
		return false;
	}
	UseEntry.push_back(Entry.second);
	f.setBase(Entry.second);
	//char Sector[512];
	//int pos = 0;
	SeekToSector(Entry.second);
	Vol.seekg(32, ios_base::cur);
	AttributeHeader AH;
	uint32_t Length = f.getSizeInfo(), Raw = 0;
	Raw = (8 - (Length % 8)) % 8;
	AH.Create(16, Length + Raw);
	AH.write(Vol);
	f.writeInfo(Vol);
	Vol.seekg(Raw, ios_base::cur);
	Size -= Length + Raw + 8;
	Length = f.getSizeSecurity();
	Raw = (8 - (Length % 8)) % 8;
	AH.Create(32, Length + Raw);
	AH.write(Vol);
	f.writeSecurity(Vol);
	Vol.seekg(Raw, ios_base::cur);
	Size -= Length + Raw + 8;
	Size -= 8;
	int pos = 0, tmp = Size;
	list<DataRun> BlockData, Cluster;
	f.getBlockData(BlockData, Size, pos);
	int EndAttribute = -1;
	while (true)
	{
		AH.Create(48, tmp - Size);
		AH.write(Vol);
		for (auto it = BlockData.begin(); it != BlockData.end(); ++it)
		{
			it->write(Vol);
			Cluster.push_back(*it);
		}
		Vol.write((char *)&EndAttribute, sizeof(int));
		if (pos != -1)
		{
			Entry = GetFreeEntry();
			if (Entry.first == 0)
			{// Need to handle in here
				ClusterManager.GetFreeCluster();
				auto it = UseEntry.begin();
				--it;
				while (true)
				{
					FreeEntry.push_front(*it);
					if (it == UseEntry.begin())
						break;
					else
						--it;
				}
				return false;
			}
			UseEntry.push_back(Entry.second);
			Vol.seekg(32, ios_base::cur);
			Size = 468;
			tmp = Size;
			++cnt;
			BlockData.clear();
			f.getBlockData(BlockData, Size, pos);
		}
		else
			break;
	}
	for (auto it = Cluster.begin(); it != Cluster.end(); ++it)
	{
		pair<uint32_t, uint32_t> c = it->convert();
		ClusterManager.update(c.first, c.second);
	}
	RecordHeader Header;
	uint64_t previous = 0;
	for (auto it = UseEntry.begin(); it != UseEntry.end(); ++it)
	{
		uint64_t EntryChild;
		++it;
		if (it != UseEntry.end())
			EntryChild = *it;
		else
		{
			EntryChild = 0;
			EntryChild = ~EntryChild;
		}
		--it;
		Header.create(1, cnt, previous, EntryChild, RootParent);
		SeekToSector(*it);
		previous = *it;
		Header.write(Vol);
	}
	return true;
}
void Volume::UpdateClusterManager(list<DataRun> &Run)
{
	for (auto it = Run.begin(); it != Run.end(); ++it)
	{
		pair<uint32_t, uint32_t> d = it->convert();
	}
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
		if (ByteCount)
		{
			Vol.write((char *)&Cluster, sizeof(char) * 4096);
			++Run.first;
			--FreeCluster.first;
			++FreeCluster.second;
		}
	}
	if (FreeCluster.first)
	{
		ClusterManager.pushBackFreeCluster(FreeCluster);
		f.addRun(Run);
	}
	fin.close();
	return { 1, Size };
}
