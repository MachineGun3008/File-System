#pragma once
#ifndef _Volume_
#define _Volume_

#include "File.h"
#include "BootSector.h"
#include "BitMap.h"
#include "RecordHeader.h"
#include "Index.h"
#include<experimental/filesystem>
namespace fs = experimental::filesystem;
class Volume
{
public:
	bool OpenVol(string path);
	void CloseVol();
	bool CreateVol(string path, uint64_t size_of_file);
	//void Read();
	//void SeekToCluster();
	//void SeekToSector();
	int AddFile(string path, uint64_t RootFolder);
	
	// bool means get data successful, uin64_t is size of file in byte
	pair<bool, uint64_t> GetDataFromFile(File &f, string path);
	//void AddFolder();
	//void DeleteFile();
	//void DeleteFolder();
	//void MoveFile();
	//void MoveFolder();
	//void CreatePass();

	// Folder
	int CreateFolder(string Name, uint64_t RootFolder);
	int AddFileToFolder(string path, uint64_t Folder);
	FileInfo GetFileInfo(uint64_t Entry);
	int AddFolder(string paht, uint64_t RootFolder);
	int AddEntryToFolder(uint64_t EntryOfFile, uint64_t Folder);
	void ExtractFile();

	void SeekToCluster(uint32_t position);
	void SeekToSector(uint64_t position);
	void GetListFreeEntry();
	pair<bool, uint64_t> GetFreeEntry();
	void PushBackEntry(uint64_t Entry);
	void UpdateClusterManager(list<DataRun> &Run);
	void display();
private:
	fstream Vol;
	BootSector Boot;
	BitMap ClusterManager;
	list<uint64_t> FreeEntry;
	//string path;
};

#endif