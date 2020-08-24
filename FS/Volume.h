#pragma once
#ifndef _Volume_
#define _Volume_

#include "File.h"
#include "BootSector.h"
#include "BitMap.h"
#include "RecordHeader.h"
#include "Index.h"
#include <direct.h>
#include <filesystem>
namespace fs = filesystem;

class Volume
{
public:
	bool OpenVol(string path);
	bool OpenVol();
	void CloseVol();
	bool CreateVol(string path, uint64_t size_of_file);
	bool IsOpen();
	//void Read();
	//void SeekToCluster();
	//void SeekToSector();
	
	// File
	// add file
	uint64_t AddFile(string path, uint64_t RootFolder);
	void AddPassword(string password, uint64_t Entry);

	// bool means get data successful, uin64_t is size of file in byte
	pair<bool, uint64_t> GetDataFromFile(File &f, string path);
	
	// delete file
	void DelFile(uint64_t Entry);
	void DelFileInfoInFolder(uint64_t FolderEntry, uint64_t FileEntry);
	
	// delete folder
	void DeleteFolder(uint64_t Entry);
	bool isDirectory(uint64_t Entry);

	//void AddFolder();
	//void DelFile();
	//void DeleteFolder();
	//void MoveFile();
	//void MoveFolder();
	//void CreatePass();

	// Folder

	// For add folder
	int CreateFolder(string Name, uint64_t RootFolder);
	int AddFileToFolder(string path, uint64_t Folder);
	FileInfo GetFileInfo(uint64_t Entry);
	uint64_t AddFolder(string paht, uint64_t RootFolder);
	int AddEntryToFolder(uint64_t EntryOfFile, uint64_t Folder);
	void MovFile(uint64_t Entry, string path);
	void MoveFolder(uint64_t Entry, string path);
	// for delete folder

	void SeekToCluster(uint32_t position);
	void SeekToSector(uint64_t position);
	void GetListFreeEntry();
	pair<bool, uint64_t> GetFreeEntry();
	void PushBackEntry(uint64_t Entry);
	void UpdateClusterManager(list<DataRun> &Run);
	void display();
	void GetFileInfoAndEntry(list<pair<string, uint64_t>> &l, uint64_t base);
	pair<bool, string> HasPass(uint64_t Entry);
private:
	fstream Vol;
	BootSector Boot;
	BitMap ClusterManager;
	list<uint64_t> FreeEntry;
	string VolPath;
};

#endif