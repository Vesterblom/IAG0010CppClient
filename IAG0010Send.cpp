// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#include "stdafx.h"
#include "IAG0010Send.h"


// Constructor
IAG0010Send::IAG0010Send(SOCKET sock, IAG0010Event *sendOK, IAG0010Event *exitTyped, IAG0010Event&	eTransferEnded):
	eventTransferEnded(eTransferEnded)
{
	clientSocket	=	sock;
	eventSendOK		=	sendOK;
	eventExitTyped	=	exitTyped;
	nSentBytes		=	0;
	sendFlags		=	0;
	overlapped		=	NULL; 
	mLock			=	NULL;
	i				=	0;
}

// Destructor
IAG0010Send::~IAG0010Send()
{
	if(!overlapped) delete overlapped;
	if(!mLock) delete mLock;
}


// Method		 ->	IAG0010Send::Run
// Param		 ->	void
// Returning	 ->	int
// Comments		 ->	Function for sending data (to put into thread)
//				 -> 0 - Keyboard has recognised EXIT command
//				 -> 1 - Exception errors
int IAG0010Send::Run(void)
{
	try
	{
		// Prep for sending
		dataBuf.buf	= &buffer[0];
		dataBuf.len	= 2048;
		overlapped	= new WSAOVERLAPPED();																					
		memset(overlapped, 0, sizeof *overlapped);

	
		mLock	=	 new IAG0010MultiLock(3);																			
		mLock->AppendEvent(*eventExitTyped);
		mLock->AppendEvent(*eventSendOK);
		mLock->AppendEvent(eventTransferEnded);


		while (TRUE)
		{
			DWORD WaitResult = mLock->Lock(INFINITE, FALSE);	
			
			switch (WaitResult)									
			{
				case WAIT_OBJECT_0:		
					_tcout << "IAG0010Send:    Exit typed, exiting " << endl;
					return 0;

				case WAIT_OBJECT_0 + 1:
					if (i == 0)
					{
						wcscpy_s(data, _T("Hello IAG0010Server"));
					}
					else
					{
						wcscpy_s(data, _T("Ready"));
					}

					//Length of the data
					len = (wcslen(data) + 1) * 2; 

					//Set up the data buffer
					memcpy(dataBuf.buf, &len, 4);
					memcpy(dataBuf.buf + 4, &data, len);

					//Number of bytes to send
					nSentBytes	= len + 4;
					dataBuf.len = nSentBytes;

					//Actual sending
					if (WSASend(clientSocket, &dataBuf,	1,	&nSentBytes, sendFlags,	overlapped, NULL) == SOCKET_ERROR)
					{
						if ((result = WSAGetLastError()) != WSA_IO_PENDING) 
						{
							_tcout << "IAG0010Send:    WSASend() failed, err " << result << endl;
							return 1;
						}

					}
					else
					{
						if (!nSentBytes) 
						{
							_tcout << "IAG0010Send:    Server has closed the connection " << endl;
							return 1;
						}
					}
					_tcout << "IAG0010Send:    " << std::setw(4) << nSentBytes << " bytes sent" <<" -----> \"" << data << "\""<< endl << endl;
					i = 1;

					//Reset the sending event to nonsignalled			
					eventSendOK->ResetEvent();
					break;

				case WAIT_OBJECT_0 + 2:
					_tcout << "IAG0010Send:    Receiving thread has timed out, no more data " << endl;
					return 1;

				// Fatal problems
				default: 
					_tcout << "IAG0010Send:    Thread multiLock failed, err " <<  WSAGetLastError() << endl;
					return 1;	
			}
		}
	}
	catch (exception &ex)
	{
		_tcout << "IAG0010Send:    " << ex.what() << endl;
		return 1;
	}
}
