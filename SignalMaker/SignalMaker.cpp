// SignalMaker.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <vector>
#include "Generator.h"
#include "Serializer.h"
#include "Defines.h"
#include "Packet.h"
// Connect Server
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

Generator g_Generator;
Serializer g_Serialier;
std::string deviceName;

SOCKET ConnectToServer(const char* ip, int port)
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket() 실패: " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);

    int result = connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "connect() 실패: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        return INVALID_SOCKET;
    }

    std::cout << "서버에 연결됨: " << ip << ":" << port << std::endl;
    return sock;
}



void InitINI()
{

}

void SendJsonPacket(SOCKET sock, const std::string& json)
{
    stData ack;
    ack.totalSize = ack.nSize + json.size();

    std::vector<char> buffer;
    buffer.resize(ack.totalSize);

    memcpy(buffer.data(), &ack , sizeof(stData));
    memcpy(buffer.data() + sizeof(stData), json.data(), json.size());

    send(sock, buffer.data(), buffer.size(), 0);
}

int main()
{
    // 장비 이름 설정
    InitINI();
    // 서버 연결
    SOCKET sock = ConnectToServer("127.0.0.1", 25000);
    if (sock == INVALID_SOCKET) {
        return 1;
    }


    int scenario = 0;
    auto startTime = std::chrono::steady_clock::now();

    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();




        if (elapsed >= 10)  // 10초 경과하면 다음 시나리오로
        {
            scenario = (scenario + 1) % MAX_SCENARIO;
            startTime = now;
            printf(">> 시나리오 변경됨: %d\n", scenario);
        }

        // Signal 생성
        Signal sig = g_Generator.GenerateSignal(deviceName, scenario);

        std::string packet = g_Serialier.SerializeToJson(sig);
        std::cout << packet << std::endl;
        // Server 에 Send
        SendJsonPacket(sock, packet);
        Sleep(1000);  // 1초 간격 송신
    }
}


