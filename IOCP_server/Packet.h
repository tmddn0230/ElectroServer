#pragma once
#pragma pack(push, 1)

#include "myDefine.h"
#include "protocol.h"
#include <vector> 

struct stHeader
{
	// unsigned short  = uint16
	unsigned short  nProtocol;
	unsigned short  nSize;
	unsigned short  nType;
	unsigned short  nCheckSum;

	stHeader()
	{
		nProtocol = nSize = nType = nCheckSum = 0;
	};
	void SetHeader(int protocol, int len)
	{
		nProtocol = nProtocol;
		nSize = len;
		nType = ELECTRO;
		nCheckSum = nType + nSize + nProtocol;
	};
};

struct stConnectAck : public stHeader
{
	// Connect
	unsigned short Index;
	stConnectAck()
	{
		Index = 0;

		SetHeader(prConnectAck, sizeof(stConnectAck));
	};
};

struct stData : public stHeader
{
	// Data Size = header + payload
	unsigned short totalSize;
	stData()
	{
		totalSize = 0;
		SetHeader(prData, sizeof(stData));
	}
};


#pragma pack(pop)  // ← 여기서 원래대로 정렬 복구

#define HEADSIZE sizeof( stHeader )
