; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "OnSip Hosted PBX TAPI Driver"
#define ShortAppName "OnSip TAPI Driver"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A89C95B3-05C1-4517-B746-5B954C9E1431}
AppName={#MyAppName}
AppVerName={#MyAppName} v1.0
AppPublisher=Junction Networks
AppPublisherURL=www.junctionnetworks.com
AppSupportURL=www.junctionnetworks.com
AppUpdatesURL=www.junctionnetworks.com
DefaultDirName={pf}\OnSip Hosted PBX TAPI Driver
DefaultGroupName=OnSip Hosted PBX TAPI Driver
DisableProgramGroupPage=yes
OutputDir=..\Release
OutputBaseFilename=OnSipTapi
Compression=lzma
SolidCompression=yes
InfoBeforeFile=installInfo.rtf

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: ..\Release\OnSipui.dll; DestDir: {sys}; Flags: ignoreversion
Source: ..\Release\OnSip.tsp; DestDir: {sys}; Flags: ignoreversion
Source: ..\Release\OnSipInstall.dll; DestDir: {app}; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: OnSip Hosted PBX Details.pdf; DestDir: {app}; Flags: ignoreversion

[Icons]
Name: {group}\OnSip Hosted PBX TAPI Driver; Filename: {app}\OnSip Hosted PBX Details.pdf
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}

[Code]

// Methods in the OnSipInstall.dll to handle adding and removing the provider in Windows
function IsProviderInstalled(): Boolean;
external 'IsProviderInstalled@files:OnSipInstall.dll stdcall';

function InstallProvider(): Boolean;
external 'InstallProvider@files:OnSipInstall.dll stdcall';

function UnInstallProvider(): Boolean;
external 'UnInstallProvider@files:OnSipInstall.dll stdcall';

// same code as UnInstallProvider, except need it specified as 'app' and 'uninstallonly'
// so we can use it for uninstall.
function UnInstallProvider2(): Boolean;
external 'UnInstallProvider2@{app}\OnSipInstall.dll stdcall uninstallonly';

//******************************************************************************

// Helper method to set the cursor for the specified control
// and all sub-controls
procedure SetControlCursor(control: TWinControl; cursor: TCursor);
var i:Integer;
	wc: TWinControl;
begin
  if (not (control = nil)) then begin
    control.Cursor := cursor;
    try
      for i:=0 to control.ControlCount-1 do begin
        wc := TWinControl(control.Controls[i]);
        if (NOT(wc = nil)) then
          SetControlCursor(wc, cursor)
        else
          control.Controls[i].Cursor := cursor;
      end; {for}
    finally

    end;{try}
  end;{if}
end;{procedure SetControlCursor}

function NextButtonClick(CurPage: Integer): Boolean;
var
  hWnd: Integer;
  ResultCode: Integer;

begin
  Result := true;

  if CurPage = wpReady then begin

	// If provider is already installed, then must uninstall first
	// so we can overwrite the TSP
	if ( IsProviderInstalled() ) then begin
	    ResultCode := MsgBox('The {#ShortAppName} is currently installed.  It must be removed from TAPI first.' + #10 + #13
			+ 'Press Yes to continue.', mbInformation, MB_YESNO );

		// If user aborted
		if ( ResultCode <> IDYES ) then begin
			Result := false;
			MsgBox('Installation was aborted',mbInformation,MB_OK);

		// else continue with uninstalling the current telephony provider
		end else begin
			SetControlCursor(WizardForm, crHourGlass);
			if ( not UnInstallProvider() ) then begin
				MsgBox('An error occurred in removing the {#ShortAppName}. ' + #10 + #13
					+ 'You may trying uninstalling from the Telephony Control Panel ' + #10 + #13
					+ 'and then run the install again', mbError, MB_OK );
				Result := false;
			end;
			SetControlCursor(WizardForm, crDefault);
	    end;
	end;

  end;

	// If install is complete, then install the new TAPI provider
	if CurPage = wpFinished then begin
		if ( not InstallProvider() ) then begin
			MsgBox('An error occurred installing the {#ShortAppName} into the Telephony Control Panel.' + #10 + #13
				+ 'You may attempt installing it manually.  See the documentation for details.', mbError, MB_OK );
		end;
	end;

end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
	// Call our function just before the actual uninstall process begins
	if CurUninstallStep = usUninstall then begin
		// Remove the TAPI provider
		UnInstallProvider2();

		// Now that we're finished with it, unload MyDll.dll from memory.
		// We have to do this so that the uninstaller will be able to remove the DLL and the {app} directory.
		UnloadDLL(ExpandConstant('{app}\OnSipInstall.dll'));
	end;
end;