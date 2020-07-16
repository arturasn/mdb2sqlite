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

#ifndef __DB_FIELD_H__
	#include "DBField.h"
#endif

class CFieldStatements
{
public:
	static std::vector<CDBFieldIndex> fFields(CDaoDatabase &db, CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &InsertStatements, std::vector<CString> &UniqueFields, 
		        CSettings &settings, CString &sStatement, CString (&ReservedKeyWords)[124], std::vector<CString> &TableField, std::vector<CString> &IndexInfo, unsigned &nWarningCount, 
				std::vector<std::wstring> &warnings);
	static void Records(CDaoTableDef &TableDef, LPCTSTR sTable, std::vector<CString> &sFieldNames, std::vector<CString> &InsertStatements);
	static void FieldCollation(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &CollateIndexFields, const bool &m_bTrimTextValues);
	static wxString CstringToWxString(const CString &ConversionString);
	
private:
	static void NotNullAdd(const CDaoFieldInfo &fieldinfo,CString &sStatement, std::vector<CString> &constraints);
	static void AutoIncrementAdd( const CDaoFieldInfo &fieldinfo, CString &sStatement, std::vector<CString> &constraints);
	static void DefaultValueAdd( const CDaoFieldInfo &fieldinfo,CDaoTableDefInfo &tabledefinfo,CString &sStatement, CDaoFieldInfo &recordinfo, unsigned &nWarningCount, std::vector<CString> &constraints, std::vector<std::wstring> &warnings);
	static void FieldTypeAdd(CDaoTableDef &TableDef, const short nType, bool &bIsText, CString &sStatement, CString &sFieldAndType);
	static void UniqueFieldAdd(const CString &sCmp, std::vector<CString> &UniqueFields,CString &sStatement, std::vector<CString> &constraints);
	static CString GetDaoFieldDescription(CString& strFieldName, CString& strTableName, CDaoDatabase& db);
	static bool GetPrimaryKey(CString &sTableName, CString &sFieldName,std::vector<CString> &indexinfo);
	
};
#endif