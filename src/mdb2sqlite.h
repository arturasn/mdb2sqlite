#ifndef __MDB2SQLITE_H__ 
#define __MDB2SQLITE_H__ 
#pragma once

#ifndef _WX_DIALOG_H_
	#include "wx/dialog.h"
#endif

#ifndef _WX_APP_H_BASE_
	#include "wx/app.h"
#endif

class wxTextCtrl;
class wxBoxSizer;
class wxCheckBox;

class CMainDlg : public wxDialog
{
public:
	 CMainDlg(const wxString sTitle, const int nX, const int nY, const int nSizeX);
	 virtual ~CMainDlg();

private:
	 void OnClose(wxCloseEvent &WXUNUSED(event));
	 void OnExit(wxCommandEvent &WXUNUSED(event));
	 void OnConvert(wxCommandEvent &WXUNUSED(event));
	 void FileOpen(wxCommandEvent &WXUNUSED(event));
	 void SettingsChoice(wxCommandEvent& WXUNUSED(event));
	 void OnDump(wxCommandEvent& WXUNUSED(event));
	 wxDECLARE_EVENT_TABLE();

private:
	void OnSaveWndInfo();

private:
	wxTextCtrl *m_pSrcFilePath;
	wxTextCtrl *m_pDestinationPath;
	wxBoxSizer *m_pTopSizer;
};

class MyApp : public wxApp
{
 public:
    virtual bool OnInit();
};

enum
{
	Close_button = wxID_HIGHEST + 1, 
	DIALOGS_FILE_OPEN,
	Settings_button,
	Convert_button,
	Dump_button
};

DECLARE_APP(MyApp)
 
#endif