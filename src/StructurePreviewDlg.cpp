#define _AFXDLL
#include "stdAfx.h"
#include "StructurePreviewDlg.h"
#include "wx/treectrl.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

namespace internal
{
	static wxString CstringToWxString(const CString &ConversionString)
	{
		CT2CA pszConvertedAnsiString (ConversionString);
		std::string strStd (pszConvertedAnsiString);
		return wxString::FromUTF8(_strdup(strStd.c_str() ) );
	}
}

CStructPreviewDlg::CStructPreviewDlg(wxWindow *pParent, std::vector<CDBTable> &tables) : wxDialog(pParent, wxID_ANY, wxT("Structure preview"), wxDefaultPosition, wxSize(700, 400), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	m_pTreeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT);
	m_pTreeCtrl->AddRoot(wxT("Virtual root"));
	
	wxInitAllImageHandlers();

	wxImageList *pImg = new wxImageList(16, 16, true);
	pImg->Add(wxBitmap(wxImage("../img/table.png")));
	pImg->Add(wxBitmap(wxImage("../img/page_green.png")));
	pImg->Add(wxBitmap(wxImage("../img/tag_blue.png")));
	pImg->Add(wxBitmap(wxImage("../img/folder.png")));
	pImg->Add(wxBitmap(wxImage("../img/cog.png")));

	m_pTreeCtrl->SetImageList(pImg);

	LoadData(tables);

	wxBoxSizer *pMainSzr = new wxBoxSizer(wxVERTICAL);

	pMainSzr->Add(m_pTreeCtrl, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer *pButtonSzr = new wxBoxSizer(wxHORIZONTAL);
	pButtonSzr->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxTOP|wxBOTTOM|wxLEFT, 5);
	pButtonSzr->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxTOP|wxBOTTOM|wxLEFT, 5);
	pMainSzr->Add(pButtonSzr);

	SetSizer(pMainSzr);

	Connect(wxID_OK, wxEVT_BUTTON, wxCommandEventHandler(CStructPreviewDlg::OnOK), nullptr, this);
}

CStructPreviewDlg::~CStructPreviewDlg()
{

}

void CStructPreviewDlg::LoadData(std::vector<CDBTable> &tables)
{
	for(auto &table : tables) {
		LoadTable(table);
	}
}

void CStructPreviewDlg::LoadTable(CDBTable &table)
{
	wxTreeItemId idroot = m_pTreeCtrl->GetRootItem();
	wxTreeItemId idtable = m_pTreeCtrl->AppendItem(idroot, internal::CstringToWxString(table.GetName()), 0);

	std::vector<CDBFieldIndex> &idxes = table.GetIndexes();
	std::vector<CDBFieldIndex> &fields = table.GetFields();

	wxTreeItemId idfields = m_pTreeCtrl->AppendItem(idtable, wxT("Fields"), 3);

	wxTreeItemId ididx;

	if( idxes.size() > 0 ) {
		ididx = m_pTreeCtrl->AppendItem(idtable, wxT("Indexes"), 3);
		m_pTreeCtrl->Expand(ididx);
	}

	for(auto &field : fields) {
		LoadFieldAndIndex(idfields, field, 1);
	}

	for(auto &idx : idxes) {
		LoadFieldAndIndex(ididx, idx, 2);
	}

	m_pTreeCtrl->Expand(idtable);
	m_pTreeCtrl->Expand(idfields);
	if( idxes.size() > 0 ) {
		m_pTreeCtrl->Expand(ididx);	
	}
}

void CStructPreviewDlg::LoadFieldAndIndex(const wxTreeItemId &parent, CDBFieldIndex &fieldidx, int idximg)
{
	wxTreeItemId idfieldidx = m_pTreeCtrl->AppendItem(parent, internal::CstringToWxString(fieldidx.GetName()), idximg);
	std::vector<CString> &constraints = fieldidx.GetConstraints();
	for(auto &constraint : constraints)
	{
		m_pTreeCtrl->AppendItem(idfieldidx, internal::CstringToWxString(constraint), 4);
	}
}

void CStructPreviewDlg::OnOK(wxCommandEvent &WXUNUSED(event))
{
	EndModal(wxID_OK);
}
