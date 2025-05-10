#include "ControlManager.h"
#include "myGlobal.h"
#include "myDefine.h"


ControlManager::ControlManager(void)
{
}

ControlManager::~ControlManager(void)
{
}

Control* ControlManager::GetControl(int uid)
{
	if (uid < 0)
		return NULL;

	int i;
	for (i = 0; i < MAX_ENGINE_CNT; ++i)
	{
		if (mControl[i].mIndex == uid)
			return &mControl[i];
	}
	return NULL;
}

bool ControlManager::AddControl(SOCKET sock, sockaddr_in ip, int& index)
{
	int i;
	for (i = 0; i < MAX_ENGINE_CNT; ++i)
	{
		if (mControl[i].mhSocket != NULL)
			continue;

		BOOL opt_val = TRUE;
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));

		struct linger Linger;
		Linger.l_onoff = 1; //링거 끄기, Time Wait
		Linger.l_linger = 0; // 안기다리고 바로 남은 데이터 버려버림
		setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&Linger, sizeof(Linger));


		mControl[i].Init(i, sock, ip);

		Log("AddControl: %d %d.%d.%d.%d", i,
			ip.sin_addr.S_un.S_un_b.s_b1,
			ip.sin_addr.S_un.S_un_b.s_b2,
			ip.sin_addr.S_un.S_un_b.s_b3,
			ip.sin_addr.S_un.S_un_b.s_b4);

		index = i;

		return true;

	}
	return false;
}


void ControlManager::SendAll(char* buff, int size)
{
}
