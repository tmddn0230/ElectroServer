#pragma once

#include "framework.h"
#include "myDefine.h"
#include "UserManager.h"
#include "ControlManager.h"

// 전역 변수 중복 정의 오류 방지
// .h만 통해 참조하도록 하니까 모듈화, 구조 분리
// 정리 & 디버깅

extern HWND gMainWnd;
extern HWND gListboxWindow;
extern HFONT gArialFont;

extern int MAX_QUEUE;
extern int MAX_THREAD;

extern CRITICAL_SECTION	g_criticalsection;				//스레드 동기화 객체
extern std::list<SOCKET>	g_Client_list;	//연결된 클라이언트 소켓 리스트.
extern SOCKET	g_UserSocket;			    // 유저 전용 리슨 소켓
extern SOCKET	g_ControlSocket;			// 관제 전용 리슨 소켓
extern HANDLE	g_Iocp;					//IOCP 핸들


extern UserManager    g_UserMgr;
extern ControlManager g_ControlMgr;

// 로그 작성용
void Log(const char* format, ...);
void DebugLog(char* filename, const char* format, ...);
