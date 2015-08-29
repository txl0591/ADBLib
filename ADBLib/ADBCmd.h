#pragma once
#include <tchar.h>
#include <stdlib.h>
#include <windows.h>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>
#include <string>
#include <WinDef.h>
#include <atlstr.h>
#include <Afxtempl.h>

using namespace std;

#define _DEBUG_	1

class ADBCmd
{
public:
	ADBCmd();
	~ADBCmd();

public:
	string ExeCmd(char* cmd);
	bool ExeGetADBState();
	bool ExeGetADBPull(char* src, char *dst);
	bool ExeGetADBPullDir(char* srcdir, char *dstdir, char* file);
	bool ExeGetPathFileList(char* srcdir);
	bool ExeDelFile(char* pathfile);
	bool ExeClearDirFile(char* path, char *file);
	void SetDebug(bool state) { mDebug = state; }

public:
	bool mDebug = false;
	CList<string, string> mList;
};

