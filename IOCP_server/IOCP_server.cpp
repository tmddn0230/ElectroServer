// IOCP_server.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "IOCP_server.h"
// additinal header
#include "myGlobal.h"
#include "myDefine.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    //윈속 초기화
    WSADATA wsa = { 0 };
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        Log("ERROR: 윈속을 초기화 할 수 없습니다.");
        return 0;
    }

    //임계영역객체를 생성한다.
    ::InitializeCriticalSection(&g_criticalsection);

    //Ctrl+C 키를 눌렀을 때 이를 감지하고 처리할 함수를 등록한다.
    if (::SetConsoleCtrlHandler(
        (PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE)
        Log("ERROR: Ctrl+C 처리기를 등록할 수 없습니다.");

    //IOCP 생성
    g_Iocp = ::CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,	//연결된 파일 없음.
        NULL,			//기존 핸들 없음.
        0,				//식별자(Key) 해당되지 않음.
        0);				//스레드 개수는 OS에 맡김.
    if (g_Iocp == NULL)
    {
        Log("ERORR: IOCP를 생성할 수 없습니다.");
        return 0;
    }

    //IOCP 스레드들 생성
    HANDLE hThread;
    DWORD dwThreadID;
    for (int i = 0; i < MAX_THREAD_CNT; ++i)
    {
        dwThreadID = 0;
        //클라이언트로부터 문자열을 수신함.
        hThread = ::CreateThread(NULL,	//보안속성 상속
            0,				//스택 메모리는 기본크기(1MB)
            IOCPThread,  	//스래드로 실행할 함수이름
            (LPVOID)NULL,	//
            0,				//생성 플래그는 기본값 사용
            &dwThreadID);	//생성된 스레드ID가 저장될 변수주소

        ::CloseHandle(hThread);
    }

    /*
    ***************************************************
    ********************* User 전용 ********************
    ***************************************************
    */

    //서버 리슨 소켓 생성
    g_UserSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
        NULL, 0, WSA_FLAG_OVERLAPPED);

    //bind()/listen()
    SOCKADDR_IN addrsvr;
    addrsvr.sin_family = AF_INET;
    addrsvr.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
    addrsvr.sin_port = ::htons(25000);

    if (::bind(g_UserSocket,
        (SOCKADDR*)&addrsvr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        Log("ERROR: 유저 전용 포트가 이미 사용중입니다.");
        ReleaseServer();
        return 0;
    }

    if (::listen(g_UserSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        Log("ERROR: 유저 리슨 상태로 전환할 수 없습니다.");
        ReleaseServer();
        return 0;
    }


    /*
    ***************************************************
    ****************** Control 전용 ********************
    ***************************************************
    */
    //서버 리슨 소켓 생성
    g_ControlSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
        NULL, 0, WSA_FLAG_OVERLAPPED);

    //bind()/listen()
    SOCKADDR_IN addrsvr_c;
    addrsvr_c.sin_family = AF_INET;
    addrsvr_c.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
    addrsvr_c.sin_port = ::htons(25001);

    if (::bind(g_ControlSocket,
        (SOCKADDR*)&addrsvr_c, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        Log("ERROR: 컨트롤 전용 포트가 이미 사용중입니다.");
        ReleaseServer();
        return 0;
    }

    if (::listen(g_ControlSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        Log("ERROR:  컨트롤 전용 리슨 상태로 전환할 수 없습니다.");
        ReleaseServer();
        return 0;
    }

    //반복해서 클라이언트의 연결을 accept() 한다.
    hThread = ::CreateThread(NULL, 0, ThreadAcceptLoop,
        (LPVOID)NULL, 0, &dwThreadID);
    ::CloseHandle(hThread);

    Log("*** IOCP 서버를 시작합니다! ***");

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_IOCPSERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IOCPSERVER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IOCPSERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IOCPSERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


/////////////////////////////////////////////////////////////////////////
//Ctrl+C 이벤트를 감지하고 프로그램을 종료한다.
BOOL CtrlHandler(DWORD dwType)
{
    if (dwType == CTRL_C_EVENT)
    {
        //ReleaseServer();

        puts("*** 채팅서버를 종료합니다! ***");
        ::WSACleanup();
        exit(0);
        return TRUE;
    }

    return FALSE;
}

DWORD __stdcall IOCPThread(LPVOID pParam)
{
    DWORD			dwTransferredSize = 0;
    DWORD			dwFlag = 0; 
    User            * pSession = NULL;
    LPWSAOVERLAPPED	pWol = NULL;
    BOOL			bResult;

    Log("[IOCP 작업자 스레드 시작]");

    while (1)
    {            // GetQueuedCompletionStatus : 비동기 소켓 I/O가 완료되면 그 “결과”를 가져오는 함수다.
        bResult = ::GetQueuedCompletionStatus(
            g_Iocp,				    //Dequeue할 IOCP 핸들.
            &dwTransferredSize,		//수신한 데이터 크기.
            (PULONG_PTR)&pSession,	//CreateIoCompletionPort에 등록한 Completion Key 값, 어떤 세션(클라이언트)에 대한 작업인지 구분하는 키
            &pWol,					//OVERLAPPED 구조체.
            INFINITE);				//이벤트를 무한정 대기.

        if (bResult == TRUE)
        {
            //정상적인 경우.

            /////////////////////////////////////////////////////////////
            //1. 클라이언트가 소켓을 정상적으로 닫고 연결을 끊은 경우.
            // control 은 pSession 에 걸릴 일이 없으니 따로 처리해야하고 여기서 종료되는 클라이언트는 user 로 한정됨
            if (dwTransferredSize == 0)
            {
                g_UserMgr.Remove(pSession);
                delete pWol;
                Log("\tGQCS: 클라이언트가 정상적으로 연결을 종료함.");
            }

            /////////////////////////////////////////////////////////////
            //2. 클라이언트가 보낸 데이터를 수신한 경우.
            // Control 모두에게 Send 할 것
            else
            {
                // control 에게 packet 전달
                g_ControlMgr.SendAll(pSession->buffer, dwTransferredSize);
                memset(pSession->buffer, 0, sizeof(pSession->buffer));

                //다시 IOCP에 등록.
                DWORD dwReceiveSize = 0;
                DWORD dwFlag = 0;
                WSABUF wsaBuf = { 0 };
                wsaBuf.buf = pSession->buffer;
                wsaBuf.len = sizeof(pSession->buffer);

                ::WSARecv(
                    pSession->mhSocket,	//클라이언트 소켓 핸들
                    &wsaBuf,			//WSABUF 구조체 배열의 주소
                    1,					//배열 요소의 개수
                    &dwReceiveSize,
                    &dwFlag,
                    pWol,
                    NULL);
                if (::WSAGetLastError() != WSA_IO_PENDING)
                    Log("\tGQCS: ERROR: WSARecv()");
            }
        }
        else
        {
            //비정상적인 경우.

            /////////////////////////////////////////////////////////////
            //3. 완료 큐에서 완료 패킷을 꺼내지 못하고 반환한 경우.
            if (pWol == NULL)
            {
                //IOCP 핸들이 닫힌 경우(서버를 종료하는 경우)도 해당된다.
                Log("\tGQCS: IOCP 핸들이 닫혔습니다.");
                break;
            }

            /////////////////////////////////////////////////////////////
            //4. 클라이언트가 비정상적으로 종료됐거나
            //   서버가 먼저 연결을 종료한 경우.
            else
            {
                if (pSession != NULL)
                {
                    g_UserMgr.Remove(pSession);
                    delete pWol;
                }

                Log("\tGQCS: 서버 종료 혹은 비정상적 연결 종료");
            }
        }
    }

    return 0;
}

DWORD __stdcall ThreadAcceptLoop(LPVOID pParam)
{
    while (true)
    {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);

        // 1. 두 리슨 소켓을 동시에 감시
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(g_UserSocket, &readSet);
        FD_SET(g_ControlSocket, &readSet);

        timeval timeout = { 1, 0 };  // 1초 타임아웃

        int result = select(0, &readSet, NULL, NULL, &timeout);
        if (result <= 0)
            continue; // 아무 것도 없거나 에러 → 다시 루프

        // 2. USER 클라이언트 접속 처리
        if (FD_ISSET(g_UserSocket, &readSet))
        {
            SOCKET hClient = accept(g_UserSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (hClient == INVALID_SOCKET)
                continue;

            Log("[USER] 새 클라이언트 접속");
            int useridx = 0;
            g_UserMgr.AddUser(hClient, clientAddr, useridx);
            User* pUser = g_UserMgr.GetUser(useridx);

            // IOCP 등록
            CreateIoCompletionPort((HANDLE)hClient, g_Iocp, (ULONG_PTR)pUser, 0);

            // 수신 등록
            WSABUF wsaBuf = { 0 };
            wsaBuf.buf = pUser->buffer;
            wsaBuf.len = sizeof(pUser->buffer);

            DWORD dwRecv = 0, dwFlag = 0;
            WSAOVERLAPPED* pWol = new WSAOVERLAPPED;
            ZeroMemory(pWol, sizeof(WSAOVERLAPPED));

            int ret = WSARecv(hClient, &wsaBuf, 1, &dwRecv, &dwFlag, pWol, NULL);
            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
            {
                Log("[USER] WSARecv 실패");
                closesocket(hClient);
                g_UserMgr.Remove(pUser);  // 세션 제거 + delete
                delete pWol;                // OVERLAPPED도 해제
                continue;
            }
        }
        // 3. CONTROL 클라이언트 접속 처리
        if (FD_ISSET(g_ControlSocket, &readSet))
        {
            SOCKET hClient = accept(g_ControlSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (hClient == INVALID_SOCKET)
                continue;

            Log("[CONTROL] 관제 클라이언트 접속");

            int useridx = 0;
            g_ControlMgr.AddControl(hClient, clientAddr, useridx);
            Control* pControl = g_ControlMgr.GetControl(useridx);

            CreateIoCompletionPort((HANDLE)hClient, g_Iocp, (ULONG_PTR)pControl, 0);

            // Control은 수신 없음 → WSARecv 등록 안 함
        }
    }

    return 0;
}

void ReleaseServer(void)
{
    //클라이언트 연결을 모두 종료한다.
    //CloseAll();
    ::Sleep(500);

    //Listen 소켓을 닫는다.
    ::shutdown(g_UserSocket, SD_BOTH);
    ::closesocket(g_UserSocket);
    g_UserSocket = NULL;

    ::shutdown(g_ControlSocket, SD_BOTH);
    ::closesocket(g_ControlSocket);
    g_ControlSocket = NULL;

    //IOCP 핸들을 닫는다. 이렇게 하면 GQCS() 함수가 FALSE를 반환하며
    //:GetLastError() 함수가 ERROR_ABANDONED_WAIT_0을 반환한다.
    //IOCP 스레드들이 모두 종료된다.
    ::CloseHandle(g_Iocp);
    g_Iocp = NULL;

    //IOCP 스레드들이 종료되기를 일정시간 동안 기다린다.
    ::Sleep(500);
    ::DeleteCriticalSection(&g_criticalsection);
}
