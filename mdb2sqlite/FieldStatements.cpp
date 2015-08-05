#define _AFXDLL
#include "stdafx.h"
#include "FieldStatements.h"
#include <afxdao.h>
#include <wx/textctrl.h>
//#include <iostream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString CFieldStatements::GetDaoFieldDescription(CString &strFieldName, CString &strTableName, CDaoDatabase &db) 
    { 
        ASSERT(db.m_pDAODatabase != NULL); 
        ASSERT(!strTableName.IsEmpty()); 
        ASSERT(!strFieldName.IsEmpty()); 
        ASSERT(db.IsOpen()); 
        COleVariant varProp(_T("Description"), VT_BSTRT); 
        COleVariant varTable(strTableName, VT_BSTRT); 
        COleVariant varField(strFieldName, VT_BSTRT); 
        DAODatabase* piDB = db.m_pDAODatabase; 
        CComPtr<DAOProperties>piPC; 
        CComPtr<DAOTableDefs>piTDC; 
        CComPtr<DAOTableDef>piTD; 
        CComPtr<DAOProperty>piP; 
        CComPtr<DAOFields>piFC; 
        CComPtr<DAOField>piF; 
        COleVariant varValue; 
        CString strT; 
        try 
        { 
            DAO_CHECK(piDB->get_TableDefs(&piTDC)); 
            DAO_CHECK(piTDC->get_Item(varTable, &piTD)); 
            DAO_CHECK(piTD->get_Fields(&piFC)); 
            DAO_CHECK(piFC->get_Item(varField, &piF)); 
            DAO_CHECK(piF->get_Properties(&piPC)); 
            DAO_CHECK(piPC->get_Item(varProp, &piP)); 
            DAO_CHECK(piP->get_Value(&varValue)); 
            if (varValue.vt == VT_BSTR) 
                strT = V_BSTRT(&varValue); 
        } 
        catch(CDaoException* e) 
        { 
            e->Delete(); 
        } 
        return strT; 
    } 
