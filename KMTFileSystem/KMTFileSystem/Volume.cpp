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
int Volume::AddFile(string path, uint64_t RootParent)
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
		if (UseEntry.empty())
			return 0;
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
		return 0;
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
				return 0;
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
	return UseEntry.front();
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
int Volume::CreateFolder(string Name, uint64_t RootFolder)
{
	pair<bool, uint64_t> Entry = GetFreeEntry();
	if (!Entry.first)
		return 0;
	int Size = 512;
	RecordHeader Header;
	Header.create(2, 1, 0, 0, RootFolder);
	SeekToSector(Entry.second);
	Header.write(Vol);
	Size -= 32;
	AttributeHeader AH;
	int Length, Raw;
	FileInfo Info;
	Info.getInfo(Name);
	Info.setBase(Entry.second);
	Info.setFileSize(0);
	Length = Info.getLength();
	Raw = (8 - (Length % 8)) % 8;
	AH.Create(16, Length + Raw);
	AH.write(Vol);
	Info.write(Vol);
	Vol.seekg(Raw, ios_base::cur);
	Size -= Length + Raw + 8;
	Security S;
	Length = S.getSize();
	Raw = (8 - (Length % 8)) % 8;
	AH.Create(32, Length + Raw);
	AH.write(Vol);
	S.write(Vol);
	Vol.seekg(Raw, ios_base::cur);
	Size -= Length + Raw + 8 + 4;
	Index I;
	Length = I.getSize();
	Raw = (8 - (Length % 8)) % 8;
	AH.Create(64, Length);
	AH.write(Vol);
	Size -= 16;
	I.create(0, Size);
	I.write(Vol);
	int EndAttribute = ~0;
	Vol.write((char *)&EndAttribute, sizeof(EndAttribute));
	return Entry.second;
}
int Volume::AddFileToFolder(string path, uint64_t Folder)
{
	uint64_t EntryOfFile = AddFile(path, Folder);
	if (!EntryOfFile)
		return 0;
	FileInfo Info = GetFileInfo(EntryOfFile);
	AttributeHeader AH;
	int Length = Info.getLength();
	int Raw = (8 - (Length % 8)) % 8;
	AH.Create(16, Length + Raw);
	Length += Raw + 8;
	bool HasAdd = false;
	list<uint64_t> Entry;
	Entry.push_back(Folder);
	RecordHeader Header;
	AttributeHeader ah;
	int offset = 0;
	while (true)
	{
		SeekToSector(Entry.back());
		Header.read(Vol);
		offset += 32;
		while (true)
		{
			ah.read(Vol);
			//offset += 8;
			int type = ah.getType();
			if (type == 64 || type == -1)
				break;
			Vol.seekg(ah.getLength(), ios_base::cur);
			offset += ah.getLength() + 8;
		}
		if (ah.getType() == -1)
		{
			uint64_t NextEntry = Header.getEntryChild();
			if ((int64_t)NextEntry == -1)
				break;
			else
			{
				Entry.push_back(NextEntry);
				offset = 0;
			}
		}
		else
		{
			//offset += 8;
			Index I;
			I.read(Vol);
			//AH.write(Vol); Info.write(Vol);
			//HasAdd = true;
			//break;
			if (I.getRemainSize() >= Length)
			{
				if (I.getLength())
					Vol.seekg(I.getLength(), ios_base::cur);
				SeekToSector(Entry.back());
				Vol.seekg(offset + 16 + I.getLength(), ios_base::cur);
				AH.write(Vol);
				Info.write(Vol);
				Vol.seekg(Raw, ios_base::cur);
				int EndMark = -1;
				Vol.write((char *)&EndMark, sizeof(int));
				I.create(I.getLength() + Length, I.getRemainSize() - Length);
				SeekToSector(Entry.back());
				Vol.seekg(offset, ios_base::cur);
				ah.Create(64, I.getSize());
				ah.write(Vol);
				I.write(Vol);
				HasAdd = true;
				break;
			}
			else
			{
				uint64_t NextEntry = Header.getEntryChild();
				if ((int64_t)NextEntry == -1)
					break;
				else
				{
					Entry.push_back(NextEntry);
					offset = 0;
				}
			}
		}
	}
	if (!HasAdd)
	{
		pair<bool, uint64_t> NewEntry = GetFreeEntry();
		if (NewEntry.first == 0)
		{
			// delete file
			return 0;
		}
		Header.create(2, Entry.size() + 1, Entry.back(), 0, Header.getFolderParent());
		SeekToSector(NewEntry.second);
		Header.write(Vol);
		AH.Create(64, 8 + Length);
		AH.write(Vol);
		Index I;
		I.create(Length, 512 - 32 - 8 - 4 - 8 - Length);
		I.write(Vol);
		AH.Create(16, Length - 8);
		AH.write(Vol);
		Info.write(Vol);
		int EndMark = -1;
		Vol.write((char *)&EndMark, sizeof(int));
		for (auto it = Entry.begin(); it != Entry.end(); ++it)
		{
			SeekToSector(*it);
			Vol.seekg(4, ios_base::cur);
			uint32_t cnt = Entry.size() + 1;
			Vol.write((char *)&cnt, sizeof(cnt));
		}
		auto it = Entry.end();
		--it;
		SeekToSector(*it);
		Vol.seekg(16, ios_base::cur);
		Vol.write((char *)&NewEntry.second, sizeof(uint64_t));
	}
	return 1;
}
FileInfo Volume::GetFileInfo(uint64_t Entry)
{
	SeekToSector(Entry);
	Vol.seekg(40, ios_base::cur);
	FileInfo ans;
	ans.read(Vol);
	return ans;
}
