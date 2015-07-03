#define _AFXDLL
#include "stdafx.h"
#include "SimpleIni.h"
 #include "IndexStatements.h"
#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
	#include "sqlitestatementexecution.h"
#endif
#ifndef __included_relationshipstatements_h__
	#include "Relationshipstatements.h"
#endif
#ifndef __INCLUDED_FIELDSTATEMENTS_H__
  #include "FieldStatements.h"
#endif
#include <wx/gauge.h>
#include <wx/event.h>
#include <wx/textctrl.h>

void CSettingsReader::ReadFromCSimpleIni(CSettings &settings)
{
	CSimpleIni ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile("settings.ini");
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
			if( tabledefinfo.m_lAttributes == 0 )                      // We choose only the elements that we need ass the database adds some system files
	   		{  
				  short nRelationCount = db.GetRelationCount();
				  if( !i && settings.m_bRelationshipAdd )
					  CRelationships::Relationhips(db, RelationFields, nRelationCount); 
				  CDaoTableDef TableDef(&db);
				  sStatement = _T("CREATE TABLE   `");  
				  sStatement += tabledefinfo.m_strName;
				  sStatement += (_T("` ("));
				  TableDef.Open(tabledefinfo.m_strName);   
				  if( settings.m_bIndexAdd ) 
						CIndexStatements::Indexes(TableDef, IndexStatements, tabledefinfo, sTableNames, nNonSystemTableCount, UniqueFields, CollateIndexFields, settings.m_bCollateNoCaseIndexAdd, settings.m_bTrimTextValues);
				  if( settings.m_bFieldsAdd ) 
						CFieldStatements::fFields(db, TableDef, tabledefinfo, InsertStatements, UniqueFields, settings, sStatement); 
				  statements.push_back(sStatement);
			} 
		 }
		delete[] sTableNames; 
		db.Close();
	}
	catch(CDaoException *e)
	{
		ASSERT(FALSE);
		e->Delete();
	}
	AfxDaoTerm();
	gauge -> SetRange(statements.size() + InsertStatements.size() + RelationFields.size() + IndexStatements.size());
	CSQLiteConversion::SqliteConversion(statements, InsertStatements, IndexStatements, RelationFields, dPath, gauge, PrgDlg);
}

