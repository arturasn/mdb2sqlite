
#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
#define __INCLUDED_SQLITESTATEMENTEXECUTION_H__
#pragma once

#include "stdafx.h"
#include "naujastestas.h"
#include "sqlite3.h"
#include <stdio.h>
#include <vector>

class CSQLiteConversion
{
public: 
	void SqliteConversion(std::vector<CString> &statements, std::vector<CString> &IndexStatements,  std:: vector<CString> &RelationFields ,char* dPath);
private:		
	void SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc);
	std::string ConvertToUTF8(const wchar_t *wstr);
};
#endif