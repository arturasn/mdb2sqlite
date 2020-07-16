#define _AFXDLL
#include "StdAfx.h"
#include "SettingsDlg.h"
#include "Settings.h"
#include "FileUtils.h"

#include "wx/wx.h"
#include "wx/statline.h"

#include <vector>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

CSettingsDlg::CSettingsDlg(wxWindow *pParent) : wxDialog(pParent, wxID_ANY, wxT("Values to export"))
{
	m_RelationshipCheckbox		= new wxCheckBox(this, wxID_ANY, L"Relationship add",                   wxDefaultPosition);
	m_RecordCheckbox		    = new wxCheckBox(this, wxID_ANY, L"Record add",                         wxDefaultPosition);
    m_NotNullValueCheckbox	    = new wxCheckBox(this, wxID_ANY, L"Not Null value add",                 wxDefaultPosition);
	m_AutoincrementCheckbox		= new wxCheckBox(this, wxID_ANY, L"AutoIncrement value add",            wxDefaultPosition);
	m_DefaultValueCheckbox		= new wxCheckBox(this, wxID_ANY, L"Default value add",                  wxDefaultPosition);
	m_IndexCheckbox			    = new wxCheckBox(this, wxID_ANY, L"Index add",                          wxDefaultPosition);
	m_UniqueFieldsCheckbox		= new wxCheckBox(this, wxID_ANY, L"Unique fields add",                  wxDefaultPosition);
	m_CollateNcIndexCheckbox	= new wxCheckBox(this, wxID_ANY, L"Collate nocase for index add",       wxDefaultPosition);
	m_CollateNcFieldsCheckbox	= new wxCheckBox(this, wxID_ANY, L"Collatee nocase for fields add",     wxDefaultPosition);
	m_TrimCheckbox			    = new wxCheckBox(this, wxID_ANY, L"Trim text values",                   wxDefaultPosition);
	m_DescriptionCheckbox		= new wxCheckBox(this, wxID_ANY, L"Add field comments",                 wxDefaultPosition);
	m_ReservedKeywordCheckbox	= new wxCheckBox(this, wxID_ANY, L"Reserved keyword list add",          wxDefaultPosition);
	m_ForeignKeySupportCheckbox = new wxCheckBox(this, wxID_ANY, L"Foreign key support",                wxDefaultPosition);
	m_PrimaryKeyCheckbox		= new wxCheckBox(this, wxID_ANY, L"Primary key support",                wxDefaultPosition);
	m_ForeignKeyPrimaryCheckbox = new wxCheckBox(this, wxID_ANY, L"Foreign key both primary fields",    wxDefaultPosition);

	RefreshSettings();
	file_utils::GetSettings(m_settings);

	m_RelationshipCheckbox->SetValue(m_settings[ESettings::eRelationshipAdd].first);
	m_RecordCheckbox->SetValue(m_settings[ESettings::eRecordAdd].first);
	m_NotNullValueCheckbox->SetValue(m_settings[ESettings::eNotNullAdd].first);
	m_AutoincrementCheckbox->SetValue(m_settings[ESettings::eAutoIncAdd].first);
	m_DefaultValueCheckbox->SetValue(m_settings[ESettings::eDefaultValueAdd].first);
	m_IndexCheckbox->SetValue(m_settings[ESettings::eIndexAdd].first);
	m_UniqueFieldsCheckbox->SetValue(m_settings[ESettings::eUniqueFieldAdd].first);
	m_CollateNcFieldsCheckbox->SetValue(m_settings[ESettings::eCollateNoCaseForFields].first);
	m_CollateNcIndexCheckbox->SetValue(m_settings[ESettings::eCollateNoCaseForIndex].first);
	m_TrimCheckbox->SetValue(m_settings[ESettings::eTrimTextValues].first);
	m_DescriptionCheckbox->SetValue(m_settings[ESettings::eAddComments].first);
	m_ReservedKeywordCheckbox->SetValue(m_settings[ESettings::eKeyWordList].first);
	m_ForeignKeySupportCheckbox->SetValue(m_settings[ESettings::eForeignKeySupport].first);
	m_PrimaryKeyCheckbox->SetValue(m_settings[ESettings::ePrimaryKeySupport].first);
	m_ForeignKeyPrimaryCheckbox->SetValue(m_settings[ESettings::eForeignKeyPrimary].first);

	wxSizer *pSettingSzr = new wxBoxSizer(wxVERTICAL);

	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_RelationshipCheckbox, 0, wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_RecordCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_NotNullValueCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_AutoincrementCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_DefaultValueCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_IndexCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_UniqueFieldsCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_CollateNcIndexCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_CollateNcFieldsCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_TrimCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_DescriptionCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_ReservedKeywordCheckbox, 0,  wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_ForeignKeySupportCheckbox, 0, wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_PrimaryKeyCheckbox, 0, wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);
	pSettingSzr->Add(m_ForeignKeyPrimaryCheckbox, 0, wxLEFT|wxRIGHT, 8);
	pSettingSzr->AddSpacer(5);

	m_RelationshipCheckbox->SetToolTip(wxT("Adds relationships between tables, update and delete sqlite statements are executed."));
	m_RecordCheckbox->SetToolTip(wxT("Adds the information that is in the records."));
	m_NotNullValueCheckbox->SetToolTip(wxT("Adds a constraint Not Null to a field if it cannot be Null."));
	m_AutoincrementCheckbox->SetToolTip(wxT("Adds a constraint PRIMARY KEY AUTOINCREMENT to a field, then when a new record is created the autoincrement field will automatically update."));
	m_DefaultValueCheckbox->SetToolTip(wxT("Adds a constraint Default value to a field."));
	m_IndexCheckbox->SetToolTip(wxT("Adds indexes."));
	m_UniqueFieldsCheckbox->SetToolTip(wxT("Adds a constraint UNIQUE to a field."));
	m_CollateNcFieldsCheckbox->SetToolTip(wxT("Adds a constraint COLLATE NO CASE to a field, when added the field is case insensitive."));
	m_CollateNcIndexCheckbox->SetToolTip(wxT("Adds a constraint COLLATE NO CASE to a index, when added  the index is case insensitive."));
	m_TrimCheckbox->SetToolTip(wxT("Trims the front and back of a field and index if there are some blank spaces."));
	m_DescriptionCheckbox->SetToolTip(wxT("Adds comments about fields."));
	m_ReservedKeywordCheckbox->SetToolTip(wxT("If we find some sqlite keywords in tables, indexes or fields error message is shown."));
	m_ForeignKeySupportCheckbox->SetToolTip(wxT("Adds foreign key support, NOTE:foreign keys only work when there are no loops between table references"));
	m_PrimaryKeyCheckbox->SetToolTip(wxT("Adds a contraint PRIMARY KEY to a field, all of this fields values ar unique"));
	m_ForeignKeyPrimaryCheckbox->SetToolTip(wxT("Do not create a relationship enforced by a foreign key where both fields are primary key (where the relationship is 1-1)") );

	pSettingSzr->Add(new wxStaticLine(this), 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
	wxBoxSizer *pHbox = new wxBoxSizer(wxHORIZONTAL);
	pHbox->Add(new wxButton(this, wxID_OK, "OK"), 0, wxLEFT|wxEXPAND, 5);
	pHbox->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxRIGHT|wxEXPAND, 5);
	pSettingSzr->Add(pHbox, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP, 5);
	SetSizerAndFit(pSettingSzr);

	Connect(wxID_OK, wxEVT_BUTTON, wxCommandEventHandler(CSettingsDlg::OnOK), nullptr, this);
}

