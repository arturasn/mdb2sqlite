#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
#define __INCLUDED_SQLITESTATEMENTEXECUTION_H__

#pragma once

#include <vector>

class CUIObs;
struct sqlite3;

class CSQLiteConversion
{
public: 
	static void SqliteConversion(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &IndexStatements,  
	                             std:: vector<CString> &RelationFields , std::vector<CString> &queries, const char* dPath, CUIObs *pObs, const std::vector<CString> &sTableNames, 
								 const bool &m_bForeignKeySupport, unsigned &nWarningCount, std::vector<int> &indextable, const std::vector<CString> &sIndexTableNames);
private:		
	static void SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc, CUIObs *pObs, unsigned &nValue, 
										 unsigned &nErrorCount, const std::vector<CString> &sTableNames, unsigned &flag,
										 unsigned &nTableCount, std::vector<int> &indextable, const std::vector<CString> &sIndexTableNames);

	static std::string ConvertToUTF8(const wchar_t *wstr);
	static inline bool exists_test1 (const std::string &name);
};
#endif 