
#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
#define __INCLUDED_SQLITESTATEMENTEXECUTION_H__
#pragma once

#include "stdafx.h"
#include "naujastestas.h"
#include "sqlite3.h"
#include <stdio.h>
#include <vector>

#pragma warning(disable : 4995)

class CSQLiteConversion
{
public: 
	void SqliteConversion(std::vector <CString> const &statements, std::vector <CString> const &IndexStatements,  std:: vector <CString> const &RelationFields ,char* dPath);
private:		
	void SqliteStatementExecution(std::vector <CString> const &statements, sqlite3 *&sqlitedatabase, int rc);
	std::string ConvertToUTF8(const wchar_t *wstr);
};
#endif