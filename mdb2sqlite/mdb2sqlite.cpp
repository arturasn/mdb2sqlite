#define _AFXDLL
#include "stdafx.h"
#include <wx/wx.h>
#include "mdb2sqlite.h"
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/progdlg.h>
#include <wx/textctrl.h>
#include "OnAction.h"
#include <fstream>
#include "SimpleIni.h"

IMPLEMENT_APP(MyApp)
wxBEGIN_EVENT_TABLE ( CustomDialog, wxDialog )
    EVT_BUTTON (Close_button, CustomDialog::OnExit ) 
	EVT_BUTTON(DIALOGS_FILE_OPEN, CustomDialog::FileOpen)
	EVT_BUTTON(Settings_button, CustomDialog::SettingsChoice )
	EVT_BUTTON(Convert_button, CustomDialog::OnConvert )
	EVT_BUTTON(Dump_button, CustomDialog::OnDump )
wxEND_EVENT_TABLE() 


bool MyApp::OnInit()
{
	remove("Settings.ini");
	int posx, posy;
	CustomDialog::GetWindowPosition(posx, posy);
    CustomDialog *custom = new CustomDialog(wxT("mdb2Sqlite"), posx, posy);
    custom->Show(true);
    return true;
}
 
CustomDialog::CustomDialog(const wxString &title, const int x, const int y)
       : wxDialog(NULL, -1, title, wxPoint(x,y), wxSize(270, 235), wxRESIZE_BORDER | wxCAPTION)
{
	wxSizer * const SourceDestination = new wxStaticBoxSizer(wxVERTICAL, this);
	wxSizer * const SourceFile = new wxStaticBoxSizer(wxVERTICAL, this, "&Source File:");
	wxBoxSizer *FilePathLine = new wxBoxSizer(wxHORIZONTAL);
	TFilePathLine = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150,20));
	FilePathLine->Add(TFilePathLine, 1, wxLEFT | wxTOP | wxEXPAND, 4);
	FilePathLine->AddSpacer(15);
	wxButton *browsebutton = new wxButton(this, DIALOGS_FILE_OPEN, "Browse", wxDefaultPosition, wxSize(60, 22)); 
	FilePathLine->Add(browsebutton, 0, wxRIGHT | wxTOP, 3);
	SourceFile->Add(FilePathLine, 1, wxEXPAND, 1);

	SourceDestination ->Add(SourceFile, 1 ,wxEXPAND);

	wxSizer * const DestinationFile = new wxStaticBoxSizer(wxVERTICAL, this, "&Destination:");
	wxBoxSizer *DestinationFilePathLine = new wxBoxSizer(wxHORIZONTAL);
	TDestinationPathLine  = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150,20));
	DestinationFilePathLine->Add(TDestinationPathLine, 1, wxLEFT | wxTOP | wxEXPAND, 4);
	DestinationFile->Add(DestinationFilePathLine, 1, wxEXPAND | wxRIGHT, 78);

	SourceDestination ->Add(DestinationFile, 1 ,wxEXPAND);

	TopSizer = new wxBoxSizer(wxVERTICAL);

	wxButton *settingsbutton = new wxButton(this, Settings_button, "settings", wxDefaultPosition, wxSize(60,25));
	wxBoxSizer *BottomSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *closebutton = new wxButton(this, Close_button, "Close", wxDefaultPosition, wxSize(60,25));
	wxButton *dumpbutton = new wxButton(this, Dump_button, "Dump", wxDefaultPosition, wxSize(60,25));
	BottomSizer->Add(closebutton, 1, wxBOTTOM | wxRIGHT, 5);
	BottomSizer->AddSpacer(5);
	BottomSizer->Add(dumpbutton, 1, wxBOTTOM | wxRIGHT, 5);
	BottomSizer->AddSpacer(5);
 
	wxButton *Convertbutton = new wxButton(this, Convert_button, "Convert", wxDefaultPosition, wxSize(60,25)); 
	BottomSizer->Add(Convertbutton , 1, wxBOTTOM | wxRIGHT, 5);
  
	TopSizer->Add(SourceDestination, 1, wxALL | wxEXPAND, 3);
	TopSizer->AddSpacer(5);
	TopSizer->Add(settingsbutton, 0, wxLEFT, 25);
	TopSizer->AddSpacer(5);
	TopSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
	TopSizer->AddSpacer(5);
	TopSizer->Add(BottomSizer, 0, wxLEFT | wxALIGN_RIGHT, 55);
	SetSizer(TopSizer);
	ShowModal();
	Destroy(); 
}
void CustomDialog::OnExit( wxCommandEvent &WXUNUSED(event) )
  {
    Close(TRUE);
  }
