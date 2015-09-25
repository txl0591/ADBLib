#include "stdafx.h"
#include "ADBCmd.h"


void CharToTchar(char * _char, TCHAR * tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

int GetCurrentDir(char* p)
{
	CString sPath;
	GetCurrentDirectory(1000, sPath.GetBufferSetLength(1000 + 1));
	int strLen = sPath.GetLength();
	int i = 0;
	for (i = 0; i<strLen; i++)
	{
		*(p + i) = sPath.GetAt(i);
	}
	*(p + i) = '\0';
	sPath.ReleaseBuffer();

	return strLen;
}

char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

int ADBCmd::GetCurrentDirAdb()
{
	CString sPath;
	if (PathExe != NULL)
	{
		free(PathExe);
		PathExe = NULL;
	}

	GetCurrentDirectory(1000, sPath.GetBufferSetLength(1000 + 1));
	int strLen = sPath.GetLength();
	PathExe = (char*)malloc(strLen + 20);
	memset(PathExe, 0, (strLen + 20));
	int i = 0;
	for (i = 0; i<strLen; i++)
	{
		*(PathExe + i) = sPath.GetAt(i);
		if (*(PathExe + i) == _T('\0'))
		{
			break;
		}
	}
	*(PathExe + i) = '\\';
	i++;
	*(PathExe + i) = 'a';
	i++;
	*(PathExe + i) = 'd';
	i++;
	*(PathExe + i) = 'b';
	i++;
	*(PathExe + i) = '.';
	i++;
	*(PathExe + i) = 'e';
	i++;
	*(PathExe + i) = 'x';
	i++;
	*(PathExe + i) = 'e';
	i++;
	*(PathExe + i) = '\0';
	sPath.ReleaseBuffer();

	return strLen;
}


string ADBCmd::ExeCmd(char* cmd)
{
	if (mDebug)
	{
		cout << "ExeCmd [" << cmd << "]" << endl;
	}

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
		sprintf_s(Cmd, "%s pull %s %s \n", PathExe, src, dst);
	}
	else
	{
		sprintf_s(Cmd, "%s pull %s \n", PathExe, src);
	}
	

	string echo = ExeCmd(Cmd);

	if (mDebug){
		cout << "Cmd [" << Cmd << "]" << endl;
		cout << "Echo [" << echo << "]" << endl;
	}

	RunProc(ECHO_OK);

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
		const char* Bfilename = ptr.data();
		char * filename = U2G(Bfilename);
		memset(Cmd, 0, sizeof(Cmd));
		char filetype[4];

		filetype[0] = Bfilename[ptr.length() - 3];
		filetype[1] = Bfilename[ptr.length() - 2];
		filetype[2] = Bfilename[ptr.length() - 1];
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
				sprintf_s(Cmd, "%s pull %s/%s %s \n", PathExe, srcdir, filename, dstdir);
			}
			else
			{
				sprintf_s(Cmd, "%s pull %s/%s \n", PathExe, srcdir, filename);
			}
			system(Cmd);
		}
		delete filename;
	}
	RunProc(ECHO_OK);
	return true;
}

bool ADBCmd::ExeGetPathFileList(char* srcdir)
{
	char Cmd[1024];
	memset(Cmd, 0, sizeof(Cmd));
	sprintf_s(Cmd, "%s shell ls %s", PathExe, srcdir);
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
	RunProc(ECHO_OK);
	return true;
}

bool ADBCmd::ExeDelFile(char* pathfile)
{
	char Cmd[1024];
	memset(Cmd, 0, sizeof(Cmd));
	sprintf_s(Cmd, "%s rm %s \n", PathExe, pathfile);
	if (mDebug){
		cout << "Cmd [" << Cmd << "]" << endl;
	}

	system(Cmd);
	RunProc(ECHO_OK);
	return true;
}

bool ADBCmd::ExeClearDirFile(char* path, char *file)
{
	char Cmd[1024];
	bool Oper = false;
	ExeGetPathFileList(path);

	POSITION pos = mList.GetHeadPosition();
	while (pos != NULL)
	{
		string ptr = mList.GetNext(pos);
		const char* Bfilename = ptr.data();
		char * filename = U2G(Bfilename);
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
			sprintf_s(Cmd, "%s rm %s/%s\n", PathExe, path, filename);
			system(Cmd);
		}
		delete filename;
	}
	RunProc(ECHO_OK);
	return true;
}

ADBCmd::ADBCmd()
{
	GetCurrentDirAdb();
}

ADBCmd::~ADBCmd()
{
}
