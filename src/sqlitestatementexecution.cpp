#define _AFXDLL
#include "stdafx.h"
#include "sqlitestatementexecution.h"
#include "sqlite3.h"
#include "wxWidgetsLink.h"
#include <wx/gauge.h>
#include <wx/textctrl.h>
#include "UIObs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

  #pragma comment (lib, "../bin/sqlite3.lib")
enum
{
	ExecuteTables = 1 << 0,
	ExecuteInserts = 1 << 1,
	ExecuteIndexes = 1 << 2,
	ExecuteTriggers = 1 << 3,
	ExecuteQueries = 1 << 4,
};

inline bool FileExists(const std::string &name) 
{
    if ( FILE *file = fopen(name.c_str(), "r") ) 
	{
        fclose(file);
        return true;
    } 

    return false;   
}

std::string CSQLiteConversion::ConvertToUTF8(const wchar_t *wstr)
{
        const int nLen = wcslen(wstr);
        if( nLen <= 0  ) return std::string();
        const int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], nLen, &strTo[0], size_needed, NULL, NULL);
        return strTo;
}

void CSQLiteConversion::SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc , CUIObs *pObs, unsigned &nValue,
												 unsigned &nErrorCount, const std::vector<CString> &sTableNames, unsigned &flag, unsigned &nTableCount, std::vector<int> &indextable, const std::vector<CString> &sIndexTableNames)
{
	 char *zErrMsg = 0;
	 unsigned index = 0;
	 auto end_it = statements.end();
     for( auto it = statements.begin(); it != end_it; ++it )
	{
			++nValue;
			pObs->SetValue(nValue);
			std::string sB = ConvertToUTF8(*it);
		    const char* pszC = _strdup(sB.c_str());		
            rc = sqlite3_exec(sqlitedatabase, pszC, NULL, 0, &zErrMsg);
            if( rc != SQLITE_OK )
	          {
				  ++nErrorCount;
				  pObs->SetDefaultStyle(wxTextAttr (wxNullColour, *wxRED));
				  wxString ErrorMessage = wxT("");
				  ErrorMessage += wxString::FromUTF8(zErrMsg);
				  ErrorMessage += " \n";
				  pObs->WriteText(ErrorMessage);
				  CT2CA pszConvertedAnsiString (*it);
	              std::string strStd (pszConvertedAnsiString);
				  ErrorMessage = wxString::FromUTF8(_strdup(strStd.c_str() ) ) + "\n";
				  pObs->WriteText(ErrorMessage);
				  if(flag & 1)
				  {
					ErrorMessage = wxT("Table: ");
					sB = ConvertToUTF8(sTableNames[nValue-1]);
					ErrorMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
					ErrorMessage += wxT(" wasnt created succesfully \n");
					pObs->WriteText(ErrorMessage);
				  }
				  pObs->SetDefaultStyle(wxTextAttr (wxNullColour));
                  sqlite3_free(zErrMsg);
			  }
			else
			{
				if(flag & ExecuteTables)
				{ 
					wxString sMessage = wxT("Table: ");
					sB = ConvertToUTF8(sTableNames[nValue-1]);
					sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
					sMessage += wxT(" created succesfully \n");
					pObs->WriteText(sMessage);
				}
				else if(flag & ExecuteInserts)
				{
					if(*it == "END TRANSACTION")
					{
						sTableNames[index];
						wxString sMessage = wxT("Table: ");
						sB = ConvertToUTF8(sTableNames[index]);
						sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
						sMessage += wxT(" records inserted \n");
						++index;
						pObs->WriteText(sMessage);
					}
				}
				else if(flag & ExecuteIndexes)
				{   
					if( index < nTableCount )
						{
							while( index < indextable.size() && !indextable[index] )
								{	
									wxString sMessage = wxT("indexes of table: ");
									sB = ConvertToUTF8(sIndexTableNames[index]);
									sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
									sMessage += wxT(" created \n");
									pObs->WriteText(sMessage);
									++index;
								}
						}
					if( index < indextable.size() ) indextable[index]--;
					if( index < nTableCount )
						{
							if( !indextable[index] )
							{
								wxString sMessage = wxT("indexes of table: ");
								sB = ConvertToUTF8(sIndexTableNames[index]);
								sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
								sMessage += wxT(" created \n");
								pObs->WriteText(sMessage);
								++index;
							}
						}
					if(index < nTableCount )
						{
							while( index < indextable.size() && indextable[index]  )
								{	
						
									wxString sMessage = wxT("indexes of table: ");
									sB = ConvertToUTF8(sIndexTableNames[index]);
									sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
									sMessage += wxT(" created \n");
									pObs->WriteText(sMessage);
									++index;
								}
						}
				}
				else if( ExecuteQueries & flag ) {
					// Currently no support
				}
		}
	}
}

