/***************************************************************************
//
// ONSIP.H
//
// TAPI Service provider for TSP++ version 3.0
// Designed for CSwitchInfo
//
// Copyright (C) 2009 Junction Networks
// All rights reserved
//
// Generated by the TSPWizard � 2009 JulMar Technology, Inc.
// 
/***************************************************************************/

#ifndef _OnSip_INC_
#define _OnSip_INC_

/*----------------------------------------------------------------------------
	INCLUDE FILES
-----------------------------------------------------------------------------*/
#include "resource.h"
#include <poolmgr.h>
#include "OnSipTapi.h"
#include "OnSipGlobal.h"

/*----------------------------------------------------------------------------
	PRE-DECLARATIONS
-----------------------------------------------------------------------------*/
class COnSipLine;
class COnSipDevice;

/*----------------------------------------------------------------------------
	CONSTANTS
-----------------------------------------------------------------------------*/
#define STATE_WAITING		(STATE_INITIAL + 1)
#define REQUEST_TIMEOUT		(10000)				// Timeout for requests (mSec)


/**************************************************************************
** COnSipEvent
**
** Structure/object which is used to encapsulate device event information
**
***************************************************************************/

class COnSip_CallEvent;
class COnSip_ConnectEvent;

// TODO!!! Add debug reference counting for events to ensure deleted.

// Base event class, never used directly,
// instead use one of the derived classes, 
//  e.g.  COnSip_CallEvent, 
class COnSipEvent
{
public:
	enum OnSipEventType { CALL_EVENT, CONNECT_EVENT };

	virtual ~COnSipEvent();

	// Return the type of COnSipEvent
	OnSipEventType getType() const
	{	return m_evtType; }

	// Return COnSip_ConnectEvent pointer for this
	// class if this is that type of instance,
	// else returns NULL
	static const COnSip_ConnectEvent* getConnectEvent(const COnSipEvent* );

	// Return COnSip_CallEvent pointer for this
	// class if this is that type of instance,
	// else returns NULL
	static const COnSip_CallEvent* getCallEvent(const COnSipEvent* );

protected:
	// Hide constructor so class cannot be created directly
	COnSipEvent(OnSipEventType evtType);

private:
	OnSipEventType m_evtType;
};

// COnSipEvent type for call events
// Wraper aroudn the State Machine CallState and CallStateData
class COnSip_CallEvent : public COnSipEvent
{
public:
	OnSipTapiCall m_onSipCall;

	COnSip_CallEvent(const OnSipTapiCall& onSipCall) : COnSipEvent(COnSipEvent::CALL_EVENT)
	{ m_onSipCall = onSipCall; }

	tstring ToString() const
	{	return Strings::stringFormat(_T("<COnSip_CallEvent %s>"), m_onSipCall.ToString().c_str()); };

	// Get the unique CallId
	DWORD CallId() const
	{	return (DWORD) m_onSipCall.m_callStateData.m_callId; }

	// Return the TAPI CallState for the OnSipTapiCall
	DWORD GetTapiCallState() const
	{	return m_onSipCall.GetTapiCallState(); }

	// Remote ID
	tstring RemoteID() const
	{	return m_onSipCall.RemoteID(); }

	// Called ID
	tstring CalledID() const
	{	return m_onSipCall.CalledID(); }

	// Get the CallType
	OnSipXmppCallType::CallType CallType() const
	{	return m_onSipCall.m_callStateData.m_callType; }
};

// COnSipEevnt for Connect events
class COnSip_ConnectEvent : public COnSipEvent
{
public:
	OnSipInitState m_onSipInit;

	OnSipInitStates::InitStates GetInitState() const
	{	return m_onSipInit.m_state; }

	TCHAR *GetInitStateAsString() const
	{	return OnSipInitStates::InitStatesToString(m_onSipInit.m_state); }

	OnSipInitStatesType::InitStatesType GetInitStateType() const
	{	return OnSipInitStatesType::GetInitStatesType(m_onSipInit.m_state); }

	TCHAR *GetInitStateTypeAsString() const
	{	return OnSipInitStatesType::InitStatesTypeToString( OnSipInitStatesType::GetInitStatesType(m_onSipInit.m_state) ); }

	COnSip_ConnectEvent(const OnSipInitState& onSipInit) : COnSipEvent(COnSipEvent::CONNECT_EVENT)
	{ m_onSipInit = onSipInit; }

};

class COnSipLine;

