#pragma once
#pragma pack(push, 1)

#include "myDefine.h"

struct stHeader
{
	// unsigned short  = uint16
	unsigned short  nSize;
	unsigned short  nType;
	unsigned short  nCheckSum;

	stHeader()
	{
		nSize = nType = nCheckSum = 0;
	};
	void SetHeader(int len)
	{
		nSize = len;
		nType = ELECTRO;
		nCheckSum = nType + nSize;
	};
};

#pragma pack(pop)  // ← 여기서 원래대로 정렬 복구

#define HEADSIZE sizeof( stHeader )
