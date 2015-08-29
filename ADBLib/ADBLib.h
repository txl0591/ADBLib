// ADBLib.h : ADBLib DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

typedef void(__stdcall *OnReportListener)(int Echo);

extern "C" _declspec(dllexport) void InitADBProcessor(OnReportListener Proc);
extern "C" _declspec(dllexport) void ADBGetFileListFromDir(char* dir);
extern "C" _declspec(dllexport) void ADBDebug(bool state);
extern "C" _declspec(dllexport) void ADBPullFile(char* src, char *dst);
extern "C" _declspec(dllexport) void ADBPullDirFile(char* dir, char *dst, char* file);
extern "C" _declspec(dllexport) void ADBDelFile(char* filename);
extern "C" _declspec(dllexport) void ADBClearFile(char* dir,char* filetype);
