// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#pragma once
#include "stdafx.h"


class IAG0010Socket : public IAG0010SocketBase
{
public:
	IAG0010Socket(IAG0010Event *p1, _TCHAR *p2);
	~IAG0010Socket();
	void OpenConnection() throw (exception);
	void Start(IAG0010Event &e);
	BOOL ConnectionError;

private:
	IAG0010Event*	eventExitTyped;
	_TCHAR*			outputFileName;
	WSADATA			WsaData;			// using in socket init for opening cnctn chk
	DWORD			Error;				// using in socket init for opening cnctn chk
	SOCKET			hClientSocket;		// using in socket init for opening cnctn chk
	SOCKADDR_IN		clientSocket;		// socket connection
	IAG0010Event*	eventSendOK;	    // event send data;
	//IAG0010Event*   eventTransferEnded;
};