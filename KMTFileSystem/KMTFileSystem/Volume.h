#pragma once
#ifndef _Volume_
#define _Volume_

#include "File.h"
#include "BootSector.h"
#include "BitMap.h"
#include "RecordHeader.h"
#include<fstream>
class Volume
{
public:
	//void OpenVol();
	//void CloseVol();
	void CreateVol(string path, uint64_t size_of_file);
	//void Read();
	//void SeekToCluster();
	//void SeekToSector();
	//void AddFile();
	//void AddFolder();
	//void DeleteFile();
	//void DeleteFolder();
	//void MoveFile();
	//void MoveFolder();
	//void CreatePass();
private:
	fstream Vol;
	BootSector Boot;
	BitMap ClusterManager;
	vector<RecordHeader> RDET;
};

#endif