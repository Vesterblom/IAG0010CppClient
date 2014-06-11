// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson
// Idea and logic was taken from lecturers example "SocketExample"

// IAG0010CppClient_Udod.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

#include "IAG0010Keyboard.h"
#include "IAG0010Socket.h"
#include "IAG0010CppClient_Udod.h"


int _tmain(int argc, _TCHAR* argv[])
{
	IAG0010Event* eventExitTyped			= NULL;
	IAG0010Event eventTransferEnded(TRUE,FALSE);
	IAG0010Keyboard* objIAG0010Keyboard		= NULL;
	IAG0010Socket* sock						= NULL;

	if (argc != 2)
	{
		_tcout << "Main: Arguments not correct. Use [" << argv[0] << "] [*.file]" << endl;
		return 1;
	}

	try
	{
		// Initialization
		if (!(eventExitTyped = new IAG0010Event(TRUE, FALSE)))													
		{
			_tcout << "Main: Creating events failed, err " << WSAGetLastError() << endl;
			throw exception();
		}

		//Create and start the keyboard thread
		objIAG0010Keyboard = new IAG0010Keyboard(eventExitTyped);								
		thread keyboardThread(&IAG0010Keyboard::Run, objIAG0010Keyboard);
	

		//Start sockets
		sock = new IAG0010Socket(eventExitTyped, argv[1]);										
		
		sock -> OpenConnection();

		if (!sock -> ConnectionError)
		{
			sock -> Start(eventTransferEnded);
		}

		else
		{
			_tcout << "Main:          Not possible to establish connection. Type exit..." << endl;
			eventExitTyped->SetEvent();
		}


		keyboardThread.join();

		delete objIAG0010Keyboard;
		delete sock;
		delete eventExitTyped;

		return 0;

	}
	catch (exception &ex) 
	{
		// Fatal error
		if (!objIAG0010Keyboard) delete objIAG0010Keyboard;
		if (!sock)				 delete sock;
		if (!eventExitTyped)	 delete eventExitTyped;

		_tcout << ex.what() << endl;
		return 1;
	}
}

