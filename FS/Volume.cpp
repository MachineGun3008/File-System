#include "Volume.h"

bool Volume::OpenVol(string path)
{
	VolPath = path;
	return OpenVol();
}
bool Volume::OpenVol()
{
	Vol.open(VolPath, ios::binary | ios::in | ios::out);
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
	if (Vol.is_open())
	{
		ClusterManager.write(Vol, Boot.getBeginOfCM());
		Vol.close();
	}
}

bool Volume::CreateVol(string p, uint64_t size_of_vol)
{
	VolPath = p;
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
	GetListFreeEntry();
	Vol.close();
	/*Vol.open(p, ios::binary | ios::in | ios::out);
	if (!Vol.is_open())
		return false;*/
	return true;
}
bool Volume::IsOpen()
{
	return Vol.is_open();
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
uint64_t Volume::AddFile(string path, uint64_t RootParent)
{
	CloseVol();
	OpenVol();
	File f;
	pair<bool, uint64_t> PushDataToVol = GetDataFromFile(f, path);
	if (PushDataToVol.first == 0)
		return 0;
	f.getInfo(path, PushDataToVol.second);

	//f.Extract();

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
		Vol.write((char*)&EndAttribute, sizeof(int));
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
	return EntryOfFile;
}

int Volume::AddEntryToFolder(uint64_t EntryOfFile, uint64_t Folder)
{
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
uint64_t Volume::AddFolder(string path, uint64_t RootFolder)
{
	string Name;
	int Begin = path.size() - 1;
	while (Begin >= 0 && path[Begin] != '\\')
		Begin--;
	++Begin;
	Name = path.substr(Begin, path.size() - Begin);
	uint64_t FolderEntry = CreateFolder(Name, RootFolder);
	if (!FolderEntry)
		return 0;
	uint64_t SubEntry = 0;
	bool isFolder = false;
	for (auto entry = fs::directory_iterator(path); entry != fs::directory_iterator(); ++entry)
	{
		isFolder = false;
		Name = entry->path().string();
		if (fs::is_directory(Name))
		{
			isFolder = true;
			SubEntry = AddFolder(Name, FolderEntry);
		}
		else
			SubEntry = AddFile(Name, FolderEntry);
		if (!SubEntry)
		{
			// Need to handle
			DeleteFolder(FolderEntry);
			return 0;
		}
		if (!AddEntryToFolder(SubEntry, FolderEntry))
		{
			//Need to Handle
			if (isFolder)
				DeleteFolder(SubEntry);
			else
				DelFile(SubEntry);
			DeleteFolder(FolderEntry);

			return 0;
		}
	}
	return FolderEntry;
}

void Volume::DelFileInfoInFolder(uint64_t FolderEntry, uint64_t FileEntry)
{
	RecordHeader temp_RecordHeader;
	AttributeHeader temp_AttributeHeader;
	FileInfo temp_FileInfo;
	Index temp_Index;
	int offset = 0;//will point to AH of the file or folder (in the index list of it's parent folder) that u want to delete

	SeekToSector(FolderEntry);

	temp_RecordHeader.read(Vol);//read and move the pointer to the end of the file's header
	offset += 32;

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's 1st attribute header (file name attribute)
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of the file name attribute
	offset += temp_AttributeHeader.getLength() + 8;

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's next attribute header (security name attribute)
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of the security atttribute
	offset += temp_AttributeHeader.getLength() + 8;

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's next attribute header (Index name attribute)
	temp_Index.read(Vol);
	offset += 16;

	int Size = temp_Index.getLength(), AlreadyRead = 0, DeleteMark = 229;

	while (AlreadyRead < Size)
	{
		temp_AttributeHeader.read(Vol);
		temp_FileInfo.read(Vol);

		AlreadyRead += temp_AttributeHeader.getLength() + 8;

		if (temp_FileInfo.getBase() == FileEntry)//check to the file's or folder's entry in the index list of the parent folder
		{
			SeekToSector(FolderEntry);
			Vol.seekg(offset, ios_base::cur);
			Vol.write((char *)&DeleteMark, sizeof(int));
			return;
		}

		offset += temp_AttributeHeader.getLength() + 8;

		// seek raw byte
		Vol.seekg(temp_AttributeHeader.getLength() - temp_FileInfo.getLength(), ios_base::cur);
	}

	while (int64_t(temp_RecordHeader.getEntryChild()) != -1)
	{
		FolderEntry = temp_RecordHeader.getEntryChild();
		offset = 0;

		SeekToSector(FolderEntry);
		temp_RecordHeader.read(Vol);
		offset += 32;

		temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's next attribute header (Index name attribute)
		temp_Index.read(Vol);
		offset += 16;

		Size = temp_Index.getLength(), AlreadyRead = 0;

		while (AlreadyRead < Size)
		{
			temp_AttributeHeader.read(Vol);
			temp_FileInfo.read(Vol);

			AlreadyRead += temp_AttributeHeader.getLength() + 8;

			if (temp_FileInfo.getBase() == FileEntry)
			{
				SeekToSector(FolderEntry);
				Vol.seekg(offset, ios_base::cur);
				Vol.write((char *)&DeleteMark, sizeof(int));
				return;
			}
			offset += temp_AttributeHeader.getLength() + 8;
		}
	}

}

void Volume::DelFile(uint64_t Entry)
{
	list <uint64_t> listOfEntries;
	uint64_t ParentFolder;
	RecordHeader temp_RecordHeader;
	AttributeHeader temp_AttributeHeader;
	DataRun temp_DataRun;

	listOfEntries.push_back(Entry);//save list of entry to change it to 0 later
	
	CloseVol();
	OpenVol();

	SeekToSector(Entry);//move the pointer to file entry that you want to delete
	
	temp_RecordHeader.read(Vol);//read and move the pointer to the end of the file's header
	ParentFolder = temp_RecordHeader.getFolderParent();//check if this file has parent folder or not

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's 1st attribute header (file name attribute)
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of the file name attribute

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's next attribute header (security name attribute)
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of the security atttribute

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's next attribute header (datarun name attribute)
	int Size = temp_AttributeHeader.getLength(), AlreadyRead = 0;

	while (AlreadyRead < Size)//release datarun position in Cluster Manager
	{
		temp_DataRun.read(Vol);
		AlreadyRead += temp_DataRun.getSize();
		pair<uint32_t, uint32_t> Cluster = temp_DataRun.convert();

		// Relese clusters have used
		ClusterManager.update(Cluster.first, Cluster.second);
	}

	while (int64_t(temp_RecordHeader.getEntryChild()) != -1)//check if datarun has more than 1 entry
	{
		Entry = temp_RecordHeader.getEntryChild();
		listOfEntries.push_back(Entry);//store the entry to change it later

		temp_RecordHeader.read(Vol);
		temp_AttributeHeader.read(Vol);

		Size = temp_AttributeHeader.getLength(), AlreadyRead = 0;

		while (AlreadyRead < Size)//release datarun position in Cluster Manager
		{
			temp_DataRun.read(Vol);
			AlreadyRead += temp_DataRun.getSize();
			pair<uint32_t, uint32_t> Cluster = temp_DataRun.convert();

			// Relese clusters have used
			ClusterManager.update(Cluster.first, Cluster.second);
		}
	}


	for (auto i = listOfEntries.begin(); i != listOfEntries.end(); ++i)
	{
		SeekToSector(*i);//move to sector i (the first sector) in the entry of the file that you want to delete
		char Empty = 0;
		Vol.write((char*)&Empty, sizeof(char));//then change it value into 0, it mean the file has been deleted

		// Release Entry for another file to use
		FreeEntry.push_back(*i);
	}

	// Delete file info of this file in parent folder
	if (ParentFolder)
		DelFileInfoInFolder(ParentFolder, listOfEntries.front());

}
bool Volume::isDirectory(uint64_t Entry)
{
	SeekToSector(Entry);
	char Flag;
	Vol.read((char *)&Flag, sizeof(Flag));
	return Flag == 2;

}
void Volume::DeleteFolder(uint64_t Entry)
{
	list<uint64_t> listOfEntries;
	list<uint64_t> FileOfFolder;

	RecordHeader temp_RecordHeader;
	AttributeHeader temp_AttributeHeader;
	Index temp_Index;
	FileInfo temp_FileInfo;
	uint64_t ParentFolder;

	listOfEntries.push_back(Entry);//save entries to delete later

	SeekToSector(Entry);//move pointer to the folder entry that you want to delete

	temp_RecordHeader.read(Vol);//read and move the pointer to the end of the folder's header
	ParentFolder = temp_RecordHeader.getFolderParent();//check if this folder has parent folder or not

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the folder name attribute
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of folder name attribute

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the security attribute
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of it

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the folder's next attribute header
	temp_Index.read(Vol);
	int Size = temp_Index.getLength(), AlreadyRead = 0;

	while (AlreadyRead < Size)//check type in the list then delete it
	{
		temp_AttributeHeader.read(Vol);
		temp_FileInfo.read(Vol);
		//if (isDirectory(temp_FileInfo.getBase()))//check if this folder or not, if it's a folder call func DeleteFolder() 
			//DeleteFolder(temp_FileInfo.getBase());
		//else//esle call func DelFile
			//DelFile(temp_FileInfo.getBase());
		if (temp_AttributeHeader.getType() != 229)//store entry of un-deleted file or folder
			FileOfFolder.push_back(temp_FileInfo.getBase());
		AlreadyRead += 8 + temp_AttributeHeader.getLength();
		Vol.seekg(temp_AttributeHeader.getLength() - temp_FileInfo.getLength(), ios_base::cur);
	}

	while (int64_t(temp_RecordHeader.getEntryChild() != -1)) //check if the list has more than 1 entry
	{
		Entry = temp_RecordHeader.getEntryChild();

		listOfEntries.push_back(Entry);

		SeekToSector(Entry);
		temp_RecordHeader.read(Vol);
		temp_AttributeHeader.read(Vol);
		temp_Index.read(Vol);

		Size = temp_Index.getLength(), AlreadyRead = 0;
		while (AlreadyRead < Size)//check type in the list then delete it
		{
			temp_AttributeHeader.read(Vol);
			temp_FileInfo.read(Vol);
			//if (isDirectory(temp_FileInfo.getBase()))//check if this folder or not, if it's a folder call func DeleteFolder() 
				//DeleteFolder(temp_FileInfo.getBase());
			//else//esle call func DelFile
				//DelFile(temp_FileInfo.getBase());
			if (temp_AttributeHeader.getType() != 229)
				FileOfFolder.push_back(temp_FileInfo.getBase());
			AlreadyRead += 8 + temp_AttributeHeader.getLength();
			Vol.seekg(temp_AttributeHeader.getLength() - temp_FileInfo.getLength(), ios_base::cur);
		}
	}

	for (auto i = listOfEntries.begin(); i != listOfEntries.end(); ++i)
	{
		SeekToSector(*i);//move to sector i (the first sector) in the entry that you want to delete
		char Empty = 0;
		Vol.write((char*)&Empty, sizeof(char));//then change it value into 0, it mean this has been deleted
		FreeEntry.push_back(*i);
	}

	for (auto i = FileOfFolder.begin(); i != FileOfFolder.end(); ++i)//delete file or folder thaht u stored before
	{
		if (isDirectory(*i))
			DeleteFolder(*i);
		else
			DelFile(*i);
	}

	if (ParentFolder)
		DelFileInfoInFolder(ParentFolder, listOfEntries.front());
}
void Volume::MovFile(uint64_t Entry, string path)
{
	//list <uint64_t> listOfEntries;
	uint64_t BaseEntry = Entry;
	RecordHeader temp_RecordHeader;
	AttributeHeader temp_AttributeHeader;
	DataRun temp_DataRun;
	FileInfo info;
	uint64_t filesize = 0;

	//listOfEntries.push_back(Entry);//save list of entry to change it to 0 later

	SeekToSector(Entry);//move the pointer to file entry that you want to move

	temp_RecordHeader.read(Vol);//read and move the pointer to the end of the file's header
	//ParentFolder = temp_RecordHeader.getFolderParent();//check if ths file has parent folder or not

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's 1st attribute header (file name attribute)

	info.read(Vol);
	filesize = info.getFileSize();
	path += info.getName();

	Vol.seekg(temp_AttributeHeader.getLength() - info.getLength(), ios_base::cur);	// seekg

	ofstream fout(path, ios::out | ios::binary);

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's next attribute header (security name attribute)
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of the security atttribute

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the file's next attribute header (datarun name attribute)
	int Size = temp_AttributeHeader.getLength(), AlreadyRead = 0;



	list<pair<uint32_t, uint32_t>> l;
	while (AlreadyRead < Size)//release datarun position in Cluster Manager
	{
		temp_DataRun.read(Vol);
		AlreadyRead += temp_DataRun.getSize();
		pair<uint32_t, uint32_t> Cluster = temp_DataRun.convert();

		l.push_back(Cluster);
	}
	while (int64_t(temp_RecordHeader.getEntryChild()) != -1)//check if datarun has more than 1 entry
	{
		Entry = temp_RecordHeader.getEntryChild();
		//listOfEntries.push_back(Entry);//store the entry to change it later

		temp_RecordHeader.read(Vol);
		temp_AttributeHeader.read(Vol);

		Size = temp_AttributeHeader.getLength(), AlreadyRead = 0;

		while (AlreadyRead < Size)//release datarun position in Cluster Manager
		{
			temp_DataRun.read(Vol);
			AlreadyRead += temp_DataRun.getSize();
			pair<uint32_t, uint32_t> Cluster = temp_DataRun.convert();

			// Relese clusters have used
			l.push_back(Cluster);
		}
	}
	list <pair<uint32_t, uint32_t>> ::iterator it;

	char s[4096];
	uint64_t WriteSize;
	for (it = l.begin(); it != l.end(); it++)
	{
		SeekToCluster(it->second);
		for (int i = 0; i < it->first; i++)
		{
			WriteSize = (4096 <= filesize) ? 4096 : filesize;
			Vol.read((char*)&s, WriteSize * sizeof(char));
			fout.write((char *)&s, WriteSize * sizeof(char));
			filesize -= WriteSize;
		}
	}

	fout.close();
	DelFile(BaseEntry);
}
void Volume::MoveFolder(uint64_t Entry, string path)
{
	//list<uint64_t> listOfEntries;
	list<uint64_t> FileOfFolder;

	RecordHeader temp_RecordHeader;
	AttributeHeader temp_AttributeHeader;
	Index temp_Index;
	FileInfo temp_FileInfo, FolderInfo;
	uint64_t BaseEntry = Entry;

	//listOfEntries.push_back(Entry);//save entries to move later

	SeekToSector(Entry);//move pointer to the folder entry that you want to move

	temp_RecordHeader.read(Vol);//read and move the pointer to the end of the folder's header
	//ParentFolder = temp_RecordHeader.getFolderParent();//check if this folder has parent folder or not

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the folder name attribute

	FolderInfo.read(Vol);

	path += FolderInfo.getName();
	_mkdir(path.c_str());
	//__std_fs_create_directory(path.c_str());

	Vol.seekg(temp_AttributeHeader.getLength() - FolderInfo.getLength(), ios_base::cur);	// seekg

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the security attribute
	Vol.seekg(temp_AttributeHeader.getLength(), ios_base::cur);//skip the info of it

	temp_AttributeHeader.read(Vol);//read and move the pointer to the end of the folder's next attribute header
	temp_Index.read(Vol);
	int Size = temp_Index.getLength(), AlreadyRead = 0;

	while (AlreadyRead < Size)//check type in the list then delete it
	{
		temp_AttributeHeader.read(Vol);
		temp_FileInfo.read(Vol);

		if (temp_AttributeHeader.getType() != 229)//store entry of un-deleted file or folder
			FileOfFolder.push_back(temp_FileInfo.getBase());
		AlreadyRead += 8 + temp_AttributeHeader.getLength();
		Vol.seekg(temp_AttributeHeader.getLength() - temp_FileInfo.getLength(), ios_base::cur);
	}

	while (int64_t(temp_RecordHeader.getEntryChild() != -1)) //check if the list has more than 1 entry
	{
		Entry = temp_RecordHeader.getEntryChild();

		//listOfEntries.push_back(Entry);

		SeekToSector(Entry);
		temp_RecordHeader.read(Vol);
		temp_AttributeHeader.read(Vol);
		temp_Index.read(Vol);

		Size = temp_Index.getLength(), AlreadyRead = 0;
		while (AlreadyRead < Size)//check type in the list then delete it
		{
			temp_AttributeHeader.read(Vol);
			temp_FileInfo.read(Vol);

			if (temp_AttributeHeader.getType() != 229)
				FileOfFolder.push_back(temp_FileInfo.getBase());
			AlreadyRead += 8 + temp_AttributeHeader.getLength();
			Vol.seekg(temp_AttributeHeader.getLength() - temp_FileInfo.getLength(), ios_base::cur);
		}
	}
	path += '\\';
	for (auto i = FileOfFolder.begin(); i != FileOfFolder.end(); ++i)//delete file or folder thaht u stored before
	{
		if (isDirectory(*i))
			MoveFolder(*i, path);
		else
			MovFile(*i, path);
	}
	DeleteFolder(BaseEntry);
}
void Volume::AddPassword(string password, uint64_t Entry)
{
	RecordHeader RH;
	AttributeHeader AH;
	Security S;
	S.encrypt(password);
	int32_t offset = 0;
	CloseVol();
	OpenVol();

	SeekToSector(Entry);
	offset += Entry * 512;
	//Vol.seekg(32, ios_base::cur);
	RH.read(Vol);
	offset += 24;

	AH.read(Vol);	// DataInfo
	offset += AH.getLength();
	Vol.seekg(AH.getLength(), ios_base::cur);

	AH.read(Vol);   // Security
	offset += AH.getLength();

	CloseVol();
	OpenVol();
	Vol.seekg(offset, ios_base::beg);

	S.write(Vol);	// ????????????????????????????????????????????????

}
void Volume::GetFileInfoAndEntry(list<pair<string, uint64_t>> &l, uint64_t base)
{
	RecordHeader RH;
	AttributeHeader AH;
	FileInfo FI;
	if (!base)
	{
		uint64_t Begin = Boot.getBeginRDET(), End = Boot.getTotalCluster();
		End = End * 13 / 100;
		End = End * 8;
		SeekToSector(Begin);
		char Sector[512];
		while (Begin < End)
		{
			int offset = 0;
			RH.read(Vol);
			offset += 32;
			if (RH.getFlag() && !RH.getEntryParent() && !RH.getFolderParent())
			{
				AH.read(Vol);
				offset += 8;
				FI.read(Vol);
				offset += FI.getLength();
				l.push_back({ FI.getName(), Begin });
			}
			Vol.seekg(512 - offset, ios_base::cur); // 512 end
			++Begin;
		}
	}
	else
	{
		SeekToSector(base);
		RH.read(Vol);
		AH.read(Vol);
		Vol.seekg(AH.getLength(), ios_base::cur);
		AH.read(Vol);
		Vol.seekg(AH.getLength(), ios_base::cur);
		AH.read(Vol);
		Index I;
		I.read(Vol);
		int Size = I.getLength(), AlreadyRead = 0;

		while (AlreadyRead < Size)//check type in the list then delete it
		{
			AH.read(Vol);
			FI.read(Vol);

			if (AH.getType() != 229)//store entry of un-deleted file or folder
			{
				l.push_back({ FI.getName(), FI.getBase() });
			}
			AlreadyRead += 8 + AH.getLength();
			Vol.seekg(AH.getLength() - FI.getLength(), ios_base::cur);
		}
		while (int64_t(RH.getEntryChild() != -1)) //check if the list has more than 1 entry
		{
			//Entry = RH.getEntryChild();

			//listOfEntries.push_back(Entry);

			SeekToSector(RH.getEntryChild());
			RH.read(Vol);
			AH.read(Vol);
			I.read(Vol);

			Size = I.getLength(), AlreadyRead = 0;
			while (AlreadyRead < Size)//check type in the list then delete it
			{
				AH.read(Vol);
				FI.read(Vol);

				if (AH.getType() != 229)
					//FileOfFolder.push_back(FI.getBase());
					l.push_back({ FI.getName(), FI.getBase() });
				AlreadyRead += 8 + AH.getLength();
				Vol.seekg(AH.getLength() - FI.getLength(), ios_base::cur);
			}
		}
	}
}
pair<bool, string> Volume::HasPass(uint64_t Entry)
{
	RecordHeader RH;
	AttributeHeader AH;
	Security S;

	CloseVol();
	OpenVol();
	int32_t offset = 0;

	SeekToSector(Entry);
	offset += Entry * 512;
	//Vol.seekg(32, ios_base::cur);
	RH.read(Vol);
	offset += 24;

	AH.read(Vol);	// DataInfo
	offset += AH.getLength();
	Vol.seekg(AH.getLength(), ios_base::cur);

	AH.read(Vol);   // Security
	offset += AH.getLength();

	CloseVol();
	OpenVol();
	Vol.seekg(offset, ios_base::beg);

	S.read(Vol);
	if (S.havePass())
		return { true, S.decrypt() };
	else
		return { 0, "" };
}
