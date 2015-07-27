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


void CSettingsReader::ReadFromCSimpleIni(CSettings &settings)
{
	CSimpleIni ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile("Settings.ini");
   /* if (rc < 0) 
		std::cout << "ERROR LOADING FROM FILE" << std::endl;*/
	settings.m_bFieldsAdd = ini.GetBoolValue(_T("Settings"),_T("FieldsAdd"),true);
	settings.m_bRelationshipAdd = ini.GetBoolValue(_T("Settings"),_T("RelationshipAdd"),true);
	settings.m_bRecordAdd = ini.GetBoolValue(_T("Settings"),_T("RecordAdd"),true);
	settings.m_bNotNullAdd = ini.GetBoolValue(_T("Settings"),_T("NotNullAdd"),true);
	settings.m_bAutoIncrementAdd = ini.GetBoolValue(_T("Settings"),_T("AutoIncrementAdd"),true);
	settings.m_bDefaultValueAdd = ini.GetBoolValue(_T("Settings"),_T("DefaultValueAdd"),true);
	settings.m_bFieldTypeAdd = ini.GetBoolValue(_T("Settings"),_T("FieldTypeAdd"),true);
	settings.m_bIndexAdd = ini.GetBoolValue(_T("Settings"),_T("IndexAdd"),true);
	settings.m_bUniqueFieldAdd = ini.GetBoolValue(_T("Settings"),_T("UniqueFieldAdd"),true);
	settings.m_bCollateNoCaseIndexAdd = ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForIndex"),true);
	settings.m_bCollateNoCaseFieldsAdd = ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForFields"),true);
	settings.m_bTrimTextValues = ini.GetBoolValue(_T("Settings"),_T("TrimTextValues"),true);
	settings.m_bAddComments = ini.GetBoolValue(_T("Settings"),_T("AddComents"),true);
	settings.m_bKeyWordList = ini.GetBoolValue(_T("Settings"),_T("KeyWordList"),true);
}

void CSettingsReader::Control(const char *Path,const char *dPath, wxGauge *&gauge , wxTextCtrl *&PrgDlg)
{
	/*if(	argc < 3 )
	{
		std::cout << "Conversion failed. Path wasnt provided." << std::endl;
		return 0;
	}*/
	AfxDaoInit();
	CSettings settings;
	std::vector<CString> statements;
	std::vector<CString> InsertStatements;
	std::vector<CString> RelationFields;
	std::vector<CString> IndexStatements;
	std::vector<CString> UniqueFields;
	std::vector<CString> CollateIndexFields;
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
	try
	{                                                           
		CDaoDatabase db;                                            
		db.Open(widepath);                                          
		short nTableCount = db.GetTableDefCount();                 
		CString *sTableNames = new CString[nTableCount];      
		short nNonSystemTableCount=0;
		for( int i = 0; i < nTableCount; ++i )
		{
			CDaoTableDefInfo tabledefinfo;                           
			db.GetTableDefInfo(i,tabledefinfo);                      
			if( tabledefinfo.m_lAttributes == 0 )                      
	   		{  
				CDaoTableDef TableDef(&db);
				sTableNames[nNonSystemTableCount] = tabledefinfo.m_strName;
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
				  short nRelationCount = db.GetRelationCount();
				  if( !i && settings.m_bRelationshipAdd )
					  CRelationships::Relationhips(db, RelationFields, nRelationCount); 
				  CDaoTableDef TableDef(&db);
				  sStatement = _T("CREATE TABLE   `");  
				  if( settings.m_bKeyWordList )
				  {
					  for( int i1 = 0; i1 < 124; ++i1 )
					  {
						  if( !(tabledefinfo.m_strName.CompareNoCase(ReservedKeyWords[i1])) )
						  {
							  wxString ErrorMessage = wxT("Error table name cannot be sqlite keyword, table name found: ");
							  PrgDlg->SetDefaultStyle(wxTextAttr (*wxRED));
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
					  CIndexStatements::Indexes(TableDef, IndexStatements, tabledefinfo, sTableNames, nNonSystemTableCount, UniqueFields, CollateIndexFields, settings.m_bCollateNoCaseIndexAdd, settings.m_bTrimTextValues, PrgDlg, settings.m_bKeyWordList, ReservedKeyWords);
				  if( settings.m_bFieldsAdd ) 
					  CFieldStatements::fFields(db, TableDef, tabledefinfo, InsertStatements, UniqueFields, settings, sStatement, PrgDlg, settings.m_bKeyWordList, ReservedKeyWords); 
				  statements.push_back(sStatement);
			} 
		 } 
		db.Close();
	
	gauge -> SetRange(statements.size() + InsertStatements.size() + RelationFields.size() + IndexStatements.size());
	CSQLiteConversion::SqliteConversion(statements, InsertStatements, IndexStatements, RelationFields, dPath, gauge, PrgDlg, sTableNames);
	delete [] sTableNames;
	}
	
	catch(CDaoException *e)
	{
		ASSERT(FALSE);
		e->Delete();
	}
	AfxDaoTerm();
}

