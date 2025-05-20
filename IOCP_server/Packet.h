#pragma once
#pragma pack(1)

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

#define HEADSIZE sizeof( stHeader )
