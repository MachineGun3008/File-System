#include "FileInfo.h"

FileInfo::FileInfo()
{
	BaseEntry = 0;
	NameLength = 0;
	Offset = 0;
	FileSize = 0;
}

FileInfo::FileInfo(ifstream& in, string path)
{
    ifstream f(path, ios::binary);
    f.seekg(0, ios::end);
    FileSize = f.tellg();
    f.close();
    HANDLE hFile1;
    FILETIME ftCreate,ftAccess,ftModify;
    SYSTEMTIME stUTC;// , stLocal;
    hFile1 = CreateFileA("D:\\text.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile1 == INVALID_HANDLE_VALUE)
    {
        printf("Could not open file, error %ul\n", GetLastError());
    }

    if (!GetFileTime(hFile1, &ftCreate, &ftAccess, &ftModify))
    {
        printf("Something wrong!\n");
    }

    FileTimeToSystemTime(&ftCreate, &stUTC);
    TimeCreate = stUTC;

    FileTimeToSystemTime(&ftAccess, &stUTC);
    TimeAccess = stUTC;

    FileTimeToSystemTime(&ftModify, &stUTC);
    TimeModify = stUTC;

    string temp;
    for (int i=path.length()-1; path[i] != '\\'; i--) 
        temp.push_back(path[i]);
    for (int i = temp.length()-1; i >= 0; i--)
        Name.push_back(temp[i]);
    
    NameLength = Name.length()*8;

    uint64_t x = 0;
    setBase(x);

    Offset = 0;
}

void FileInfo::setBase(uint64_t base)
{
    BaseEntry = base;
}

void FileInfo::displayInfo()
{
    cout << "____FILE_INFO___" << endl;
    cout << "FileName: " << Name  << endl;
    cout << "FIleSize: " << FileSize << " bytes" << endl;

    cout << "Time Create: " << endl;
    cout << TimeCreate.wHour + 7 << ":" << TimeCreate.wMinute << " " << TimeCreate.wDay << "/" << TimeCreate.wMonth << "/" << TimeCreate.wYear << endl;

    cout << "Time Access: " << endl;
    cout << TimeAccess.wHour + 7 << ":" << TimeAccess.wMinute << " " << TimeAccess.wDay << "/" << TimeAccess.wMonth << "/" << TimeAccess.wYear << endl;

    cout << "Time Modify: " << endl;
    cout << TimeModify.wHour + 7 << ":" << TimeModify.wMinute << " " <<TimeModify.wDay << "/" << TimeModify.wMonth << "/" << TimeModify.wYear << endl;

}

FileInfo::~FileInfo()
{
}
