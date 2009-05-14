/***************************************************************************
//
// ONSIPUI.cpp
//
// TAPI Service provider for TSP++ version 3.0
// User-Interface main entrypoint
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
#include "OnSipUI.h"
#include "config.h"
#include <string.h>
#include <spbstrm.h>
#include "utils.h"
#include "OnSipGlobal.h"
#include "Logger.h"

/*-------------------------------------------------------------------------------*/
// RTTI Support
/*-------------------------------------------------------------------------------*/
IMPLEMENT_DYNCREATE(CUIDevice, CTSPUIDevice)
IMPLEMENT_DYNCREATE(CUILine, CTSPUILineConnection)

/*----------------------------------------------------------------------------
	GLOBALS AND CONSTANTS
-----------------------------------------------------------------------------*/
CTspUIApp theApp;

/*****************************************************************************
** Procedure:  CTspUIApp::CTspUIApp
**
** Arguments:  void
**
** Returns:    void
**
** Description:  Constructor for the UI application dll
**
*****************************************************************************/
CTspUIApp::CTspUIApp() : 
	CServiceProvider(PROVIDER_NAME)
{
	// You must assign the overridden objects to the library 
	// so they are serialized properly when reloaded if you add additional
	// data to the iostream (read/write overridden).
	SetRuntimeObjects(
		RUNTIME_CLASS(CUIDevice),		// Device override
		RUNTIME_CLASS(CUILine),			// Line device
		NULL, 
		NULL);

}// CTspUIApp::CTspUIApp

/*****************************************************************************
** Procedure:  CTspUIApp::providerInstall
**
** Arguments:  'dwPermanentProviderID' - Provider ID
**             'pwndOwner' - Owner window handle
**
** Returns:    TAPI 2.x result code
**
** Description:  This method is invoked when the TSP is to be installed via 
**               the TAPI install code.  It should insure that all the correct 
**               files are there, and write out the initial .INI settings.
**
*****************************************************************************/
LONG CTspUIApp::providerInstall(DWORD dwPermanentProviderID, CWnd* pwndOwner)
{
	// Verify that our TSP is not already installed; in most implementations
	// you should only allow for one device per TSP.
	DWORD dwMyPPid = 0;
	LONG lResult = IsProviderInstalled(_T("OnSip.tsp"), &dwMyPPid);
	if (lResult == 0) // Returns LINEERR_NOMULTIPLEINSTANCE if already installed
	{
		// Create the device object which represents this provider. 
		AddDevice(dwPermanentProviderID);

		// Pass through to the TSP++ library to add registry keys and such.
		// If that is successful, then run the configuration for this device.
		lResult = CServiceProvider::providerInstall(dwPermanentProviderID, pwndOwner);
		if (lResult == 0)
		{
			// TODO: Show a user interface dialog if necessary and prompt the
			// user for information concerning line and phone devices present
			// on the hardware.
			CConfigDlg dlg(pwndOwner);

			// Get our device class
			CTSPUIDevice* pDevice = GetDeviceByIndex(0);
			Logger::log_debug("CTspUIApp::providerConfig pDevice=%p",pDevice);
			ASSERT (pDevice != NULL);
			CUIDevice* pOurDevice = static_cast<CUIDevice*>(pDevice);

			dlg.SetValues(pOurDevice->m_phoneNumber,pOurDevice->m_userName,pOurDevice->m_password,pOurDevice->m_domain);
			if (dlg.DoModal() == IDOK)
			{
				dlg.GetValues(pOurDevice->m_phoneNumber,pOurDevice->m_userName,pOurDevice->m_password,pOurDevice->m_domain);
				// create a line and phone so the generated provider will have
				// devices. The first parameter is a unique identifier for the device
				// which is used to locate the device in the provider code.
				pDevice->AddLine(new CUILine(LINE_ID_UNIQUE, CTSPUILineConnection::Station, pOurDevice->m_phoneNumber.c_str()));
				// Save off all our registry data. This is a built-in function of 
				// the TSP++ UI library and dumps all created objects into the registry.
				SaveObjects();
			}
			
			// Otherwise cancel the installation.
			else
				lResult = LINEERR_OPERATIONFAILED;
		}
	}
	return lResult;

}// CTspUIApp::providerInstall

