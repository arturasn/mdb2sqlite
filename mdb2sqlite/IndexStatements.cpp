#define _AFXDLL
#include "stdafx.h"
#include "IndexStatements.h"
#include <afxdao.h>
#include <wx/textctrl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CIndexStatements::Indexes(CDaoTableDef &TableDef, std::vector<CString> &IndexStatements, const CDaoTableDefInfo &tabledefinfo, CString *&sTableNames, const short &nTableCount, 
	                           std::vector<CString> &UniqueFields, std::vector<CString> &CollateIndexFields, const bool &m_bCollateNoCaseIndexAdd, const bool &m_bTrimTextValues,  
							   wxTextCtrl *&PrgDlg, const bool &bKeyWordList, CString (&ReservedKeyWords)[124])
{
	CString sParrent;
	CString sStatement;
	CString sStatement2;
	short nIndexCount = TableDef.GetIndexCount();
	for( int i1 = 0; i1 < nIndexCount; ++i1 )
		{
			CDaoIndexInfo indexinfo;
			TableDef.GetIndexInfo(i1,indexinfo,AFX_DAO_ALL_INFO);
			if( indexinfo.m_strName.Find('{') != -1 || indexinfo.m_strName.Find('}') != -1 || IndexFilter(tabledefinfo,indexinfo,sTableNames,nTableCount) )
				continue;
			if( indexinfo.m_bUnique == TRUE )
				sStatement = _T("CREATE UNIQUE INDEX ");
			else 
				sStatement = _T("CREATE INDEX ");
			if ( bKeyWordList )
			{
				for( int i2 = 0; i2 < 124; ++i2 )
				{
					if( !(indexinfo.m_strName.CompareNoCase(ReservedKeyWords[i1])) )
						  {
							  wxString ErrorMessage = wxT("WARNING: index found as sqlite keyword this could lead to unexpected behaviour, index name found: ");
							  PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
							  CT2CA pszConvertedAnsiString (indexinfo.m_strName);
							  std::string strStd (pszConvertedAnsiString);
							  ErrorMessage += wxString::FromUTF8(_strdup(strStd.c_str() ) );
							  ErrorMessage += wxT("\n");
							  PrgDlg->WriteText(ErrorMessage);
							  PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
						  }

				}
			}
			sParrent = tabledefinfo.m_strName;
			sStatement += tabledefinfo.m_strName;
			sStatement += _T("_");
			sStatement += indexinfo.m_strName;
			sStatement += _T(" ON ");
			sStatement += tabledefinfo.m_strName;
			sStatement += _T("(");
			short nColumnCount = indexinfo.m_nFields;
			for( int i2 = 0; i2 < nColumnCount; ++i2 )
			{
				sStatement += _T("'");
				if( indexinfo.m_bUnique == TRUE )
				{
					sStatement2 = tabledefinfo.m_strName;
					if( m_bTrimTextValues )
						sStatement2 += indexinfo.m_pFieldInfos[i2].m_strName.TrimLeft().TrimRight();
					else 
						sStatement2 += indexinfo.m_pFieldInfos[i2].m_strName;
					UniqueFields.push_back(sStatement2);
				}
				if( m_bTrimTextValues )
					sParrent += indexinfo.m_pFieldInfos[i2].m_strName.TrimLeft().TrimRight();
				else 
					sParrent += indexinfo.m_pFieldInfos[i2].m_strName;
				if( m_bTrimTextValues )
					sStatement += indexinfo.m_pFieldInfos[i2].m_strName.TrimLeft().TrimRight();	
				else 
					sStatement += indexinfo.m_pFieldInfos[i2].m_strName;					  
				sStatement += _T("'");
				if( m_bCollateNoCaseIndexAdd )
				{
					if( IsIndexFieldText(sParrent, CollateIndexFields) ) 
						sStatement +=  _T(" COLLATE NOCASE ");
				}
				if( i2 == nColumnCount-1 )
					sStatement += _T(")");
				else 
					sStatement += _T(",");
			}
			sStatement += _T(";");
			IndexStatements.push_back(sStatement);
		}
}

bool CIndexStatements::IndexFilter(const CDaoTableDefInfo &tabledefinfo, const CDaoIndexInfo &indexinfo, CString *&sTableNames, const short &nTableCount)
{
	if( indexinfo.m_strName.Find(tabledefinfo.m_strName) == -1 )
		 return false;
	int nStart = indexinfo.m_strName.Find(tabledefinfo.m_strName,tabledefinfo.m_strName.GetLength());
    for( int i2 = 0; i2 < nTableCount; ++i2 )
	 {

      if( (indexinfo.m_strName.Find(sTableNames[i2]) != -1) && sTableNames[i2].Compare(tabledefinfo.m_strName) )
         return true;
      if( !(indexinfo.m_strName.Left(nStart).Compare(sTableNames[i2])) )
         return true;
	 }
   return false;
}

bool CIndexStatements::IsIndexFieldText(CString sParrent , std::vector<CString> &CollateIndexFields)
{
	auto end_it = CollateIndexFields.end();
	for( auto i3 = CollateIndexFields.begin(); i3 != end_it; ++i3 )
	{
		if( !sParrent.Compare(*i3) )
			return true;
	}
	return false;
}