#ifndef INCLUDED_INDEXSTATEMENTS_H
#define INCLUDED_INDEXSTATEMENTS_H
#pragma once

#include "stdafx.h"

#include "stdafx.h"
#include "stdafx.h"
#include "naujastestas.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>

class CIndexStatements
{
public: 
	void Indexes(CDaoTableDef &TableDef, std::vector <CString> &IndexStatements, const CDaoTableDefInfo &tabledefinfo, CString *&sTableNames, const short &nTableCount, std::vector <CString> &UniqueFields);
	bool IndexFilter(const CDaoTableDefInfo &tabledefinfo, const CDaoIndexInfo &indexinfo, CString *&sTableNames, const short &nTableCount);
};
#endif