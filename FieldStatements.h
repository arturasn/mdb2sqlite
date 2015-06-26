#ifndef __INCLUDED_FIELDSTATEMENTS_H__
#define __INCLUDED_FIELDSTATEMENTS_H__

#pragma once

#ifndef __INCLUDED_NAUJASTESTAS_H__
	#include "naujastestas.h"
#endif

class CFieldStatements
{
public:
	void fFields(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &InsertStatements, std::vector<CString> &UniqueFields,CSettings &settings ,CString &sStatement);
	void Records(CDaoTableDef &TableDef, const CDaoTableDefInfo &tabledefinfo, short nFieldCount, CString *&sFieldnames, std::vector<CString> &InsertStatements);
	void FieldCollation(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector<CString> &CollateIndexFields, const bool &m_bTrimTextValues);
private:
	void NotNullAdd(const CDaoFieldInfo &fieldinfo,CString &sStatement);
	void AutoIncrementAdd( const CDaoFieldInfo &fieldinfo, CString &sStatement);
	void DefaultValueAdd( const CDaoFieldInfo &fieldinfo,CDaoTableDefInfo &tabledefinfo,CString &sStatement);
	void FieldTypeAdd(CDaoTableDef &TableDef, const CDaoFieldInfo &recordinfo, bool &bIsText, CString &sStatement);
	void UniqueFieldAdd(const CDaoFieldInfo &fieldinfo, const CDaoTableDefInfo &tabledefinfo, std::vector<CString> &UniqueFields,CString &sStatement);
};
#endif