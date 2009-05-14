/***************************************************************************
//
// LINE.CPP
//
// TAPI Service provider for TSP++ version 3.0
// Line management functions
//
// Copyright (C) 2009 Junction Networks
// All rights reserved
//
// Generated by the TSPWizard � 2009 JulMar Technology, Inc.
// 
/***************************************************************************/

/*-------------------------------------------------------------------------------*/
// INCLUDE FILES
/*-------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "OnSip.h"
#include <spbstrm.h>

/*----------------------------------------------------------------------------
	GLOBALS AND CONSTANTS
-----------------------------------------------------------------------------*/
const UINT MAXCALLDATA_SIZE = 4096;

/*-------------------------------------------------------------------------------*/
// TSPI Request map
/*-------------------------------------------------------------------------------*/
BEGIN_TSPI_REQUEST(COnSipLine)
	ON_AUTO_TSPI_REQUEST(REQUEST_SETCALLDATA)
	ON_TSPI_REQUEST_MAKECALL(OnMakeCall)
	ON_TSPI_REQUEST_DIAL(OnDial)
	ON_TSPI_REQUEST_DROPCALL(OnDropCall)
END_TSPI_REQUEST()

/*****************************************************************************
** Procedure:  COnSipLine::COnSipLine
**
** Arguments: void
**
** Returns:    void
**
** Description:  Constructor for the line object
**
*****************************************************************************/
COnSipLine::COnSipLine()
{
}// COnSipLine::COnSipLine

/*****************************************************************************
** Procedure:  COnSipLine::~COnSipLine
**
** Arguments: void
**
** Returns:    void
**
** Description:  Destructor for the line object
**
*****************************************************************************/
COnSipLine::~COnSipLine()
{
	// TODO: destroy any allocated memory here

}// COnSipLine::~COnSipLine

/*****************************************************************************
** Procedure:  COnSipLine::read
**
** Arguments:  'istm' - Input stream
**
** Returns:    pointer to istm
**
** Description:  This function is called to serialize data in from the
**               registry.  The line object has already been completely
**               initialized by the TSP++ library
**
*****************************************************************************/
TStream& COnSipLine::read(TStream& istm)
{
	// Always call the base class to read in default line information
	CTSPILineConnection::read(istm);

	// TODO: Read any information stored in the line stream

    LPLINEDEVCAPS lpLineCaps = GetLineDevCaps();
	// TODO: Adjust the device capabilities for this line

	// TODO: Add any WAV devices which will be used for I/O - locate your WAV drive
	// and use the device identifier given by the multimedia system.
	// AddDeviceClass(_T("wave/in"), dwWaveInDeviceID);
	// AddDeviceClass(_T("wave/out"), dwWaveOutDeviceID);

	// Configure the line based on the type
	switch (GetLineType())
	{
		case Station:			InitializeStation(); break;
		default:				break;
	}

	return istm;

}// COnSipLine::read

/*****************************************************************************
** Procedure:  COnSipLine::InitializeStation
**
** Arguments:  void
**
** Returns:    void
**
** Description:  This function is called to initialize a station line object.
**
*****************************************************************************/
void COnSipLine::InitializeStation()
{
	LPLINEDEVCAPS lpCaps = GetLineDevCaps();



	// TODO: Adjust the ADDRESSCAPS based on this being a station
	for (int iAddress = 0; iAddress < GetAddressCount(); iAddress++)
	{
		CTSPIAddressInfo* pAddress = GetAddress(iAddress);
		_TSP_ASSERTE (pAddress != NULL);

		// Initialize the address information to be the basic set of information
		InitAddress(pAddress);

		// TODO: Adjust the address capabilities based on a station capabilities.
		// Look at the COnSipLine::InitAddress function to see how the
		// address is configured for all lines.
	}

}// COnSipLine::InitializeStation

/*****************************************************************************
** Procedure:  COnSipLine::InitAddress
**
** Arguments:  'pAddr' - Address object to initialize
**
** Returns:    void
**
** Description:  This function is called to initialize a single address
**
*****************************************************************************/
void COnSipLine::InitAddress(CTSPIAddressInfo* pAddress)
{
	_TSP_ASSERTE (pAddress != NULL);
	LPLINEADDRESSCAPS lpAddrCaps = pAddress->GetAddressCaps();
	lpAddrCaps->dwMaxCallDataSize = MAXCALLDATA_SIZE;

	// Set the dialtone modes

	lpAddrCaps->dwDialToneModes &= ~(LINEDIALTONEMODE_INTERNAL | LINEDIALTONEMODE_EXTERNAL | LINEDIALTONEMODE_SPECIAL);

	// TODO: Adjust the address capabilities to reflect the device
	// features and abilities. The wizard has set the following flags
	// for you based on your requested feature set:

	// The phone is not automatically taken off hook
	lpAddrCaps->dwAddrCapFlags &= ~LINEADDRCAPFLAGS_ORIGOFFHOOK;

}// COnSipLine::InitAddress

/*****************************************************************************
** Procedure:  COnSipLine::OnAddressFeaturesChanged
**
** Arguments:  'pAddr' - Address features are changing on
**             'dwFeatures' - New features for address
**
** Returns:    New features for address
**
** Description: This method is called when the features of the address change
**              It gives the derived code an opportunity to adjust the reflected
**              features before TAPI is notified.
**
*****************************************************************************/
DWORD COnSipLine::OnAddressFeaturesChanged (CTSPIAddressInfo* pAddr, DWORD dwFeatures)
{
	// TODO: Adjust any features for the address based on the current state of the
	// hardware. This is where you can restrict the features shown in the provider for
	// special cases.
	return CTSPILineConnection::OnAddressFeaturesChanged(pAddr, dwFeatures);

}// COnSipLine::OnAddressFeaturesChanged

