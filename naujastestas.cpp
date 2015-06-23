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

 #pragma warning(disable : 4995)
void CMain::ReadFromCSimpleIni(CSettings &settings)
{
	CSimpleIni ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile("settings.ini");
    if (rc < 0) 
		cout << "ERROR LOADING FROM FILE" << endl;
	settings.bFieldsAdd = ini.GetBoolValue(_T("Settings"),_T("fieldsadd"),true);
	settings.bRelationshipAdd = ini.GetBoolValue(_T("Settings"),_T("Relationshipadd"),true);
	settings.bRecordAdd = ini.GetBoolValue(_T("Settings"),_T("RecordAdd"),true);
	settings.bNotNullAdd = ini.GetBoolValue(_T("Settings"),_T("NotNullAdd"),true);
	settings.bAutoIncrementAdd = ini.GetBoolValue(_T("Settings"),_T("AutoIncrementAdd"),true);
	settings.bDefaultValueAdd = ini.GetBoolValue(_T("Settings"),_T("DefaultValueAdd"),true);
	settings.bFieldTypeAdd = ini.GetBoolValue(_T("Settings"),_T("FieldTypeAdd"),true);
	settings.bIndexAdd = ini.GetBoolValue(_T("Settings"),_T("IndexAdd"),true);
	settings.bUniqueFieldAdd = ini.GetBoolValue(_T("Settings"),_T("UniqueFieldAdd"),true);
	settings.bCollateNoCaseIndexAdd = ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForIndex"),true);
	settings.bCollateNoCaseFieldsAdd = ini.GetBoolValue(_T("Settings"),_T("CollateNoCaseForFields"),true);
}

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		cout << "Conversion failed. Path wasnt provided." << endl;
		return 0;
	}
	AfxDaoInit();
	CSQLiteConversion CSQLiteConversionObject;
	CFieldStatements CFieldStatementsObject;
	CIndexStatements CIndexStatementsObject;
	CSettings settings;
	CRelationships CRelationshipsObject;
	vector <CString> statements;
	vector <CString> RelationFields;
	vector <CString> IndexStatements;
	vector <CString> UniqueFields;
	vector <CString> CollateIndexFields;
    CString widepath = (CString)argv[1];
	CMain::ReadFromCSimpleIni(settings);
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
				if(settings.bCollateNoCaseIndexAdd) 
					CFieldStatementsObject.FieldCollation(TableDef,tabledefinfo,CollateIndexFields);
			}
		}
		for(int i = 0; i < nTableCount; ++i)
		  {
			CDaoTableDefInfo tabledefinfo;                           // Create tabledefinfo  object & save all information in it about the table we are currently analyzing
			db.GetTableDefInfo(i,tabledefinfo);                      // tabledefinfo we save information about the i-th table
			if(tabledefinfo.m_lAttributes == 0)                      // We choose only the elements that we need
	   		{  
				  short nRelationCount = db.GetRelationCount();
				  if(!i && settings.bRelationshipAdd)
				  {
					  #pragma deprecated(Relationships)
					 CRelationshipsObject.Relationhips(db,RelationFields,nRelationCount); 
				  }
				  CDaoTableDef TableDef(&db);
				  statements.push_back(_T("CREATE TABLE   `"));  
				  statements.back() += tabledefinfo.m_strName;
				  statements.back() += (_T("` ("));
				  TableDef.Open(tabledefinfo.m_strName); //We open one of tabledefinfo structural objects (this time table name)  
				  if(settings.bIndexAdd) 
					  {
                          #pragma deprecated(Indexes)
						  CIndexStatementsObject.Indexes(TableDef,IndexStatements,tabledefinfo,sTableNames,nNonSystemTableCount,UniqueFields,CollateIndexFields,settings.bCollateNoCaseIndexAdd);
				      }
				  if(settings.bFieldsAdd) 
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

