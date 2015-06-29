#ifndef __INCLUDED_FIELDSTATEMENTS_H__
#define __INCLUDED_FIELDSTATEMENTS_H__

#pragma once

#include <afxdao.h>
#include <vector>
#include "naujastestas.h"

class CFieldStatements
{
public:
	static void fFields(CDaoDatabase &db, CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &InsertStatements, std::vector<CString> &UniqueFields, CSettings &settings, CString &sStatement);
	static void Records(CDaoTableDef &TableDef, const CDaoTableDefInfo &tabledefinfo, short nFieldCount, CString *&sFieldnames, std::vector<CString> &InsertStatements);
	static void FieldCollation(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &CollateIndexFields, const bool &m_bTrimTextValues);
	
private:
	static void NotNullAdd(const CDaoFieldInfo &fieldinfo,CString &sStatement);
	static void AutoIncrementAdd( const CDaoFieldInfo &fieldinfo, CString &sStatement);
	static void DefaultValueAdd( const CDaoFieldInfo &fieldinfo,CDaoTableDefInfo &tabledefinfo,CString &sStatement);
	static void FieldTypeAdd(CDaoTableDef &TableDef, const CDaoFieldInfo &recordinfo, bool &bIsText, CString &sStatement);
	static void UniqueFieldAdd(const CDaoFieldInfo &fieldinfo, const CDaoTableDefInfo &tabledefinfo, std::vector<CString> &UniqueFields,CString &sStatement);
	static CString GetDaoFieldDescription(CString& strFieldName, CString& strTableName, CDaoDatabase& db);
};
#endif