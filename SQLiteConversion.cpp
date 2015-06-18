#include "stdafx.h"
#include "naujastestas.h"
#include <iostream>
#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <afxdao.h>
#include <afxdb.h>
#include <vector>
#include <string>
#include "SimpleIni.h"
#include "SQLiteConversion.h"
#include <afxstr.h>
CStringT CSQLiteConversion::ConvertToUTF8(const wchar_t *wstr)
    {
        const int nLen = wcslen(wstr);
        if( nLen <= 0  ) {
            return CString();
        }

        const int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, NULL, 0, NULL, NULL);

        std::string strTo(size_needed, 0);
		
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, &strTo[0], size_needed, NULL, NULL);
        CString sToCS;
        sToCS.Format(_T("%S"), strTo.c_str());
        return sToCS;
    }
static int CSQLiteConversion::callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
void CSQLiteConversion::SqliteStatementExecution(std::vector <CString> const &statements, sqlite3 *&sqlitedatabase, int rc)
	{
	 char *zErrMsg = 0;
	 unsigned nVectorLength = statements.size();
	 for(unsigned i = 0; i < nVectorLength; i++)
	 {
		ConvertToUTF8(statements[i]);
	    LPCTSTR pszA = statements[i];
        CStringA sB(pszA);
        const char* pszC = sB; 
        rc = sqlite3_exec(sqlitedatabase, pszC, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK )
	      {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
			wcout << (LPCTSTR)statements[i] << endl;
          }
	    else
          fprintf(stdout, "Table created successfully\n");
	 }
   }
void CSQLiteConversion::SqliteConversion(std::vector <CString> const &statements, std::vector <CString> const &IndexStatements, std::vector <CString> const &RelationFields)
 {
    sqlite3 *sqlitedatabase;
    int  rcc;
     /* Open database */
     rcc = sqlite3_open("testas21.sqlite", &sqlitedatabase);
     if( rcc )
	 {
         fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sqlitedatabase));
         exit(0);
     }
	 else
	 {
        fprintf(stdout, "Opened database successfully\n");
        /* Execute SQL statements */
	    SqliteStatementExecution(statements,sqlitedatabase,rcc);
		SqliteStatementExecution(RelationFields,sqlitedatabase,rcc);
	    SqliteStatementExecution(IndexStatements,sqlitedatabase,rcc);
	 }
     sqlite3_close(sqlitedatabase);
 }