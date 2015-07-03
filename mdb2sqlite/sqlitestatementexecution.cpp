#include "stdafx.h"
#include "sqlitestatementexecution.h"
#include "sqlite3.h"
#include <wx/gauge.h>
#include <wx/textctrl.h>
//#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

  #pragma comment (lib, "sqlite3.lib")

inline bool FileExists(const std::string &name) 
{
    if ( FILE *file = fopen(name.c_str(), "r") ) 
	{
        fclose(file);
        return true;
    } 
	else 
        return false;   
}
std::string CSQLiteConversion::ConvertToUTF8(const wchar_t *wstr)
    {
        const int nLen = wcslen(wstr);
        if( nLen <= 0  )
            return std::string();
        const int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
void CSQLiteConversion::SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc , wxGauge *&gauge, int &nValue, wxTextCtrl *&PrgDlg)
	{
	 char *zErrMsg = 0;
	 auto end_it = statements.end();
     for ( auto it = statements.begin(); it != end_it; ++it )
		{
			++nValue;
			gauge ->SetValue(nValue);
			std::string sB = ConvertToUTF8(*it);
		    const char* pszC = _strdup(sB.c_str());		
            rc = sqlite3_exec(sqlitedatabase, pszC, NULL, 0, &zErrMsg);
            if( rc != SQLITE_OK )
	          {
           //     fprintf(stderr, "SQL error: %s\n", zErrMsg);
				  PrgDlg->SetDefaultStyle(wxTextAttr (*wxRED));
				  wxString ErrorMessage = wxT("      ");
				  ErrorMessage += wxString::FromUTF8(zErrMsg);
				  ErrorMessage += " \n";
				  PrgDlg->WriteText(ErrorMessage);
				  PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
                  sqlite3_free(zErrMsg);
			  }
			else
			{
				//fprintf(stdout, "Table created successfully\n");
				
				PrgDlg->WriteText(wxT("Table created succesfully \n"));
			}
		 }
   }
void CSQLiteConversion::SqliteConversion(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &IndexStatements, 
	std:: vector<CString> &RelationFields , const char *dPath, wxGauge *&gauge, wxTextCtrl *&PrgDlg)
 {
	char *zErrMsg = 0;
    sqlite3 *sqlitedatabase;
    int  rcc;
	if( FileExists(dPath) )
	{
		//printf( "%s %s %s" , "Target file:", dPath, " will be overwritten. \n");
		remove(dPath);
	}
    rcc = sqlite3_open(dPath, &sqlitedatabase);
    if( rcc )
	  {
        // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sqlitedatabase));
         exit(0);
      }
	else
	  {
		  int nValue=0;
         //  fprintf(stdout, "Opened database successfully\n");
		 sqlite3_exec(sqlitedatabase, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
		 sqlite3_exec(sqlitedatabase, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);
	     SqliteStatementExecution(statements,sqlitedatabase,rcc, gauge,nValue,PrgDlg);
		 SqliteStatementExecution(InsertStatements,sqlitedatabase,rcc, gauge,nValue,PrgDlg);
		 SqliteStatementExecution(RelationFields,sqlitedatabase,rcc,gauge,nValue,PrgDlg);
	     SqliteStatementExecution(IndexStatements,sqlitedatabase,rcc,gauge,nValue,PrgDlg);
	  }
    sqlite3_close(sqlitedatabase);
 }