/*****************************************************************************
** Procedure:  CTspUIApp::providerConfig
**
** Arguments:  'dwPPID' - Provider ID
**             'pwndOwner' - Owner window handle
**
** Returns:    TAPI 2.x result code
**
** Description:  This method is invoked when the user selects our ServiceProvider
**				 icon in the control panel.  It should invoke the configuration 
**               dialog which must be provided by the derived class.
**
*****************************************************************************/
LONG CTspUIApp::providerConfig(DWORD dwProviderID, CWnd* pwndOwner)
{
	Logger::SetWin32Level( Logger::LEVEL_DEBUG );
	Logger::log_debug("CTspUIApp::providerConfig providerID=%ld",dwProviderID);

	// TODO: Show a user interface dialog of all the configuration
	// data for all lines and phones on the hardware.
	CConfigDlg dlg(pwndOwner);

	// Get the object to store
	CTSPUIDevice* pDevice = GetDeviceByIndex(0);
	Logger::log_debug("CTspUIApp::providerConfig pDevice=%p",pDevice);
	ASSERT (pDevice != NULL);
	CUIDevice* pOurDevice = static_cast<CUIDevice*>(pDevice);

	Logger::log_debug("CTspUIApp::providerConfig pOurDevice=%p",pOurDevice);

	dlg.SetValues(pOurDevice->m_phoneNumber,pOurDevice->m_userName,pOurDevice->m_password,pOurDevice->m_domain);
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetValues(pOurDevice->m_phoneNumber,pOurDevice->m_userName,pOurDevice->m_password,pOurDevice->m_domain);
		// Save off all our registry data. This is a built-in function of 
		// the TSP++ UI library and dumps all created objects into the registry.
		SaveObjects();
	}
	return 0;

}// CTspUIApp::providerConfig

/*****************************************************************************
** Procedure:  CTspUIApp::lineConfigDialog
**
** Arguments:  'dwDeviceID' - Line Device ID
**             'pwndOwner' - Owner window handle
**             'strDeviceClass' - Device class we are working with
**
** Returns:    TAPI 2.x result code
**
** Description:  This method is called to display the line configuration dialog
**				 when the user requests it through either the TAPI api or the 
**               control panel applet.
**
*****************************************************************************/
LONG CTspUIApp::lineConfigDialog(DWORD dwDeviceID, CWnd* pwndOwner, CString& /*strDeviceClass*/) 
{
	// Convert the device id into a permanent device id to ensure that we
	// have the correct line device.  Normally the dwDeviceID is simply an
	// index into our line array but if there is more than one provider installed
	// the base might not be zero.
	DWORD dwpLID;
	if (GetUISP()->GetPermanentIDFromLineDeviceID(dwDeviceID, &dwpLID) == 0)
	{
		// NOTE: This assumes a single device object per provider - this is not a 
		// requirement of TSP++, but is typical of most providers.
		CTSPUILineConnection* pLine = GetUISP()->GetDeviceByIndex(0)->FindLineConnectionByPermanentID(dwpLID);
		if (pLine != NULL)
		{
			// TODO: Show user-interface for configuring this specific
			// line device.
			return FALSE;
		}
	}
	return LINEERR_OPERATIONFAILED;

}// CTspUIApp::lineConfigDialog

/*****************************************************************************
** Procedure:  CUIDevice::CUIDevice
**
** Arguments:  void
**
** Returns:    void
**
** Description: Default constructor for the device object
**
*****************************************************************************/
CUIDevice::CUIDevice()
{
	// TODO: Initialize any data

}// CUIDevice::CUIDevice

/*****************************************************************************
** Procedure:  CUIDevice::read
**
** Arguments:  'istm' - Input iostream
**
** Returns:    iostream reference
**
** Description: This is called to read information in from the registry.
**
*****************************************************************************/
TStream& CUIDevice::read(TStream& istm)
{
	Logger::log_debug( _T("CUIDevice::read for ProviderID=%ld"), m_dwPermProviderID );
	
	// Always call the base class!
	CTSPUIDevice::read(istm);

	// Read values directly from the registry
	m_userName = GetUISP()->ReadProfileString(m_dwPermProviderID, REG_USERNAME, _T("") );
	m_password = GetUISP()->ReadProfileString(m_dwPermProviderID, REG_PASSWORD, _T("") );
	m_domain =  GetUISP()->ReadProfileString(m_dwPermProviderID, REG_DOMAIN, _T("") );
	m_phoneNumber = GetUISP()->ReadProfileString(m_dwPermProviderID, REG_PHONENUMBER, _T("") );

	return istm;

}// CUIDevice::read

