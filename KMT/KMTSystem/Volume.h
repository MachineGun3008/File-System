#pragma once
#ifndef _Volume_
#define _Volume_

#include "File.h"
#include "BootSector.h"
#include "BitMap.h"
#include "RecordHeader.h"

class Volume
{
public:
	void OpenVol();
	void CloseVol();
	void CreateVol();
	void Read();
	void SeekToCluster();
	void SeekToSector();
	void AddFile();
	void AddFolder();
	void DeleteFile();
	void DeleteFolder();
	void MoveFile();
	void MoveFolder();
	void CreatePass();
private:
	File Vol;
	BootSector Boot;
	BitMap ClusterManager;
	vector<RecordHeader> RDET;
};

#endif