#pragma once
#include "Control.h"

// Control ���� ����� 
// Control�� ������ ���Ÿ� �ϰ� ��
// Send �� ���⼭ ����� �������� ���� Control�� ���� �� ����

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
	SendAll �� ���� 2�� �ۿ� ����Ͱ� ���� ����. ����� ����Ϳ� �����͸� ��� ���� ����
	*/
	void SendAll(char* buff, int size);
};

