
#ifndef INCLUDED_SQLITESTATEMENTEXECUTION_H
#define INCLUDED_SQLITESTATEMENTEXECUTION_H
#pragma once

#include "stdafx.h"
#include "naujastestas.h"
#include "sqlite3.h"
#include <stdio.h>
#include <vector>

class CSQLiteConversion
{
public: 
	void SqliteConversion(std::vector <CString> const &statements, std::vector <CString> const &IndexStatements,  std:: vector <CString> const &RelationFields);
private:		
	void SqliteStatementExecution(std::vector <CString> const &statements, sqlite3 *&sqlitedatabase, int rc);
    static int callback(void *NotUsed, int argc, char **argv, char **azColName);
	CString ConvertToUTF8(const wchar_t *wstr);
};
#endif