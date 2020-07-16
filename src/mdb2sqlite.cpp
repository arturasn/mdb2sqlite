#define _AFXDLL
#include "stdafx.h"
#include "mdb2sqlite.h"
#include "FileUtils.h"
#include "OnAction.h"
#include "SettingsDlg.h"

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/filename.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

IMPLEMENT_APP(MyApp)
wxBEGIN_EVENT_TABLE ( CMainDlg, wxDialog )
    EVT_BUTTON (Close_button, CMainDlg::OnExit ) 
	EVT_BUTTON(DIALOGS_FILE_OPEN, CMainDlg::FileOpen)
	EVT_BUTTON(Settings_button, CMainDlg::SettingsChoice )
	EVT_BUTTON(Convert_button, CMainDlg::OnConvert )
	EVT_BUTTON(Dump_button, CMainDlg::OnDump )
	EVT_CLOSE(CMainDlg::OnClose )
wxEND_EVENT_TABLE() 

namespace internal
{
	static void ShowMessageDlg(wxWindow *pParent, wxString sMsg, wxString sTitle)
	{
		wxMessageDialog dlg(pParent, sMsg, sTitle);
		dlg.ShowModal();
	}
};

bool MyApp::OnInit()
{
	int nPosX, nPosY, nSizeX;
	file_utils::GetWindowInformation(nPosX, nPosY, nSizeX);

	CMainDlg *pDlg = new CMainDlg(wxT("mdb2SQLite"), nPosX, nPosY, nSizeX);
    	pDlg->ShowModal();
	pDlg->Destroy();

    return true;
}

CMainDlg::CMainDlg(const wxString sTitle, const int nX, const int nY, const int nSizeX)
       : wxDialog(nullptr, wxID_ANY, sTitle, wxPoint(nX, nY), wxSize(nSizeX, 235), wxRESIZE_BORDER|wxCAPTION|wxCLOSE_BOX)
{
	wxIcon icon;
	icon.LoadFile("../img/sqliteicon.ico", wxBITMAP_TYPE_ICO);
	SetIcon(icon);

	m_pGauge = nullptr;
	m_pPrgDlg = nullptr;

	wxSizer *pSrcDest			= new wxStaticBoxSizer(wxVERTICAL, this);
	wxSizer *pSrcFile			= new wxStaticBoxSizer(wxVERTICAL, this, "&Source File:");
	wxBoxSizer *pFilePathSzr		= new wxBoxSizer(wxHORIZONTAL);
	wxButton *pBrowseBtn			= new wxButton(this, DIALOGS_FILE_OPEN, "Browse", wxDefaultPosition, wxSize(60, 22));
	m_pSrcFilePath				= new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, 20));
	wxSizer *pDestFile			= new wxStaticBoxSizer(wxVERTICAL, this, "&Destination:");
	wxBoxSizer *pDestFilePath		= new wxBoxSizer(wxHORIZONTAL);
	m_pDestinationPath			= new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150,20));
	m_pTopSizer				= new wxBoxSizer(wxVERTICAL);
	wxButton *pSettingsBtn			= new wxButton(this, Settings_button, "settings", wxDefaultPosition, wxSize(60,25));
	wxBoxSizer *pBottomSzr			= new wxBoxSizer(wxHORIZONTAL);
	wxButton *pCloseBtn			= new wxButton(this, Close_button, "Close", wxDefaultPosition, wxSize(60,25));
	wxButton *pDumpBtn			= new wxButton(this, Dump_button, "Dump", wxDefaultPosition, wxSize(60,25));
	wxButton *pConvertBtn			= new wxButton(this, Convert_button, "Convert", wxDefaultPosition, wxSize(60,25)); 

	pFilePathSzr->Add(m_pSrcFilePath, 1, wxLEFT|wxTOP|wxEXPAND, 4);
	pFilePathSzr->AddSpacer(15); 
	pFilePathSzr->Add(pBrowseBtn, 0, wxRIGHT|wxTOP, 3);
	
	pSrcFile->Add(pFilePathSzr, 0, wxEXPAND, 1);
	pSrcDest->Add(pSrcFile, 0, wxEXPAND);

	pDestFilePath->Add(m_pDestinationPath, 1, wxLEFT|wxTOP|wxEXPAND, 4);
	pDestFile->Add(pDestFilePath, 0, wxEXPAND|wxRIGHT, 78);

	pSrcDest->Add(pDestFile, 0, wxEXPAND);

	pBottomSzr->Add(pCloseBtn, 1, wxBOTTOM|wxRIGHT, 5);
	pBottomSzr->AddSpacer(5);
	pBottomSzr->Add(pDumpBtn, 1, wxBOTTOM|wxRIGHT, 5);
	pBottomSzr->AddSpacer(5);
	pBottomSzr->Add(pConvertBtn, 1, wxBOTTOM|wxRIGHT, 5);
  
	m_pTopSizer->Add(pSrcDest, 0, wxALL|wxEXPAND, 3);
	m_pTopSizer->AddSpacer(5);
	m_pTopSizer->Add(pSettingsBtn, 0, wxLEFT, 15);
	m_pTopSizer->AddSpacer(5);
	m_pTopSizer->Add(new wxStaticLine(this), 0, wxEXPAND|wxLEFT|wxRIGHT, 8);
	m_pTopSizer->AddSpacer(5);
	m_pTopSizer->Add(pBottomSzr, 0, wxALIGN_RIGHT, 5);

	int nWidth, nHeight;
	GetSize(&nWidth, &nHeight);
	SetMinSize(wxSize(wxDefaultCoord, nHeight));
	SetMaxSize(wxSize(wxDefaultCoord, GetMinSize().y));

	SetSizer(m_pTopSizer);
}

