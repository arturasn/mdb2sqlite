#define _AFXDLL
#include "stdafx.h"
#include "SimpleIni.h"
#include <afxdao.h>
#include "IndexStatements.h"
#include "sqlitestatementexecution.h"
#include "Relationshipstatements.h"
#include "FieldStatements.h"
#include "StructurePreviewDlg.h"
#include <wx/gauge.h>
#include <wx/textctrl.h>
#include <fstream>
#include <codecvt>
#include "DBTable.h"
#include "UIObs.h"

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
	settings.m_ForeignKeyPrimary = ini.GetBoolValue(_T("Settings"),_T("ForeignKeyPrimary"),true);
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

bool CSettingsReader::Control(const char *Path, const char *dPath, CUIObs *pObs, bool bConvert)
{
	AfxDaoInit();
	std::vector<CString> statements, InsertStatements, RelationFields, IndexStatements, UniqueFields, CollateIndexFields, 
	TableField, ForeignKeySupportinfo, primary_fields;

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
	CSettings settings;
	CSettingsReader::ReadFromCSimpleIni(settings);                                                        
	CDaoDatabase db;                                            
	db.Open(widepath);                                          
	int nTableCount = db.GetTableDefCount();                 
	std::vector<CString> sTableNames;
	std::vector<CString> sIndexTableNames; 
	std::vector<int> indextable;
	std::vector<CDBTable> structure;
	std::vector<std::wstring> warnings;

	for( int i = 0; i < nTableCount; ++i )
	{
		CDaoTableDefInfo tabledefinfo;                           
		db.GetTableDefInfo(i, tabledefinfo);                      
		if( tabledefinfo.m_lAttributes == 0 )                      
	   	{  
			CDaoTableDef TableDef(&db);
			sTableNames.push_back(tabledefinfo.m_strName);
			TableDef.Open(tabledefinfo.m_strName);
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
				if( settings.m_bKeyWordList )
				{
					for( int i1 = 0; i1 < 124; ++i1 )
					{
						if( !(tabledefinfo.m_strName.CompareNoCase(ReservedKeyWords[i1])) )
						{
							++nWarningCount;
							std::wstring sErrorMessage = L"WARNING: table name found as sqlite keyword this could lead to unexpected behaviour, table name found: ";
							pObs->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
							CT2CA pszConvertedAnsiString (tabledefinfo.m_strName);
							std::string strStd (pszConvertedAnsiString);
                            sErrorMessage += wxString::FromUTF8(_strdup(strStd.c_str() ) );
                            sErrorMessage += L"\n";
							pObs->WriteText(sErrorMessage);
							pObs->SetDefaultStyle(wxTextAttr (wxNullColour));
						}

					}
				}
				sStatement += tabledefinfo.m_strName;
				sStatement += (_T("` ("));
				TableDef.Open(tabledefinfo.m_strName);
				std::vector<CDBFieldIndex> indexes, fields;
				if( settings.m_bIndexAdd ) 
				{
					sIndexTableNames.push_back(tabledefinfo.m_strName);
					indexes = CIndexStatements::Indexes(TableDef, IndexStatements, tabledefinfo, sTableNames, UniqueFields, CollateIndexFields, 
					                             settings.m_bCollateNoCaseIndexAdd, settings.m_bTrimTextValues, settings.m_bKeyWordList, ReservedKeyWords, primary_fields, nWarningCount,
												 indextable, warnings);
				}

				fields = CFieldStatements::fFields(db, TableDef, tabledefinfo, InsertStatements, UniqueFields, settings, sStatement, ReservedKeyWords, TableField, primary_fields, nWarningCount, warnings); 
				CDBTable table(tabledefinfo.m_strName, fields, indexes);
				structure.push_back(table);
				statements.push_back(sStatement);
		} 
	} 

	if( bConvert )
	{
		CStructPreviewDlg dlg(nullptr, structure);
		bool bOK = dlg.ShowModal() == wxID_OK;
		if( !bOK ) {
			AfxDaoTerm();
			return false;
		}

		pObs->CreateAdditionalItems();
	}

	for(const auto &message : warnings)
	{
		pObs->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
		pObs->WriteText(message);
		pObs->SetDefaultStyle(wxTextAttr (wxNullColour));
	}

	std::vector<CString> queries;
	//Currently not supported
	if( false )
	{
		int nQueryCnt = db.GetQueryDefCount();
		for(int i = 0; i < nQueryCnt; ++i) 
		{
			CDaoQueryDefInfo info;
			db.GetQueryDefInfo(i, info);
			CDaoQueryDef queryObj(&db);
			queryObj.Open(info.m_strName);
			CString SQL = _T("CREATE VIEW ") + info.m_strName + _T(" AS ") + queryObj.GetSQL();
			queries.push_back(SQL);
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
		CRelationships::ForeignKeySupport(db, nRelationCount, TableField, ForeignKeySupportinfo, sTableNames, statements, beginning, end, InsertStatements, 
			                              isPossibleToAddForeignKeys, nWarningCount, settings.m_ForeignKeyPrimary, pObs);
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
	if( !bConvert )
	{
		CSettingsReader::Dumping(statements, InsertStatements, RelationFields, IndexStatements, dPath);
		AfxDaoTerm();
	}
	else
	{
	  pObs->SetRange(statements.size() + InsertStatements.size() + RelationFields.size() + IndexStatements.size());
	  CSQLiteConversion::SqliteConversion(statements, InsertStatements, IndexStatements, RelationFields, queries, dPath, pObs, sTableNames, settings.m_bForeignkeySupport, nWarningCount, 
		  indextable, sIndexTableNames);
	  AfxDaoTerm();
	}

	return true;
}

