#define _AFXDLL
#include "stdafx.h"
#include <wx/wx.h>
#include "mdb2sqlite.h"
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/progdlg.h>
#include <wx/textctrl.h>
#include "naujastestas.h"
#include <fstream>
#include "SimpleIni.h"

IMPLEMENT_APP(MyApp)
wxBEGIN_EVENT_TABLE ( CustomDialog, wxDialog )
    EVT_BUTTON (Close_button, CustomDialog::OnExit ) 
	EVT_BUTTON(DIALOGS_FILE_OPEN, CustomDialog::FileOpen)
	EVT_BUTTON(Settings_button,CustomDialog::SettingsChoice )
	EVT_BUTTON(Convert_button,CustomDialog::OnConvert )
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

  vbox = new wxBoxSizer(wxVERTICAL);

  wxButton *settingsbutton = new wxButton(this, Settings_button, "settings", wxDefaultPosition, wxSize(60,25));
  wxBoxSizer *hhhbox = new wxBoxSizer(wxHORIZONTAL);
  wxButton *closebutton = new wxButton(this, Close_button, "Close", wxDefaultPosition, wxSize(60,25));
  hhhbox->Add(closebutton, 1, wxBOTTOM | wxRIGHT, 5);
  hhhbox->AddSpacer(5);

 
  wxButton *Convertbutton = new wxButton(this, Convert_button, "Convert", wxDefaultPosition, wxSize(60,25)); 
  hhhbox->Add(Convertbutton , 1, wxBOTTOM | wxRIGHT, 5);
  
  vbox->Add(SourceDestination, 1, wxALL | wxEXPAND, 3);
  vbox->AddSpacer(5);
  vbox->Add(settingsbutton, 0, wxLEFT, 25);
  vbox->AddSpacer(5);
  vbox->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
  vbox->AddSpacer(5);
  vbox->Add(hhhbox, 0, wxLEFT | wxALIGN_RIGHT, 120);
  SetSizer(vbox);
  ShowModal();
  Destroy(); 
}
void CustomDialog::OnExit( wxCommandEvent& event )
  {
    Close(TRUE);
  }
void CustomDialog::OnConvert(wxCommandEvent& event )
{
	if( TFilePathLine->GetValue().IsEmpty() || TDestinationPathLine->GetValue().IsEmpty() )
	{
		wxMessageDialog convertdialog(this, wxT("Destination or Path is missing"), wxT("Error"));
		convertdialog.ShowModal();
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
		if(TDestinationPathLine->GetValue().SubString(TDestinationPathLine->GetValue().length()-7,TDestinationPathLine->GetValue().length()) == wxT(".sqlite"))
		{
			wxGauge *gauge =  new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(270,15));
			wxTextCtrl *PrgDlg = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250,120), wxTE_MULTILINE | wxTE_RICH);
			vbox->Add(PrgDlg, 0, wxEXPAND | wxALL, 5);
			vbox->Add(gauge, 0, wxEXPAND | wxALL, 5);
			Layout();
            Fit();
			Refresh();
			Update();
			CSettingsReader::Control(Path, dPath, gauge, PrgDlg);
		    wxMessageDialog convertdialog(this, FileForExport, wxT("Succesfully exported to sqlite"));
		    convertdialog.ShowModal();
		}
		else
		{
			wxMessageDialog convertdialog(this, wxT("In the destination path you should provide a sqlite file in which to store information"), wxT("Destination path wasn`t correctly provided"));
			convertdialog.ShowModal();
		}
	}
	
}
void CustomDialog::FileOpen(wxCommandEvent &WXUNUSED(event) )
{
	wxFileDialog dialog(this, wxT("Conversion file selection"), wxEmptyString, wxEmptyString, wxT("mdb files (*.mdb)|*.mdb"));
	dialog.CentreOnParent();
	dialog.SetDirectory(wxGetHomeDir());
	if (dialog.ShowModal() == wxID_OK)
	{
		wxString info;
		FileForExport = wxT("Succesfully exported file: ");
		FileForExport += wxString::Format(wxT("%s"), dialog.GetFilename().c_str());
		FileForExport += wxT(" to sqlite");
		TFilePathLine->AppendText(dialog.GetPath().c_str());
		if(TDestinationPathLine->GetValue().IsEmpty())
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
	wxDialog *dlg = new wxDialog(this, wxID_ANY, wxT("Values to export"), wxDefaultPosition, wxSize(220,320));
	const wxString choices[] =
    {
        wxT("Fields add"), wxT("Relationship add"), wxT("Record add"), wxT("Not Null value add"), wxT("AutoIncrement value add"),
        wxT("Default value add"), wxT("Field type add"), wxT("Index add"), wxT("Unique Fields add"), wxT("Collate nocase for index add"),
        wxT("Collatee nocase for fields add"), wxT("Trim text values"), wxT("Add field comments"), wxT("Reserved keyword list add")
    };
    wxCheckListBox *Selections = new wxCheckListBox(dlg, wxID_ANY, wxDefaultPosition, wxSize(50,230), WXSIZEOF(choices), choices);
	wxSizer *settingsizer = new wxStaticBoxSizer(wxVERTICAL, dlg);
	settingsizer->Add(Selections, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	settingsizer->AddSpacer(10);
	settingsizer->Add(new wxStaticLine(dlg), 0, wxEXPAND | wxLEFT | wxRIGHT, 3);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(new wxButton(dlg, wxID_OK, "OK", wxDefaultPosition, wxSize(70,25)), 0, wxLEFT, 5);
	hbox->Add(new wxButton(dlg, wxID_CANCEL, "Cancel", wxDefaultPosition, wxSize(70,25)), 0, wxLEFT, 5);
	settingsizer->Add(hbox, 0, wxLEFT | wxTOP, 5);
	dlg->SetSizer(settingsizer);
	if(dlg->ShowModal() == wxID_OK)
		SaveToIni(Selections);
	dlg->Destroy();
}
void CustomDialog::SaveToIni(wxCheckListBox *&Selections)
{
	std::ofstream settingfile("Settings.ini");
	settingfile << "[Settings]" << std::endl;
	settingfile << "FieldsAdd = "; 
	                             if(Selections->IsChecked(0)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "RelationshipAdd = "; 
	                             if(Selections->IsChecked(1))  
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "RecordAdd = "; 
	                             if(Selections->IsChecked(2)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "NotNullAdd = "; 
	                             if(Selections->IsChecked(3)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "AutoIncrementAdd = "; 
	                             if(Selections->IsChecked(4)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "DefaultValueAdd = "; 
	                             if(Selections->IsChecked(5)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "FieldTypeAdd = "; 
	                             if(Selections->IsChecked(6)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "IndexAdd = "; 
	                             if(Selections->IsChecked(7)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "UniqueFieldAdd = "; 
	                             if(Selections->IsChecked(8)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "CollateNoCaseForIndex = "; 
	                             if(Selections->IsChecked(9)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
    settingfile << "CollateNoCaseForFields = "; 
	                             if(Selections->IsChecked(10)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "TrimTextValues = "; 
	                             if(Selections->IsChecked(11)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "AddComments = "; 
	                             if(Selections->IsChecked(12)) 
									settingfile << "true" << std::endl; 
								 else settingfile << "false" << std::endl;
	settingfile << "KeyWordList = ";
	                             if(Selections->IsChecked(13))
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