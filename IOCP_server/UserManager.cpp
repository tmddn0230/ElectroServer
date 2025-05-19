#include "UserManager.h"
#include "myGlobal.h"
#include "myDefine.h"


UserManager::UserManager(void)
{

}

UserManager::~UserManager(void)
{
}

User* UserManager::GetUser(int uid)
{
	if (uid < 0)
		return NULL;

	int i;
	for (i = 0; i < MAX_THREAD_CNT; ++i)
	{
		if (mUser[i].mIndex == uid)
			return &mUser[i];
	}
	return NULL;
}

bool UserManager::AddUser(SOCKET sock, sockaddr_in ip, int& index)
{
	int i;
	for (i = 0; i < MAX_THREAD_CNT; ++i)
	{
		if (mUser[i].mhSocket != NULL)
			continue;

		BOOL opt_val = TRUE;
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));

		struct linger Linger;
		Linger.l_onoff = 1; //링거 끄기, Time Wait
		Linger.l_linger = 0; // 안기다리고 바로 남은 데이터 버려버림
		setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&Linger, sizeof(Linger));


		mUser[i].Init(i, sock, ip);

		Log(L"AddUser: %d %d.%d.%d.%d", i,
			ip.sin_addr.S_un.S_un_b.s_b1,
			ip.sin_addr.S_un.S_un_b.s_b2,
			ip.sin_addr.S_un.S_un_b.s_b3,
			ip.sin_addr.S_un.S_un_b.s_b4);

		index = i;

		return true;

	}
	return false;
}

void UserManager::Remove(User* user)
{
    // LogOut User
    user->LogOut();
    delete user;
}