void CSettingsDlg::OnOK(wxCommandEvent &WXUNUSED(event))
{
	RefreshSettings();
	EndModal(wxID_OK);
}

void CSettingsDlg::RefreshSettings()
{
	m_settings.resize(ESettings::eSize);
	m_settings[ESettings::eRelationshipAdd]			= std::make_pair(m_RelationshipCheckbox->IsChecked(), _T("RelationshipAdd"));
	m_settings[ESettings::eRecordAdd]			= std::make_pair(m_RecordCheckbox->IsChecked(), _T("RecordAdd"));
	m_settings[ESettings::eNotNullAdd]			= std::make_pair(m_NotNullValueCheckbox->IsChecked(), _T("NotNullAdd"));
	m_settings[ESettings::eAutoIncAdd]			= std::make_pair(m_AutoincrementCheckbox->IsChecked(), _T("AutoIncrementAdd"));
	m_settings[ESettings::eDefaultValueAdd]			= std::make_pair(m_DefaultValueCheckbox->IsChecked(), _T("DefaultValueAdd"));
	m_settings[ESettings::eIndexAdd]			= std::make_pair(m_IndexCheckbox->IsChecked(), _T("IndexAdd"));
	m_settings[ESettings::eUniqueFieldAdd]			= std::make_pair(m_UniqueFieldsCheckbox->IsChecked(), _T("UniqueFieldAdd"));
	m_settings[ESettings::eCollateNoCaseForIndex]		= std::make_pair(m_CollateNcIndexCheckbox->IsChecked(), _T("CollateNoCaseForIndex"));
	m_settings[ESettings::eCollateNoCaseForFields]		= std::make_pair(m_CollateNcFieldsCheckbox->IsChecked(), _T("CollateNoCaseForFields"));
	m_settings[ESettings::eTrimTextValues]			= std::make_pair(m_TrimCheckbox->IsChecked(), _T("TrimTextValues"));
	m_settings[ESettings::eAddComments]			= std::make_pair(m_DescriptionCheckbox->IsChecked(), _T("AddComments"));
	m_settings[ESettings::eKeyWordList]			= std::make_pair(m_ReservedKeywordCheckbox->IsChecked(), _T("KeyWordList"));
	m_settings[ESettings::eForeignKeySupport]		= std::make_pair(m_ForeignKeySupportCheckbox->IsChecked(), _T("ForeignKeySupport"));
	m_settings[ESettings::ePrimaryKeySupport]		= std::make_pair(m_PrimaryKeyCheckbox->IsChecked(), _T("PrimaryKeySupport"));
	m_settings[ESettings::eForeignKeyPrimary]		= std::make_pair(m_ForeignKeyPrimaryCheckbox->IsChecked(), _T("ForeignKeyPrimary"));
}

std::vector<std::pair<bool, std::wstring>> CSettingsDlg::GetSettingsArr() const
{
	return m_settings;
}
