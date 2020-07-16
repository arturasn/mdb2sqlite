#define _AFXDLL
#include "stdafx.h"
#include "IndexStatements.h"
#include <afxdao.h>
#include <wx/textctrl.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

std::vector<CDBFieldIndex> CIndexStatements::Indexes(CDaoTableDef &TableDef, std::vector<CString> &IndexStatements, const CDaoTableDefInfo &tabledefinfo, std::vector<CString> &sTableNames, 
	                           std::vector<CString> &UniqueFields, std::vector<CString> &CollateIndexFields, const bool &m_bCollateNoCaseIndexAdd, const bool &m_bTrimTextValues,  
							   const bool &bKeyWordList, CString (&ReservedKeyWords)[124], std::vector<CString> &primary_fields, unsigned &nWarningCount, std::vector<int> &indextable, std::vector<std::wstring> &warnings)
{
	CString sParrent;
	CString sStatement;
	CString sStatement2;
	short nIndexCount = TableDef.GetIndexCount();
	indextable.push_back(nIndexCount);
	std::vector<CDBFieldIndex> indexes;

	for( int i1 = 0; i1 < nIndexCount; ++i1 )
	{
		CString sIndexName = "";
		std::vector<CString> constraints;

		CDaoIndexInfo indexinfo;
		TableDef.GetIndexInfo(i1, indexinfo, AFX_DAO_ALL_INFO);
		if( indexinfo.m_strName.Find('{') != -1 || indexinfo.m_strName.Find('}') != -1 || IndexFilter(tabledefinfo.m_strName, indexinfo.m_strName, sTableNames) )
		{
			--indextable[indextable.size() - 1];
			continue;
		}
		if( indexinfo.m_bPrimary )
		{
			for(int i2 = 0; i2 < indexinfo.m_nFields; ++i2) 
			{
				CString temp = tabledefinfo.m_strName;
				temp += indexinfo.m_pFieldInfos[i2].m_strName;
				primary_fields.push_back(temp);
			}

		}
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
					++nWarningCount;
					std::wstring sErrorMessage = wxT("WARNING: index found as sqlite keyword this could lead to unexpected behaviour, index name found: ");
					//PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
					CT2CA pszConvertedAnsiString (indexinfo.m_strName);
					std::string strStd (pszConvertedAnsiString);
                    sErrorMessage += wxString::FromUTF8(_strdup(strStd.c_str() ) );
                    sErrorMessage += wxT("\n");
					warnings.push_back(sErrorMessage);
					//PrgDlg->WriteText(ErrorMessage);
					//PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
				}
			}
		}

		sParrent = tabledefinfo.m_strName;

		sIndexName = tabledefinfo.m_strName;
		sIndexName += _T("_");
		sIndexName += indexinfo.m_strName;;

		sStatement += sIndexName;
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
				if( IsIndexFieldText(sParrent, CollateIndexFields) ) {
					sStatement +=  _T(" COLLATE NOCASE ");
					constraints.push_back(_T("COLLATE NOCASE"));
				}
			}
			if( i2 == nColumnCount-1 )
				sStatement += _T(")");
			else 
				sStatement += _T(",");
		}
		sStatement += _T(";");
		IndexStatements.push_back(sStatement);
		indexes.push_back(CDBFieldIndex(sIndexName, constraints));
	}

	return indexes;
}

bool CIndexStatements::IndexFilter(const CString &sTable, const CString &sIndexTableName, std::vector<CString> &sTableNames)
{
	if( sIndexTableName.Find(sTable) == -1 ) return false;
	int nStart = sIndexTableName.Find(sTable, sTable.GetLength());

	for( const auto &table : sTableNames )
	{
      if( (sIndexTableName.Find(table) != -1) && table.Compare(sTable) )  {
		  return true;
	  }

      if(  !(sIndexTableName.Left(nStart).Compare(table)) ) {
		  return true;
	  }
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