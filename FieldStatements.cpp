#pragma once

#include "stdafx.h"
#include "FieldStatements.h"
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
void CFieldStatements::fFields(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector <CString> &statements, std::vector <CString> &UniqueFields, CSettings &CSettingsObject)
{
	 
	short nFieldCount = TableDef.GetFieldCount();        // Counts how manys fields there are in the database of a current sellected table
	CString *sFieldnames = new CString[nFieldCount];     // creating dynamic array of field names
	for(int j = 0; j < nFieldCount; j++)
			  {
				  
				  CDaoFieldInfo fieldinfo;                                          // Create a fieldinfo object in which we will store information about the field
				  TableDef.GetFieldInfo(j,fieldinfo,AFX_DAO_ALL_INFO);              // We store the information about the j-th field
				  sFieldnames[j] = fieldinfo.m_strName;                            // saving field names into an array
				  statements.back() += _T("      `");
				  statements.back() += fieldinfo.m_strName;
				  statements.back() += (_T("`     ")); 
				  CDaoRecordset rc;
				  rc.Open(&TableDef);
				  CDaoFieldInfo recordinfo;
				  rc.GetFieldInfo(j,recordinfo);
				  if(CSettingsObject.bFieldTypeAdd) FieldTypeAdd(TableDef,recordinfo,statements);
				  if(CSettingsObject.bNotNullAdd)  NotNullAdd(fieldinfo,statements);
				  if(CSettingsObject.bDefaultValueAdd) DefaultValueAdd(fieldinfo,statements);
				  if(CSettingsObject.bAutoIncrementAdd) AutoIncrementAdd(fieldinfo,statements);
				  if(CSettingsObject.bUniqueFieldAdd)  UniqueFieldAdd(statements,fieldinfo,tabledefinfo,UniqueFields);
					if(j != nFieldCount-1)
						statements.back() += (_T(","));
			    	else
						statements.back() += (_T(");"));
			  }
	if(CSettingsObject.bRecordAdd) Records(TableDef,tabledefinfo,nFieldCount,sFieldnames,statements);
}
	void CFieldStatements::NotNullAdd(const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements)
{
	 if(fieldinfo.m_bRequired)
		statements.back() += _T(" NOT NULL");
}
void CFieldStatements::AutoIncrementAdd( const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements)
{
	if(fieldinfo.m_lAttributes & dbAutoIncrField) // Checking if it should be autoincremented
					   statements.back() += _T(" PRIMARY KEY AUTOINCREMENT");
}
void CFieldStatements::DefaultValueAdd( const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements)
{
	if(fieldinfo.m_strDefaultValue != "")
				   {
				   statements.back() += _T(" DEFAULT ");
				   statements.back() += (LPCTSTR)fieldinfo.m_strDefaultValue;
				   }
}
void CFieldStatements::FieldTypeAdd(CDaoTableDef &TableDef, const CDaoFieldInfo &recordinfo, std::vector <CString> &statements)
{
	              CDaoRecordset recordset;
			      recordset.Open(&TableDef);
				   switch(recordinfo.m_nType)
				     {
				        case dbBoolean: statements.back() += (_T("INTEGER"));break;
				        case dbByte: statements.back() += (_T("INTEGER"));  break;
				        case dbInteger: statements.back() += (_T("INTEGER"));  break;
						case dbLong : statements.back() += (_T("INTEGER"));  break;
			   	        case dbCurrency: statements.back() += (_T("INTEGER"));  break;
				        case dbSingle: statements.back() += (_T("SINGLE"));  break;
				        case dbDouble: statements.back() += (_T("REAL"));  break;
				        case dbDate: statements.back() += (_T("DATE")); break;
						case dbBinary: statements.back() += (_T("BLOB")); break;
				        case dbText: statements.back() += (_T("TEXT"));  break;
						case dbLongBinary: statements.back() += (_T("BLOB"));  break;
						case dbMemo: statements.back() += (_T("TEXT"));  break;
						case dbGUID: ASSERT(FALSE); statements.back() += (_T("GUID"));  break;
						case dbBigInt: statements.back() += (_T("INTEGER"));  break;
						case dbVarBinary: statements.back() += (_T("BLOB"));  break;
						case dbChar: statements.back() += (_T("TEXT"));  break;
						case dbNumeric: statements.back() += (_T("NUMERIC"));  break;
						case dbDecimal: statements.back() += (_T("INTEGER"));  break;
						case dbFloat: statements.back() += (_T("REAL"));  break;
						case dbTime: statements.back() += (_T("TIME"));  break;
						case dbTimeStamp: ASSERT(FALSE); statements.back() += (_T("TIME"));  break;
				        default: break;
				      }
}
void CFieldStatements::UniqueFieldAdd(std::vector <CString> &statements, const CDaoFieldInfo &fieldinfo, const CDaoTableDefInfo &tabledefinfo, std::vector <CString> &UniqueFields)
{
	CString temp = tabledefinfo.m_strName;
	temp += fieldinfo.m_strName;
	unsigned nVectorSize = UniqueFields.size();
	for(int m = 0; m < nVectorSize; m++)
	{
		if(!(temp.Compare(UniqueFields[m])))
		{
			statements.back() += _T(" UNIQUE");
			break;
		}
	}
}
void CFieldStatements::Records(CDaoTableDef &TableDef, const CDaoTableDefInfo &tabledefinfo, short nFieldCount, CString *&sFieldnames, std::vector <CString> &statements)
{
		COleVariant COlevar;                                        // variable which stores variables of all types
	    CDaoRecordset recordset;
	    recordset.Open(&TableDef);
	    CString sParrent;
	    sParrent = _T("INSERT INTO   `");
	    sParrent += ((LPCTSTR)tabledefinfo.m_strName);
	    sParrent += _T("`  (");
		  for(int j = 0; j < nFieldCount; j++)
	      {
			  sParrent += _T("`");
			  sParrent += sFieldnames[j]; 
			  if(j != nFieldCount-1)
			  	 sParrent += _T("`, ");
			  else sParrent += "`)";
		  }
			  while(!recordset.IsEOF())
			     { 
					statements.push_back(sParrent);
			      	statements.back() += _T(" VALUES (");
			    	for(int m = 0; m < nFieldCount;m++)
			        	{
				        	recordset.GetFieldValue(sFieldnames[m], COlevar);
							if(COlevar.vt == VT_NULL)
                               statements.back() += _T("NULL");
							else if(COlevar.vt == VT_I2 || COlevar.vt == VT_I4 || COlevar.vt == VT_R4 || COlevar.vt == VT_R8 || COlevar.vt == VT_BOOL || COlevar.vt == VT_CY || COlevar.vt == VT_UI2 || COlevar.vt == VT_UI4 || COlevar.vt ==  VT_I8 || COlevar.vt == VT_UI8 || COlevar.vt == VT_INT || COlevar.vt ==  VT_UINT || COlevar.vt == VT_BLOB)
                                  statements.back() += COlevar;
							else 
							{
								statements.back() += _T("'");
								CString sString = COlevar;
								sString.Replace(_T("'"), _T("''"));
								statements.back() += sString;
								statements.back() += _T("'");
							}
				        	if(m != nFieldCount - 1)
					        	statements.back() += _T(", ");
					        else statements.back() += _T(");");
				        }
					recordset.MoveNext(); 
		         }
			  delete[] sFieldnames;           // deleting dynamic array
}