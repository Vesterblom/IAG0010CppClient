// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#include "stdafx.h"
#include "IAG0010Socket.h"
#include "IAG0010Receive.h"
#include "IAG0010Send.h"

// Link with ws2_32.lib 
// I had some linking problems woth WSAGetLastErr... this way linking works, but not sure about those WSA methdods.
// Ensure that the build environment links to the Winsock Library file Ws2_32.lib.
// Applications that use Winsock must be linked with the Ws2_32.lib library file. 
// The #pragma comment indicates to the linker that the Ws2_32.lib file is needed.
#pragma comment(lib,"WS2_32")

// Constructor
IAG0010Socket::IAG0010Socket(IAG0010Event *p1, _TCHAR *p2) 
	: IAG0010SocketBase(p1, p2) 
{
	eventExitTyped	= p1;
	outputFileName	= p2;
	hClientSocket	= INVALID_SOCKET;
	ConnectionError = FALSE;
	eventSendOK		= NULL;
}

// Destructor
IAG0010Socket::~IAG0010Socket()
{
	if(!eventSendOK) delete eventSendOK;
}


// Method		 ->	IAG0010Keyboard::OpenConnection
// Param		 ->	void
// Returning	 ->	void
// Comments		 ->	Method tries to create connection, 
//				    flag ConnectionError is set, if errors occur
void IAG0010Socket::OpenConnection()
{
	try
	{
		eventSendOK = new IAG0010Event(TRUE, FALSE);														

		// Initializations for socket
		// Initialize Windows socket support
		if (Error = WSAStartup(MAKEWORD(2, 0), &WsaData)) 
		{
			throw exception ("IAG0010Socket: WSAStartup() failed, err ", WSAGetLastError());
		}
		else if ((hClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			throw exception ("IAG0010Socket: socket() failed, err ", WSAGetLastError());
		}

		// Connection to socket
		clientSocket.sin_family		 = AF_INET;
		clientSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
		clientSocket.sin_port		 = htons(1234);

		if (connect(hClientSocket, (SOCKADDR*) &clientSocket, sizeof(clientSocket)) == SOCKET_ERROR)
		{ 
			throw exception ("IAG0010Socket: Unable to connect to server, err ", WSAGetLastError());
		}
	}
	catch (exception &ex)
	{
		ConnectionError = TRUE;
		cout << ex.what() << endl;
	}
}



// Method		 ->	IAG0010Socket::Start
// Param		 ->	IAG0010Event &e
// Returning	 ->	void
// Comments		 ->	Method starts the data transfer 
//					e is the reference to the event that becomes signalled when the transfer
//					has ended, data receiving and sending threads stopped and the connection
//					closed. When e is signalled, the main thread can exit
void IAG0010Socket::Start(IAG0010Event &eventTransferEnded)
{
	IAG0010Receive * objIAG0010Receive = NULL;
	IAG0010Send* objIAG0010Send = NULL;
	
	try
	{
		// Receiving thread creation and launching
		objIAG0010Receive = new IAG0010Receive(hClientSocket, eventSendOK, eventExitTyped,  outputFileName,eventTransferEnded);
		thread receivingThread(&IAG0010Receive::Run, objIAG0010Receive);
		

		// Sending thread creation and launching
		objIAG0010Send = new IAG0010Send(hClientSocket, eventSendOK, eventExitTyped,eventTransferEnded);
		thread sendingThread(&IAG0010Send::Run, objIAG0010Send);	
		
		receivingThread.join();
		sendingThread.join();
		
		_tcout << "IAG0010Socket:  Transfer thrads joined " << endl;

		delete objIAG0010Receive;
		delete objIAG0010Send;

	}
	catch (exception &ex)
	{
		ConnectionError = TRUE;

		if (!objIAG0010Receive) delete objIAG0010Receive;
		if (!objIAG0010Send)	delete objIAG0010Send;

		cout << ex.what() << endl;

		return;
	}
}
