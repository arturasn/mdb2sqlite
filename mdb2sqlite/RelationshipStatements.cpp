#define _AFXDLL
#include "stdafx.h"
#include "RelationshipStatements.h"
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
		for( int i1 = 0; i1 < nRelationCount; ++i1 )
		{
			db.GetRelationInfo(i1,relationinfo,AFX_DAO_ALL_INFO);
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