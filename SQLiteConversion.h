//#include "stdafx.h"
//#include "naujastestas.h"
//#include <iostream>
//#include "sqlite3.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <fstream>
//#include <afxdao.h>
//#include <afxdb.h>
//#include <vector>
//#include <string>
//#include "SimpleIni.h"





#include <afxstr.h>
#include "RelationshipStatements.h"

using namespace std;

CString a;


class CSQLiteConversion
{
public: 
	void SqliteConversion(std::vector <CString> const &statements, std::vector <CString> const &IndexStatements,  std:: vector <CString> const &RelationFields);
private:		
	void SqliteStatementExecution(std::vector <CString> const &statements, sqlite3 *&sqlitedatabase, int rc);
    static int callback(void *NotUsed, int argc, char **argv, char **azColName);
	CString ConvertToUTF8(const wchar_t *wstr);
}