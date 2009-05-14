/***************************************************************************
//
// UNSOLICITED.CPP
//
// TAPI Service provider for TSP++ version 3.0
// Unsolicited event handler
//
// Copyright (C) 2009 Junction Networks
// All rights reserved
//
// Generated by the TSPWizard � 2009 JulMar Technology, Inc.
// 
/***************************************************************************/

/*----------------------------------------------------------------------------
	INCLUDE FILES
-----------------------------------------------------------------------------*/
#include "stdafx.h"
#include "OnSip.h"

/*****************************************************************************
** Procedure:  COnSipLine::UnsolicitedEvent
**
** Arguments:  'lpBuff' - The event block pointer
**
** Returns:    void
**
** Description:  This function is called when no request processed a given
**               response from the device and it was directed at this line.
**
*****************************************************************************/
bool COnSipLine::UnsolicitedEvent(LPCVOID lpBuff)
{
	// Cast the input opaque pointer back to an event block
	const COnSipEvent* pEvent = static_cast<const COnSipEvent*>(lpBuff);

	// See if Call Event
	const COnSip_CallEvent* pCallEvent = COnSipEvent::getCallEvent(pEvent);

	// See if Connect event
	const COnSip_ConnectEvent* pConnectEvent = COnSipEvent::getConnectEvent(pEvent);

	Logger::log_debug(_T("COnSipLine::UnsolicitedEvent evtType=%d pCallEvt=%p pConnectEvt=%p"), pEvent->getType(), pCallEvent, pConnectEvent );

	if ( pCallEvent != NULL )
		return UnsolicitedCallEvent(pCallEvent);
	else if ( pConnectEvent != NULL )
		return UnsolicitedConnectedEvent(pConnectEvent);
	else
	{
		Logger::log_error( _T("COnSipLine::UnsolicitedEvent UNKOWN event") );
		_ASSERT(false);
		return false;
	}
}

bool COnSipLine::UnsolicitedConnectedEvent(const COnSip_ConnectEvent *pConnectEvent)
{
	Logger::log_debug(_T("COnSipLine::UnsolicitedConnectedEvent line=%p"), this );

	// Get the connection state and state type
	OnSipInitStates::InitStates state = pConnectEvent->GetInitState();
	TCHAR *szState = pConnectEvent->GetInitStateAsString();
	OnSipInitStatesType::InitStatesType stateType = pConnectEvent->GetInitStateType();
	TCHAR *szStateType = pConnectEvent->GetInitStateTypeAsString();

	Logger::log_debug(_T("COnSipLine::UnsolicitedConnectedEvent line=%p state=%s stateType=%s"), this, szState, szStateType );

	// If fatal type of error, then close line
	if ( stateType == OnSipInitStatesType::FATAL )
	{
		Logger::log_error( _T("COnSipLine::UnsolicitedConnectedEvent FATAL %s forcing LINE_CLOSE"), szState );
		this->ForceClose();
		return true;
	}

	// Assume we handle it, there are no other lines for this to be sent to
	return true;
}

bool COnSipLine::UnsolicitedCallEvent(const COnSip_CallEvent *pCallEvent)
{
	// See if we have TAPI call already for this call
	CTSPICallAppearance* pCall = FindCallByCallID(pCallEvent->CallId());
	Logger::log_debug(_T("COnSipLine::UnsolicitedCallEvent callId=%ld call=%s pCall=%p "), pCallEvent->CallId(), pCallEvent->ToString().c_str(), pCall );

	// If a new call detected
	if ( pCall == NULL )
	{
		Logger::log_debug(_T("COnSipLine::UnsolicitedCallEvent newCall callId=%ld callType=%s"), pCallEvent->CallId(), OnSipXmppCallType::CallTypeToString(pCallEvent->CallType()) );
		
		// If incoming call
		if ( pCallEvent->CallType() == OnSipXmppCallType::IncomingCall )
		{
			pCall = OnNewCallDetected( false, pCall, pCallEvent );
			return true;
		}
		// If physical call
		if ( pCallEvent->CallType() == OnSipXmppCallType::PhysicalCall )
		{
			pCall = OnNewCallDetected( false, pCall, pCallEvent );
			return true;
		}

		// Shouldn't get here
		Logger::log_error(_T("COnSipLine::UnsolicitedCallEvent unrecognized newCall %s"), pCallEvent->ToString().c_str());
		return false;
	}
	// else if active call
	else if ( pCall != NULL )
	{
		// Get the event TAPI CallState
		DWORD dwCallState = pCallEvent->GetTapiCallState();
		// Get the current TAPI callstate
		DWORD dwCurCallState = pCall->GetCallState();
		Logger::log_debug(_T("COnSipLine::UnsolicitedCallEvent activeCall callId=%ld evtTapiCallState=%lx curTapiCallState=%lx"), pCallEvent->CallId(), dwCallState, dwCurCallState );

		// If callstates have changed
		if ( dwCallState != dwCurCallState )
		{
			Logger::log_debug(_T("COnSipLine::UnsolicitedCallEvent callStateChange callId=%ld tapiCallState=%lx"), pCallEvent->CallId(), dwCurCallState );
			pCall->SetCallState( dwCallState, 0, LINEMEDIAMODE_INTERACTIVEVOICE );
		}
		return true;
	}

	Logger::log_debug(_T("COnSipLine::UnsolicitedCallEvent not handled") );
	return false;

}// COnSipLine::UnsolicitedEvent
