#define _AFXDLL
#include "stdafx.h"
#include "FieldStatements.h"
#include <afxdao.h>
#include "wxWidgetsLink.h"
#include <wx/textctrl.h>
#include <fstream>
#include <iomanip>

#ifdef _DEBUG
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

	for(const auto &it : indexinfo) 
		if( !(temp.Compare(it)) )
			return true;

	return false;
}

wxString CFieldStatements::CstringToWxString(const CString &ConversionString)
{
	CT2CA pszConvertedAnsiString (ConversionString);
	std::string strStd (pszConvertedAnsiString);
	return wxString::FromUTF8(_strdup(strStd.c_str() ) );
}

std::vector<CDBFieldIndex> CFieldStatements::fFields(CDaoDatabase &db, CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &InsertStatements, std::vector<CString> &UniqueFields, 
	CSettings &settings, CString &sStatement,CString (&ReservedKeyWords)[124], std::vector<CString> &TableField, std::vector<CString> &primary_fields, unsigned &nWarningCount, std::vector<std::wstring> &warnings)
{
		size_t nFieldCount = TableDef.GetFieldCount();
		std::vector<CString> sFieldnames; 
		bool bIsText;
		CString Temp2;
		std::vector<CDBFieldIndex> fields;
		std::vector<std::wstring> real_primary_keys;
		for(size_t i1 = 0; i1 < nFieldCount; ++i1)
		{
			CString sFieldNameAndType = "";
			std::vector<CString> constraints;
			bIsText = false;
			CDaoFieldInfo fieldinfo;                                          
			TableDef.GetFieldInfo(i1, fieldinfo, AFX_DAO_ALL_INFO);
			bool isPrimary = CFieldStatements::GetPrimaryKey(tabledefinfo.m_strName,fieldinfo.m_strName,primary_fields);
			if( !isPrimary ) 
			{
				CString Temp = tabledefinfo.m_strName;
				Temp += fieldinfo.m_strName;
				TableField.push_back(Temp);
			}

			if( settings.m_bTrimTextValues ) sFieldnames.push_back(fieldinfo.m_strName.TrimRight().TrimLeft()); 
			else sFieldnames.push_back(fieldinfo.m_strName);

			sStatement += _T("`");

			if( settings.m_bTrimTextValues ) sFieldNameAndType = fieldinfo.m_strName.TrimLeft().TrimRight();
			else sFieldNameAndType = fieldinfo.m_strName;

			sStatement += sFieldNameAndType;

			if( settings.m_bKeyWordList )
			{
				for(size_t i2 = 0; i2 < 124; ++i2 )
				{
					if( !(fieldinfo.m_strName.CompareNoCase(ReservedKeyWords[i2])) )
					{
							++nWarningCount;
							std::wstring sErrorMessage = L"WARNING: found field name sqlite keyword this could lead to unexpected behaviour, found on table: ";
                            sErrorMessage += CstringToWxString(tabledefinfo.m_strName);
                            sErrorMessage += L" field name: ";
                            sErrorMessage += CstringToWxString(fieldinfo.m_strName);
                            sErrorMessage += L"\n";
							warnings.push_back(sErrorMessage);
					}
				}
			}
			sStatement += (_T("` ")); 
			CDaoRecordset rc;
			rc.Open(&TableDef);
			CDaoFieldInfo recordinfo;
			rc.GetFieldInfo(i1,recordinfo);

			sFieldNameAndType += _T("  Type: ");
			FieldTypeAdd(TableDef, recordinfo.m_nType, bIsText, sStatement, sFieldNameAndType);

			if( settings.m_bNotNullAdd ) {
				NotNullAdd(fieldinfo, sStatement, constraints);
			}

			if( settings.m_bDefaultValueAdd )  {
				DefaultValueAdd(fieldinfo, tabledefinfo, sStatement, recordinfo, nWarningCount, constraints, warnings);
			}

			if( settings.m_bAutoIncrementAdd ) {
				AutoIncrementAdd(fieldinfo, sStatement, constraints);
			}

			bool bRealPrimaryKey = isPrimary && settings.m_PrimaryKeySupport && (!(fieldinfo.m_lAttributes & dbAutoIncrField) || !settings.m_bAutoIncrementAdd);
			if( bRealPrimaryKey ) 
			{
				real_primary_keys.push_back(std::wstring(fieldinfo.m_strName));
			}

			// Primary key already by default is unique.
			// TODO: Multiple primary fields if unique needs to be fixed.
			if( settings.m_bUniqueFieldAdd && !bRealPrimaryKey ) 
			{
				CString sCmp = tabledefinfo.m_strName + fieldinfo.m_strName;
				UniqueFieldAdd(sCmp, UniqueFields, sStatement, constraints);
			}

			if( bIsText && settings.m_bCollateNoCaseFieldsAdd ) {
				sStatement += _T(" COLLATE NOCASE");
				constraints.push_back(_T("COLLATE NOCASE"));
			}
				if( settings.m_bAddComments )
			{
				Temp2 = GetDaoFieldDescription(fieldinfo.m_strName, tabledefinfo.m_strName, db);
				if( !Temp2.IsEmpty() )
				{
					sStatement += _T(" /*");
					sStatement += Temp2;
					sStatement += _T(" */");
				}
			}
			if( i1 != nFieldCount - 1 ) sStatement += (_T(","));
			fields.push_back(CDBFieldIndex(sFieldNameAndType, constraints));	
		}

		std::wstring sPrimaryKeysStatement = L", PRIMARY KEY(";
		size_t nCnt = real_primary_keys.size();
		for(size_t i = 0; i < nCnt; ++i) {
			sPrimaryKeysStatement += real_primary_keys[i];
			if( i != nCnt - 1 ) {
				sPrimaryKeysStatement += L",";
			}
			else {
				sPrimaryKeysStatement += L")";
			}
		}

		if( nCnt > 0 ) {
			sStatement += sPrimaryKeysStatement.c_str();
		}

		sStatement += L");";

	if( settings.m_bRecordAdd ) {
		Records(TableDef, tabledefinfo.m_strName, sFieldnames, InsertStatements);
	}

	return fields;
}

