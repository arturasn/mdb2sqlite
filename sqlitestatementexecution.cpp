#include "stdafx.h"
#include "sqlitestatementexecution.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

  #pragma comment (lib, "sqlite3.lib")

std::string CSQLiteConversion::ConvertToUTF8(const wchar_t *wstr)
    {
        const int nLen = wcslen(wstr);
        if( nLen <= 0  ) {
            return std::string();
        }

        const int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, NULL, 0, NULL, NULL);

        std::string strTo(size_needed, 0);
		
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
void CSQLiteConversion::SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc)
	{
	 char *zErrMsg = 0;
	 auto end_it = statements.end();
      for (auto it = statements.begin(); it != end_it; ++it)
	 {
		std::string sB = ConvertToUTF8(*it);
        const char* pszC = _strdup(sB.c_str());		
        rc = sqlite3_exec(sqlitedatabase, pszC, NULL, 0, &zErrMsg);
        if( rc != SQLITE_OK )
	      {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);

          }
	    else
          fprintf(stdout, "Table created successfully\n");
	 }
   }
void CSQLiteConversion::SqliteConversion(std::vector<CString> &statements, std::vector<CString> &IndexStatements, std::vector<CString> &RelationFields , char* dPath)
 {
	char *zErrMsg = 0;
    sqlite3 *sqlitedatabase;
    int  rcc;
    rcc = sqlite3_open(dPath, &sqlitedatabase);
     if( rcc )
	   {
         fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sqlitedatabase));
         exit(0);
       }
	 else
	   {
         fprintf(stdout, "Opened database successfully\n");
		 sqlite3_exec(sqlitedatabase, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
		 sqlite3_exec(sqlitedatabase, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);
	     SqliteStatementExecution(statements,sqlitedatabase,rcc);
		 SqliteStatementExecution(RelationFields,sqlitedatabase,rcc);
	     SqliteStatementExecution(IndexStatements,sqlitedatabase,rcc);
	   }
     sqlite3_close(sqlitedatabase);
 }