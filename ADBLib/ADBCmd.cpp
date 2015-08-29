#include "stdafx.h"
#include "ADBCmd.h"


void CharToTchar(char * _char, TCHAR * tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

string ADBCmd::ExeCmd(char* cmd)
{
	//创建匿名管道
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return "";
	}
	//设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite)
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	//启动命令行
	PROCESS_INFORMATION pi;
	TCHAR cmdline[200];
	CharToTchar(cmd, cmdline);
	if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		return "";
	}
	//立即关闭hWrite
	CloseHandle(hWrite);
	//读取命令行返回值
	std::string strRet;
	char buff[1024] = { 0 };
	DWORD dwRead = 0;
	while (ReadFile(hRead, buff, 1024, &dwRead, NULL))
	{
		strRet.append(buff, dwRead);
	}
	CloseHandle(hRead);
	return strRet;
}

bool ADBCmd::ExeGetADBState()
{
	char buf[100];
	string echo = ExeCmd("adb get-state");
	const char *ptr1 = echo.data();

	char *p = strtok_s((char*)ptr1, (const char*)"device", (char**)&buf);

	if (p == NULL)
	{
		return false;
	}

	return true;
}

bool ADBCmd::ExeGetADBPull(char* src, char *dst)
{
	char Cmd[200];
	memset(Cmd, 0, sizeof(Cmd));
	if (dst != NULL)
	{
		sprintf_s(Cmd, "adb pull %s %s \n", src, dst);
	}
	else
	{
		sprintf_s(Cmd, "adb pull %s \n", src);
	}
	if (mDebug){
		cout << "Cmd [" << Cmd << "]" << endl;
	}

	ExeCmd(Cmd);
	return true;
}

bool ADBCmd::ExeGetADBPullDir(char* srcdir, char *dstdir, char* file)
{
	char Cmd[200];
	bool Oper = false;
	ExeGetPathFileList(srcdir);

	POSITION pos = mList.GetHeadPosition();
	while (pos != NULL)
	{
		string ptr = mList.GetNext(pos);
		const char* filename = ptr.data();
		memset(Cmd, 0, sizeof(Cmd));
		char filetype[4];

		filetype[0] = filename[ptr.length() - 3];
		filetype[1] = filename[ptr.length() - 2];
		filetype[2] = filename[ptr.length() - 1];
		filetype[3] = '\0';

		if (NULL == file)
		{
			Oper = true;
		}
		else
		{
			if (!strcmp(filetype, file))
			{
				Oper = true;
			}
		}

		if (Oper)
		{
			if (dstdir != NULL)
			{
				sprintf_s(Cmd, "adb pull %s/%s %s \n", srcdir, filename, dstdir);
			}
			else
			{
				sprintf_s(Cmd, "adb pull %s/%s \n", srcdir, filename);
			}
			ExeCmd(Cmd);
		}
	}

	return true;
}

bool ADBCmd::ExeGetPathFileList(char* srcdir)
{
	char Cmd[200];
	memset(Cmd, 0, sizeof(Cmd));
	sprintf_s(Cmd, "adb shell ls %s", srcdir);
	if (!mList.IsEmpty()){
		mList.RemoveAll();
	}
	string echo = ExeCmd(Cmd);
	const char *ptra = echo.data();
	char *next_token = nullptr;
	char *strToken = strtok_s((char*)ptra, (const char*)"\r\n\\", (char**)&next_token);
	while (strToken != NULL) {
		string s(strToken);
		mList.AddTail(s);
		strToken = strtok_s(nullptr, (const char*)"\r\n\\", &next_token);
	}
	
	if (mDebug)
	{
		POSITION pos = mList.GetHeadPosition();
		while (pos != NULL)
		{
			string ptr = mList.GetNext(pos);
			cout << "List [" + ptr + "]" << endl;
		}
	}
	return true;
}

bool ADBCmd::ExeDelFile(char* pathfile)
{
	char Cmd[200];
	memset(Cmd, 0, sizeof(Cmd));
	sprintf_s(Cmd, "adb shell rm %s \n", pathfile);
	if (mDebug){
		cout << "Cmd [" << Cmd << "]" << endl;
	}

	ExeCmd(Cmd);
	return true;
}

bool ADBCmd::ExeClearDirFile(char* path, char *file)
{
	char Cmd[200];
	bool Oper = false;
	ExeGetPathFileList(path);

	POSITION pos = mList.GetHeadPosition();
	while (pos != NULL)
	{
		string ptr = mList.GetNext(pos);
		const char* filename = ptr.data();
		memset(Cmd, 0, sizeof(Cmd));
		char filetype[4];

		filetype[0] = filename[ptr.length() - 3];
		filetype[1] = filename[ptr.length() - 2];
		filetype[2] = filename[ptr.length() - 1];
		filetype[3] = '\0';

		if (NULL == file)
		{
			Oper = true;
		}
		else
		{
			if (!strcmp(filetype, file))
			{
				Oper = true;
			}
		}

		if (Oper)
		{
			sprintf_s(Cmd, "adb shel rm %s/%s\n", path, filename);
			ExeCmd(Cmd);
		}
	}

	return true;
}

ADBCmd::ADBCmd()
{
}

ADBCmd::~ADBCmd()
{
}
