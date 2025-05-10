#pragma once
#include "framework.h"
#include "myDefine.h"

class Control
{
public:
	SOCKET mhSocket;

	int mThreadNum;
	int mIndex;

	int mSendSize;
	char mSendBuffer[MAX_SEND];

	bool Init(int index, SOCKET sock, sockaddr_in ip);

	void Clear();

};

