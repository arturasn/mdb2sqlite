#include "stdafx.h"
#include "IndexStatements.h"
#include "naujastestas.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4995)

void CIndexStatements::Indexes(CDaoTableDef &TableDef, std::vector <CString> &IndexStatements, const CDaoTableDefInfo &tabledefinfo, CString *&sTableNames, const short &nTableCount, std::vector <CString> &UniqueFields, std::vector <CString> &CollateIndexFields, const bool &bCollateNoCaseIndexAdd)
{
	CString sParrent;
	 short nIndexCount = TableDef.GetIndexCount();
			  for(int i1 = 0; i1 < nIndexCount; ++i1)
			  {
				  CDaoIndexInfo indexinfo;
				  TableDef.GetIndexInfo(i1,indexinfo,AFX_DAO_ALL_INFO);
				  if(indexinfo.m_strName.Find('{') != -1 || indexinfo.m_strName.Find('}') != -1 || IndexFilter(tabledefinfo,indexinfo,sTableNames,nTableCount))
				     continue;
				  if(indexinfo.m_bUnique == TRUE)
					  IndexStatements.push_back(_T("CREATE UNIQUE INDEX "));
				  else IndexStatements.push_back(_T("CREATE INDEX "));
				  sParrent = tabledefinfo.m_strName;
				  IndexStatements.back() += tabledefinfo.m_strName;
				  IndexStatements.back() += _T("_");
				  IndexStatements.back() += indexinfo.m_strName;
				  IndexStatements.back() += _T(" ON ");
				  IndexStatements.back() += tabledefinfo.m_strName;
				  IndexStatements.back() += _T("(");
				  short nColumnCount = indexinfo.m_nFields;
				  for(int i2 = 0; i2 < nColumnCount; ++i2)
				  {
					  IndexStatements.back() += _T("'");
					  if(indexinfo.m_bUnique == TRUE)
					  {
						  UniqueFields.push_back(tabledefinfo.m_strName);
						  UniqueFields.back() += indexinfo.m_pFieldInfos[i2].m_strName;
					  }
					  sParrent += indexinfo.m_pFieldInfos[i2].m_strName;
					  IndexStatements.back() += indexinfo.m_pFieldInfos[i2].m_strName;					  
					  IndexStatements.back() += _T("'");
					  if(bCollateNoCaseIndexAdd)
					   {
					     if(IsIndexFieldText(sParrent,CollateIndexFields)) 
						    	IndexStatements.back() +=  _T(" COLLATE NOCASE ");
					   }
					  if(i2 == nColumnCount-1)
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
  int nStart = indexinfo.m_strName.Find(tabledefinfo.m_strName,tabledefinfo.m_strName.GetLength());
  for(int i2 = 0; i2 < nTableCount; ++i2)
    {

      if((indexinfo.m_strName.Find(sTableNames[i2]) != -1) && sTableNames[i2].Compare(tabledefinfo.m_strName))
         return true;
      if(!(indexinfo.m_strName.Left(nStart).Compare(sTableNames[i2])))
         return true;
    }
   return false;
}

bool CIndexStatements::IsIndexFieldText(CString sParrent , std::vector <CString> &CollateIndexFields)
{
	unsigned nVectorLength = CollateIndexFields.size();
	for(unsigned i3 = 0; i3 < nVectorLength; ++i3)
	{
		if(!sParrent.Compare(CollateIndexFields[i3]))
			return true;
	}
	return false;
}