CMainDlg::~CMainDlg()
{

}

void CMainDlg::OnClose( wxCloseEvent &WXUNUSED(event) )
{
	OnSaveWndInfo();
	EndModal(wxID_CANCEL);
}

void CMainDlg::OnExit( wxCommandEvent &WXUNUSED(event) )
{
	OnSaveWndInfo();
    Close(true);
}

void CMainDlg::OnSaveWndInfo()
{
	int nPosX, nPosY;
	int nSizeX, nSizeY;

	GetPosition(&nPosX, &nPosY);
	GetSize(&nSizeX, &nSizeY);

	file_utils::SaveWindowInformation(nPosX, nPosY, nSizeX);

	EndModal(wxID_CANCEL);
}

void CMainDlg::OnDump( wxCommandEvent &WXUNUSED(event) )
{
	wxFileDialog dlg(this, wxT("Save file"), wxEmptyString, wxT("dump.txt"), wxT("Text files (*.txt)|*.txt"),
					 wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	
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

			CSettingsReader::Control(pSrcPath, pDstPath, this, false);
			internal::ShowMessageDlg(this, wxT("Succesfully dumped."), wxT("Dump information"));
		}

	}
}

void CMainDlg::OnConvert(wxCommandEvent &WXUNUSED(event) )
{
	wxString sPathMDB		= m_pSrcFilePath->GetValue();
	wxString sPathSQLite	= m_pDestinationPath->GetValue();

	if( sPathMDB.IsEmpty() || sPathSQLite.IsEmpty() ) {
		internal::ShowMessageDlg(this, wxT("Destination or source path is missing"), wxT("Error"));
	}
	else 
	{
		wxFileName filename(sPathMDB);
		wxString sFilename = filename.GetFullName();
	
		const char *pSrcPath		= sPathMDB.mb_str();
		const char *pDPath		= sPathSQLite.mb_str();

		bool bRet = CSettingsReader::Control(pSrcPath, pDPath, this, true);
		if( bRet )
			internal::ShowMessageDlg(this, wxString::Format(wxT("Succesfully exported %s to SQLite"), sFilename), wxT("Succesfully exported to SQLite"));
	}
}

void CMainDlg::CreateAdditionalItems()
{
	int nPosX, nPosY;
	GetSize(&nPosX, &nPosY);
	SetMaxSize(wxSize(nPosX, wxDefaultCoord));

	m_pGauge		=  new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(nPosX, 15));
	m_pPrgDlg		= new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 120), wxTE_MULTILINE|wxTE_RICH);
	m_pTopSizer->Add(m_pPrgDlg, 1, wxEXPAND|wxALL, 5);
	m_pTopSizer->Add(m_pGauge, 0, wxEXPAND|wxALL, 5);
	Layout(); Fit(); Refresh(); Update();
	SetMaxSize(wxSize(wxDefaultCoord, wxDefaultCoord));
	SetMinSize(wxSize(wxDefaultCoord, wxDefaultCoord));
}

void CMainDlg::SetDefaultStyle(const wxTextAttr &atr)
{
	if( m_pPrgDlg ) {
		m_pPrgDlg->SetDefaultStyle(atr);
	}
}

void CMainDlg::WriteText(std::wstring sMsg)
{
	if( m_pPrgDlg ) {
		m_pPrgDlg->WriteText(sMsg);
	}
}

void CMainDlg::SetRange(int range)
{
	if( m_pGauge ) {
		m_pGauge->SetRange(range);
	}
}

void CMainDlg::SetValue(int value)
{
	if( m_pGauge ) {
		m_pGauge->SetValue(value);
	}
}

void CMainDlg::FileOpen(wxCommandEvent &WXUNUSED(event) )
{
	wxFileDialog dlg(this, wxT("Conversion file selection"), wxEmptyString, wxEmptyString, wxT("mdb files (*.mdb)|*.mdb"));
	dlg.SetDirectory(wxGetHomeDir());
	if ( dlg.ShowModal() == wxID_OK )
	{
		m_pSrcFilePath->SetValue(dlg.GetPath());
		wxString sDest = m_pDestinationPath->GetValue();
		if( sDest.IsEmpty() )
		{
			m_pDestinationPath->WriteText(dlg.GetDirectory());
			m_pDestinationPath->AppendText(wxT("\\"));
			m_pDestinationPath->AppendText(dlg.GetFilename());

			int nLength = m_pDestinationPath->GetValue().length();
			m_pDestinationPath->Replace(nLength - 4, nLength, wxT(".sqlite"));
		}
	}
}

void CMainDlg::SettingsChoice(wxCommandEvent& WXUNUSED(event) )
{
	CSettingsDlg dlg(this);
	int nRet = dlg.ShowModal();

	if( nRet == wxID_OK )
	{
		std::vector<std::pair<bool, std::wstring>> settings = dlg.GetSettingsArr();
		file_utils::SaveSettings(settings);
	}
}