bool CFieldStatements::GetPrimaryKey(CString &sTableName, CString &sFieldName,std::vector<CString> &indexinfo)
{
	    CString temp = sTableName;
		temp += sFieldName;
		auto end_it = indexinfo.end();
		for(auto it = indexinfo.begin(); it != end_it; ++it)
		{
			if( !(temp.Compare(*it)) )
				return true;
		}
		return false;
}
wxString CFieldStatements::CstringToWxString(const CString &ConversionString)
{
	CT2CA pszConvertedAnsiString (ConversionString);
	std::string strStd (pszConvertedAnsiString);
	return wxString::FromUTF8(_strdup(strStd.c_str() ) );
}
void CFieldStatements::fFields(CDaoDatabase &db, CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &InsertStatements, std::vector<CString> &UniqueFields, 
	CSettings &settings, CString &sStatement,CString (&ReservedKeyWords)[124], std::vector<CString> &TableField, std::vector<CString> &IndexInfo, wxTextCtrl *PrgDlg /*= NULL*/)
{
	   
		short nFieldCount = TableDef.GetFieldCount();        
		CString *sFieldnames = new CString[nFieldCount];  
		bool bIsText;
		for( int i1 = 0; i1 < nFieldCount; ++i1 )
			{
				bIsText = false;
				CDaoFieldInfo fieldinfo;                                          
				TableDef.GetFieldInfo(i1,fieldinfo,AFX_DAO_ALL_INFO);
				bool isPrimary = CFieldStatements::GetPrimaryKey(tabledefinfo.m_strName,fieldinfo.m_strName,IndexInfo);
				if( !isPrimary ) 
				{
					CString Temp = tabledefinfo.m_strName;
					Temp += fieldinfo.m_strName;
					TableField.push_back(Temp);
				}
				if( settings.m_bTrimTextValues ) 
					sFieldnames[i1] = fieldinfo.m_strName.TrimRight().TrimLeft(); 
				else 
					sFieldnames[i1] = fieldinfo.m_strName;
				sStatement += _T("`");
				if( settings.m_bTrimTextValues )
					sStatement += fieldinfo.m_strName.TrimLeft().TrimRight();
				else 
					sStatement += fieldinfo.m_strName;
				if( settings.m_bKeyWordList && PrgDlg != NULL )
				{
					for( int i2 = 0; i2 < 124; ++i2 )
					{
						if( !(fieldinfo.m_strName.CompareNoCase(ReservedKeyWords[i2])) )
						{
							  wxString ErrorMessage = wxT("WARNING: found field name sqlite keyword this could lead to unexpected behaviour, found on table: ");
							  PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
							  ErrorMessage += CstringToWxString(tabledefinfo.m_strName);
							  ErrorMessage += wxT(" field name: ");
							  ErrorMessage += CstringToWxString(fieldinfo.m_strName);
							  ErrorMessage += wxT("\n");
							  PrgDlg->WriteText(ErrorMessage);
							  PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
						}
					}
				}
				sStatement += (_T("` ")); 
				CDaoRecordset rc;
				rc.Open(&TableDef);
				CDaoFieldInfo recordinfo;
				rc.GetFieldInfo(i1,recordinfo);
				FieldTypeAdd(TableDef, recordinfo, bIsText, sStatement);
				if( settings.m_bNotNullAdd )  
					NotNullAdd(fieldinfo, sStatement);
				if( settings.m_bDefaultValueAdd ) 
					DefaultValueAdd(fieldinfo, tabledefinfo, sStatement, recordinfo, PrgDlg);
				if( settings.m_bAutoIncrementAdd )
					AutoIncrementAdd(fieldinfo, sStatement);
				else if( isPrimary && settings.m_PrimaryKeySupport)
					sStatement += _T(" PRIMARY KEY");
				if( settings.m_bUniqueFieldAdd ) 
					UniqueFieldAdd(fieldinfo, tabledefinfo, UniqueFields, sStatement);
				if( bIsText && settings.m_bCollateNoCaseFieldsAdd )
					sStatement += _T(" COLLATE NOCASE");
				 if( settings.m_bAddComments )
				{
					sStatement += _T(" /*");
					sStatement += GetDaoFieldDescription(fieldinfo.m_strName, tabledefinfo.m_strName, db);
				    sStatement += _T(" */");
				}
				if( i1 != nFieldCount-1 )
					sStatement += (_T(","));
			    else
					sStatement+= (_T(");"));
					
			}
		if( settings.m_bRecordAdd ) 
			Records(TableDef, tabledefinfo, nFieldCount, sFieldnames, InsertStatements);
}
void CFieldStatements::NotNullAdd(const CDaoFieldInfo &fieldinfo,CString &sStatement)
{
	 if( fieldinfo.m_bRequired )
		sStatement += _T(" NOT NULL");
}
void CFieldStatements::AutoIncrementAdd(const CDaoFieldInfo &fieldinfo, CString &sStatement)
{
	if( fieldinfo.m_lAttributes & dbAutoIncrField ) 
		sStatement += _T(" PRIMARY KEY AUTOINCREMENT");
}
void CFieldStatements::DefaultValueAdd( const CDaoFieldInfo &fieldinfo, CDaoTableDefInfo &tabledefinfo, CString &sStatement, CDaoFieldInfo &recordinfo, wxTextCtrl *&PrgDlg)
{
	if( !(fieldinfo.m_strDefaultValue.IsEmpty()) )
		{
			if ( recordinfo.m_nType == dbBoolean )
			{
				if( !(fieldinfo.m_strDefaultValue.CompareNoCase(_T("Yes"))) )
				{
					sStatement += _T(" DEFAULT -1");
					return;
				}
				if( !(fieldinfo.m_strDefaultValue.CompareNoCase(_T("No"))) )
				{
					sStatement += _T(" DEFAULT 0");
					return;
				}
				if( !(fieldinfo.m_strDefaultValue.CompareNoCase(_T("true"))) )
				{
					sStatement += _T(" DEFAULT -1");
					return;
				}
				if( !(fieldinfo.m_strDefaultValue.CollateNoCase(_T("false"))) )
				{
					sStatement += _T(" DEFAULT 0");
					return;
				}
				if( !(fieldinfo.m_strDefaultValue.CollateNoCase(_T("1"))) )
				{
					sStatement += _T(" DEFAULT -1");
					return;
				}
				if( !(fieldinfo.m_strDefaultValue.CollateNoCase(_T("0"))) )
				{
					sStatement += _T(" DEFAULT 0");
					return;
				}
				//std::wcout << "Unable to recognize default value:" << (LPCTSTR)fieldinfo.m_strDefaultValue << " on Table:" << (LPCTSTR)tabledefinfo.m_strName << " Column:" << (LPCTSTR)fieldinfo.m_strName << std::endl;
				PrgDlg->SetDefaultStyle(wxTextAttr (*wxRED));
				wxString ErrorMessage = wxT("Unable to recognize default value: ");
				ErrorMessage += CstringToWxString(fieldinfo.m_strDefaultValue);
				ErrorMessage += wxT(" on table: ");
				ErrorMessage += CstringToWxString(tabledefinfo.m_strName);
				ErrorMessage += wxT(" column: ");
				ErrorMessage += CstringToWxString(fieldinfo.m_strName);
				ErrorMessage += wxT("\n");
				PrgDlg->WriteText(ErrorMessage);
				PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
			}
			else 
			{
				sStatement += _T(" DEFAULT ");
				sStatement += fieldinfo.m_strDefaultValue;
			}
		}
}
void CFieldStatements::FieldTypeAdd(CDaoTableDef &TableDef, const CDaoFieldInfo &recordinfo, bool &bIsText, CString &sStatement)
{
			CDaoRecordset recordset;
			recordset.Open(&TableDef);
			switch( recordinfo.m_nType )
				{
				case dbBoolean: 
						sStatement += (_T("INTEGER"));
						break;
				case dbByte: 
						sStatement += (_T("INTEGER")); 
						break;
				case dbInteger: 
						sStatement += (_T("INTEGER"));  
						break;
				case dbLong : 
						sStatement += (_T("INTEGER"));  
						break;
				case dbCurrency: 
						sStatement += (_T("INTEGER")); 
						break;
				case dbSingle: 
						sStatement += (_T("INTEGER")); 
						break;
				case dbDouble: 
						sStatement += (_T("REAL")); 
						break;
				case dbDate: 
						sStatement += (_T("INTEGER")); 
						break;
				case dbBinary: 
						sStatement += (_T("BLOB")); 
						break;
				case dbText: 
						sStatement += (_T("TEXT")); 
						bIsText = true; 
						break;
				case dbLongBinary: 
						sStatement += (_T("BLOB"));  
						break;
				case dbMemo: 
						sStatement += (_T("TEXT")); 
						bIsText = true;
						break;
				case dbGUID: 
						ASSERT(FALSE); 
						sStatement += (_T("TEXT"));  
						bIsText = true; 
						break;
				case dbBigInt: 
						sStatement += (_T("INTEGER"));  
						break;
				case dbVarBinary: 
						sStatement += (_T("BLOB")); 
						break;
				case dbChar: 
						sStatement += (_T("TEXT")); 
						bIsText = true; 
						break;
				case dbNumeric: 
						sStatement += (_T("INTEGER"));
						break;
				case dbDecimal: 
						sStatement += (_T("INTEGER")); 
						break;
				case dbFloat: 
						sStatement += (_T("REAL")); 
						break;
				case dbTime: 
						sStatement += (_T("INTEGER")); 
						break;
				case dbTimeStamp: 
						ASSERT(FALSE); 
						sStatement += (_T("INTEGER")); 
						break;
				default:
						break;
				}
}
void CFieldStatements::UniqueFieldAdd(const CDaoFieldInfo &fieldinfo, const CDaoTableDefInfo &tabledefinfo, std::vector<CString> &UniqueFields, CString &sStatement)
{
	CString temp = tabledefinfo.m_strName;
	temp += fieldinfo.m_strName;
	auto end_it = UniqueFields.end();
	for( auto i2 = UniqueFields.begin(); i2 != end_it; ++i2 )
	{
		if( !(temp.Compare(*i2)) )
		{
			sStatement += _T(" UNIQUE");
			break;
		}
	}
}
void CFieldStatements::Records(CDaoTableDef &TableDef, const CDaoTableDefInfo &tabledefinfo, short nFieldCount, CString *&sFieldnames, std::vector <CString> &InsertStatements)
{
		COleVariant COlevar;                                        
	    CDaoRecordset recordset;
	    recordset.Open(&TableDef);
	    CString sParrent;
	    sParrent = _T("INSERT INTO   `");
	    sParrent += ((LPCTSTR)tabledefinfo.m_strName);
	    sParrent += _T("`  (");
		for( int i1 = 0; i1 < nFieldCount; ++i1 )
			{
				sParrent += _T("`");
				sParrent += sFieldnames[i1]; 
				if( i1 != nFieldCount-1 )
				  	sParrent += _T("`, ");
				else 
					sParrent += "`)";
			}
				CString sStatement;
				InsertStatements.push_back(_T("BEGIN TRANSACTION"));
				while( !recordset.IsEOF() )
			     { 
					sStatement = sParrent;
			      	sStatement += _T(" VALUES (");
			    	for( int i2 = 0; i2 < nFieldCount; ++i2 )
			        	{
				        	recordset.GetFieldValue(sFieldnames[i2], COlevar);
							if( COlevar.vt == VT_NULL )
							    sStatement += _T("NULL");
							else if( COlevar.vt == VT_I2 || COlevar.vt == VT_I4 || COlevar.vt == VT_R4 || COlevar.vt == VT_R8 || COlevar.vt == VT_BOOL || COlevar.vt == VT_CY || COlevar.vt == VT_UI2 || COlevar.vt == VT_UI4 || COlevar.vt ==  VT_I8 || COlevar.vt == VT_UI8 || COlevar.vt == VT_INT || COlevar.vt ==  VT_UINT || COlevar.vt == VT_BLOB )
									sStatement += COlevar;
								 else 
									{
										sStatement += _T("'");
										CString sString = COlevar;
										sString.Replace(_T("'"), _T("''"));
										sStatement += sString;
										sStatement += _T("'");
									}
				        	if( i2 != nFieldCount - 1 )
					        	sStatement += _T(", ");
					        else
							    { 
							     sStatement += _T(");");
							     InsertStatements.push_back(sStatement);
							    }
				        }
					recordset.MoveNext(); 
		         }
		InsertStatements.push_back(_T("END TRANSACTION"));
		delete[] sFieldnames;           
}
void CFieldStatements::FieldCollation(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &CollateIndexFields, const bool &m_bTrimTextValues)
{
	TableDef.Open(tabledefinfo.m_strName);
	short nFieldCount = TableDef.GetFieldCount(); 
	CString sStatement;
	for( int i1 = 0; i1 < nFieldCount; ++i1 )
		{
			CDaoFieldInfo fieldinfo;                                          
			TableDef.GetFieldInfo(i1,fieldinfo,AFX_DAO_ALL_INFO); 
			CDaoRecordset rc;
			rc.Open(&TableDef);
			CDaoFieldInfo recordinfo;
			rc.GetFieldInfo(i1,recordinfo);
			switch( recordinfo.m_nType )
			{
				case dbText: 
						sStatement = tabledefinfo.m_strName;  
						m_bTrimTextValues? sStatement += fieldinfo.m_strName.TrimLeft().TrimRight() : sStatement += fieldinfo.m_strName ;
						break;
				case dbMemo: 
						sStatement = tabledefinfo.m_strName; 
						m_bTrimTextValues? sStatement += fieldinfo.m_strName.TrimLeft().TrimRight() : sStatement += fieldinfo.m_strName ; 
						break;
				case dbGUID: 
						sStatement = tabledefinfo.m_strName;
						m_bTrimTextValues? sStatement += fieldinfo.m_strName.TrimLeft().TrimRight() : sStatement += fieldinfo.m_strName ;
						break;
                case dbChar: 
						sStatement = tabledefinfo.m_strName;
						m_bTrimTextValues? sStatement += fieldinfo.m_strName.TrimLeft().TrimRight() : sStatement += fieldinfo.m_strName ;
						break;
				default: 
						break;
			}
			CollateIndexFields.push_back(sStatement);
		 }
}