void CFieldStatements::NotNullAdd(const CDaoFieldInfo &fieldinfo,CString &sStatement, std::vector<CString> &constraints)
{
	 if( fieldinfo.m_bRequired ) {
		sStatement += _T(" NOT NULL");
		constraints.push_back("NOT NULL");
	 }
}

void CFieldStatements::AutoIncrementAdd(const CDaoFieldInfo &fieldinfo, CString &sStatement, std::vector<CString> &constraints)
{
	if( fieldinfo.m_lAttributes & dbAutoIncrField ) {
		sStatement += _T(" PRIMARY KEY AUTOINCREMENT");
		constraints.push_back("PRIMARY KEY");
		constraints.push_back("AUTOINCREMENT");
	}
}

void CFieldStatements::DefaultValueAdd( const CDaoFieldInfo &fieldinfo, CDaoTableDefInfo &tabledefinfo, CString &sStatement, CDaoFieldInfo &recordinfo, unsigned &nWarningCount, 
	                                    std::vector<CString> &constraints, std::vector<std::wstring> &warnings)
{
	if( !(fieldinfo.m_strDefaultValue.IsEmpty()) )
	{
		if ( recordinfo.m_nType == dbBoolean )
		{
			if( !(fieldinfo.m_strDefaultValue.CompareNoCase(_T("Yes"))) )
			{
				sStatement += _T(" DEFAULT 1");
				constraints.push_back("DEFAULT 1");
				return;
			}
			if( !(fieldinfo.m_strDefaultValue.CompareNoCase(_T("No"))) )
			{
				sStatement += _T(" DEFAULT 0");
				constraints.push_back("DEFAULT 0");
				return;
			}
			if( !(fieldinfo.m_strDefaultValue.CompareNoCase(_T("true"))) )
			{
				sStatement += _T(" DEFAULT 1");
				constraints.push_back("DEFAULT 1");
				return;
			}
			if( !(fieldinfo.m_strDefaultValue.CollateNoCase(_T("false"))) )
			{
				sStatement += _T(" DEFAULT 0");
				constraints.push_back("DEFAULT 0");
				return;
			}
			if( !(fieldinfo.m_strDefaultValue.CollateNoCase(_T("1"))) )
			{
				sStatement += _T(" DEFAULT 1");
				constraints.push_back("DEFAULT 1");
				return;
			}
			if( !(fieldinfo.m_strDefaultValue.CollateNoCase(_T("0"))) )
			{
				sStatement += _T(" DEFAULT 0");
				constraints.push_back("DEFAULT 0");
				return;
			}

			++nWarningCount;
			std::wstring sErrorMessage = L"Unable to recognize default value: ";
            sErrorMessage += CstringToWxString(fieldinfo.m_strDefaultValue);
            sErrorMessage += L" on table: ";
            sErrorMessage += CstringToWxString(tabledefinfo.m_strName);
            sErrorMessage += L" column: ";
            sErrorMessage += CstringToWxString(fieldinfo.m_strName);
            sErrorMessage += L"\n";
			warnings.push_back(sErrorMessage);
		}
		else 
		{
			CString sDef = _T("DEFAULT ");
            CString sDefaulValue;
            if (fieldinfo.m_strDefaultValue.Find(L"Now()") == -1) 
            {
                sDef += fieldinfo.m_strDefaultValue;
                sDefaulValue = fieldinfo.m_strDefaultValue;
            }
            else 
            {
                sDef += L"CURRENT_TIMESTAMP";
                sDefaulValue = L"CURRENT_TIMESTAMP";
            }

			constraints.push_back(sDef);
			sStatement += _T(" DEFAULT ");
            sStatement += sDefaulValue;
		}
	}
}

