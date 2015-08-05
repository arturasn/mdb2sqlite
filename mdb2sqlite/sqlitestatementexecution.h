#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
#define __INCLUDED_SQLITESTATEMENTEXECUTION_H__

#pragma once

#include <vector>

class wxGauge;
class wxTextCtrl;

struct sqlite3;

class CSQLiteConversion
{
public: 
	static void SqliteConversion(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &IndexStatements,  
	                             std:: vector<CString> &RelationFields , const char* dPath, wxGauge *&gauge, wxTextCtrl *&PrgDlg, CString *&sTableNames, 
								 const bool &m_bForeignKeySupport);
private:		
	static void SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc , wxGauge *&gauge, unsigned &nValue, wxTextCtrl *&PrgDlg, 
		                                 unsigned &nErrorCount, CString *sTableNames = NULL);
	static std::string ConvertToUTF8(const wchar_t *wstr);
	static inline bool exists_test1 (const std::string &name);
};
#endif