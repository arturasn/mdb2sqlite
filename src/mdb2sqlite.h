#ifndef __MDB2SQLITE_H__ 
#define __MDB2SQLITE_H__ 
#pragma once

#ifndef _WX_DIALOG_H_
	#include "wx/dialog.h"
#endif

#ifndef _WX_APP_H_BASE_
	#include "wx/app.h"
#endif

#ifndef __UI_OBS_H__
	#include "UIObs.h"
#endif

class wxTextCtrl;
class wxBoxSizer;
class wxCheckBox;
class wxTextAttr;
class wxGauge;

class CMainDlg : public wxDialog,
				 public CUIObs
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

public:
	virtual void CreateAdditionalItems() override;
	virtual void SetDefaultStyle(const wxTextAttr &atr) override;
	virtual void WriteText(wxString sMsg) override;
	virtual void SetRange(int range) override;
	virtual void SetValue(int value) override;
private:
	void OnSaveWndInfo();

private:
	wxTextCtrl *m_pSrcFilePath;
	wxTextCtrl *m_pDestinationPath;
	wxBoxSizer *m_pTopSizer;

	wxGauge *m_pGauge;
	wxTextCtrl *m_pPrgDlg;
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