void CustomDialog::OnDump( wxCommandEvent &WXUNUSED(event) )
{
	wxFileDialog dumpdialog(this, wxT("Save file"), wxEmptyString, wxT("dump.txt"), wxT("Text files (*.txt)|*.txt"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dumpdialog.ShowModal() == wxID_OK)
	{
		if( !(TFilePathLine->GetValue().IsEmpty()) )
		{
			char Path[100];
			char dPath[100];
			strcpy(Path, (const char*)TFilePathLine->GetValue().mb_str(wxConvUTF8));
			strcpy(dPath, (const char*)dumpdialog.GetPath().mb_str(wxConvUTF8));
			CSettingsReader::Control(Path, dPath);
			wxMessageDialog dumpdialogsuccess(this, wxT("Succesfully dumped."), wxT("Dump information"));
			dumpdialogsuccess.ShowModal();
			dumpdialogsuccess.Destroy();
		}
		else 
		{
			wxMessageDialog dumpdialogfail(this, wxT("Export file path was not provided."), wxT("Error"));
			dumpdialogfail.ShowModal();
			dumpdialogfail.Destroy();
		}
	}
}
void CustomDialog::OnConvert(wxCommandEvent &WXUNUSED(event) )
{
	if( TFilePathLine->GetValue().IsEmpty() || TDestinationPathLine->GetValue().IsEmpty() )
	{
		wxMessageDialog convertdialog(this, wxT("Destination or Path is missing"), wxT("Error"));
		convertdialog.ShowModal();
		convertdialog.Destroy();
	}
	else 
	{
		int *posx = new int, 
			*posy = new int;
	    GetPosition(posx,posy);
		SaveWindowPosition(posx, posy);
		delete posx;
		delete posy;
        char Path[100];
		char dPath[100];
        strcpy(Path, (const char*)TFilePathLine->GetValue().mb_str(wxConvUTF8));
		strcpy(dPath, (const char*)TDestinationPathLine->GetValue().mb_str(wxConvUTF8));
		if( TDestinationPathLine->GetValue().SubString( TDestinationPathLine->GetValue().length()-7,TDestinationPathLine->GetValue().length() ) == wxT(".sqlite") )
		{
			wxGauge *gauge =  new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(270,15));
			wxTextCtrl *PrgDlg = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250,120), wxTE_MULTILINE | wxTE_RICH);
			TopSizer->Add(PrgDlg, 0, wxEXPAND | wxALL, 5);
			TopSizer->Add(gauge, 0, wxEXPAND | wxALL, 5);
			Layout();
            Fit();
			Refresh();
			Update();
			CSettingsReader::Control(Path, dPath, gauge, PrgDlg);
		    wxMessageDialog convertdialog(this, FileForExport, wxT("Succesfully exported to sqlite"));
		    convertdialog.ShowModal();
			convertdialog.Destroy();
		}
		else
		{
			wxMessageDialog convertdialog(this, wxT("In the destination path you should provide a sqlite file in which to store information"), wxT("Destination path wasn`t correctly provided"));
			convertdialog.ShowModal();
			convertdialog.Destroy();
		}
	}
	
}
void CustomDialog::FileOpen(wxCommandEvent &WXUNUSED(event) )
{
	wxFileDialog dialog(this, wxT("Conversion file selection"), wxEmptyString, wxEmptyString, wxT("mdb files (*.mdb)|*.mdb"));
	dialog.CentreOnParent();
	dialog.SetDirectory(wxGetHomeDir());
	if ( dialog.ShowModal() == wxID_OK )
	{
		wxString info;
		FileForExport = wxT("Succesfully exported file: ");
		FileForExport += wxString::Format(wxT("%s"), dialog.GetFilename().c_str());
		FileForExport += wxT(" to sqlite");
		TFilePathLine->AppendText(dialog.GetPath().c_str());
		if( TDestinationPathLine->GetValue().IsEmpty() )
		{
			TDestinationPathLine->AppendText(dialog.GetDirectory().c_str());
			TDestinationPathLine->AppendText(wxT("\\"));
			TDestinationPathLine->AppendText(dialog.GetFilename().c_str());
			TDestinationPathLine->Replace(TDestinationPathLine->GetValue().length()-4,TDestinationPathLine->GetValue().length(),wxT(".sqlite"));
		}
		info.Printf( wxT("You have selected: %s\n"), dialog.GetFilename().c_str());
        wxMessageDialog dialog2(this, info, wxT("Selected file"));
		dialog2.ShowModal();
	}
	else
	{
		wxMessageDialog dialog2(this, "File not selected", wxT("File not selected"));
		dialog2.ShowModal();
	}

}
void CustomDialog::SettingsChoice(wxCommandEvent& WXUNUSED(event) )
{
    wxDialog *dlg = new wxDialog(this, wxID_ANY, wxT("Values to export"), wxDefaultPosition, wxSize(220,315));

	wxCheckBox *RelationshipCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Relationship add"), wxDefaultPosition);
	wxCheckBox *RecordCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Record add"), wxDefaultPosition);
	wxCheckBox *NotNullValueCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Not Null value add"), wxDefaultPosition);
	wxCheckBox *AutoincrementCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("AutoIncrement value add"), wxDefaultPosition);
	wxCheckBox *DefaultValueCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Default value add"), wxDefaultPosition);
	wxCheckBox *IndexCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Index add"), wxDefaultPosition);
	wxCheckBox *UniqueFieldsCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Unique fields add"), wxDefaultPosition);
	wxCheckBox *CollateNcIndexCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Collate nocase for index add"), wxDefaultPosition);
	wxCheckBox *CollateNcFieldsCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Collatee nocase for fields add"), wxDefaultPosition);
	wxCheckBox *TrimCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Trim text values"), wxDefaultPosition);
	wxCheckBox *DescriptionCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Add field comments"), wxDefaultPosition);
	wxCheckBox *ReservedKeywordCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Reserved keyword list add"), wxDefaultPosition);

	wxSizer *settingsizer = new wxBoxSizer(wxVERTICAL);

	settingsizer->AddSpacer(5);
	settingsizer->Add(RelationshipCheckbox, 0, wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(RecordCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(NotNullValueCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(AutoincrementCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(DefaultValueCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(IndexCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(UniqueFieldsCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(CollateNcIndexCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(CollateNcFieldsCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(TrimCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(DescriptionCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(ReservedKeywordCheckbox, 0,  wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(8);

	RelationshipCheckbox->SetToolTip(wxT("Adds relationships between tables, update and delete sqlite statements are executed."));
	RecordCheckbox->SetToolTip(wxT("Adds the information that is in the records."));
	NotNullValueCheckbox->SetToolTip(wxT("Adds a constraint Not Null to a field if it cannot be Null."));
	AutoincrementCheckbox->SetToolTip(wxT("Adds a constraint PRIMARY KEY AUTOINCREMENT to a field, then when a new record is created the autoincrement field will automatically update."));
	DefaultValueCheckbox->SetToolTip(wxT("Adds a constraint Default value to a field."));
	IndexCheckbox->SetToolTip(wxT("Adds indexes."));
	UniqueFieldsCheckbox->SetToolTip(wxT("Adds a constraint UNIQUE to a field."));
	CollateNcFieldsCheckbox->SetToolTip(wxT("Adds a constraint COLLATE NO CASE to a field, when added the field is case insensitive."));
	CollateNcIndexCheckbox->SetToolTip(wxT("Adds a constraint COLLATE NO CASE to a index, when added  the index is case insensitive."));
	TrimCheckbox->SetToolTip(wxT("Trims the front and back of a field and index if there are some blank spaces."));
	DescriptionCheckbox->SetToolTip(wxT("Adds comments about fields."));
	ReservedKeywordCheckbox->SetToolTip(wxT("If we find some sqlite keywords in tables, indexes or fields error message is shown."));

	settingsizer->Add(new wxStaticLine(dlg), 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(new wxButton(dlg, wxID_OK, "OK", wxDefaultPosition, wxSize(70,25)), 0, wxLEFT, 5);
	hbox->Add(new wxButton(dlg, wxID_CANCEL, "Cancel", wxDefaultPosition, wxSize(70,25)), 0, wxLEFT, 5);
	settingsizer->Add(hbox, 0, wxLEFT | wxTOP, 5);
	dlg->SetSizer(settingsizer);
	if( dlg->ShowModal() == wxID_OK )
	{
		SaveToIni(RelationshipCheckbox,RecordCheckbox, NotNullValueCheckbox, AutoincrementCheckbox, DefaultValueCheckbox, IndexCheckbox, UniqueFieldsCheckbox, CollateNcFieldsCheckbox, 
			CollateNcIndexCheckbox,TrimCheckbox,DescriptionCheckbox,ReservedKeywordCheckbox);
	}
	dlg->Destroy();
}
void CustomDialog::SaveToIni(wxCheckBox *&Rlc, wxCheckBox *&Rcc, wxCheckBox *&NnVc, wxCheckBox *&Aic, wxCheckBox *&Dvc, wxCheckBox *&indc, wxCheckBox *&Ufc, wxCheckBox *&CNcFc, wxCheckBox *&CNcIc,
	wxCheckBox *&Tc, wxCheckBox *&Dscrc, wxCheckBox *&RKc)
{
	std::ofstream settingfile("Settings.ini");
	settingfile << "[Settings]" << std::endl;
	settingfile << "RelationshipAdd = "; 
	                             if( Rlc->IsChecked() )  
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "RecordAdd = "; 
	                             if( Rcc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "NotNullAdd = "; 
	                             if( NnVc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "AutoIncrementAdd = "; 
	                             if( Aic->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "DefaultValueAdd = "; 
	                             if( Dvc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "IndexAdd = "; 
	                             if( indc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "UniqueFieldAdd = "; 
	                             if( Ufc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "CollateNoCaseForIndex = "; 
	                             if( CNcFc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "CollateNoCaseForFields = "; 
	                             if( CNcIc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "TrimTextValues = "; 
	                             if( Tc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "AddComments = "; 
	                             if( Dscrc->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "KeyWordList = ";
	                             if( RKc->IsChecked() )
									 settingfile << "true" << std::endl;
								 else settingfile << "false" << std::endl;
	settingfile.close();
}
void CustomDialog::SaveWindowPosition(int *& posx, int *& posy)
{
	std::ofstream WindowPositionFile("WindowPosition.ini");
	WindowPositionFile << "[WindowPosition]" << std::endl;
	WindowPositionFile << "x = " << *posx << std::endl;
	WindowPositionFile << "y = " << *posy << std::endl;
	WindowPositionFile.close();
}
void CustomDialog::GetWindowPosition(int &posx, int &posy)
{
	CSimpleIni ini;
    ini.SetUnicode();
    ini.LoadFile("WindowPosition.ini");
	posx = ini.GetLongValue(_T("WindowPosition"),_T("x"),550);
	posy = ini.GetLongValue(_T("WindowPosition"),_T("y"),200);
}
