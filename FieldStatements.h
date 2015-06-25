#ifndef __INCLUDED_FIELDSTATEMENTS_H__
#define __INCLUDED_FIELDSTATEMENTS_H__
#pragma once

#include "stdafx.h"
#include "naujastestas.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>

#pragma warning(disable : 4995)

class CFieldStatements
{
public:
	void fFields(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector <CString> &statements, std::vector <CString> &UniqueFields,CSettings &settings);
	void Records(CDaoTableDef &TableDef, const CDaoTableDefInfo &tabledefinfo, short nFieldCount, CString *&sFieldnames, std::vector <CString> &statements);
	void FieldCollation(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector <CString> &CollateIndexFields, const bool &m_bTrimTextValues);
private:
	void NotNullAdd(const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements);
	void AutoIncrementAdd( const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements);
	void DefaultValueAdd( const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements);
	void FieldTypeAdd(CDaoTableDef &TableDef, const CDaoFieldInfo &recordinfo, std::vector <CString> &statements, bool &bIsText);
	void UniqueFieldAdd(std::vector <CString> &statements, const CDaoFieldInfo &fieldinfo, const CDaoTableDefInfo &tabledefinfo, std::vector <CString> &UniqueFields);
};
#endif