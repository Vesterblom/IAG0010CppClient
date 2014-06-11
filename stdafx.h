// Evgenia Udod TTÜ
// 120042IASMM
// V.Leppikson
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once;
#pragma warning(disable : 4290)
#pragma warning(disable : 4996)

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <WinSock2.h>
#include "mswsock.h"


#if defined(UNICODE)
#define _tcout std::wcout
#else
#define _tcout std::cout
#endif

using namespace std;
typedef std::basic_string<TCHAR> _tstring;


#include "IAG0010CppClientBase.h"



// TODO: reference additional headers your program requires here
