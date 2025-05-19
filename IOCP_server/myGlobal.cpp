#include "myGlobal.h"



//#pragma warning ( disable : 4244 ) // conversion from 'LRESULT' to 'int', possible loss of data
//#pragma warning ( disable : 4996 ) // This function or variable may be unsafe. Consider using vsprintf_s instead. 
								   // To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details. (vsprintf, sprintf, fopen)

HWND gMainWnd = NULL;
HWND gListboxWindow = NULL;
HFONT gArialFont = NULL;

int MAX_QUEUE = 0;
int MAX_THREAD = 0;

// extern 정의 
CRITICAL_SECTION g_criticalsection;
std::list<SOCKET> g_Client_list;
SOCKET	g_UserSocket;
SOCKET	g_ControlSocket;
HANDLE	g_Iocp;

UserManager    g_UserMgr;
ControlManager g_ControlMgr;

int logcount = 0;



std::string WcharToAnsi(std::wstring strWchar)
{
	std::string ret;
	if (strWchar.length() <= 0)
		return ret;

	int iLength = WideCharToMultiByte(CP_ACP, 0, strWchar.c_str(), -1, NULL, 0, NULL, FALSE);

	char* pChar = new char[iLength + 1];
	memset(pChar, 0, iLength + 1);

	WideCharToMultiByte(CP_ACP, 0, strWchar.c_str(), -1, pChar, iLength + 1, NULL, FALSE);
	ret = pChar;

	SAFE_DELETE_ARRAY(pChar);

	return ret;
}

std::string WcharToUtf8(std::wstring strWchar)
{
	std::string ret;
	if (strWchar.length() <= 0)
		return ret;

	int iLength = WideCharToMultiByte(CP_UTF8, 0, strWchar.c_str(), -1, NULL, 0, NULL, FALSE);

	char* pChar = new char[iLength + 1];
	memset(pChar, 0, iLength + 1);

	WideCharToMultiByte(CP_UTF8, 0, strWchar.c_str(), -1, pChar, iLength + 1, NULL, FALSE);
	ret = pChar;

	SAFE_DELETE_ARRAY(pChar);

	return ret;
}

void Log(const wchar_t* format, ...)
{

	va_list argptr;
	wchar_t msg[4096];
	memset(msg, 0x00, sizeof(msg));
	va_start(argptr, format);
	vswprintf_s(msg, format, argptr);
	va_end(argptr);

	int count = 0;
	count = SendMessageW(gListboxWindow, LB_GETCOUNT, 0, 0);
	if (count > 1000)
	{
		SendMessageW(gListboxWindow, LB_RESETCONTENT, 0, 0);
		logcount++;
	}

	SendMessageW(gListboxWindow, LB_INSERTSTRING, 0, (LPARAM)msg);
	char buffer[1024];
	memset(buffer, 0x00, sizeof(buffer));
	sprintf_s(buffer, "Log\\Log%d.txt", logcount);

	std::string ansiLog = WcharToAnsi(msg);
	DebugLog(buffer, "%s\n", ansiLog.c_str());
}

void DebugLog(char* filename, const char* format, ...)
{
	va_list argptr;
	char msg[10000] = {};
	va_start(argptr, format);
	vsprintf_s(msg, sizeof(msg), format, argptr);
	va_end(argptr);

	FILE* fp = nullptr;
	errno_t err = fopen_s(&fp, filename, "at");  

	if (err != 0 || fp == nullptr) {
		printf("파일 열기 실패!\n");
	}
	else {
		fprintf(fp, "%s\n", msg);  
		fclose(fp);
	}
}
