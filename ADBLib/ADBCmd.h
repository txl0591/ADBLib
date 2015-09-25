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

#define ECHO_OK						0
#define ECHO_ERROR_DEVICE_MISS		1
#define ECHO_ERROR_PULLFILE			2
#define ECHO_ERROR_PULLDIR			3
#define ECHO_ERROR_DELFILE			4
#define ECHO_ERROR_CLEARDIR			5

typedef void(__stdcall *OnReportListener)(int Echo);

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
	void SetProc(OnReportListener proc) { mOnReportListener = proc; }
	void RunProc(int echo){ if (mOnReportListener) mOnReportListener(echo); }
	int GetCurrentDirAdb();

public:
	bool mDebug = false;
	OnReportListener mOnReportListener = NULL;
	CList<string, string> mList;
	char* PathExe = NULL;
};

