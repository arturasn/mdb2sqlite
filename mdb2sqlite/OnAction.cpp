#define _AFXDLL
#include "stdafx.h"
#include "SimpleIni.h"
#include <afxdao.h>
#ifndef __INCLUDED_INDEXSTATEMENTS_H__
 #include "IndexStatements.h"
#endif
#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
	#include "sqlitestatementexecution.h"
#endif
#ifndef __INCLUDED_RELATIONSHIPSTATEMENTS_H__
	#include "Relationshipstatements.h"
#endif
#ifndef __INCLUDED_FIELDSTATEMENTS_H__
  #include "FieldStatements.h"
#endif
#include <wx/gauge.h>
#include <wx/textctrl.h>
#include <fstream>
#include <codecvt>


void CSettingsReader::ReadFromCSimpleIni(CSettings &settings)
{
	CSimpleIni ini;
    ini.SetUnicode();
    ini.LoadFile("Settings.ini");
	settings.m_bRelationshipAdd = ini.GetBoolValue(_T("Settings"),_T("RelationshipAdd"),true);
	settings.m_bRecordAdd = ini.GetBoolValue(_T("Settings"),_T("RecordAdd"),true);
	settings.m_bNotNullAdd = ini.GetBoolValue(_T("Settings"),_T("NotNullAdd"),true);
	settings.m_bAutoIncrementAdd = ini.GetBoolValue(_T("Settings"),_T("AutoIncrementAdd"),true);
	settings.m_bDefaultValueAdd = ini.GetBoolValue(_T("Settings"),_T("DefaultValueAdd"),true);
	settings.m_bIndexAdd = ini.GetBoolValue(_T("Settings"),_T("IndexAdd"),true);
	settings.m_bUniqueFieldAdd = ini.GetBoolValue(_T("Settings"),_T("UniqueFieldAdd"),true);
	settings.m_bCollateNoCaseIndexAdd = ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForIndex"),true);
	settings.m_bCollateNoCaseFieldsAdd = ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForFields"),true);
	settings.m_bTrimTextValues = ini.GetBoolValue(_T("Settings"),_T("TrimTextValues"),true);
	settings.m_bAddComments = ini.GetBoolValue(_T("Settings"),_T("AddComments"),true);
	settings.m_bKeyWordList = ini.GetBoolValue(_T("Settings"),_T("KeyWordList"),true);
	settings.m_bForeignkeySupport = ini.GetBoolValue(_T("Settings"),_T("ForeignKeySupport"),true);
	settings.m_PrimaryKeySupport = ini.GetBoolValue(_T("Settings"),_T("PrimaryKeySupport"),true);
}
void CSettingsReader::Dumping(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &RelationFields, std::vector<CString> &IndexStatements, 
	                          const char *&dPath)
{
	std::wofstream dumpfile(dPath);
	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
    dumpfile.imbue(loc);
	auto Statementsize = statements.end();
	auto InsertStatementsize = InsertStatements.end();
	auto RelationFieldssize = RelationFields.end();
	auto IndexStatementsize = IndexStatements.end();
	for( auto it = statements.begin(); it != Statementsize; ++it)
		dumpfile << (LPCTSTR)*it << std::endl;
	for( auto it = InsertStatements.begin(); it != InsertStatementsize; ++it)
	{
		if(*it != _T("BEGIN TRANSACTION") && *it != _T("END TRANSACTION"))
		   dumpfile << (LPCTSTR)*it << std::endl;
		else dumpfile << std::endl;
	}
	dumpfile << std::endl;
	for( auto it = RelationFields.begin(); it != RelationFieldssize; ++it)
		dumpfile << (LPCTSTR)*it << std::endl;
	dumpfile << std::endl;
	for( auto it = IndexStatements.begin(); it != IndexStatementsize; ++it)
		dumpfile << (LPCTSTR)*it << std::endl;
	dumpfile.close();
}
void CSettingsReader::Control(const char *Path, const char *dPath, wxGauge *gauge /*= NULL*/, wxTextCtrl *PrgDlg /*= NULL*/)
{
	AfxDaoInit();
	CSettings settings;
	std::vector<CString> statements;
	std::vector<CString> InsertStatements;
	std::vector<CString> RelationFields;
	std::vector<CString> IndexStatements;
	std::vector<CString> UniqueFields;
	std::vector<CString> CollateIndexFields;
	std::vector<CString> TableField;
	std::vector<CString> ForeignKeySupportinfo;
	std::vector<CString> IndexInfo;
	unsigned nWarningCount = 0;
	CString ReservedKeyWords[] = {"ABORT", "ACTION", "ADD", "AFTER", "ALL", "ALTER", "ANALYZE", "AND", "AS", "ASC", "ATTACH", "AUTOINCREMENT", "BEFORE", "BEGIN", "BETWEEN", "BY", "CASCADE", "CASE", 
		"CAST", "CHECK", "COLLATE", "COLUMN", "COMMIT", "CONFLICT", "CONSTRAINT", "CREATE", "CROSS", "CURRENT_DATE", "CURRENT_TIME", "CURRENT_TIMESTAMP", "DATABASE", "DEFAULT", "DEFERRABLE",
        "DEFERRED", "DELETE", "DESC", "DETACH", "DISTINCT", "DROP", "EACH", "ELSE", "END", "ESCAPE", "EXCEPT", "EXCLUSIVE", "EXISTS", "EXPLAIN", "FAIL", "FOR", "FOREIGN", "FROM", "FULL",
        "GLOB", "GROUP", "HAVING", "IF", "IGNORE", "IMMEDIATE", "IN", "INDEX", "INDEXED", "INITIALLY", "INNER", "INSERT", "INSTEAD", "INTERSECT", "INTO", "IS", "ISNULL", "JOIN", "KEY", "LEFT",
        "LIKE", "LIMIT", "MATCH", "NATURAL", "NO", "NOT", "NOTNULL", "NULL", "OF", "OFFSET", "ON", "OR", "ORDER", "OUTER", "PLAN", "PRAGMA", "PRIMARY", "QUERY", "RAISE", "RECURSIVE", "REFERENCES",
        "REGEXP", "REINDEX", "RELEASE", "RENAME", "REPLACE", "RESTRICT", "RIGHT", "ROLLBACK", "ROW", "SAVEPOINT", "SELECT", "SET", "TABLE", "TEMP", "TEMPORARY", "THEN", "TO", "TRANSACTION", "TRIGGER",
        "UNION", "UNIQUE", "UPDATE", "USING", "VACUUM", "VALUES", "VIEW", "VIRTUAL", "WHEN", "WHERE", "WITH", "WITHOUT"};
	CString sStatement;
    CString widepath = (CString)Path;
	CSettingsReader::ReadFromCSimpleIni(settings);                                                        
	CDaoDatabase db;                                            
	db.Open(widepath);                                          
	short nTableCount = db.GetTableDefCount();                 
	CString *sTableNames = new CString[nTableCount]; 
	CString *sTableNames2 = new CString[nTableCount]; 
	int *IndexTable = new int[nTableCount];
	short nNonSystemTableCount=0;
	for( int i = 0; i < nTableCount; ++i )
	{
		CDaoTableDefInfo tabledefinfo;                           
		db.GetTableDefInfo(i,tabledefinfo);                      
		if( tabledefinfo.m_lAttributes == 0 )                      
	   	{  
			CDaoTableDef TableDef(&db);
			sTableNames[nNonSystemTableCount] = tabledefinfo.m_strName;
			TableDef.Open(tabledefinfo.m_strName);
			sTableNames2[nNonSystemTableCount] = tabledefinfo.m_strName;
			IndexTable[nNonSystemTableCount] = TableDef.GetIndexCount();
			nNonSystemTableCount++;
			if( settings.m_bCollateNoCaseIndexAdd ) 
				CFieldStatements::FieldCollation(TableDef, tabledefinfo, CollateIndexFields, settings.m_bTrimTextValues);
		}
	}
	for( int i = 0; i < nTableCount; ++i )
	{
		CDaoTableDefInfo tabledefinfo;                           
		db.GetTableDefInfo(i,tabledefinfo);                      
		if( tabledefinfo.m_lAttributes == 0 )                      // We choose only the elements that we need as the database adds some system files
	   	{  
				if( !i && settings.m_bRelationshipAdd )
				{
					short nRelationCount = db.GetRelationCount();
					CRelationships::Relationhips(db, RelationFields, nRelationCount); 
				}
				CDaoTableDef TableDef(&db);
				sStatement = _T("CREATE TABLE `");  
				if( settings.m_bKeyWordList && PrgDlg != NULL)
				{
					for( int i1 = 0; i1 < 124; ++i1 )
					{
						if( !(tabledefinfo.m_strName.CompareNoCase(ReservedKeyWords[i1])) )
						{
							++nWarningCount;
							wxString ErrorMessage = wxT("WARNING: table name found as sqlite keyword this could lead to unexpected behaviour, table name found: ");
							PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
							CT2CA pszConvertedAnsiString (tabledefinfo.m_strName);
							std::string strStd (pszConvertedAnsiString);
							ErrorMessage += wxString::FromUTF8(_strdup(strStd.c_str() ) );
							ErrorMessage += wxT("\n");
							PrgDlg->WriteText(ErrorMessage);
							PrgDlg->SetDefaultStyle(wxTextAttr (wxNullColour));
						}

					}
				}
				sStatement += tabledefinfo.m_strName;
				sStatement += (_T("` ("));
				TableDef.Open(tabledefinfo.m_strName);   
				if( settings.m_bIndexAdd ) 
				{

					if(PrgDlg != NULL)
					   CIndexStatements::Indexes(TableDef, IndexStatements, tabledefinfo, sTableNames, nNonSystemTableCount, UniqueFields, CollateIndexFields, 
					                             settings.m_bCollateNoCaseIndexAdd, settings.m_bTrimTextValues, settings.m_bKeyWordList, ReservedKeyWords, IndexInfo, nWarningCount, PrgDlg);
					else CIndexStatements::Indexes(TableDef, IndexStatements, tabledefinfo, sTableNames, nNonSystemTableCount, UniqueFields, CollateIndexFields, 
					                             settings.m_bCollateNoCaseIndexAdd, settings.m_bTrimTextValues, settings.m_bKeyWordList, ReservedKeyWords, IndexInfo, nWarningCount);
				}
				if(PrgDlg != NULL)
					CFieldStatements::fFields(db, TableDef, tabledefinfo, InsertStatements, UniqueFields, settings, sStatement, ReservedKeyWords, TableField, IndexInfo, nWarningCount, PrgDlg); 
				else CFieldStatements::fFields(db, TableDef, tabledefinfo, InsertStatements, UniqueFields, settings, sStatement, ReservedKeyWords, TableField, IndexInfo, nWarningCount);
				statements.push_back(sStatement);
		} 
	} 
if( settings.m_bForeignkeySupport )
	{
		unsigned nRelationCount = db.GetRelationCount();

		unsigned nInsertLength = InsertStatements.size();
		std::vector<int> beginning;
		std::vector<int> end;
		for(unsigned i = 0; i < nInsertLength; ++i)
		{
			if(InsertStatements[i] == "BEGIN TRANSACTION")
				beginning.push_back(i);
			if(InsertStatements[i] == "END TRANSACTION")
				end.push_back(i);
		}
		bool isPossibleToAddForeignKeys = true;
		if(PrgDlg != NULL)
			CRelationships::ForeignKeySupport(db, nRelationCount, TableField, ForeignKeySupportinfo, sTableNames, statements, beginning, end, InsertStatements, isPossibleToAddForeignKeys, nWarningCount, PrgDlg);
		else CRelationships::ForeignKeySupport(db, nRelationCount, TableField, ForeignKeySupportinfo, sTableNames, statements, beginning, end, InsertStatements, isPossibleToAddForeignKeys, nWarningCount);
		if( isPossibleToAddForeignKeys )
		{   
			unsigned nVectorLength = statements.size();
			unsigned nForeignKeySupportCount = ForeignKeySupportinfo.size();
			for( unsigned i = 0; i < nVectorLength; ++i )
			{
				sTableNames[i] += _T("FOREIGN KEY");
				for( unsigned i3 = 0; i3 < nForeignKeySupportCount; ++i3 )
				{
					if( !( sTableNames[i].Compare(ForeignKeySupportinfo[i3].Left(sTableNames[i].GetLength())) ) )
					{
						statements[i] = statements[i].Left(statements[i].GetLength()-2);
						CString temp = _T(", ");
						ForeignKeySupportinfo[i3] = ForeignKeySupportinfo[i3].Right(ForeignKeySupportinfo[i3].GetLength()-sTableNames[i].GetLength() + 11);
						temp += ForeignKeySupportinfo[i3];
						temp += _T(");");
						statements[i] += temp;
					}
				}
			}
			for( unsigned i = 0; i < nVectorLength; ++i )
				sTableNames[i] = sTableNames[i].Left(sTableNames[i].GetLength() - 11);
		}
	}
	db.Close();
	if(PrgDlg == NULL)
	{
		CSettingsReader::Dumping(statements, InsertStatements, RelationFields, IndexStatements, dPath);
		delete [] sTableNames;
		AfxDaoTerm();
		return;
	}
	else
	{
	  gauge -> SetRange(statements.size() + InsertStatements.size() + RelationFields.size() + IndexStatements.size());
	  CSQLiteConversion::SqliteConversion(statements, InsertStatements, IndexStatements, RelationFields, dPath, gauge, PrgDlg, sTableNames, settings.m_bForeignkeySupport, nWarningCount, 
		                                  IndexTable, sTableNames2);
	  delete [] sTableNames;
	  AfxDaoTerm();
	}
	
}

