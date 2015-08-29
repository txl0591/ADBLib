// ADBLib.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "ADBCmd.h"
#include "ADBLib.h"

ADBCmd* mADBCmd;
OnReportListener mOnReportListener = NULL;
void InitADBProcessor(OnReportListener Proc)
{
	mADBCmd = new ADBCmd();
	mOnReportListener = Proc;
}

void ADBGetFileListFromDir(char* dir)
{
	mADBCmd->ExeGetPathFileList(dir);
}

void ADBDebug(bool state)
{
	mADBCmd->SetDebug(state);
}

void ADBPullFile(char* src, char *dst)
{
	mADBCmd->ExeGetADBPull(src, dst);
}

void ADBPullDirFile(char* dir, char *dst, char* file)
{
	mADBCmd->ExeGetADBPullDir(dir, dst, file);
}

void ADBDelFile(char* filename)
{
	mADBCmd->ExeDelFile(filename);
}

void ADBClearFile(char* dir, char* filetype)
{
	mADBCmd->ExeClearDirFile(dir, filetype);
}