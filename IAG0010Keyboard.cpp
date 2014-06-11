// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#include "stdafx.h"
#include "IAG0010Keyboard.h"

// Constructor
IAG0010Keyboard::IAG0010Keyboard(IAG0010Event *exitTyped)
{
	eventExitTyped = exitTyped;
}

// Destructor
IAG0010Keyboard::~IAG0010Keyboard() {}


// Method		 ->	IAG0010Keyboard::Run
// Param		 ->	void
// Returning	 ->	int
// Comments		 ->		
//				 -> 0 - Keyboard has recognised EXIT command, eventExitTyped becomes signalled
//				 -> 1 - Exception of function
int IAG0010Keyboard::Run(void)
{
	// Prepare the keyboard	
	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdIn == INVALID_HANDLE_VALUE)
	{
		throw exception("IAG0010Keyboard: GetStdHandle() failed, err", GetLastError());
	}

	if (!SetConsoleMode(hStdIn, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT))
	{
		throw exception("IAG0010Keyboard:  SetConsoleMode() failed, err", GetLastError());
	}

	while (TRUE)
	{
		try
		{
			// Readconsole -> If the function succeeds, the return value is nonzero.
			if (!ReadConsole(hStdIn, CommandBuf, 80, &nReadChars, NULL)) 
			{ 
				throw exception("IAG0010Keyboard:  ReadConsole() failed, err ", GetLastError());
				return 1; 
			} 

			// The command in buf ends with "\r\n", we have to get rid of them
			CommandBuf[nReadChars - 2] = 0;

			// Case-insensitive comparation
			if (!_tcsicmp(CommandBuf, _T("exit")))	
			{
				// To force the other threads to quit
				eventExitTyped->SetEvent();			
				break;
			}
			else
			{
				_tcout << "IAG0010Keyboard:  cmd \"" << CommandBuf << "\" won't work" << endl;
			}
		}
		catch (exception &ex)
		{
			cout << ex.what() << endl;
			return 1;
		}
	}
	return 0;
}
