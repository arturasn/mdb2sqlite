#include "stdafx.h"
#include "RelationshipStatements.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void CRelationships::Relationhips(CDaoDatabase &db, std::vector<CString> &RelationFields, const short nRelationCount)
{
			  CDaoRelationInfo relationinfo;
			  for(int i1 = 0; i1 < nRelationCount; ++i1)
			  {
				  db.GetRelationInfo(i1,relationinfo,AFX_DAO_ALL_INFO);
				  short nRelationFields = relationinfo.m_nFields;
				  RelationFields.push_back(_T("CREATE TRIGGER "));
				  RelationFields.back() += relationinfo.m_strName;
				  RelationFields.back() += _T("Delete AFTER DELETE ON ");
				  RelationFields.back() +=  relationinfo.m_strTable;
				  RelationFields.back() += _T(" FOR EACH ROW BEGIN");
				  for(int i2 = 0; i2 < nRelationFields; ++i2)
				  { 
					  RelationFields.back() += _T(" DELETE FROM ");
				      RelationFields.back() += relationinfo.m_strForeignTable;
				      RelationFields.back() += _T(" WHERE ");
                      RelationFields.back() += relationinfo.m_pFieldInfos[i2].m_strForeignName;
					  RelationFields.back() += _T(" = OLD.");
					  RelationFields.back() += relationinfo.m_pFieldInfos[i2].m_strName;
					  RelationFields.back() += _T(";");
				  }
				  RelationFields.back() += _T(" END; ");
			  }
			  for(int i1 = 0; i1 < nRelationCount; ++i1)
			  {
				  db.GetRelationInfo(i1,relationinfo,AFX_DAO_ALL_INFO);
				  short nRelationFields = relationinfo.m_nFields;
				  RelationFields.push_back(_T("CREATE TRIGGER "));
				  RelationFields.back() += relationinfo.m_strName;
				  RelationFields.back() += _T("Update AFTER UPDATE OF ");
				  for(int i2 = 0; i2 < nRelationFields; ++i2)
				  {
					  RelationFields.back() += relationinfo.m_pFieldInfos[i2].m_strName;
					  if(i2 != nRelationFields-1)
						  RelationFields.back() += _T(", ");
				  }
				  RelationFields.back() += _T(" ON ");
				  RelationFields.back() += relationinfo.m_strTable;
				  RelationFields.back() += _T(" BEGIN ");
				  for(int i2 = 0; i2 < nRelationFields; ++i2)
				  {
				  RelationFields.back() += _T("UPDATE ");
				  RelationFields.back() += relationinfo.m_strForeignTable;
				  RelationFields.back() += _T(" SET ");

                      RelationFields.back() += relationinfo.m_pFieldInfos[i2].m_strForeignName;
					  RelationFields.back() += _T(" = new.");
					  RelationFields.back() += relationinfo.m_pFieldInfos[i2].m_strName;
				      RelationFields.back() += _T(" WHERE ");
					  RelationFields.back() += relationinfo.m_pFieldInfos[i2].m_strForeignName;
					  RelationFields.back() += _T(" = OLD.");
					  RelationFields.back() += relationinfo.m_pFieldInfos[i2].m_strName;
					  RelationFields.back() += _T("; ");
				  }
				  RelationFields.back() += _T("END; ");
			  }		  
}