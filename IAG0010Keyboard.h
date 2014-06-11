// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson

#pragma once
#include "stdafx.h"


class IAG0010Keyboard
{
public:
	IAG0010Keyboard(IAG0010Event *exitTyped);
	int Run(void);
	~IAG0010Keyboard();

private:
	IAG0010Event*	eventExitTyped;
	HANDLE			hStdIn;				// A handle to the console input buffer
	DWORD			nReadChars;			// A pointer to a variable that receives the number of characters actually read
	TCHAR			CommandBuf[81];		// A pointer to a buffer that receives the data read from the console input buffer
};
