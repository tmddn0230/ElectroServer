#pragma once
#include "User.h"

// User ������ 
// user �� ������ �۽Ÿ� �ϰ� ������ ���� �ʴ´�. 
// ��, count �뵵 �����θ� ����

class UserManager
{
public:

	User mUser[MAX_THREAD_CNT];



	// Function
	UserManager(void);
	~UserManager(void);

	User* GetUser(int uid);

	bool AddUser(SOCKET sock, sockaddr_in ip, int& index);
	void Remove(User* user);

};

