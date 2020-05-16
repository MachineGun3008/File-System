#pragma once
#ifndef _Volume_
#define _Volume_

#include "File.h"
#include "BootSector.h"
#include "BitMap.h"
#include "RecordHeader.h"
#include<fstream>
#include<list>
class Volume
{
public:
	bool OpenVol(string path);
	void CloseVol();
	bool CreateVol(string path, uint64_t size_of_file);
	//void Read();
	//void SeekToCluster();
	//void SeekToSector();
	bool AddFile(string path, uint64_t RootFolder);
	
	// bool means get data successful, uin64_t is size of file in byte
	pair<bool, uint64_t> GetDataFromFile(File &f, string path);
	//void AddFolder();
	//void DeleteFile();
	//void DeleteFolder();
	//void MoveFile();
	//void MoveFolder();
	//void CreatePass();
	void SeekToCluster(uint32_t position);
	void SeekToSector(uint64_t position);
	void GetListFreeEntry();
	pair<bool, uint64_t> GetFreeEntry();
	void PushBackEntry(uint64_t Entry);
	void display();
private:
	fstream Vol;
	BootSector Boot;
	BitMap ClusterManager;
	list<uint64_t> FreeEntry;
	//string path;
};

#endif