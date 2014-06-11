// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#pragma once
#include "stdafx.h"


class IAG0010Send
{
public:
	IAG0010Send(SOCKET sock, IAG0010Event *sendOK, IAG0010Event *exitTyped, IAG0010Event&	eTransferEnded);
	~IAG0010Send();
	int Run(void);

private:
	IAG0010Event*		eventExitTyped;		// it's time to exit
	IAG0010Event*		eventSendOK;		// it's time to send
	SOCKET				clientSocket;
	WSABUF				dataBuf;  
	CHAR				buffer[2048];
	DWORD				nSentBytes;
	DWORD				sendFlags;
	WSAOVERLAPPED*		overlapped;
	IAG0010MultiLock*	mLock;
	TCHAR				data[2044];
	DWORD				len;
	int					i;
	DWORD				result;
	IAG0010Event&		eventTransferEnded;

};

