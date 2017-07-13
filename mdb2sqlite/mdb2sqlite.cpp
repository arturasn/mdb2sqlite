#define _AFXDLL
#include "stdafx.h"
#include "FileUtils.h"
#include "mdb2sqlite.h"
#include <wx/wx.h>
#include <wx/statline.h>
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
	EVT_CLOSE(CustomDialog::OnClose )
wxEND_EVENT_TABLE() 

bool MyApp::OnInit()
{
	int nPosX, nPosY, nSizeX;
	file_utils::GetWindowInformation(nPosX, nPosY, nSizeX);

    CustomDialog *custom = new CustomDialog(wxT("mdb2SQLite"), nPosX, nPosY, nSizeX);
    custom->Show();
    return true;
}

namespace internal
{
	static void ShowMessageDlg(wxWindow *pParent, wxString sMsg, wxString sTitle)
	{
		wxMessageDialog dlg(pParent, sMsg, sTitle);
		dlg.ShowModal();
	}
};

CustomDialog::CustomDialog(const wxString sTitle, const int x, const int y, const int sizex)
       : wxDialog(nullptr, wxID_ANY, sTitle, wxPoint(x, y), wxSize(sizex, 235), wxRESIZE_BORDER|wxCAPTION|wxCLOSE_BOX )
{
	wxIcon icon;
	icon.LoadFile("sqliteicon.ico", wxBITMAP_TYPE_ICO);
	SetIcon(icon);
	wxSizer * const SourceDestination = new wxStaticBoxSizer(wxVERTICAL, this);
	wxSizer * const SourceFile = new wxStaticBoxSizer(wxVERTICAL, this, "&Source File:");
	wxBoxSizer *FilePathLine = new wxBoxSizer(wxHORIZONTAL);
	m_pSrcFilePath = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, 20));
	FilePathLine->Add(m_pSrcFilePath, 1, wxLEFT|wxTOP|wxEXPAND, 4);
	FilePathLine->AddSpacer(15);
	wxButton *browsebutton = new wxButton(this, DIALOGS_FILE_OPEN, "Browse", wxDefaultPosition, wxSize(60, 22)); 
	FilePathLine->Add(browsebutton, 0, wxRIGHT | wxTOP, 3);
	SourceFile->Add(FilePathLine, 0, wxEXPAND, 1);

	SourceDestination ->Add(SourceFile, 0 ,wxEXPAND);

	wxSizer * const DestinationFile = new wxStaticBoxSizer(wxVERTICAL, this, "&Destination:");
	wxBoxSizer *DestinationFilePathLine = new wxBoxSizer(wxHORIZONTAL);
	m_pDestinationPath  = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150,20));
	DestinationFilePathLine->Add(m_pDestinationPath, 1, wxLEFT | wxTOP | wxEXPAND, 4);
	DestinationFile->Add(DestinationFilePathLine, 0, wxEXPAND | wxRIGHT, 78);

	SourceDestination ->Add(DestinationFile, 0 ,wxEXPAND);

	m_pTopSizer = new wxBoxSizer(wxVERTICAL);

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
  
	m_pTopSizer->Add(SourceDestination, 0, wxALL | wxEXPAND, 3);
	m_pTopSizer->AddSpacer(5);
	m_pTopSizer->Add(settingsbutton, 0, wxLEFT, 15);
	m_pTopSizer->AddSpacer(5);
	m_pTopSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
	m_pTopSizer->AddSpacer(5);
	m_pTopSizer->Add(BottomSizer, 0, wxLEFT|wxALIGN_RIGHT, 55);

	int nWidth, nHeight;
	GetSize(&nWidth, &nHeight);
	SetMinSize(wxSize(wxDefaultCoord, nHeight));
	SetMaxSize(wxSize(wxDefaultCoord, GetMinSize().y));

	SetSizer(m_pTopSizer);
	ShowModal();

	Destroy(); 
}

void CustomDialog::OnExit( wxCommandEvent &WXUNUSED(event) )
{
	OnSaveWndInfo();
    Close(TRUE);
}

void CustomDialog::OnSaveWndInfo()
{
	int nPosX, nPosY;
	int nSizeX, nSizeY;

	GetPosition(&nPosX, &nPosY);
	GetSize(&nSizeX, &nSizeY);

	file_utils::SaveWindowInformation(nPosX, nPosY, nSizeX);

	EndModal(wxID_CANCEL);
}

