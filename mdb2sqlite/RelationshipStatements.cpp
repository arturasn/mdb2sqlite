#define _AFXDLL
#include "stdafx.h"
#include <wx/textctrl.h>
#include "RelationshipStatements.h"
#include "FieldStatements.h"
#include <afxdao.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void CRelationships::Relationhips(CDaoDatabase &db, std::vector<CString> &RelationFields, const short nRelationCount)
{
		CString sStatement;
		CString temp;
		CDaoRelationInfo relationinfo;
		for( int i1 = 0; i1 < nRelationCount; ++i1 )
		{
			db.GetRelationInfo(i1,relationinfo,AFX_DAO_ALL_INFO);
			if(relationinfo.m_lAttributes & dbRelationDeleteCascade)
			{
				short nRelationFields = relationinfo.m_nFields;
				sStatement = _T("CREATE TRIGGER ");
				sStatement += relationinfo.m_strName;
				sStatement += _T("Delete AFTER DELETE ON ");
				sStatement +=  relationinfo.m_strTable;
				sStatement += _T(" FOR EACH ROW BEGIN");
				for( int i2 = 0; i2 < nRelationFields; ++i2 )
				{ 
					sStatement += _T(" DELETE FROM ");
					sStatement += relationinfo.m_strForeignTable;
					sStatement += _T(" WHERE ");
					sStatement += relationinfo.m_pFieldInfos[i2].m_strForeignName;
					sStatement += _T(" = OLD.");
					sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
					sStatement += _T(";");
				}
				sStatement += _T(" END; ");
				RelationFields.push_back(sStatement);
			}
		}
		for( int i1 = 0; i1 < nRelationCount; ++i1 )
		{
				db.GetRelationInfo(i1,relationinfo,AFX_DAO_ALL_INFO);
				if(relationinfo.m_lAttributes & dbRelationUpdateCascade)
				{
					short nRelationFields = relationinfo.m_nFields;
					sStatement = _T("CREATE TRIGGER ");
					sStatement += relationinfo.m_strName;
					sStatement += _T("Update AFTER UPDATE OF ");
					for( int i2 = 0; i2 < nRelationFields; ++i2 )
					{
						sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
						if( i2 != nRelationFields-1 )
							sStatement += _T(", ");
					}
					sStatement += _T(" ON ");
					sStatement += relationinfo.m_strTable;
					sStatement += _T(" BEGIN ");
					for( int i2 = 0; i2 < nRelationFields; ++i2 )
					{
						sStatement += _T("UPDATE ");
						sStatement += relationinfo.m_strForeignTable;
						sStatement += _T(" SET ");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strForeignName;
						sStatement += _T(" = new.");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
						sStatement += _T(" WHERE ");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strForeignName;
						sStatement += _T(" = OLD.");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
						sStatement += _T("; ");
					}
					sStatement += _T("END; ");
					RelationFields.push_back(sStatement);
				}
		}		  
}
void CRelationships::ForeignKeySupport(CDaoDatabase &db, const unsigned &nRelationCount, std::vector<CString> (&TableField)[2], std::vector<CString> &RelationFields, wxTextCtrl *PrgDlg /*NULL*/)
{
	CString temp;
	CDaoRelationInfo relationinfo;
	for( unsigned i = 0; i < nRelationCount; ++i )
	{
		db.GetRelationInfo(i,relationinfo,AFX_DAO_ALL_INFO);
		if(relationinfo.m_lAttributes & dbRelationDontEnforce)
			continue;
		temp = relationinfo.m_strTable;
		temp += _T("FOREIGN KEY(");
		unsigned nRelationFields = relationinfo.m_nFields;
		for( unsigned i1 = 0; i1 < nRelationFields; ++i1)
		{
			temp += relationinfo.m_pFieldInfos[i1].m_strName;
			if( i1 != nRelationFields-1 )
				temp += _T(",");
			else temp += ")";
		}
		temp += _T(" REFERENCES ");
		temp += relationinfo.m_strForeignTable;
		temp += _T("(");
		for( unsigned i1 = 0; i1 < nRelationFields; ++i1 )
		{
			temp += relationinfo.m_pFieldInfos[i1].m_strForeignName;
			if( i1 != nRelationFields-1 )
				temp += _T(",");
			else temp += ")";
			if(PrgDlg != NULL)
			{
				CString temp2 = relationinfo.m_strForeignTable;
				temp2 += relationinfo.m_pFieldInfos[i1].m_strForeignName;
				unsigned nVectorLength = TableField[0].size();
				for(unsigned i2 = 0; i2 < nVectorLength; ++i2)
				{
					if(!(TableField[0][i2].Compare(temp2)))
					{
						wxString WarningMessage = wxT("WARNING: Foreign key of table: ");
						WarningMessage += CFieldStatements::CstringToWxString(relationinfo.m_strName);
						WarningMessage += wxT(" References table: ");
						WarningMessage += CFieldStatements::CstringToWxString(relationinfo.m_strForeignTable);
						WarningMessage += wxT(" field: ");
						WarningMessage += CFieldStatements::CstringToWxString(relationinfo.m_pFieldInfos[i1].m_strForeignName);
						WarningMessage += wxT(" which is not a PRIMARY KEY.\n");
						PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
						PrgDlg->WriteText(WarningMessage);
						PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
					}
				}
			}
		}  
		RelationFields.push_back(temp);
	}
}