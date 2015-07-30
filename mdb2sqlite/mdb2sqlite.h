#ifndef __MDB2SQLITE_H__ 
#define __MDB2SQLITE_H__ 
#pragma once

class CustomDialog : public wxDialog
{
public:
	 CustomDialog(const wxString& title, const int x, const int y);
	 wxTextCtrl *TFilePathLine;
	 wxTextCtrl *TDestinationPathLine;
	 wxString FileForExport;
	 wxBoxSizer *TopSizer;
	 void ProgressBar(char *Path, char *dPath);
	 void CustomDialog::SaveToIni(wxCheckBox *&RelationshipCheckbox, wxCheckBox *&RecordCheckbox, wxCheckBox *&NotNullValueCheckbox, wxCheckBox *&AutoincrementCheckbox, wxCheckBox *&DefaultValueCheckbox,
	                         wxCheckBox *&IndexCheckbox, wxCheckBox *&UniqueFieldsCheckbox, wxCheckBox *&CollateNcIndexCheckbox, wxCheckBox *&CollateNcFieldsCheckbox, wxCheckBox *&TrimCheckbox,
							 wxCheckBox *&DescriptionCheckbox, wxCheckBox *&ReservedKeywordCheckbox);
	 void ReadFromIni(wxCheckBox *&RelationshipCheckbox, wxCheckBox *&RecordCheckbox, wxCheckBox *&NotNullValueCheckbox, wxCheckBox *&AutoincrementCheckbox, wxCheckBox *&DefaultValueCheckbox,
	                         wxCheckBox *&IndexCheckbox, wxCheckBox *&UniqueFieldsCheckbox, wxCheckBox *&CollateNcIndexCheckbox, wxCheckBox *&CollateNcFieldsCheckbox, wxCheckBox *&TrimCheckbox,
							 wxCheckBox *&DescriptionCheckbox, wxCheckBox *&ReservedKeywordCheckbox);
	 void SaveWindowPosition(int *& posx, int *& posy);
	 static void GetWindowPosition(int &posx, int &posy);
private:
	 void OnExit( wxCommandEvent &WXUNUSED(event) );
	 void OnConvert(wxCommandEvent &WXUNUSED(event) );
	 void FileOpen(wxCommandEvent &WXUNUSED(event) );
	 void SettingsChoice(wxCommandEvent& WXUNUSED(event) );
	 void OnDump(wxCommandEvent& WXUNUSED(event) );
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
	Convert_button,
	Dump_button
};

DECLARE_APP(MyApp)
 
#endif