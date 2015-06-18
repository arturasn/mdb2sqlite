#pragma once

#include "stdafx.h"
#include "IndexStatements.h"
#include "stdafx.h"
#include "naujastestas.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
void CIndexStatements::Indexes(CDaoTableDef &TableDef, std::vector <CString> &IndexStatements, const CDaoTableDefInfo &tabledefinfo, CString *&sTableNames, const short &nTableCount, std::vector <CString> &UniqueFields)
{
	 short nIndexCount = TableDef.GetIndexCount();
			  for(int j = 0; j < nIndexCount; j++)
			  {
				  CDaoIndexInfo indexinfo;
				  TableDef.GetIndexInfo(j,indexinfo,AFX_DAO_ALL_INFO);
				  if(indexinfo.m_strName.Find('{') != -1 || indexinfo.m_strName.Find('}') != -1 || IndexFilter(tabledefinfo,indexinfo,sTableNames,nTableCount))
				     continue;
				  if(indexinfo.m_bUnique == TRUE)
					  IndexStatements.push_back(_T("CREATE UNIQUE INDEX "));
				  else IndexStatements.push_back(_T("CREATE INDEX "));
				  IndexStatements.back() += tabledefinfo.m_strName;
				  IndexStatements.back() += _T("_");
				  IndexStatements.back() += indexinfo.m_strName;
				  IndexStatements.back() += _T(" ON ");
				  IndexStatements.back() += tabledefinfo.m_strName;
				  IndexStatements.back() += _T("(");
				  short nColumnCount = indexinfo.m_nFields;
				  for(int m = 0; m < nColumnCount; m++)
				  {
					  IndexStatements.back() += _T("'");
					  if(indexinfo.m_bUnique == TRUE)
					  {
						  UniqueFields.push_back(tabledefinfo.m_strName);
						  UniqueFields.back() += indexinfo.m_pFieldInfos[m].m_strName;
					  }
					  IndexStatements.back() += indexinfo.m_pFieldInfos[m].m_strName;
					  IndexStatements.back() += _T("'");
					  if(m == nColumnCount-1)
						  IndexStatements.back() += _T(")");
					  else IndexStatements.back() += _T(",");
				  }
				  IndexStatements.back() += _T(";");
			  }
}
bool CIndexStatements::IndexFilter(const CDaoTableDefInfo &tabledefinfo, const CDaoIndexInfo &indexinfo, CString *&sTableNames, const short &nTableCount)
{
	if(indexinfo.m_strName.Find(tabledefinfo.m_strName) == -1)
	  return false;
	CString temp = tabledefinfo.m_strName + _T("1");
	CString temp2 = tabledefinfo.m_strName + _T("2");
	CString temp3 = tabledefinfo.m_strName + tabledefinfo.m_strName;
	for(int m = 0; m < nTableCount; m++)
	{
		
		if((indexinfo.m_strName.Find(sTableNames[m]) != -1) && sTableNames[m].Compare(tabledefinfo.m_strName))
			return true;
		if(indexinfo.m_strName.Find(temp) != -1 || indexinfo.m_strName.Find(temp2) != -1 || indexinfo.m_strName.Find(temp3) != -1)
			return true;
	}
	return false;
}