/*****************************************************************************
** Procedure:  COnSipLine::OnLineFeaturesChanged
**
** Arguments:  'dwFeatures' - New features for line
**
** Returns:    New features for address
**
** Description: This method is called when the features of the line change.
**              It gives the derived code an opportunity to adjust the reflected
**              features before TAPI is notified.
**
*****************************************************************************/
DWORD COnSipLine::OnLineFeaturesChanged (DWORD dwFeatures)
{
	// TODO: Adjust any features for the line based on the current state of the
	// hardware. This is where you can restrict the features shown in the provider for
	// special cases.
	return CTSPILineConnection::OnLineFeaturesChanged(dwFeatures);

}// COnSipLine::OnLineFeaturesChanged

/*****************************************************************************
** Procedure:  COnSipLine::OnCallFeaturesChanged
**
** Arguments:  'pCall' - Call that changed
**             'dwCallFeatures' - new feature list
**
** Returns:    true/false success indicator
**
** Description: This method is called whenever the call features have changed due to
**              state changes.  The address/line have also been adjusted by the time
**              this function is called.
**
*****************************************************************************/
DWORD COnSipLine::OnCallFeaturesChanged(CTSPICallAppearance* pCall, DWORD dwCallFeatures)
{      
	// TODO: Adjust the call features for the given call based on information
	// associated with the call and hardware capabilities.
	return CTSPILineConnection::OnCallFeaturesChanged(pCall, dwCallFeatures);

}// COnSipLine::OnCallFeaturesChanged

/*****************************************************************************
** Procedure:  COnSipLine::OpenDevice
**
** Arguments:  void
**
** Returns:    void
**
** Description: This method is called when the phone is opened by TAPI.
**              It may be used to check the link to the switch and fail the
**              lineOpen() call to TAPI so that no line operations can
**              occur.
**
*****************************************************************************/
bool COnSipLine::OpenDevice()
{
	// TODO: Validate the connection to the hardware and return FALSE if it
	// is not connected or pass through to the default implementation if it is.
	return CTSPILineConnection::OpenDevice();

}// COnSipLine::OpenDevice

/*****************************************************************************
** Procedure:  COnSipLine::OnTimer
**
** Arguments:  void
**
** Returns:    void
**
** Description: This method is called periodically by the interval timer
**
*****************************************************************************/
void COnSipLine::OnTimer()
{
	// Poll the active request for timeout. This is not necessary if you
	// do not implement time-out conditions for the switch. Note that the
	// interval is controlled by the "SetIntervalTimer" in device.cpp (Init)
	ReceiveData();

}// COnSipLine::OnTimer

CTSPICallAppearance* COnSipLine::OnNewCallDetected(bool fPlaced,
							CTSPICallAppearance* pCall,
							const COnSip_CallEvent* callEvent)
{
	Logger::log_debug( _T("COnSipLine::OnNewCallDetected %s"), callEvent->ToString().c_str() );

	// If we have not yet created a call then do so. We may already have a call
	// object created if we are placing the call from the TSP.
	if (pCall == NULL)
	{
		// Create the call appearance and set the call-id in case a subsequent request
		// on another line is searching for this call.
		pCall = GetAddress(0)->CreateCallAppearance();
		pCall->SetCallID( callEvent->CallId() );

		// Set the callstate to offering or dialing for this new call.
		if (fPlaced)
			pCall->SetCallState(LINECALLSTATE_DIALING, 0, LINEMEDIAMODE_INTERACTIVEVOICE);
		else
			pCall->SetCallState(LINECALLSTATE_OFFERING, LINEOFFERINGMODE_INACTIVE, LINEMEDIAMODE_INTERACTIVEVOICE);
	}				

	// Set Caller-Id and Called-Id
	// TODO??  Is Caller-ID and Called-ID different depending on call direction????
//	if ( !fPlaced )
	DWORD dwFlags = LINECALLPARTYID_ADDRESS;
//		if (!peCaller->GetName().empty())
//			dwFlags |= LINECALLPARTYID_NAME;
	pCall->SetCallerIDInformation(dwFlags, callEvent->RemoteID().c_str(), NULL );
//	pCall->SetCalledIDInformation(dwFlags, callEvent->RemoteID().c_str(), NULL );

	// If we have no reason code for this call then assign it now.
	LPLINECALLINFO lpci = pCall->GetCallInfo();
	if (lpci->dwReason == LINECALLREASON_UNKNOWN)
	{
		pCall->SetCallReason(LINECALLREASON_DIRECT);
	}

	// Set the call origin for this call.
	// If this line is the originator of the call then mark as an outbound call.
	if (fPlaced)
	{
		pCall->SetCallOrigin(LINECALLORIGIN_OUTBOUND);
	}
	// Otherwise mark as an incoming call.
	else 
	{
//		pCall->SetCallOrigin(LINECALLORIGIN_EXTERNAL | LINECALLORIGIN_INBOUND);
		pCall->SetCallOrigin(LINECALLORIGIN_INBOUND);
	}
	return pCall;

}// CJTLine::OnNewCallDetected