#define _AFXDLL
#include "stdafx.h"
#include <wx/wx.h>
#include <wx/statline.h>
#include "OnAction.h"
#include <fstream>
#include "SimpleIni.h"
#include "mdb2sqlite.h"

IMPLEMENT_APP(MyApp)
wxBEGIN_EVENT_TABLE ( CustomDialog, wxDialog )
    EVT_BUTTON (Close_button, CustomDialog::OnExit ) 
	EVT_BUTTON(DIALOGS_FILE_OPEN, CustomDialog::FileOpen)
	EVT_BUTTON(Settings_button, CustomDialog::SettingsChoice )
	EVT_BUTTON(Convert_button, CustomDialog::OnConvert )
	EVT_BUTTON(Dump_button, CustomDialog::OnDump )
	EVT_CLOSE(CustomDialog::OnClose )
wxEND_EVENT_TABLE() 



bool MyApp::OnInit()
{
	int posx, 
		posy, 
		sizex;
	CustomDialog::GetWindowInformation(posx, posy, sizex);
    CustomDialog *custom = new CustomDialog(wxT("mdb2Sqlite"), posx, posy, sizex);
    custom->Show(true);
    return true;
}
 
CustomDialog::CustomDialog(const wxString &title, const int x, const int y, const int sizex)
       : wxDialog(NULL, -1, title, wxPoint(x,y), wxSize(sizex, 235), wxRESIZE_BORDER | wxCAPTION | wxCLOSE_BOX )
{
	wxIcon icon;
	icon.LoadFile("sqliteicon.ico", wxBITMAP_TYPE_ICO);
	SetIcon(icon);
	wxSizer * const SourceDestination = new wxStaticBoxSizer(wxVERTICAL, this);
	wxSizer * const SourceFile = new wxStaticBoxSizer(wxVERTICAL, this, "&Source File:");
	wxBoxSizer *FilePathLine = new wxBoxSizer(wxHORIZONTAL);
	TFilePathLine = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150,20));
	FilePathLine->Add(TFilePathLine, 1, wxLEFT | wxTOP | wxEXPAND, 4);
	FilePathLine->AddSpacer(15);
	wxButton *browsebutton = new wxButton(this, DIALOGS_FILE_OPEN, "Browse", wxDefaultPosition, wxSize(60, 22)); 
	FilePathLine->Add(browsebutton, 0, wxRIGHT | wxTOP, 3);
	SourceFile->Add(FilePathLine, 0, wxEXPAND, 1);

	SourceDestination ->Add(SourceFile, 0 ,wxEXPAND);

	wxSizer * const DestinationFile = new wxStaticBoxSizer(wxVERTICAL, this, "&Destination:");
	wxBoxSizer *DestinationFilePathLine = new wxBoxSizer(wxHORIZONTAL);
	TDestinationPathLine  = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150,20));
	DestinationFilePathLine->Add(TDestinationPathLine, 1, wxLEFT | wxTOP | wxEXPAND, 4);
	DestinationFile->Add(DestinationFilePathLine, 0, wxEXPAND | wxRIGHT, 78);

	SourceDestination ->Add(DestinationFile, 0 ,wxEXPAND);

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
  
	TopSizer->Add(SourceDestination, 0, wxALL | wxEXPAND, 3);
	TopSizer->AddSpacer(5);
	TopSizer->Add(settingsbutton, 0, wxLEFT, 15);
	TopSizer->AddSpacer(5);
	TopSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
	TopSizer->AddSpacer(5);
	TopSizer->Add(BottomSizer, 0, wxLEFT | wxALIGN_RIGHT, 55);
	int *width = new int,
		*height = new int;
	GetSize(width,height);
	SetMinSize(wxSize(wxDefaultCoord, *height));
	SetMaxSize(wxSize(wxDefaultCoord, GetMinSize().y));
	SetSizer(TopSizer);
	ShowModal();
	delete width;
	delete height;
	delete closebutton;
	delete settingsbutton;
	delete dumpbutton;
	delete Convertbutton;
	Destroy(); 
}
void CustomDialog::OnExit( wxCommandEvent &WXUNUSED(event) )
  {
	int *posx = new int, 
		*posy = new int,
	    *sizex = new int,
		*sizey = new int;
	 GetPosition(posx,posy);
	 GetSize(sizex, sizey);
	 SaveWindowInformation(posx, posy, sizex);
	 delete posx;
	 delete posy;
	 delete sizex;
	 delete sizey;
	EndModal( wxID_CANCEL );
    Close(TRUE);
  }
