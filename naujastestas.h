#ifndef INCLUDED_NAUJASTESTAS_H
#define INCLUDED_NAUJASTESTAS_H
#pragma once

#include "Resource.h"

#pragma warning(disable : 4995)

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
	 static void ReadFromCSimpleIni(CSettings &settings);
};
#endif