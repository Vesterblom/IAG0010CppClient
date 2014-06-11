// Evgenia Udod TTÜ. Author of this file is V.Leppikson
// 120042IASMM
// V.Leppikson

#pragma once
#include "stdafx.h"


//
//**************************************************************************************************************
//                          CLASS IAG0010Event
//**************************************************************************************************************
//
class IAG0010Event
{ 
private:
	HANDLE m_hEvent;
public:
	IAG0010Event()																// Constructor 1
	{
		m_hEvent = NULL;
	}

	IAG0010Event(BOOL bManualReset, BOOL bInitialState) throw (exception)			// Constructor 2
	{ 
		m_hEvent = CreateEvent(NULL, bManualReset, bInitialState, NULL);
		if (m_hEvent == NULL)
			throw exception((string("Unable to create event, error ") + to_string(GetLastError())).c_str());
	}

	HANDLE GetHandle()
	{
		return m_hEvent;
	}

	BOOL SetEvent()
	{
		return m_hEvent != NULL ? ::SetEvent(m_hEvent) : FALSE;
	}

	BOOL ResetEvent()
	{
		return m_hEvent != NULL ? ::ResetEvent(m_hEvent) : FALSE;
	}

	DWORD Lock() throw (exception)
	{
		if (m_hEvent == NULL)
			throw exception("Event not initialized");
		if (WaitForSingleObject(m_hEvent, INFINITE) != WAIT_OBJECT_0)
			throw exception((string("Waiting failed, error ") + to_string(GetLastError())).c_str());
		return WAIT_OBJECT_0;
	}

	DWORD Lock(DWORD timeout) throw (exception)
	{
		if (m_hEvent == NULL)
			throw exception("Event not initialized");
		switch (WaitForSingleObject(m_hEvent, timeout))
		{
		case WAIT_OBJECT_0:
			return WAIT_OBJECT_0;
		case WAIT_TIMEOUT:
			return WAIT_TIMEOUT;
		default:
			throw exception((string("Waiting failed, error ") + to_string(GetLastError())).c_str());
		}
	}

	~IAG0010Event()										// Destructor
	{
		if (m_hEvent)
			CloseHandle(m_hEvent);
	}
	friend class IAG0010MultiLock;
};
//
//**************************************************************************************************************
//                          CLASS IAG0010MultiLock
//**************************************************************************************************************
//
class IAG0010MultiLock
{
private:
	int m_nEvents;  // number of events
	int m_nMaxEvents; // max allowed value of events
	HANDLE *m_pHandles; // vector of handles
public:
	IAG0010MultiLock(int nMax)  throw (exception)								// Constructor 1
	{
		if (nMax <= 0)
			throw exception("Illegal arguments");
		m_nMaxEvents = nMax;
		m_pHandles = new HANDLE[nMax];
		m_nEvents = 0;
	}

	void AppendEvent(IAG0010Event& event) throw (exception)						// Write events into array
	{
		if (m_nMaxEvents == m_nEvents)
			throw exception("Object is full");
		if (event.m_hEvent == NULL)
			throw exception("Illegal argument");
		*(m_pHandles + m_nEvents) = event.m_hEvent;
		m_nEvents++;
	}

	DWORD Lock(DWORD timeout = INFINITE, BOOL WaitForAll = TRUE)  throw (exception)
	{
		if (m_pHandles == NULL || !m_nEvents)
			throw exception("Event(s) not initialized");
		DWORD Result = WaitForMultipleObjects(m_nEvents, m_pHandles, WaitForAll, timeout); 
		switch (Result)
		{
		case WAIT_TIMEOUT:
			return WAIT_TIMEOUT;
		case WAIT_FAILED:
			throw exception((string("Waiting failed, error ") + to_string(GetLastError())).c_str());
		default:
			return Result; // index specifying the signalled event
		}
	}

	~IAG0010MultiLock()						// Destructor
	{
		if (m_pHandles)
			delete m_pHandles;
	}
};
//
//**************************************************************************************************************
//                          CLASS IAG0010Socket
//**************************************************************************************************************
//
class IAG0010SocketBase
{
public:
	//I have written here "{};" for construct, because otherwise I cant link... EU
	IAG0010SocketBase(IAG0010Event *p1, _TCHAR *p2) throw (exception){}
	// Constructor
	// p1 - pointer to event which becomes signalled when the user has typed the "exit" command
	// p2 - pointer to the output file name (actually, to the command line parameter)
	// Exception is thrown when the input parameters are not correct
	virtual void OpenConnection() throw (exception){}
	// Creates the socket and establishes the connection with the server.
	// Exception is thrown when the connection was not established
	virtual void Start(IAG0010Event &e){}
	// Starts the data transfer
	// e is the reference to the event that becomes signalled when the transfer
	// has ended, data receiving and sending threads stopped and the connection
	// closed. When e is signalled, the main thread can exit. 
};