void CSQLiteConversion::SqliteConversion(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &IndexStatements, 
										 std:: vector<CString> &RelationFields, std::vector<CString> &queries, const char *dPath, CUIObs *pObs, const std::vector<CString> &sTableNames, 
										 const bool &m_bForeignKeySupport, unsigned &nWarningCount, std::vector<int> &indextable, const std::vector<CString> &sIndexTableNames)
 {
	char *zErrMsg			= 0;
	unsigned nErrorCount	= 0;
	unsigned nValue			= 0;
	unsigned flag			= ExecuteTables;
	unsigned nTableCount	= statements.size();
    sqlite3 *sqlitedatabase;
    int rcc;
	if( FileExists(dPath) ) {
		remove(dPath);
	}

    rcc = sqlite3_open(dPath, &sqlitedatabase);

    if( rcc ) {
        exit(0);
    }
	else
	{
		 sqlite3_exec(sqlitedatabase, "PRAGMA synchronous = OFF",		NULL, NULL, &zErrMsg);
		 sqlite3_exec(sqlitedatabase, "PRAGMA journal_mode = MEMORY",	NULL, NULL, &zErrMsg);

		 if( m_bForeignKeySupport ) {
			 sqlite3_exec(sqlitedatabase, "PRAGMA foreign_keys = ON", NULL, NULL, &zErrMsg);
		 }

		 SqliteStatementExecution(statements, sqlitedatabase, rcc, pObs, nValue, nErrorCount, sTableNames, flag, nTableCount, indextable, sIndexTableNames);
		 flag = ExecuteInserts;
		 SqliteStatementExecution(InsertStatements, sqlitedatabase, rcc, pObs, nValue, nErrorCount, sTableNames, flag, nTableCount, indextable, sIndexTableNames);
		 flag = ExecuteTriggers;
		 pObs->WriteText(wxT("Starting trigger creation \n"));
		 SqliteStatementExecution(RelationFields, sqlitedatabase, rcc, pObs, nValue,  nErrorCount, sTableNames, flag, nTableCount, indextable, sIndexTableNames);
		 pObs->WriteText(wxT("All triggers created \n"));
		 flag = ExecuteQueries;
		 SqliteStatementExecution(queries, sqlitedatabase, rcc, pObs, nValue,  nErrorCount, sTableNames, flag, nTableCount, indextable, sIndexTableNames);
		 flag = ExecuteIndexes;
		 pObs->WriteText(wxT("Starting index creation \n"));
	     SqliteStatementExecution(IndexStatements, sqlitedatabase, rcc, pObs, nValue, nErrorCount, sTableNames, flag, nTableCount, indextable, sIndexTableNames);
		 pObs->WriteText(wxT("All indexes created \n"));
	 }

	 wxString ConclusionMessage = wxT("Statements executed successfully: ");
	 ConclusionMessage << nValue-nErrorCount;
	 pObs->SetDefaultStyle(wxTextAttr (*wxBLUE));
	 pObs->WriteText(ConclusionMessage);
	 ConclusionMessage = wxT("\nErrors: ");
	 ConclusionMessage << nErrorCount;
	 pObs->WriteText(ConclusionMessage);
	 ConclusionMessage = wxT(", Warnings: ");
	 ConclusionMessage << nWarningCount;
	 pObs->WriteText(ConclusionMessage);
     sqlite3_close(sqlitedatabase);
 }