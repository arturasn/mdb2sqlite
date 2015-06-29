#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
#define __INCLUDED_SQLITESTATEMENTEXECUTION_H__

#pragma once

#include <vector>

struct sqlite3;

class CSQLiteConversion
{
public: 
	void SqliteConversion(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &IndexStatements,  std:: vector<CString> &RelationFields , char* dPath);
private:		
	void SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc);
	std::string ConvertToUTF8(const wchar_t *wstr);
	inline bool exists_test1 (const std::string &name);
};
#endif