void CFieldStatements::FieldTypeAdd(CDaoTableDef &TableDef, const short nType, bool &bIsText, CString &sStatement, CString &sFieldAndType)
{
	CDaoRecordset recordset;
	recordset.Open(&TableDef);
	CString sInt = L"INTEGER";
	CString sReal = L"REAL";
	CString sText = L"TEXT";
	CString sBlob = L"BLOB";
    CString sDateTime = L"DATETIME";

	switch(nType)
	{
		case dbBoolean: 
				sStatement += sInt;
				sFieldAndType += sInt;
				break;
		case dbByte: 
				sStatement += sInt; 
				sFieldAndType += sInt;
				break;
		case dbInteger: 
				sStatement += sInt;
				sFieldAndType += sInt;
				break;
		case dbLong : 
				sStatement += sInt;
				sFieldAndType += sInt;
				break;
		case dbCurrency: 
				sStatement += sInt;
				sFieldAndType += sInt;
				break;
		case dbSingle: 
				sStatement += sInt;
				sFieldAndType += sInt;
				break;
		case dbDouble: 
				sStatement += sReal;
				sFieldAndType += sReal;
				break;
		case dbDate: 
				sStatement += sDateTime;
				sFieldAndType += sDateTime;
				break;
		case dbBinary: 
				sStatement += sBlob; 
				sFieldAndType += sBlob;
				break;
		case dbText: 
				sStatement += sText;
				sFieldAndType += sText;
				bIsText = true; 
				break;
		case dbLongBinary: 
				sStatement += sBlob;
				sFieldAndType += sBlob;
				break;
		case dbMemo: 
				sStatement += sText; 
				sFieldAndType += sText;
				bIsText = true;
				break;
		case dbGUID: 
				ASSERT(FALSE); 
				sStatement += sText;  
				sFieldAndType += sText;
				bIsText = true; 
				break;
		case dbBigInt: 
				sStatement += sInt; 
				sFieldAndType += sInt;
				break;
		case dbVarBinary: 
				sStatement += sBlob; 
				sFieldAndType += sBlob;
				break;
		case dbChar: 
				sStatement += sText; 
				sFieldAndType += sText;
				bIsText = true; 
				break;
		case dbNumeric: 
				sStatement += sInt;
				sFieldAndType += sInt;
				break;
		case dbDecimal: 
				sStatement += sInt; 
				sFieldAndType += sInt;
				break;
		case dbFloat: 
				sStatement += sReal; 
				sFieldAndType += sReal;
				break;
		case dbTime: 
				sStatement += sDateTime;
				sFieldAndType += sDateTime;
				break;
		case dbTimeStamp: 
				ASSERT(FALSE); 
				sStatement += sDateTime;
				sFieldAndType += sDateTime;
				break;
		default:
				break;
	}
}

void CFieldStatements::UniqueFieldAdd(const CString &sCmp, std::vector<CString> &UniqueFields, CString &sStatement, std::vector<CString> &constraints)
{
	for(const auto &it : UniqueFields) {
		if( !(sCmp.Compare(it)) )
		{
			sStatement += _T(" UNIQUE");
			constraints.push_back("UNIQUE");
			break;
		}
	}
}

inline bool GetBinaryFromVariant(COleVariant &ovData, byte **ppBuf, size_t *pcBufLen)
{
        bool fRetVal = false;

    //Binary data is stored in the variant as an array of unsigned char
        if (ovData.vt == (VT_ARRAY|VT_UI1))  // (OLE SAFEARRAY)
        {
        //Retrieve size of array
        *pcBufLen = ovData.parray->rgsabound[0].cElements;

        *ppBuf = new BYTE[*pcBufLen]; //Allocate a buffer to store the data
        if (*ppBuf != NULL)
        {
            void * pArrayData;

            //Obtain safe pointer to the array
            SafeArrayAccessData(ovData.parray,&pArrayData);

            //Copy the bitmap into our buffer
            memcpy(*ppBuf, pArrayData, *pcBufLen);
            
            //Unlock the variant data
            SafeArrayUnaccessData(ovData.parray);
            fRetVal = true;
        }
        }

        return fRetVal;
}

