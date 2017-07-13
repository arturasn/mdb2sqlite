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

class CustomDialog : public wxDialog
{
public:
	 CustomDialog(const wxString sTitle, const int x, const int y, const int sizex);
	 static void SaveToIni(wxCheckBox *&RelationshipCheckbox, wxCheckBox *&RecordCheckbox, wxCheckBox *&NotNullValueCheckbox, wxCheckBox *&AutoincrementCheckbox, wxCheckBox *&DefaultValueCheckbox,
	                         wxCheckBox *&IndexCheckbox, wxCheckBox *&UniqueFieldsCheckbox, wxCheckBox *&CollateNcIndexCheckbox, wxCheckBox *&CollateNcFieldsCheckbox, wxCheckBox *&TrimCheckbox,
							 wxCheckBox *&DescriptionCheckbox, wxCheckBox *&ReservedKeywordCheckbox, wxCheckBox *&ForeignKeySupportCheckbox, wxCheckBox *&PrimaryKeyCheckbox, wxCheckBox *&ForeignKeyPrimaryCheckbox);
	 static void ReadFromIni(wxCheckBox *&RelationshipCheckbox, wxCheckBox *&RecordCheckbox, wxCheckBox *&NotNullValueCheckbox, wxCheckBox *&AutoincrementCheckbox, wxCheckBox *&DefaultValueCheckbox,
	                         wxCheckBox *&IndexCheckbox, wxCheckBox *&UniqueFieldsCheckbox, wxCheckBox *&CollateNcIndexCheckbox, wxCheckBox *&CollateNcFieldsCheckbox, wxCheckBox *&TrimCheckbox,
							 wxCheckBox *&DescriptionCheckbox, wxCheckBox *&ReservedKeywordCheckbox, wxCheckBox *&ForeignKeySupportCheckbox, wxCheckBox *&PrimaryKeyCheckbox, wxCheckBox *&ForeignKeyPrimaryCheckbox);
//Events
private:
	 void OnClose( wxCloseEvent &WXUNUSED(event) );
	 void OnExit( wxCommandEvent &WXUNUSED(event) );
	 void OnConvert(wxCommandEvent &WXUNUSED(event) );
	 void FileOpen(wxCommandEvent &WXUNUSED(event) );
	 void SettingsChoice(wxCommandEvent& WXUNUSED(event) );
	 void OnDump(wxCommandEvent& WXUNUSED(event) );
	 wxDECLARE_EVENT_TABLE();

//Operations
private:
	void OnSaveWndInfo();

//Attributes
private:
	wxTextCtrl *m_pSrcFilePath;
	wxTextCtrl *m_pDestinationPath;
	wxBoxSizer *m_pTopSizer;

	wxString m_FileForExport;
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