#pragma once
#include "User.h"

// User 관리자 
// user 는 데이터 송신만 하고 수신은 하지 않는다. 
// 즉, count 용도 정도로만 쓰임

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

