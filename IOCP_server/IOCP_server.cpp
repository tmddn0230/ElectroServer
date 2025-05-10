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

    //���� �ʱ�ȭ
    WSADATA wsa = { 0 };
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        Log("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
        return 0;
    }

    //�Ӱ迵����ü�� �����Ѵ�.
    ::InitializeCriticalSection(&g_criticalsection);

    //Ctrl+C Ű�� ������ �� �̸� �����ϰ� ó���� �Լ��� ����Ѵ�.
    if (::SetConsoleCtrlHandler(
        (PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE)
        Log("ERROR: Ctrl+C ó���⸦ ����� �� �����ϴ�.");

    //IOCP ����
    g_Iocp = ::CreateIoCompletionPort(
        INVALID_HANDLE_VALUE,	//����� ���� ����.
        NULL,			//���� �ڵ� ����.
        0,				//�ĺ���(Key) �ش���� ����.
        0);				//������ ������ OS�� �ñ�.
    if (g_Iocp == NULL)
    {
        Log("ERORR: IOCP�� ������ �� �����ϴ�.");
        return 0;
    }

    //IOCP ������� ����
    HANDLE hThread;
    DWORD dwThreadID;
    for (int i = 0; i < MAX_THREAD_CNT; ++i)
    {
        dwThreadID = 0;
        //Ŭ���̾�Ʈ�κ��� ���ڿ��� ������.
        hThread = ::CreateThread(NULL,	//���ȼӼ� ���
            0,				//���� �޸𸮴� �⺻ũ��(1MB)
            IOCPThread,  	//������� ������ �Լ��̸�
            (LPVOID)NULL,	//
            0,				//���� �÷��״� �⺻�� ���
            &dwThreadID);	//������ ������ID�� ����� �����ּ�

        ::CloseHandle(hThread);
    }

    /*
    ***************************************************
    ********************* User ���� ********************
    ***************************************************
    */

    //���� ���� ���� ����
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
        Log("ERROR: ���� ���� ��Ʈ�� �̹� ������Դϴ�.");
        ReleaseServer();
        return 0;
    }

    if (::listen(g_UserSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        Log("ERROR: ���� ���� ���·� ��ȯ�� �� �����ϴ�.");
        ReleaseServer();
        return 0;
    }


    /*
    ***************************************************
    ****************** Control ���� ********************
    ***************************************************
    */
    //���� ���� ���� ����
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
        Log("ERROR: ��Ʈ�� ���� ��Ʈ�� �̹� ������Դϴ�.");
        ReleaseServer();
        return 0;
    }

    if (::listen(g_ControlSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        Log("ERROR:  ��Ʈ�� ���� ���� ���·� ��ȯ�� �� �����ϴ�.");
        ReleaseServer();
        return 0;
    }

    //�ݺ��ؼ� Ŭ���̾�Ʈ�� ������ accept() �Ѵ�.
    hThread = ::CreateThread(NULL, 0, ThreadAcceptLoop,
        (LPVOID)NULL, 0, &dwThreadID);
    ::CloseHandle(hThread);

    Log("*** IOCP ������ �����մϴ�! ***");

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
//Ctrl+C �̺�Ʈ�� �����ϰ� ���α׷��� �����Ѵ�.
BOOL CtrlHandler(DWORD dwType)
{
    if (dwType == CTRL_C_EVENT)
    {
        //ReleaseServer();

        puts("*** ä�ü����� �����մϴ�! ***");
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

    Log("[IOCP �۾��� ������ ����]");

    while (1)
    {            // GetQueuedCompletionStatus : �񵿱� ���� I/O�� �Ϸ�Ǹ� �� ��������� �������� �Լ���.
        bResult = ::GetQueuedCompletionStatus(
            g_Iocp,				    //Dequeue�� IOCP �ڵ�.
            &dwTransferredSize,		//������ ������ ũ��.
            (PULONG_PTR)&pSession,	//CreateIoCompletionPort�� ����� Completion Key ��, � ����(Ŭ���̾�Ʈ)�� ���� �۾����� �����ϴ� Ű
            &pWol,					//OVERLAPPED ����ü.
            INFINITE);				//�̺�Ʈ�� ������ ���.

        if (bResult == TRUE)
        {
            //�������� ���.

            /////////////////////////////////////////////////////////////
            //1. Ŭ���̾�Ʈ�� ������ ���������� �ݰ� ������ ���� ���.
            // control �� pSession �� �ɸ� ���� ������ ���� ó���ؾ��ϰ� ���⼭ ����Ǵ� Ŭ���̾�Ʈ�� user �� ������
            if (dwTransferredSize == 0)
            {
                g_UserMgr.Remove(pSession);
                delete pWol;
                Log("\tGQCS: Ŭ���̾�Ʈ�� ���������� ������ ������.");
            }

            /////////////////////////////////////////////////////////////
            //2. Ŭ���̾�Ʈ�� ���� �����͸� ������ ���.
            // Control ��ο��� Send �� ��
            else
            {
                // control ���� packet ����
                g_ControlMgr.SendAll(pSession->buffer, dwTransferredSize);
                memset(pSession->buffer, 0, sizeof(pSession->buffer));

                //�ٽ� IOCP�� ���.
                DWORD dwReceiveSize = 0;
                DWORD dwFlag = 0;
                WSABUF wsaBuf = { 0 };
                wsaBuf.buf = pSession->buffer;
                wsaBuf.len = sizeof(pSession->buffer);

                ::WSARecv(
                    pSession->mhSocket,	//Ŭ���̾�Ʈ ���� �ڵ�
                    &wsaBuf,			//WSABUF ����ü �迭�� �ּ�
                    1,					//�迭 ����� ����
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
            //���������� ���.

            /////////////////////////////////////////////////////////////
            //3. �Ϸ� ť���� �Ϸ� ��Ŷ�� ������ ���ϰ� ��ȯ�� ���.
            if (pWol == NULL)
            {
                //IOCP �ڵ��� ���� ���(������ �����ϴ� ���)�� �ش�ȴ�.
                Log("\tGQCS: IOCP �ڵ��� �������ϴ�.");
                break;
            }

            /////////////////////////////////////////////////////////////
            //4. Ŭ���̾�Ʈ�� ������������ ����ưų�
            //   ������ ���� ������ ������ ���.
            else
            {
                if (pSession != NULL)
                {
                    g_UserMgr.Remove(pSession);
                    delete pWol;
                }

                Log("\tGQCS: ���� ���� Ȥ�� �������� ���� ����");
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

        // 1. �� ���� ������ ���ÿ� ����
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(g_UserSocket, &readSet);
        FD_SET(g_ControlSocket, &readSet);

        timeval timeout = { 1, 0 };  // 1�� Ÿ�Ӿƿ�

        int result = select(0, &readSet, NULL, NULL, &timeout);
        if (result <= 0)
            continue; // �ƹ� �͵� ���ų� ���� �� �ٽ� ����

        // 2. USER Ŭ���̾�Ʈ ���� ó��
        if (FD_ISSET(g_UserSocket, &readSet))
        {
            SOCKET hClient = accept(g_UserSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (hClient == INVALID_SOCKET)
                continue;

            Log("[USER] �� Ŭ���̾�Ʈ ����");
            int useridx = 0;
            g_UserMgr.AddUser(hClient, clientAddr, useridx);
            User* pUser = g_UserMgr.GetUser(useridx);

            // IOCP ���
            CreateIoCompletionPort((HANDLE)hClient, g_Iocp, (ULONG_PTR)pUser, 0);

            // ���� ���
            WSABUF wsaBuf = { 0 };
            wsaBuf.buf = pUser->buffer;
            wsaBuf.len = sizeof(pUser->buffer);

            DWORD dwRecv = 0, dwFlag = 0;
            WSAOVERLAPPED* pWol = new WSAOVERLAPPED;
            ZeroMemory(pWol, sizeof(WSAOVERLAPPED));

            int ret = WSARecv(hClient, &wsaBuf, 1, &dwRecv, &dwFlag, pWol, NULL);
            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
            {
                Log("[USER] WSARecv ����");
                closesocket(hClient);
                g_UserMgr.Remove(pUser);  // ���� ���� + delete
                delete pWol;                // OVERLAPPED�� ����
                continue;
            }
        }
        // 3. CONTROL Ŭ���̾�Ʈ ���� ó��
        if (FD_ISSET(g_ControlSocket, &readSet))
        {
            SOCKET hClient = accept(g_ControlSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (hClient == INVALID_SOCKET)
                continue;

            Log("[CONTROL] ���� Ŭ���̾�Ʈ ����");

            int useridx = 0;
            g_ControlMgr.AddControl(hClient, clientAddr, useridx);
            Control* pControl = g_ControlMgr.GetControl(useridx);

            CreateIoCompletionPort((HANDLE)hClient, g_Iocp, (ULONG_PTR)pControl, 0);

            // Control�� ���� ���� �� WSARecv ��� �� ��
        }
    }

    return 0;
}

void ReleaseServer(void)
{
    //Ŭ���̾�Ʈ ������ ��� �����Ѵ�.
    //CloseAll();
    ::Sleep(500);

    //Listen ������ �ݴ´�.
    ::shutdown(g_UserSocket, SD_BOTH);
    ::closesocket(g_UserSocket);
    g_UserSocket = NULL;

    ::shutdown(g_ControlSocket, SD_BOTH);
    ::closesocket(g_ControlSocket);
    g_ControlSocket = NULL;

    //IOCP �ڵ��� �ݴ´�. �̷��� �ϸ� GQCS() �Լ��� FALSE�� ��ȯ�ϸ�
    //:GetLastError() �Լ��� ERROR_ABANDONED_WAIT_0�� ��ȯ�Ѵ�.
    //IOCP ��������� ��� ����ȴ�.
    ::CloseHandle(g_Iocp);
    g_Iocp = NULL;

    //IOCP ��������� ����Ǳ⸦ �����ð� ���� ��ٸ���.
    ::Sleep(500);
    ::DeleteCriticalSection(&g_criticalsection);
}