void CustomDialog::OnClose( wxCloseEvent &WXUNUSED(event) )
{
	OnSaveWndInfo();
	EndModal(wxID_CANCEL);
}

void CustomDialog::OnDump( wxCommandEvent &WXUNUSED(event) )
{
	wxFileDialog dlg(this, wxT("Save file"), wxEmptyString, wxT("dump.txt"), wxT("Text files (*.txt)|*.txt"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sSrcPath = m_pSrcFilePath->GetValue();
		wxString sDstPath = dlg.GetPath();

		if ( sSrcPath.IsEmpty() ) {
			internal::ShowMessageDlg(this, wxT("Export file path was not provided."), wxT("Error"));
		}
		else
		{
			const char *pSrcPath = sSrcPath.mb_str();
			const char *pDstPath = sDstPath.mb_str();

			CSettingsReader::Control(pSrcPath, pDstPath);
			internal::ShowMessageDlg(this, wxT("Succesfully dumped."), wxT("Dump information"));
		}

	}
}

void CustomDialog::OnConvert(wxCommandEvent &WXUNUSED(event) )
{
	wxString sPathMDB		= m_pSrcFilePath->GetValue();
	wxString sPathSQLite	= m_pDestinationPath->GetValue();
	int nSize				= sPathSQLite.length();

	if( sPathMDB.IsEmpty() || sPathSQLite.IsEmpty() ) {
		internal::ShowMessageDlg(this, wxT("Destination or Source Path is missing"), wxT("Error"));
	}
	else 
	{
		int nPosX;
		int nPosY;
		GetSize(&nPosX, &nPosY);
		SetMaxSize(wxSize(nPosX, wxDefaultCoord));

		const char *pSrcPath		= sPathMDB.mb_str();
		const char *pDPath			= sPathSQLite.mb_str();
		if( sPathSQLite.SubString(nSize - 7, nSize) == wxT(".sqlite") )
		{
			wxGauge *pGauge		=  new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(nPosX, 15));
			wxTextCtrl *pPrgDlg	= new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 120), wxTE_MULTILINE|wxTE_RICH);
			m_pTopSizer->Add(pPrgDlg, 1, wxEXPAND|wxALL, 5);
			m_pTopSizer->Add(pGauge, 0, wxEXPAND|wxALL, 5);
			Layout();
            Fit();
			Refresh();
			Update();
			SetMaxSize(wxSize(wxDefaultCoord, wxDefaultCoord));
            SetMinSize(wxSize(wxDefaultCoord, wxDefaultCoord));
			CSettingsReader::Control(pSrcPath, pDPath, pGauge, pPrgDlg);
			internal::ShowMessageDlg(this, m_FileForExport, wxT("Succesfully exported to SQLite"));
		}
		else {
			internal::ShowMessageDlg(this, wxT("SQLite database path should be provided."), wxT("Error"));
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
		m_FileForExport = wxT("Succesfully exported file: ");
		m_FileForExport += wxString::Format(wxT("%s"), dialog.GetFilename().c_str());
		m_FileForExport += wxT(" to sqlite");
		m_pSrcFilePath->WriteText(dialog.GetPath().c_str());
		if( m_pDestinationPath->GetValue().IsEmpty() )
		{
			m_pDestinationPath->WriteText(dialog.GetDirectory().c_str());
			m_pDestinationPath->AppendText(wxT("\\"));
			m_pDestinationPath->AppendText(dialog.GetFilename().c_str());

			int nLength = m_pDestinationPath->GetValue().length();
			m_pDestinationPath->Replace(nLength - 4, nLength, wxT(".sqlite"));
		}
	}
	else {
		internal::ShowMessageDlg(this, wxT("File not selected"), wxT("File not selected"));
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
	settingfile << "[Settings]" << '\n';
	settingfile << "RelationshipAdd = "; 
	                             if( RelationshipCheckbox->IsChecked() )  
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "RecordAdd = "; 
	                             if( RecordCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
    settingfile << "NotNullAdd = "; 
	                             if( NotNullValueCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "AutoIncrementAdd = "; 
	                             if( AutoincrementCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
    settingfile << "DefaultValueAdd = "; 
	                             if( DefaultValueCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "IndexAdd = "; 
	                             if( IndexCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "UniqueFieldAdd = "; 
	                             if( UniqueFieldsCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "CollateNoCaseForIndex = "; 
	                             if( CollateNcIndexCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
    settingfile << "CollateNoCaseForFields = "; 
	                             if( CollateNcFieldsCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "TrimTextValues = "; 
	                             if( TrimCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "AddComments = "; 
	                             if( DescriptionCheckbox->IsChecked() ) 
									settingfile << "true" << '\n'; 
								 else settingfile << "false" << '\n';
	settingfile << "KeyWordList = ";
	                             if( ReservedKeywordCheckbox->IsChecked() )
									 settingfile << "true" << '\n';
								 else settingfile << "false" << '\n';
    settingfile << "ForeignKeySupport = ";
	                             if( ForeignKeySupportCheckbox->IsChecked() )
									 settingfile << "true" << '\n';
								 else settingfile << "false" << '\n';
	settingfile << "PrimaryKeySupport = ";
								if( PrimaryKeyCheckbox->IsChecked() )
									settingfile << "true" << '\n';
								else settingfile << "false" << '\n';
	settingfile << "ForeignKeyPrimary = ";
	                            if( ForeignKeyPrimaryCheckbox -> IsChecked() )
									settingfile << "true" << '\n';
								else settingfile << "false" << '\n';
	settingfile.close();
}

void CustomDialog::ReadFromIni(wxCheckBox *&RelationshipCheckbox, wxCheckBox *&RecordCheckbox, wxCheckBox *&NotNullValueCheckbox, wxCheckBox *&AutoincrementCheckbox, wxCheckBox *&DefaultValueCheckbox,
	                         wxCheckBox *&IndexCheckbox, wxCheckBox *&UniqueFieldsCheckbox, wxCheckBox *&CollateNcIndexCheckbox, wxCheckBox *&CollateNcFieldsCheckbox, wxCheckBox *&TrimCheckbox,
							 wxCheckBox *&DescriptionCheckbox, wxCheckBox *&ReservedKeywordCheckbox, wxCheckBox *&ForeignKeySupportCheckbox, wxCheckBox *&PrimaryKeyCheckbox, wxCheckBox *&ForeignKeyPrimaryCheckbox)
{
	CSimpleIni ini;
    ini.SetUnicode();
	ini.LoadFile("Settings.ini");
	LPCTSTR sKey = L"Settings";
	RelationshipCheckbox->SetValue(ini.GetBoolValue(sKey,_T("RelationshipAdd"), true));
	RecordCheckbox->SetValue(ini.GetBoolValue(sKey, _T("RecordAdd"), true));
	NotNullValueCheckbox->SetValue(ini.GetBoolValue(sKey, _T("NotNullAdd"), true));
	AutoincrementCheckbox->SetValue(ini.GetBoolValue(sKey, _T("AutoIncrementAdd"), true));
	DefaultValueCheckbox->SetValue(ini.GetBoolValue(sKey, _T("DefaultValueAdd"), true));
	IndexCheckbox->SetValue(ini.GetBoolValue(sKey, _T("IndexAdd"), true));
	UniqueFieldsCheckbox->SetValue(ini.GetBoolValue(sKey, _T("UniqueFieldAdd"), true));
	CollateNcIndexCheckbox->SetValue(ini.GetBoolValue(sKey, _T("CollateNoCaseForIndex"), true));
	CollateNcFieldsCheckbox->SetValue(ini.GetBoolValue(sKey, _T("CollateNoCaseForFields"), true));
	TrimCheckbox->SetValue(ini.GetBoolValue(sKey, _T("TrimTextValues"), true));
	DescriptionCheckbox-> SetValue(ini.GetBoolValue(sKey, _T("AddComments"), true));
	ReservedKeywordCheckbox-> SetValue(ini.GetBoolValue(sKey, _T("KeyWordList"), true));
	ForeignKeySupportCheckbox->SetValue(ini.GetBoolValue(sKey, _T("ForeignKeySupport"), true));
	PrimaryKeyCheckbox->SetValue(ini.GetBoolValue(sKey ,_T("PrimaryKeySupport"), true));
	ForeignKeyPrimaryCheckbox->SetValue(ini.GetBoolValue(sKey ,_T("ForeignKeyPrimary"), true));
}
