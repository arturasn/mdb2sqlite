#ifndef __SETTINGS_DLG_H__ 
#define __SETTINGS_DLG_H__ 
#pragma once

#ifndef _WX_WIDGETS_LINK_H_
	#include "wxWidgetsLink.h"
#endif

#ifndef _WX_DIALOG_H_
	#include "wx/dialog.h"
#endif

class wxCheckBox;

class CSettingsDlg : public wxDialog
{
public:
	 CSettingsDlg(wxWindow *pParent);
	 virtual ~CSettingsDlg() { }

public:
	std::vector<std::pair<bool, std::wstring>> GetSettingsArr() const;

private:
	void RefreshSettings();
	void OnOK(wxCommandEvent &WXUNUSED(event));

private:
	wxCheckBox *m_RelationshipCheckbox;
	wxCheckBox *m_RecordCheckbox;
    wxCheckBox *m_NotNullValueCheckbox;
	wxCheckBox *m_AutoincrementCheckbox;
	wxCheckBox *m_DefaultValueCheckbox;
	wxCheckBox *m_IndexCheckbox;
	wxCheckBox *m_UniqueFieldsCheckbox;
	wxCheckBox *m_CollateNcIndexCheckbox;
	wxCheckBox *m_CollateNcFieldsCheckbox;
	wxCheckBox *m_TrimCheckbox;
	wxCheckBox *m_DescriptionCheckbox;
	wxCheckBox *m_ReservedKeywordCheckbox;
	wxCheckBox *m_ForeignKeySupportCheckbox;
	wxCheckBox *m_PrimaryKeyCheckbox;
	wxCheckBox *m_ForeignKeyPrimaryCheckbox;

	std::vector<std::pair<bool, std::wstring>> m_settings;
};

#endif