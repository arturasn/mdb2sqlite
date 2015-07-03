#ifndef __MDB2SQLITE_H__ 
#define __MDB2SQLITE_H__ 
#pragma once

class CustomDialog : public wxDialog
{
public:
	 CustomDialog(const wxString& title);
	 wxTextCtrl *TFilePathLine;
	 wxTextCtrl *TDestinationPathLine;
	 wxString FileForExport;
	 wxCheckListBox *Selections;
	 wxBoxSizer *vbox;
	 void OnExit( wxCommandEvent& event );
	 void OnConvert(wxCommandEvent& event );
	 void FileOpen(wxCommandEvent &WXUNUSED(event) );
	 void SettingsChoice(wxCommandEvent& WXUNUSED(event) );
	 void ProgressBar(char *Path, char *dPath);
	 wxDECLARE_EVENT_TABLE();
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
	Convert_button
};

DECLARE_APP(MyApp)
 
#endif