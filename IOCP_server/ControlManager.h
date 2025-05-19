#pragma once
#include "Control.h"

// Control 관제 모니터 
// Control은 데이터 수신만 하게 됨
// Send 를 여기서 해줘야 엔진으로 만든 Control이 받을 수 있음

class ControlManager
{
public:

	Control mControl[MAX_ENGINE_CNT];



	// Function
	ControlManager(void);
	~ControlManager(void);


	Control* GetControl(int uid);

	bool AddControl(SOCKET sock, sockaddr_in ip, int& index);



	/*
	SendAll 만 구성 2개 밖에 모니터가 되지 않음. 연결된 모니터엔 데이터를 모두 보낼 예정
	*/
	void SendAll(char* buff, int size);
};

