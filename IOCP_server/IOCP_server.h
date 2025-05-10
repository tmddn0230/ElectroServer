#pragma once

#include "resource.h"

BOOL CtrlHandler(DWORD dwType);

DWORD WINAPI IOCPThread(LPVOID pParam);
DWORD WINAPI ThreadAcceptLoop(LPVOID pParam);

void ReleaseServer(void);
