// naujastestas.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "naujastestas.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <afxdao.h>
#include <afxdb.h>
#include <vector>
#include "SimpleIni.h"
#include "FieldStatements.h"
#include "IndexStatements.h"
#include "sqlitestatementexecution.h"
#include "RelationshipStatements.h"

 #pragma warning(disable : 4995)
void CSettingsReader::ReadFromCSimpleIni(CSettings &settings)
{
	CSimpleIni ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile("settings.ini");
    if (rc < 0) 
		std::cout << "ERROR LOADING FROM FILE" << std::endl;
	settings.m_bFieldsAdd = ini.GetBoolValue(_T("Settings"),_T("fieldsadd"),true);
	settings.m_bRelationshipAdd = ini.GetBoolValue(_T("Settings"),_T("Relationshipadd"),true);
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
}

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		std::cout << "Conversion failed. Path wasnt provided." << std::endl;
		return 0;
	}
	AfxDaoInit();
	CSQLiteConversion CSQLiteConversionObject;
	CFieldStatements CFieldStatementsObject;
	CIndexStatements CIndexStatementsObject;
	CSettings settings;
	CRelationships CRelationshipsObject;
	std::vector <CString> statements;
	std::vector <CString> RelationFields;
	std::vector <CString> IndexStatements;
	std::vector <CString> UniqueFields;
	std::vector <CString> CollateIndexFields;
    CString widepath = (CString)argv[1];
	CSettingsReader::ReadFromCSimpleIni(settings);
	try
	{                                                           
		CDaoDatabase db;                                            // Path to the file that we want
		db.Open(widepath);                                          // Create CDaoDatabase object
		short nTableCount = db.GetTableDefCount();               // Open the database file that we want   
		CString *sTableNames = new CString[nTableCount];      // Get how many tables are in the selected database
		short nNonSystemTableCount=0;
		for(int i = 0; i < nTableCount; ++i)
		{
			CDaoTableDefInfo tabledefinfo;                           
			db.GetTableDefInfo(i,tabledefinfo);                      
			if(tabledefinfo.m_lAttributes == 0)                      
	   		{  
				CDaoTableDef TableDef(&db);
				sTableNames[nNonSystemTableCount] = tabledefinfo.m_strName;
				nNonSystemTableCount++;
				if(settings.m_bCollateNoCaseIndexAdd) 
					CFieldStatementsObject.FieldCollation(TableDef,tabledefinfo,CollateIndexFields,settings.m_bTrimTextValues);
			}
		}
		for(int i = 0; i < nTableCount; ++i)
		  {
			CDaoTableDefInfo tabledefinfo;                           // Create tabledefinfo  object & save all information in it about the table we are currently analyzing
			db.GetTableDefInfo(i,tabledefinfo);                      // tabledefinfo we save information about the i-th table
			if(tabledefinfo.m_lAttributes == 0)                      // We choose only the elements that we need
	   		{  
				  short nRelationCount = db.GetRelationCount();
				  if(!i && settings.m_bRelationshipAdd)
				  {
					  #pragma deprecated(Relationships)
					 CRelationshipsObject.Relationhips(db,RelationFields,nRelationCount); 
				  }
				  CDaoTableDef TableDef(&db);
				  statements.push_back(_T("CREATE TABLE   `"));  
				  statements.back() += tabledefinfo.m_strName;
				  statements.back() += (_T("` ("));
				  TableDef.Open(tabledefinfo.m_strName); //We open one of tabledefinfo structural objects (this time table name)  
				  if(settings.m_bIndexAdd) 
					  {
                          #pragma deprecated(Indexes)
						  CIndexStatementsObject.Indexes(TableDef,IndexStatements,tabledefinfo,sTableNames,nNonSystemTableCount,UniqueFields,CollateIndexFields,settings.m_bCollateNoCaseIndexAdd,settings.m_bTrimTextValues);
				      }
				  if(settings.m_bFieldsAdd) 
					  {
						  #pragma deprecated(fFields)
						  CFieldStatementsObject.fFields(TableDef, tabledefinfo, statements,UniqueFields,settings);  
				      }
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
	CSQLiteConversionObject.SqliteConversion(statements,IndexStatements,RelationFields,argv[2]);
    return 0;
}

