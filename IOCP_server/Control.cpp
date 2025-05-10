#include "Control.h"
#include "myGlobal.h" // header 에 집어넣으면 순환참조가 됨

bool Control::Init(int index, SOCKET sock, sockaddr_in ip)
{
	if (index < 0 || index >= MAX_THREAD_CNT)
		return false;

	Clear();

	mhSocket = sock;
	mIndex = index;
	mThreadNum = index;

	int t = mThreadNum;
	if (t < 0 || t > MAX_QUEUE)
		return false;

	return true;
}

void Control::Clear()
{
	if (mhSocket != NULL)
	{
		shutdown(mhSocket, SD_SEND);
		shutdown(mhSocket, SD_RECEIVE);
	}

	if (mhSocket)
	{
		closesocket(mhSocket);
		mhSocket = NULL;
	}

	mSendSize = 0;
	memset(mSendBuffer, 0x00, sizeof(mSendBuffer));

	mIndex = INVALID_VALUE;
	mThreadNum = INVALID_VALUE;
}
