// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#include "stdafx.h"
#include "IAG0010Receive.h"


// Constructor
IAG0010Receive::IAG0010Receive(SOCKET sock, IAG0010Event *sendOK, IAG0010Event *exitTyped,  _TCHAR *file, IAG0010Event&	eTransferEnded):
	eventTransferEnded(eTransferEnded)
{
	clientSocket			=	sock;
	eventSendOK				=	sendOK;
	eventExitTyped			=	exitTyped;
	fileName				=	file;
	nReceivedBytes			=	0;
	receivedFlags			=	0;
	overlapped				=	NULL; 
	muLock					=	NULL;
	eventReceivingComplete	=	NULL;
}

// Destructor
IAG0010Receive::~IAG0010Receive()
{
	if(!overlapped) delete overlapped;
	if(!muLock) delete muLock;
	if(!eventReceivingComplete) delete eventReceivingComplete;
}


// Method		 ->	IAG0010Receive::WriteToFile
// Param		 ->	WSABUF dataBuf
// Returning	 ->	int
// Comments		 ->	Function for checking the data and write to file
//				 -> 0 - Data written to file, if data received, seding thread can proceed
//				 -> 1 - Exception errors, unable to write to file
int IAG0010Receive::WriteToFile(WSABUF dataBuf)
{
	int len;
	TCHAR data[2044];
	DWORD written;

	// Collect Data
	memcpy (&len, dataBuf.buf, 4);
	memcpy (&data, dataBuf.buf + 4, len);

	// Did we receive the Hello packet?
	if (!(wcscmp(data,_T("Hello IAG0010Client"))))
	{
		_tcout << "IAG0010Receive:   Handshake received   \"Hello IAG0010Client\""  << endl;
		eventSendOK->SetEvent();
		return 0;
	}
	else
	{
		//Data to file
		if (!WriteFile(hFile, data, len, &written, NULL))
		{
			_tcout << "IAG0010Receive: Unable to write into file "<< GetLastError() << endl;
			return 1;
			
		}
		else 
		{
			_tcout << "IAG0010Receive: " << std::setw(4) << written << " bytes written " << endl;
			eventSendOK->SetEvent(); //Set the sending event
			return 0;
		}
	}
}

// Method		 ->	IAG0010Receive::CreateFil
// Param		 ->	void
// Returning	 ->	int
// Comments		 ->	Function for creating file
//				 -> 0 - File created
//		         -> 1 - Unable to create and open file, if so, then sending/receiving must be shut down, playing here with exitEvent
int IAG0010Receive::CreateF(void)
{
	hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		eventExitTyped->SetEvent();
		_tcout << "IAG0010Receive: Creation of file not suceeded, exiting " << endl;
		return 1;
	}
	else 
	{
		_tcout << "IAG0010Receive: File created for storing received data: " << fileName << endl << endl;
		return 0;
	}
}

// Method		 ->	IAG0010Receive::Run
// Param		 ->	void
// Returning	 ->	int
// Comments		 ->	Receiving the data and writing to file (to put into thread)
//				 -> 0 - OK
//				 -> 1 - errors
int IAG0010Receive::Run(void)
{
	try
	{
		// Preparations
		dataBuf.buf = &buffer[0];
		dataBuf.len = 2048;
	
		overlapped = new WSAOVERLAPPED();																			
		memset(overlapped, 0, sizeof *overlapped);
		eventReceivingComplete = new IAG0010Event(TRUE,FALSE);														
		overlapped->hEvent = eventReceivingComplete->GetHandle();
	
		muLock = new IAG0010MultiLock(2);																			
		muLock->AppendEvent(*eventExitTyped);
		muLock->AppendEvent(*eventReceivingComplete);

		CreateF();

		while (TRUE)
		{	
			if (WSARecv(clientSocket, &dataBuf,	1, &nReceivedBytes, &receivedFlags, overlapped,	NULL) == SOCKET_ERROR)
			{  
				if ((err = WSAGetLastError()) != WSA_IO_PENDING)
				{  
					_tcout << "IAG0010Receive: WSARecv() failed, err " << err << endl;
					return 1;
				}

				// wait for data
				DWORD WaitResult = muLock->Lock(5000, FALSE); 
			
				switch (WaitResult) 
				{
					// Waiting stopped because eventExitTyped has become signaled, i.e. the user has decided to exit
					case WAIT_OBJECT_0:
						_tcout << "IAG0010Receive: Exit typed, exiting " << endl;
						return 0;

					case WAIT_OBJECT_0 + 1:
						// Waiting stopped because Overlapped.hEvent is now signaled, i.e. the receiving operation has ended. 
						// Now we have to see how many bytes we have got.
						eventReceivingComplete->ResetEvent(); 

						// to be ready for the next data package
						if (WSAGetOverlappedResult(clientSocket, overlapped, &nReceivedBytes, FALSE, &receivedFlags))
						{
							_tcout << "IAG0010Receive: " << std::setw(4) << nReceivedBytes << " bytes received " << endl;
							if (WriteToFile(dataBuf) != 0)
							{
								_tcout << "IAG0010Receive: Error in data processing and writing to file " << endl;
								return 1;
							}
							break;
						}
						else
						{
							_tcout << "IAG0010Receive: WSAGetOverlappedResult() failed, err " << GetLastError() << endl;
							return 1;
						}

					case WAIT_TIMEOUT:				
						eventTransferEnded.SetEvent();
						_tcout << "IAG0010Receive: Timeout, no more data " << endl;
						return 0;

					default: 
						_tcout << "IAG0010Receive: ReceivingThread MultiLock failed, err " << WSAGetLastError() << endl;
						return 1;
					}
			}
			else
			{  
				// Returned immediately without socket error
				if (!nReceivedBytes)
				{ 
					_tcout << "IAG0010Receive: Server has closed the connection " << endl;
					eventExitTyped->SetEvent();
					return 1;
				}
				else
				{
					_tcout << "IAG0010Receive: " << std::setw(4) << nReceivedBytes << " bytes received " << endl;
	
					if (WriteToFile(dataBuf) != 0)
					{
						_tcout << "IAG0010Receive: Error in data processing and writing to file " << endl;
						return 1;
					}
				}
			}
		}
	}
	catch (exception &ex)
	{
		_tcout << ex.what() << endl;
		return 1;
	}
}
