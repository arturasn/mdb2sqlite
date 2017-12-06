#ifndef __INCLUDED_FIELDSTATEMENTS_H__
#define __INCLUDED_FIELDSTATEMENTS_H__

#pragma once

class CDaoDatabase;
class CDaoTableDef;
class wxTextCtrl;
class wxString;
struct CDaoTableDefInfo;
struct CDaoFieldInfo;

#include "OnAction.h"

class CFieldStatements
{
public:
	static void fFields(CDaoDatabase &db, CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &InsertStatements, std::vector<CString> &UniqueFields, 
		        CSettings &settings, CString &sStatement, CString (&ReservedKeyWords)[124], std::vector<CString> &TableField, std::vector<CString> &IndexInfo, unsigned &nWarningCount, 
				wxTextCtrl *PrgDlg = NULL);
	static void Records(CDaoTableDef &TableDef, LPCTSTR sTable, std::vector<CString> &sFieldNames, std::vector<CString> &InsertStatements);
	static void FieldCollation(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &CollateIndexFields, const bool &m_bTrimTextValues);
	static wxString CstringToWxString(const CString &ConversionString);
	
private:
	static void NotNullAdd(const CDaoFieldInfo &fieldinfo,CString &sStatement);
	static void AutoIncrementAdd( const CDaoFieldInfo &fieldinfo, CString &sStatement);
	static void DefaultValueAdd( const CDaoFieldInfo &fieldinfo,CDaoTableDefInfo &tabledefinfo,CString &sStatement, CDaoFieldInfo &recordinfo, unsigned &nWarningCount, wxTextCtrl *PrgDlg = NULL);
	static void FieldTypeAdd(CDaoTableDef &TableDef, const short nType, bool &bIsText, CString &sStatement);
	static void UniqueFieldAdd(const CString &sCmp, std::vector<CString> &UniqueFields,CString &sStatement);
	static CString GetDaoFieldDescription(CString& strFieldName, CString& strTableName, CDaoDatabase& db);
	static bool GetPrimaryKey(CString &sTableName, CString &sFieldName,std::vector<CString> &indexinfo);
	
};
#endif