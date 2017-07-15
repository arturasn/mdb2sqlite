#ifndef __SETTING_H__
#define __SETTING_H__
#pragma once

enum ESettings
{
	eRelationshipAdd = 0,
	eRecordAdd,
	eNotNullAdd,
	eAutoIncAdd,
	eDefaultValueAdd,
	eIndexAdd,
	eUniqueFieldAdd,
	eCollateNoCaseForIndex,
	eCollateNoCaseForFields,
	eTrimTextValues,
	eAddComments,
	eKeyWordList,
	eForeignKeySupport,
	ePrimaryKeySupport,
	eForeignKeyPrimary,
	eSize
};

#endif