/**************************************************************************
** COnSipServiceProvider
**
** Main provider object which manages the lifetime of the TSP
**
***************************************************************************/
class COnSipServiceProvider : public CServiceProvider
{
// Constructor
public:
    COnSipServiceProvider();
	// Overrides from CServiceProvider
public:
	virtual void TraceOut(TString& strBuff);
};

/**************************************************************************
** COnSipDevice
**
** Device object which manages a connection to hardware device
**
***************************************************************************/

class COnSipDevice : public CTSPIDevice
{
// Unavailable functions (due to base class)
private:
	COnSipDevice(const COnSipDevice&);
	const COnSipDevice& operator=(const COnSipDevice&);
	std::auto_ptr<OnSipTapi> m_OnSipTapi;
	CriticalSection m_cs;
	volatile bool m_bInitialized;
	list<COnSip_CallEvent *> _getEvents( list< OnSipTapiCall >& lstCallEvents );
	list<COnSip_ConnectEvent *> _getEvents( list< OnSipInitState >& lstInitEvents );
	// Processes the event in TAPI, passes it on to the TAPI thread pools
	bool _processEvent( COnSipEvent* pEvent );
	volatile OnSipInitStatesType::InitStatesType m_initStateType;

// Class Data
protected:
	HANDLE m_hevtStop;						// Stop event
	HANDLE m_hConnThread;					// Thread running hardware connection
	HANDLE m_hOpenDevice;					// Event to signal when device has been opened
	CRITICAL_SECTION m_csData;				// CS protecting queue
	CThreadPoolMgr<DWORD, COnSipEvent*> m_mgrThreads;	// Thread pool manager class

private:
	void _killThreads();
	static unsigned __stdcall MainConnThread(void* pParam);
	
// Constructor/Destructor
public:
	COnSipDevice();
	virtual ~COnSipDevice();

	long DRV_DropCall(const COnSipLine* pLine, CTSPICallAppearance* pCall);
	long DRV_MakeCall(const COnSipLine* pLine, const TString& strDigits, DWORD dwCountryCode=0);

// Overridden functions from CTSPIDevice
protected:
	friend class COnSipLine;
	virtual bool Init(DWORD dwProviderId, DWORD dwBaseLine, DWORD dwBasePhone, DWORD dwLines, DWORD dwPhones, HPROVIDER hProvider, ASYNC_COMPLETION lpfnCompletion);
	virtual TStream& read(TStream& istm);
	virtual bool OpenDevice (CTSPIConnection* pConn);
	virtual bool CloseDevice (CTSPIConnection* pConn);

	unsigned ConnectionThread();
	void OnConnect(bool bConnect);

// Internal functions
protected:
	CTSPIConnection* LocateOwnerFromEvent(COnSipEvent* pEvent);
};

/**************************************************************************
** COnSipLine
**
** Line object which manages a single line on the hardware
**
***************************************************************************/
class COnSipLine : public CTSPILineConnection
{
// Unavailable functions (due to base class)
private:
	COnSipLine(const COnSipLine&);
	const COnSipLine& operator=(const COnSipLine&);

// Constructor/Destructor
public:
	COnSipLine();
	virtual ~COnSipLine();

// Overrides from CTSPILineConnection
public:
	COnSipDevice* GetDeviceInfo() const { return (COnSipDevice*) CTSPIConnection::GetDeviceInfo(); }
	virtual TStream& read(TStream& istm);

protected:
	virtual DWORD OnAddressFeaturesChanged (CTSPIAddressInfo* pAddr, DWORD dwFeatures);
	virtual DWORD OnLineFeaturesChanged(DWORD dwLineFeatures);
	virtual DWORD OnCallFeaturesChanged(CTSPICallAppearance* pCall, DWORD dwCallFeatures);
	virtual bool OpenDevice();
	virtual bool UnsolicitedEvent(LPCVOID lpBuff);

	virtual void OnTimer();

	// Create the event map
	DECLARE_TSPI_REQUESTS()

	// TSPI handlers
	bool OnDropCall(RTDropCall* pReq, LPCVOID lpBuff);
	bool OnMakeCall(RTMakeCall* pReq, LPCVOID lpBuff);
	bool OnDial(RTDial* pReq, LPCVOID lpBuff);

	CTSPICallAppearance* OnNewCallDetected(bool fPlaced,CTSPICallAppearance* pCall, const COnSip_CallEvent* callEvent);

// Internal methods
private:
	void InitializeStation();
	void InitAddress(CTSPIAddressInfo* pAddress);
	bool UnsolicitedCallEvent(const COnSip_CallEvent *pCallEvent);
	bool UnsolicitedConnectedEvent(const COnSip_ConnectEvent *pCallEvent);

};

#endif // _OnSip_INC_