/*****************************************************************************
** Procedure:  CUIDevice::write
**
** Arguments:  'ostm' - Input iostream
**
** Returns:    iostream reference
**
** Description: This is called to read information in from the registry.
**
*****************************************************************************/
TStream& CUIDevice::write(TStream& ostm) const
{
	Logger::log_debug( _T("CUIDevice::write for ProviderId=%ld"), m_dwPermProviderID );

	// Always call the base class!
	CTSPUIDevice::write(ostm);

	// Write values directly to the registry
	GetUISP()->WriteProfileString(m_dwPermProviderID, REG_USERNAME, m_userName.c_str() );
	GetUISP()->WriteProfileString(m_dwPermProviderID, REG_PASSWORD, m_password.c_str() );
	GetUISP()->WriteProfileString(m_dwPermProviderID, REG_DOMAIN, m_domain.c_str() );
	GetUISP()->WriteProfileString(m_dwPermProviderID, REG_PHONENUMBER, m_phoneNumber.c_str() );

	return ostm;

}// CUIDevice::write

/*****************************************************************************
** Procedure:  CUILine::CUILine
**
** Arguments:  void
**
** Returns:    void
**
** Description: Default constructor for the line object
**
*****************************************************************************/
CUILine::CUILine()
{
	// TODO: Initialize any data

}// CUILine::CUILine

/*****************************************************************************
** Procedure:  CUILine::CUILine
**
** Arguments:  'dwDeviceID' - Device identifier
**             'iType' - Type of line device (Station, RoutePoint, etc.)
**             'pszName' - Name of line device
**
** Returns:    void
**
** Description:  Constructor for the line device
**
*****************************************************************************/
CUILine::CUILine(DWORD dwDeviceID, int iType, LPCTSTR pszName) : 
	CTSPUILineConnection(dwDeviceID, iType, pszName)
{
    // Create each dialable number (address) which exists on each line.
	for (int iAddress = 0; iAddress < 1; iAddress++)
	{
		// TODO: Replace these entries with the name and dialable number
		// of each address created.
		TCHAR chBuff[20], chBuff2[20];
		wsprintf(chBuff, _T("Address %d"), iAddress);	
		wsprintf(chBuff2, _T("%d"), 100+iAddress);

		switch (GetLineType())
		{
			case CTSPUILineConnection::Station:
				CreateAddress(chBuff2, chBuff, true, true,
					LINEMEDIAMODE_UNKNOWN | LINEMEDIAMODE_INTERACTIVEVOICE,	LINEBEARERMODE_VOICE,
					0, 0, NULL, 1, 0, 0, 0, 0);
				break;
			default: ASSERT(FALSE);
		}
	}

}// CUILine::CUILine

/*****************************************************************************
** Procedure:  CUILine::read
**
** Arguments:  'istm' - Input iostream
**
** Returns:    iostream reference
**
** Description: This is called to read information in from the registry.
**
*****************************************************************************/
TStream& CUILine::read(TStream& istm)
{
	// Always call the base class!
	CTSPUILineConnection::read(istm);

	// TODO: Read any addition information from the registry stream
	// using the direct stream >> and << operators.
	//
	// WARNING: Make sure to coordinate the read/write functions

	return istm;

}// CUILine::read

/*****************************************************************************
** Procedure:  CUILine::write
**
** Arguments:  'ostm' - Input iostream
**
** Returns:    iostream reference
**
** Description: This is called to read information in from the registry.
**
*****************************************************************************/
TStream& CUILine::write(TStream& ostm) const
{
	// Always call the base class!
	CTSPUILineConnection::write(ostm);

	// TODO: Write any addition information into the registry stream
	// using the direct stream >> and << operators.
	//
	// WARNING: Make sure to coordinate the read/write functions

	return ostm;

}// CUILine::write


