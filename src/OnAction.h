#ifndef __INCLUDED_ONACTION_H__
#define __INCLUDED_ONACTION_H__
#pragma once

class wxGauge;
class wxTextCtrl;
class CUIObs;

#include <vector>

class CSettings
{
 public:
		bool m_bRelationshipAdd;
		bool m_bRecordAdd;
		bool m_bNotNullAdd; 
		bool m_bAutoIncrementAdd;
		bool m_bDefaultValueAdd; 
		bool m_bIndexAdd;
		bool m_bUniqueFieldAdd;
		bool m_bCollateNoCaseIndexAdd;
		bool m_bCollateNoCaseFieldsAdd;
		bool m_bTrimTextValues;
		bool m_bAddComments;
		bool m_bKeyWordList;
		bool m_bForeignkeySupport;
		bool m_PrimaryKeySupport;
		bool m_ForeignKeyPrimary;
};

class CSettingsReader
{
   public:
		static void ReadFromCSimpleIni(CSettings &settings);
		static bool Control(const char *Path, const char *dPath, CUIObs *pObs, bool bConvert);
   private:
	    static void Dumping(std::vector<CString> &statements, std::vector<CString> &InsertStatements, std::vector<CString> &RelationFields, std::vector<CString> &IndexStatements, 
			                const char *&dPath);

};
#endif