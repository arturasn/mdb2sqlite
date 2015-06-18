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
 using namespace std;

void CMain::ReadFromCSimpleIni(CSettings &CSettingsObject)
{
	CSimpleIni ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile("settings.ini");
    if (rc < 0) 
		cout << "ERROR LOADING FROM FILE" << endl;
	CSettingsObject.bFieldsAdd = ini.GetBoolValue(_T("Settings"),_T("fieldsadd"),true);
	CSettingsObject.bRelationshipAdd = ini.GetBoolValue(_T("Settings"),_T("Relationshipadd"),true);
	CSettingsObject.bRecordAdd = ini.GetBoolValue(_T("Settings"),_T("RecordAdd"),true);
	CSettingsObject.bNotNullAdd = ini.GetBoolValue(_T("Settings"),_T("NotNullAdd"),true);
	CSettingsObject.bAutoIncrementAdd = ini.GetBoolValue(_T("Settings"),_T("AutoIncrementAdd"),true);
	CSettingsObject.bDefaultValueAdd = ini.GetBoolValue(_T("Settings"),_T("DefaultValueAdd"),true);
	CSettingsObject.bFieldTypeAdd = ini.GetBoolValue(_T("Settings"),_T("FieldTypeAdd"),true);
	CSettingsObject.bIndexAdd = ini.GetBoolValue(_T("Settings"),_T("IndexAdd"),true);
	CSettingsObject.bUniqueFieldAdd = ini.GetBoolValue(_T("Settings"),_T("UniqueFieldAdd"),true);
}

int main(int argc, char* argv[])
{
	AfxDaoInit();
	CSQLiteConversion CSQLiteConversionObject;
	CFieldStatements CFieldStatementsObject;
	CIndexStatements CIndexStatementsObject;
	CSettings CSettingsObject;
	CRelationships CRelationshipsObject;
	CMain::ReadFromCSimpleIni(CSettingsObject);
	vector <CString> statements;
	vector <CString> RelationFields;
	vector <CString> IndexStatements;
	vector <CString> UniqueFields;
    CString widepath = (CString)argv[1];                        // Path to the file that we want
	CDaoDatabase db;                                            // Create CDaoDatabase object
	db.Open(widepath);                                          // Open the database file that we want
	short nTableCount = db.GetTableDefCount();                  // Get how many tables are in the selected database
	CString *sTableNames = new CString[nTableCount];
	short nNonSystemTableCount=0;
	for(int i = 0; i < nTableCount; i++)
	{
		CDaoTableDefInfo tabledefinfo;                           
        db.GetTableDefInfo(i,tabledefinfo);                      
		if(tabledefinfo.m_lAttributes == 0)                      
	   	{  
			sTableNames[nNonSystemTableCount] = tabledefinfo.m_strName;
			nNonSystemTableCount++;
		}
	}
	for(int i = 0; i < nTableCount; i++)
      {
        CDaoTableDefInfo tabledefinfo;                           // Create tabledefinfo  object & save all information in it about the table we are currently analyzing
        db.GetTableDefInfo(i,tabledefinfo);                      // tabledefinfo we save information about the i-th table
		if(tabledefinfo.m_lAttributes == 0)                      // We choose only the elements that we need
	   	{  
			  short nRelationCount = db.GetRelationCount();
			  if(!i && CSettingsObject.bRelationshipAdd)
				 CRelationshipsObject.Relationhips(db,RelationFields,nRelationCount); 
		      CDaoTableDef TableDef(&db);
		      statements.push_back(_T("CREATE TABLE   `"));  
			  statements.back() += tabledefinfo.m_strName;
			  statements.back() += (_T("` ("));
			  TableDef.Open(tabledefinfo.m_strName); //We open one of tabledefinfo structural objects (this time table name)  
			  if(CSettingsObject.bIndexAdd) CIndexStatementsObject.Indexes(TableDef,IndexStatements,tabledefinfo,sTableNames,nNonSystemTableCount,UniqueFields);
			  if(CSettingsObject.bFieldsAdd) CFieldStatementsObject.fFields(TableDef, tabledefinfo, statements,UniqueFields,CSettingsObject);    
		} 
     }
	delete[] sTableNames; 
	db.Close();
	AfxDaoTerm();
	CSQLiteConversionObject.SqliteConversion(statements,IndexStatements,RelationFields);
    return 0;
}

