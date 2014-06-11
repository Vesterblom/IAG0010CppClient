// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#pragma once
#include "stdafx.h"

class IAG0010Receive
{
public:
	IAG0010Receive(SOCKET sock, IAG0010Event *sendOK, IAG0010Event *exitTyped,  _TCHAR *file, IAG0010Event&	eTransferEnded);
	~IAG0010Receive();
	int Run(void);
	int WriteToFile(WSABUF dataBuf);
	int CreateF(void);

private:
	SOCKET				clientSocket;
	IAG0010Event*		eventSendOK;	
	IAG0010Event*		eventExitTyped;
	DWORD				nReceivedBytes;
	DWORD				receivedFlags;
	DWORD				err;
	WSAOVERLAPPED*		overlapped;
	IAG0010MultiLock*	muLock;
	WSABUF				dataBuf;
	HANDLE				hStdIn;
	HANDLE				hFile;
	_TCHAR*				fileName;
	CHAR				buffer[2048];
	IAG0010Event*		eventReceivingComplete;
	IAG0010Event&		eventTransferEnded;
};
