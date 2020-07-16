#ifndef __INCLUDED_INDEXSTATEMENTS_H__
#define __INCLUDED_INDEXSTATEMENTS_H__

#pragma once

class CDaoTableDef;
struct CDaoTableDefInfo;
struct CDaoIndexInfo;
class wxTextCtrl;

#include <vector>

#ifndef _WX_WIDGETS_LINK_H_
	#include "wxWidgetsLink.h"
#endif

#ifndef __DB_FIELD_H__
	#include "DBField.h"
#endif

#ifndef _WX_WXSTRING_H__
	#include "wx/string.h"
#endif

class CIndexStatements
{
public: 
	static std::vector<CDBFieldIndex> Indexes(CDaoTableDef &TableDef, std::vector<CString> &IndexStatements, const CDaoTableDefInfo &tabledefinfo, std::vector<CString> &sTableNames, 
		                std::vector<CString> &UniqueFields, std::vector<CString> &CollateIndexFields, const bool &m_bCollateNoCaseIndexAdd, const bool &m_bTrimTextValues,  
						const bool &bKeyWordList, CString (&ReservedKeyWords)[124], std::vector<CString> &IndexInfo, unsigned &nWarningCount, std::vector<int> &indextable, std::vector<std::wstring> &warnings);

	static bool IndexFilter(const CString &sTable, const CString &sIndexTableName, std::vector<CString> &sTableNames);
private:
	static bool IsIndexFieldText(CString sParrent , std::vector <CString> &CollateIndexFields);
};

#endif