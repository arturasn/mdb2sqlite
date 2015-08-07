#define _AFXDLL



#include "stdafx.h"
#include "sqlitestatementexecution.h"
#include "sqlite3.h"
#include <wx/gauge.h>
#include <wx/textctrl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

  #pragma comment (lib, "sqlite3.lib")
enum
{
	ExecuteTables = 1 << 0,
	ExecuteInserts = 1 << 1,
	ExecuteIndexes = 1 << 2,
	ExecuteTriggers = 1 << 3
};

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
void CSQLiteConversion::SqliteStatementExecution(std::vector<CString> &statements, sqlite3 *&sqlitedatabase, int rc , wxGauge *&gauge, unsigned &nValue, wxTextCtrl *&PrgDlg, 
	                                             unsigned &nErrorCount, CString *sTableNames, unsigned &flag, unsigned &nTableCount, int *IndexTable, CString *sTableNames2)
	{
	 char *zErrMsg = 0;
	 unsigned index = 0;
	 auto end_it = statements.end();
     for( auto it = statements.begin(); it != end_it; ++it )
		{
			++nValue;
			gauge ->SetValue(nValue);
			std::string sB = ConvertToUTF8(*it);
		    const char* pszC = _strdup(sB.c_str());		
            rc = sqlite3_exec(sqlitedatabase, pszC, NULL, 0, &zErrMsg);
            if( rc != SQLITE_OK )
	          {
				  ++nErrorCount;
				  PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour, *wxRED));
				  wxString ErrorMessage = wxT("");
				  ErrorMessage += wxString::FromUTF8(zErrMsg);
				  ErrorMessage += " \n";
				  PrgDlg->WriteText(ErrorMessage);
				  CT2CA pszConvertedAnsiString (*it);
	              std::string strStd (pszConvertedAnsiString);
				  ErrorMessage = wxString::FromUTF8(_strdup(strStd.c_str() ) ) + "\n";
				  PrgDlg->WriteText(ErrorMessage);
				  if(flag & 1)
				  {
					ErrorMessage = wxT("Table: ");
					sB = ConvertToUTF8(sTableNames[nValue-1]);
					ErrorMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
					ErrorMessage += wxT(" wasnt created succesfully \n");
					PrgDlg->WriteText(ErrorMessage);
				  }
				  PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
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
					PrgDlg->WriteText(sMessage);
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
						PrgDlg->WriteText(sMessage);
					}
				}
				else if(flag & ExecuteIndexes)
				{   
					if( index < nTableCount )
						{
							while( !IndexTable[index] )
								{	
									wxString sMessage = wxT("indexes of table: ");
									sB = ConvertToUTF8(sTableNames2[index]);
									sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
									sMessage += wxT(" created \n");
									PrgDlg->WriteText(sMessage);
									++index;
								}
						}
					IndexTable[index]--;
					if( index < nTableCount )
						{
							if( !IndexTable[index] )
							{
								wxString sMessage = wxT("indexes of table: ");
								sB = ConvertToUTF8(sTableNames2[index]);
								sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
								sMessage += wxT(" created \n");
								PrgDlg->WriteText(sMessage);
								++index;
							}
						}
					if(index < nTableCount )
						{
							while( !IndexTable[index] )
								{	
						
									wxString sMessage = wxT("indexes of table: ");
									sB = ConvertToUTF8(sTableNames2[index]);
									sMessage += wxString::FromUTF8(_strdup(sB.c_str() ) );
									sMessage += wxT(" created \n");
									PrgDlg->WriteText(sMessage);
									++index;
								}
						}
					
				}
				else if(flag & ExecuteTriggers)
				{
					wxString sMessage = wxT("Trigger created successfully \n");
					PrgDlg->WriteText(sMessage);
				}
			}
		 }
   }
void CSQLiteConversion::SqliteConversion(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &IndexStatements, 
	                                     std:: vector<CString> &RelationFields , const char *dPath, wxGauge *&gauge, wxTextCtrl *&PrgDlg, CString *&sTableNames, 
										 const bool &m_bForeignKeySupport, unsigned &nWarningCount, int *&IndexTable, CString *&sTableNames2)
 {
	char *zErrMsg = 0;
	unsigned nErrorCount = 0;
	unsigned nValue = 0;
	unsigned flag = ExecuteTables;
	unsigned nTableCount = statements.size();
    sqlite3 *sqlitedatabase;
    int  rcc;
	if( FileExists(dPath) )
	{
		remove(dPath);
	}
    rcc = sqlite3_open(dPath, &sqlitedatabase);
    if( rcc )
	  {
         exit(0);
      }
	else
	  {
		 sqlite3_exec(sqlitedatabase, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
		 sqlite3_exec(sqlitedatabase, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);
		 if( m_bForeignKeySupport )
			 sqlite3_exec(sqlitedatabase, "PRAGMA foreign_keys = ON", NULL, NULL, &zErrMsg);
		 SqliteStatementExecution(statements, sqlitedatabase, rcc, gauge, nValue, PrgDlg, nErrorCount, sTableNames, flag, nTableCount);
		 flag = ExecuteInserts;
		 SqliteStatementExecution(InsertStatements, sqlitedatabase, rcc, gauge, nValue, PrgDlg, nErrorCount, sTableNames, flag, nTableCount);
		 flag = ExecuteTriggers;
		 SqliteStatementExecution(RelationFields, sqlitedatabase, rcc, gauge, nValue, PrgDlg,  nErrorCount, sTableNames, flag, nTableCount);
		 wxString sMessage = wxT("All triggers created \n");
		 PrgDlg->WriteText(sMessage);
		 flag = ExecuteIndexes;
	     SqliteStatementExecution(IndexStatements, sqlitedatabase, rcc, gauge, nValue, PrgDlg, nErrorCount, sTableNames, flag, nTableCount, IndexTable, sTableNames2);
	  }
	 wxString ConclusionMessage = wxT("Statements executed successfully: ");
	 ConclusionMessage << nValue-nErrorCount;
	 PrgDlg->SetDefaultStyle(wxTextAttr (*wxBLUE));
	 PrgDlg->WriteText(ConclusionMessage);
	 ConclusionMessage = wxT("\nErrors: ");
	 ConclusionMessage << nErrorCount;
	 PrgDlg->WriteText(ConclusionMessage);
	 ConclusionMessage = wxT(", Warnings: ");
	 ConclusionMessage << nWarningCount;
	 PrgDlg->WriteText(ConclusionMessage);
     sqlite3_close(sqlitedatabase);
 }