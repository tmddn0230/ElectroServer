#pragma once

#include "framework.h"
#include "myDefine.h"

class User
{

public:
	SOCKET mhSocket;

	int mThreadNum;
	int mIndex;

	int mRecvWrite;
	char buffer[MAX_RECV];  // 8kb , 받기만 할 뿐

	bool Init(int index, SOCKET sock, sockaddr_in ip);

	void Parse(char* packet, int recvSize);
	void HandleElectroPacket(const std::string& json);

	void Clear();
	void LogOut();
};



