#pragma once
#ifndef _Menu_
#define _Menu_
#include "Header.h"
#include"Volume.h"
class Menu
{
public:
	int StartMenu();
	//int OpenVol();
	int CreateVol();
	int OperatorForFolder();
	int OperatorForFile(uint64_t Entry);
	int ListFile();

private:
	Volume Volum;
	list <list<pair<string, uint64_t>>> Folder; // <name of file, Base Entry>
	list<uint64_t> BaseFolder;// Base entry of current folder
};

#endif