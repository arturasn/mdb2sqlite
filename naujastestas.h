#ifndef __INCLUDED_NAUJASTESTAS_H__
#define __INCLUDED_NAUJASTESTAS_H__
#pragma once

#include "Resource.h"
#include <iostream>
#include <afxdao.h>
#include <afxdb.h>
#include <vector>
#include "SimpleIni.h"
#ifndef __INCLUDED_INDEXSTATEMENTS_H__
	#include "IndexStatements.h"
#endif
#ifndef __INCLUDED_SQLITESTATEMENTEXECUTION_H__
	#include "sqlitestatementexecution.h"
#endif
#ifndef __INCLUDED_RELATIONSHIPSTATEMENTS_H__
	#include "RelationshipStatements.h"
#endif

class CSettings
 {
 public:
	bool m_bFieldsAdd; 
	bool m_bRelationshipAdd;
	bool m_bRecordAdd;
	bool m_bNotNullAdd; 
	bool m_bAutoIncrementAdd;
	bool m_bDefaultValueAdd; 
	bool m_bFieldTypeAdd;
	bool m_bIndexAdd;
	bool m_bUniqueFieldAdd;
	bool m_bCollateNoCaseIndexAdd;
	bool m_bCollateNoCaseFieldsAdd;
	bool m_bTrimTextValues;
 };
class CSettingsReader
{
   public:
	 static void ReadFromCSimpleIni(CSettings &settings);
};
#endif