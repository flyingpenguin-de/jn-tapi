/***************************************************************************
//
// Config.cpp
//
// TAPI Service provider for TSP++ version 3.0
// Configuration dialog
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

/*-------------------------------------------------------------------------------*/
// MFC Message map
/*-------------------------------------------------------------------------------*/
BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
END_MESSAGE_MAP()

/*****************************************************************************
** Procedure:  CConfigDlg::CConfigDlg
**
** Arguments:  'pParent' - Parent window handle
**
** Returns:    void
**
** Description: Constructor for the sample configuration dialog
**
*****************************************************************************/
CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/) : CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}// CConfigDlg::CConfigDlg

void CConfigDlg::SetValues(const tstring& phoneNumber,const tstring& userName,const tstring& passWord,const tstring& domain)
{
	// Get the device variables and assign to the dialog 
	m_passWord = passWord.c_str();
	m_domain = domain.c_str();
	m_phoneNumber = phoneNumber.c_str();
	m_userName = userName.c_str();
}

void CConfigDlg::GetValues(tstring& phoneNumber,tstring& userName,tstring& passWord,tstring& domain)
{
	passWord = m_passWord;
	domain = m_domain;
	phoneNumber = m_phoneNumber;
	userName = m_userName;
}

/*****************************************************************************
** Procedure:  CConfigDlg::DoDataExchange
**
** Arguments:  'pDX' - CDataExchange element
**
** Returns:    void
**
** Description: This connects window controls up with C++ objects.
**
*****************************************************************************/
void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_EDIT_PHONENUMBER, m_txtPhoneNumber);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_txtUserName);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_txtPassword);
	DDX_Control(pDX, IDC_EDIT_DOMAIN, m_txtDomain);

	DDX_Text( pDX, IDC_EDIT_PHONENUMBER, m_phoneNumber );
	DDX_Text( pDX, IDC_EDIT_USERNAME, m_userName );
	DDX_Text( pDX, IDC_EDIT_PASSWORD, m_passWord );
	DDX_Text( pDX, IDC_EDIT_DOMAIN, m_domain );

}// CConfigDlg::DoDataExchange

/*****************************************************************************
** Procedure:  CConfigDlg::OnInitDialog
**
** Arguments:  void
**
** Returns:    TRUE if Windows is to decide the first control to have focus.
**
** Description: This is called during the dialog's initialization.
**
*****************************************************************************/
BOOL CConfigDlg::OnInitDialog()
{
	// Connect up the controls to the objects.
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}// CConfigDlg::OnInitDialog


void CConfigDlg::OnBnClickedOk()
{
	OnOK();
}
