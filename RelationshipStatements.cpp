#include "stdafx.h"
#include "naujastestas.h"
#include "RelationshipStatements.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CRelationships::Relationhips(CDaoDatabase &db, std::vector <CString> &RelationFields, const short nRelationCount)
{
			  CDaoRelationInfo relationinfo;
			  for(int j = 0; j < nRelationCount; j++)
			  {
				  db.GetRelationInfo(j,relationinfo,AFX_DAO_ALL_INFO);
				  short nRelationFields = relationinfo.m_nFields;
				  RelationFields.push_back(_T("CREATE TRIGGER "));
				  RelationFields.back() += relationinfo.m_strName;
				  RelationFields.back() += _T("Delete AFTER DELETE ON ");
				  RelationFields.back() +=  relationinfo.m_strTable;
				  RelationFields.back() += _T(" FOR EACH ROW BEGIN");
				  for(int m = 0; m < nRelationFields; m++)
				  { 
					  RelationFields.back() += _T(" DELETE FROM ");
				      RelationFields.back() += relationinfo.m_strForeignTable;
				      RelationFields.back() += _T(" WHERE ");
                      RelationFields.back() += relationinfo.m_pFieldInfos[m].m_strForeignName;
					  RelationFields.back() += _T(" = OLD.");
					  RelationFields.back() += relationinfo.m_pFieldInfos[m].m_strName;
					  RelationFields.back() += _T(";");
				  }
				  RelationFields.back() += _T(" END; ");
			  }
			  for(int j = 0; j < nRelationCount; j++)
			  {
				  db.GetRelationInfo(j,relationinfo,AFX_DAO_ALL_INFO);
				  short nRelationFields = relationinfo.m_nFields;
				  RelationFields.push_back(_T("CREATE TRIGGER "));
				  RelationFields.back() += relationinfo.m_strName;
				  RelationFields.back() += _T("Update AFTER UPDATE OF ");
				  for(int m = 0; m < nRelationFields; m++)
				  {
					  RelationFields.back() += relationinfo.m_pFieldInfos[m].m_strName;
					  if(m != nRelationFields-1)
						  RelationFields.back() += _T(", ");
				  }
				  RelationFields.back() += _T(" ON ");
				  RelationFields.back() += relationinfo.m_strTable;
				  RelationFields.back() += _T(" BEGIN ");
				  for(int m = 0; m < nRelationFields; m++)
				  {
				  RelationFields.back() += _T("UPDATE ");
				  RelationFields.back() += relationinfo.m_strForeignTable;
				  RelationFields.back() += _T(" SET ");

                      RelationFields.back() += relationinfo.m_pFieldInfos[m].m_strForeignName;
					  RelationFields.back() += _T(" = new.");
					  RelationFields.back() += relationinfo.m_pFieldInfos[m].m_strForeignName;
				      RelationFields.back() += _T(" WHERE ");
					  RelationFields.back() += relationinfo.m_pFieldInfos[m].m_strForeignName;
					  RelationFields.back() += _T(" = OLD.");
					  RelationFields.back() += relationinfo.m_pFieldInfos[m].m_strName;
					  RelationFields.back() += _T("; ");
				  }
				  RelationFields.back() += _T("END; ");
			  }		  
}