inline std::wstring ConvertFromUTF8(const char *str)
{
    ASSERT(str);

    const auto nLen = ::strlen(str);
    if ( nLen <= 0 ) {
        return L"";
    }

    const int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], nLen, NULL, 0);
    std::wstring wstrTo(size_needed, '\0');
    // Win32's UTF8 transcode since Vista uses SSE internally to great effect
    MultiByteToWideChar(CP_UTF8, 0, &str[0], nLen, &wstrTo[0], size_needed);
    return wstrTo;

    // http://stackoverflow.com/questions/4358870/convert-wstring-to-string-encoded-in-utf-8
    // but:
    // https://stackoverflow.com/questions/26196686/utf8-utf16-codecvt-poor-performance
    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter_utf8_utf16;
    //const std::wstring str_utf16 = converter_utf8_utf16.from_bytes(str);
    //return str_utf16;      
}

inline void hexDump (const void * addr, const int len, std::string &result) {
    const unsigned char * pc = (const unsigned char *)addr;
    for (int i = 0; i < len; i++) 
	{
		char buffer[2];
		_snprintf(buffer, sizeof(buffer), "%02x", pc[i]);
		result += buffer[0];
		result += buffer[1];
    }
}

void CFieldStatements::Records(CDaoTableDef &TableDef, LPCTSTR sTable, std::vector<CString> &sFieldNames, std::vector <CString> &InsertStatements)
{
		COleVariant COlevar;                                        
	    CDaoRecordset recordset;
	    recordset.Open(&TableDef);
	    CString sParrent = L"INSERT INTO   `";
	    sParrent += sTable;
	    sParrent += L"`  (";

		size_t nFieldCount = sFieldNames.size();

		for(size_t i1 = 0; i1 < nFieldCount; ++i1)
		{
				sParrent += L"`";
				sParrent += sFieldNames[i1]; 
				if( i1 != nFieldCount - 1 ) sParrent += L"`, ";
				else sParrent += L"`)";
		}

		CString sStatement;
		InsertStatements.push_back(L"BEGIN TRANSACTION");

		while( !recordset.IsEOF() )
		{ 
			sStatement = sParrent;
			sStatement += L" VALUES (";
			for(size_t i2 = 0; i2 < nFieldCount; ++i2)
			{
				recordset.GetFieldValue(sFieldNames[i2], COlevar);
				if( COlevar.vt == VT_NULL ) sStatement += L"NULL";
				// Special case for bitmaps
				else if( COlevar.vt == 8209 ) {
					unsigned char *pData = nullptr;
					size_t nSize;
					GetBinaryFromVariant(COlevar, &pData, &nSize);
					std::fstream imgout("MyImage.bin", std::ios::out | std::ios::binary);
					imgout.write(reinterpret_cast<char*>(pData), nSize);
					imgout.close();
					std::string sResult;
					hexDump(pData, nSize, sResult); 
					CString temp = ConvertFromUTF8(sResult.c_str()).c_str();
					CString s = L"x'" + temp; 
					s += L"'";
					sStatement += s;
					free(pData);
				}
				else if( COlevar.vt == VT_I2 || COlevar.vt == VT_I4 || COlevar.vt == VT_R4 || COlevar.vt == VT_R8 || COlevar.vt == VT_BOOL || COlevar.vt == VT_CY || COlevar.vt == VT_UI2 || COlevar.vt == VT_UI4 || COlevar.vt ==  VT_I8 || COlevar.vt == VT_UI8 || COlevar.vt == VT_INT || COlevar.vt ==  VT_UINT || COlevar.vt == VT_BLOB ) sStatement += COlevar;
				else 
				{
					sStatement += L"'";
					CString sString = COlevar;
					sString.Replace(L"'", L"''");
					sStatement += sString;
					sStatement += L"'";
				}
				if( i2 != nFieldCount - 1 ) sStatement += L", ";
				else 
				{ 
					sStatement += L");";
					InsertStatements.push_back(sStatement);
				}
			}

			recordset.MoveNext(); 
		}

		InsertStatements.push_back(_T("END TRANSACTION"));          
}

void CFieldStatements::FieldCollation(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &CollateIndexFields, const bool &m_bTrimTextValues)
{
	size_t nFieldCount = TableDef.GetFieldCount(); 
	CString sStatement;

	for(size_t i1 = 0; i1 < nFieldCount; ++i1)
	{
		CDaoFieldInfo fieldinfo;                                          
		TableDef.GetFieldInfo(i1, fieldinfo, AFX_DAO_ALL_INFO); 
		CDaoRecordset rc;
		rc.Open(&TableDef);
		CDaoFieldInfo recordinfo;
		rc.GetFieldInfo(i1, recordinfo);
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