void CustomDialog::OnClose( wxCloseEvent &WXUNUSED(event) )
{
	int *posx = new int, 
		*posy = new int,
		*sizex = new int,
	    *sizey = new int;
	 GetPosition(posx,posy);
	 GetSize(sizex, sizey);
	 SaveWindowInformation(posx, posy, sizex);
	 delete posx;
	 delete posy;
	 delete sizex;
	 delete sizey;
	 EndModal( wxID_CANCEL );
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
		GetSize(posx,posy);
		SetMaxSize(wxSize(*posx,wxDefaultCoord));
		char Path[100] = {0};
		char dPath[100] = {0};
        strcpy(Path, (const char*)TFilePathLine->GetValue().mb_str(wxConvUTF8));
		strcpy(dPath, (const char*)TDestinationPathLine->GetValue().mb_str(wxConvUTF8));
		if( TDestinationPathLine->GetValue().SubString( TDestinationPathLine->GetValue().length()-7,TDestinationPathLine->GetValue().length() ) == wxT(".sqlite") )
		{
			wxGauge *gauge =  new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(*posx,15));
			wxTextCtrl *PrgDlg = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250,120), wxTE_MULTILINE | wxTE_RICH);
			TopSizer->Add(PrgDlg, 1, wxEXPAND | wxALL, 5);
			TopSizer->Add(gauge, 0, wxEXPAND | wxALL, 5);
			Layout();
            Fit();
			Refresh();
			Update();
			SetMaxSize(wxSize(wxDefaultCoord,wxDefaultCoord));
            SetMinSize(wxSize(wxDefaultCoord,wxDefaultCoord));
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
		delete posx;
		delete posy;
	}
	
}
void CustomDialog::FileOpen(wxCommandEvent &WXUNUSED(event) )
{
	wxFileDialog dialog(this, wxT("Conversion file selection"), wxEmptyString, wxEmptyString, wxT("mdb files (*.mdb)|*.mdb"));
	dialog.CentreOnParent();
	dialog.SetDirectory(wxGetHomeDir());
	if ( dialog.ShowModal() == wxID_OK )
	{
		FileForExport = wxT("Succesfully exported file: ");
		FileForExport += wxString::Format(wxT("%s"), dialog.GetFilename().c_str());
		FileForExport += wxT(" to sqlite");
		TFilePathLine->WriteText(dialog.GetPath().c_str());
		if( TDestinationPathLine->GetValue().IsEmpty() )
		{
			TDestinationPathLine->WriteText(dialog.GetDirectory().c_str());
			TDestinationPathLine->AppendText(wxT("\\"));
			TDestinationPathLine->AppendText(dialog.GetFilename().c_str());
			TDestinationPathLine->Replace(TDestinationPathLine->GetValue().length()-4,TDestinationPathLine->GetValue().length(),wxT(".sqlite"));
		}
	}
	else
	{
		wxMessageDialog dialog2(this, "File not selected", wxT("File not selected"));
		dialog2.ShowModal();
	}
	dialog.Destroy();
}
void CustomDialog::SettingsChoice(wxCommandEvent& WXUNUSED(event) )
{
    wxDialog *dlg = new wxDialog(this, wxID_ANY, wxT("Values to export"), wxDefaultPosition, wxSize(220,370));

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
	wxCheckBox *ForeignKeySupportCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Foreign key support"), wxDefaultPosition);
	wxCheckBox *PrimaryKeyCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Primary key support"), wxDefaultPosition);
	wxCheckBox *ForeignKeyPrimaryCheckbox = new wxCheckBox(dlg, wxID_ANY, wxT("Foreign key both primary fields"), wxDefaultPosition);

	ReadFromIni(RelationshipCheckbox, RecordCheckbox, NotNullValueCheckbox, AutoincrementCheckbox, DefaultValueCheckbox, IndexCheckbox, UniqueFieldsCheckbox, 
			    CollateNcIndexCheckbox, CollateNcFieldsCheckbox, TrimCheckbox, DescriptionCheckbox, ReservedKeywordCheckbox, ForeignKeySupportCheckbox, PrimaryKeyCheckbox, ForeignKeyPrimaryCheckbox);

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
	settingsizer->AddSpacer(5);
	settingsizer->Add(ForeignKeySupportCheckbox, 0, wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(PrimaryKeyCheckbox, 0, wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);
	settingsizer->Add(ForeignKeyPrimaryCheckbox, 0, wxLEFT | wxRIGHT, 8);
	settingsizer->AddSpacer(5);

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
	ForeignKeySupportCheckbox->SetToolTip(wxT("Adds foreign key support, NOTE:foreign keys only work when there are no loops between table references"));
	PrimaryKeyCheckbox->SetToolTip(wxT("Adds a contraint PRIMARY KEY to a field, all of this fields values ar unique"));
	ForeignKeyPrimaryCheckbox->SetToolTip(wxT("Do not create a relationship enforced by a foreign key where both fields are primary key (where the relationship is 1-1)") );

	settingsizer->Add(new wxStaticLine(dlg), 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(new wxButton(dlg, wxID_OK, "OK", wxDefaultPosition, wxSize(70,25)), 0);
	hbox->Add(new wxButton(dlg, wxID_CANCEL, "Cancel", wxDefaultPosition, wxSize(70,25)), 0, wxLEFT, 5);
	settingsizer->AddSpacer(5);
	settingsizer->Add(hbox, 0, wxLEFT | wxRIGHT, 37);
	dlg->SetSizer(settingsizer);
	if( dlg->ShowModal() == wxID_OK )
	{
		SaveToIni(RelationshipCheckbox, RecordCheckbox, NotNullValueCheckbox, AutoincrementCheckbox, DefaultValueCheckbox, IndexCheckbox, UniqueFieldsCheckbox, 
			CollateNcIndexCheckbox, CollateNcFieldsCheckbox, TrimCheckbox, DescriptionCheckbox, ReservedKeywordCheckbox, ForeignKeySupportCheckbox, PrimaryKeyCheckbox, ForeignKeyPrimaryCheckbox);
	}
	dlg->Destroy();
}
void CustomDialog::SaveToIni(wxCheckBox *&RelationshipCheckbox, wxCheckBox *&RecordCheckbox, wxCheckBox *&NotNullValueCheckbox, wxCheckBox *&AutoincrementCheckbox, wxCheckBox *&DefaultValueCheckbox,
	                         wxCheckBox *&IndexCheckbox, wxCheckBox *&UniqueFieldsCheckbox, wxCheckBox *&CollateNcIndexCheckbox, wxCheckBox *&CollateNcFieldsCheckbox, wxCheckBox *&TrimCheckbox,
							 wxCheckBox *&DescriptionCheckbox, wxCheckBox *&ReservedKeywordCheckbox, wxCheckBox *&ForeignKeySupportCheckbox, wxCheckBox *&PrimaryKeyCheckbox, wxCheckBox *&ForeignKeyPrimaryCheckbox)
{
	std::ofstream settingfile("Settings.ini");
	settingfile << "[Settings]" << std::endl;
	settingfile << "RelationshipAdd = "; 
	                             if( RelationshipCheckbox->IsChecked() )  
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "RecordAdd = "; 
	                             if( RecordCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "NotNullAdd = "; 
	                             if( NotNullValueCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "AutoIncrementAdd = "; 
	                             if( AutoincrementCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "DefaultValueAdd = "; 
	                             if( DefaultValueCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "IndexAdd = "; 
	                             if( IndexCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "UniqueFieldAdd = "; 
	                             if( UniqueFieldsCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "CollateNoCaseForIndex = "; 
	                             if( CollateNcIndexCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "CollateNoCaseForFields = "; 
	                             if( CollateNcFieldsCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "TrimTextValues = "; 
	                             if( TrimCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "AddComments = "; 
	                             if( DescriptionCheckbox->IsChecked() ) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "KeyWordList = ";
	                             if( ReservedKeywordCheckbox->IsChecked() )
									 settingfile << "true" << std::endl;
								 else settingfile << "false" << std::endl;
    settingfile << "ForeignKeySupport = ";
	                             if( ForeignKeySupportCheckbox->IsChecked() )
									 settingfile << "true" << std::endl;
								 else settingfile << "false" << std::endl;
	settingfile << "PrimaryKeySupport = ";
								if( PrimaryKeyCheckbox->IsChecked() )
									settingfile << "true" << std::endl;
								else settingfile << "false" << std::endl;
	settingfile << "ForeignKeyPrimary = ";
	                            if( ForeignKeyPrimaryCheckbox -> IsChecked() )
									settingfile << "true" << std::endl;
								else settingfile << "false" << std::endl;
	settingfile.close();

	delete RelationshipCheckbox;
	delete RecordCheckbox; 
	delete NotNullValueCheckbox; 
	delete AutoincrementCheckbox;
	delete DefaultValueCheckbox; 
	delete IndexCheckbox; 
	delete UniqueFieldsCheckbox; 
    delete CollateNcIndexCheckbox;
	delete CollateNcFieldsCheckbox; 
	delete TrimCheckbox; 
	delete DescriptionCheckbox;
	delete ReservedKeywordCheckbox;
	delete ForeignKeySupportCheckbox;
	delete PrimaryKeyCheckbox;
	delete ForeignKeyPrimaryCheckbox;
}
void CustomDialog::ReadFromIni(wxCheckBox *&RelationshipCheckbox, wxCheckBox *&RecordCheckbox, wxCheckBox *&NotNullValueCheckbox, wxCheckBox *&AutoincrementCheckbox, wxCheckBox *&DefaultValueCheckbox,
	                         wxCheckBox *&IndexCheckbox, wxCheckBox *&UniqueFieldsCheckbox, wxCheckBox *&CollateNcIndexCheckbox, wxCheckBox *&CollateNcFieldsCheckbox, wxCheckBox *&TrimCheckbox,
							 wxCheckBox *&DescriptionCheckbox, wxCheckBox *&ReservedKeywordCheckbox, wxCheckBox *&ForeignKeySupportCheckbox, wxCheckBox *&PrimaryKeyCheckbox, wxCheckBox *&ForeignKeyPrimaryCheckbox)
{
	CSimpleIni ini;
    ini.SetUnicode();
	ini.LoadFile("Settings.ini");
	RelationshipCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("RelationshipAdd"),true));
	RecordCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("RecordAdd"),true));
	NotNullValueCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("NotNullAdd"),true));
	AutoincrementCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("AutoIncrementAdd"),true));
	DefaultValueCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("DefaultValueAdd"),true));
	IndexCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("IndexAdd"),true));
	UniqueFieldsCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("UniqueFieldAdd"),true));
	CollateNcIndexCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForIndex"),true));
	CollateNcFieldsCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForFields"),true));
	TrimCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("TrimTextValues"),true));
	DescriptionCheckbox-> SetValue(ini.GetBoolValue(_T("Settings"),_T("AddComments"),true));
	ReservedKeywordCheckbox-> SetValue(ini.GetBoolValue(_T("Settings"),_T("KeyWordList"),true));
	ForeignKeySupportCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("ForeignKeySupport"),true));
	PrimaryKeyCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("PrimaryKeySupport"),true));
	ForeignKeyPrimaryCheckbox->SetValue(ini.GetBoolValue(_T("Settings"),_T("ForeignKeyPrimary"),true));
}
void CustomDialog::SaveWindowInformation(int *& posx, int *& posy, int *& sizex)
{
	std::ofstream WindowPositionFile("WindowInformation.ini");
	WindowPositionFile << "[WindowPosition]" << std::endl;
	WindowPositionFile << "x = " << *posx << std::endl;
	WindowPositionFile << "y = " << *posy << std::endl;
	WindowPositionFile << "[WindowSize]" << std::endl;
	WindowPositionFile << "width = " << *sizex << std::endl;
	WindowPositionFile.close();
}
void CustomDialog::GetWindowInformation(int &posx, int &posy, int &sizex)
{
	CSimpleIni ini;
    ini.SetUnicode();
    ini.LoadFile("WindowInformation.ini");
	posx = ini.GetLongValue(_T("WindowPosition"),_T("x"),550);
	posy = ini.GetLongValue(_T("WindowPosition"),_T("y"),200);
	sizex = ini.GetLongValue(_T("WindowSize"),_T("width"),270);
}
