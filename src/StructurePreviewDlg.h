#ifndef __STRUCTURE_PREVIEW_DLG_H__ 
#define __STRUCTURE_PREVIEW_DLG_H__ 
#pragma once

#ifndef _WX_WIDGETS_LINK_H_
	#include "wxWidgetsLink.h"
#endif

#ifndef _WX_DIALOG_H_
	#include "wx/dialog.h"
#endif

#ifndef _WX_TREECTRL_H_BASE_
	#include "wx/treectrl.h"
#endif

#include <vector>

#ifndef __DB_TABLE_H__
	#include "DBTable.h"
#endif

class CStructPreviewDlg : public wxDialog
{
public:
	 CStructPreviewDlg(wxWindow *pParent, std::vector<CDBTable> &tables);
	 virtual ~CStructPreviewDlg();

private:
	void LoadData(std::vector<CDBTable> &tables);
	void LoadTable(CDBTable &table);
	void LoadFieldAndIndex(wxTreeItemId idparent, CDBFieldIndex &fieldIdx, int idximg);

public:
	void OnOK(wxCommandEvent &WXUNUSED(event));

private:
	wxTreeCtrl *m_pTreeCtrl;
};

#endif