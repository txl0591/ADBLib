// ADBLib.h : ADBLib DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

typedef void(__stdcall *OnReportListener)(int Echo);

extern "C" _declspec(dllexport) void InitADBProcessor(OnReportListener Proc);
extern "C" _declspec(dllexport) void ADBGetFileListFromDir(char* dir);
extern "C" _declspec(dllexport) void ADBDebug(bool state);
extern "C" _declspec(dllexport) void ADBPullFile(char* src, char *dst);
extern "C" _declspec(dllexport) void ADBPullDirFile(char* dir, char *dst, char* file);
extern "C" _declspec(dllexport) void ADBDelFile(char* filename);
extern "C" _declspec(dllexport) void ADBClearFile(char* dir,char* filetype);
