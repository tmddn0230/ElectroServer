#pragma once

#include "framework.h"
#include "myDefine.h"
#include "UserManager.h"
#include "ControlManager.h"

// ���� ���� �ߺ� ���� ���� ����
// .h�� ���� �����ϵ��� �ϴϱ� ���ȭ, ���� �и�
// ���� & �����

extern HWND gMainWnd;
extern HWND gListboxWindow;
extern HFONT gArialFont;

extern int MAX_QUEUE;
extern int MAX_THREAD;

extern CRITICAL_SECTION	g_criticalsection;				//������ ����ȭ ��ü
extern std::list<SOCKET>	g_Client_list;	//����� Ŭ���̾�Ʈ ���� ����Ʈ.
extern SOCKET	g_UserSocket;			    // ���� ���� ���� ����
extern SOCKET	g_ControlSocket;			// ���� ���� ���� ����
extern HANDLE	g_Iocp;					//IOCP �ڵ�


extern UserManager    g_UserMgr;
extern ControlManager g_ControlMgr;

// �α� �ۼ���
void Log(const char* format, ...);
void DebugLog(char* filename, const char* format, ...);
