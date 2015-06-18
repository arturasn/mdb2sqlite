#ifndef INCLUDED_FIELDSTATEMENTS_H
#define INCLUDED_FIELDSTATEMENTS_H
#pragma once

#include "stdafx.h"
#include "stdafx.h"
#include "stdafx.h"
#include "naujastestas.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>

class CFieldStatements
{
public:
	void fFields(CDaoTableDef &TableDef, CDaoTableDefInfo &tabledefinfo, std::vector <CString> &statements, std::vector <CString> &UniqueFields,CSettings &CSettingsObject);
	void Records(CDaoTableDef &TableDef, const CDaoTableDefInfo &tabledefinfo, short nFieldCount, CString *&sFieldnames, std::vector <CString> &statements);
private:
	void NotNullAdd(const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements);
	void AutoIncrementAdd( const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements);
	void DefaultValueAdd( const CDaoFieldInfo &fieldinfo, std::vector <CString> &statements);
	void FieldTypeAdd(CDaoTableDef &TableDef, const CDaoFieldInfo &recordinfo, std::vector <CString> &statements);
	void UniqueFieldAdd(std::vector <CString> &statements, const CDaoFieldInfo &fieldinfo, const CDaoTableDefInfo &tabledefinfo, std::vector <CString> &UniqueFields);
};
#endif