#include "User.h"
#include "myGlobal.h" // header �� ��������� ��ȯ������ ��
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

    // �ּ��� ����� �־�� �Ľ� ����
    if (recvSize < sizeof(stHeader))
    {
        Log(L"[User] ERROR: Too small send data size, not have header");
        return;
    }

    stHeader* pHeader = (stHeader*)buffer;

    // ���Ἲ Ȯ��
    if (pHeader->nCheckSum != (pHeader->nSize + pHeader->nType))
    {
        Log(L"[User] ERROR: Wrong CheckSum");
        return;
    }

    // ��ü ���� ���� ����
    if (recvSize < pHeader->nSize)
    {
        Log(L"[User] ERROR: Shortage packet size");
        return;
    }

    // Ÿ�� �б�
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
        Log(L"[����] �� �� ���� ��Ŷ Ÿ��: %d", pHeader->nProtocol);
        break;
    }
}

void User::HandleElectroPacket(const std::string& json)
{
    Log(L"[����] ���� ��Ŷ ����: %S", json.c_str());

    // �Ľ� ������ JSON �ļ��� ���� ����:
    // - ���� split
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
