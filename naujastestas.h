#ifndef INCLUDED_NAUJASTESTAS_H
#define INCLUDED_NAUJASTESTAS_H
#pragma once

#include "Resource.h"

class CSettings
 {
 public:
	       bool bFieldsAdd, 
		        bRelationshipAdd, 
		        bRecordAdd, 
		        bNotNullAdd, 
		        bAutoIncrementAdd,
		        bDefaultValueAdd, 
		        bFieldTypeAdd,
		        bIndexAdd,
		        bUniqueFieldAdd; 
 };
class CMain
{
   public:
	 static void ReadFromCSimpleIni(CSettings &CSettingsObject);
};
#endif