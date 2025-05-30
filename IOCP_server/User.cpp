#include "User.h"
#include "myGlobal.h" // header 에 집어넣으면 순환참조가 됨
#include "Packet.h"

bool User::Init(int index, SOCKET sock, sockaddr_in ip)
{
	if (index < 0 || index >= MAX_THREAD_CNT)
		return false;

	Clear();

	mhSocket = sock;
	mIndex = index;
	mThreadNum = index;

	int t = mThreadNum;
	if (t < 0 || t > MAX_QUEUE)
		return false;

	return true;
}

void User::Parse(char* packet, int recvSize)
{

    // 최소한 헤더는 있어야 파싱 가능
    if (recvSize < sizeof(stHeader))
    {
        Log(L"[User] ERROR: Too small send data size, not have header");
        return;
    }

    stHeader* pHeader = (stHeader*)buffer;

    // 무결성 확인
    if (pHeader->nCheckSum != (pHeader->nSize + pHeader->nType))
    {
        Log(L"[User] ERROR: Wrong CheckSum");
        return;
    }

    // 전체 수신 길이 검증
    if (recvSize < pHeader->nSize)
    {
        Log(L"[User] ERROR: Shortage packet size");
        return;
    }

    // 타입 분기
    switch (pHeader->nProtocol)
    {
    case prConnectAck:
        break;


    case prData:
    {
        int jsonLen = pHeader->nSize - sizeof(stHeader);
        if (jsonLen <= 0) return;

        std::string json(buffer + sizeof(stHeader), jsonLen);
        HandleElectroPacket(json);
        break;
    }

    default:
        Log(L"[서버] 알 수 없는 패킷 타입: %d", pHeader->nProtocol);
        break;
    }
}

void User::HandleElectroPacket(const std::string& json)
{
    Log(L"[서버] 전기 패킷 수신: %S", json.c_str());

    // 파싱 로직은 JSON 파서에 따라 선택:
    // - 직접 split
    // - nlohmann/json
    // - RapidJSON
}

void User::Clear()
{
	if (mhSocket != NULL)
	{
		shutdown(mhSocket, SD_SEND);
		shutdown(mhSocket, SD_RECEIVE);
	}

	if (mhSocket)
	{
		closesocket(mhSocket);
		mhSocket = NULL;
	}

	mRecvWrite = 0;
	memset(buffer, 0x00, sizeof(buffer));

	mIndex = INVALID_VALUE;
	mThreadNum = INVALID_VALUE;
}

void User::LogOut()
{
	if (mhSocket) {
		int t = mThreadNum;
		if (t < 0 || t > MAX_QUEUE)
			return;
	}
	Clear();
}
