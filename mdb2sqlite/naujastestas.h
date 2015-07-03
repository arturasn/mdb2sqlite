#ifndef __INCLUDED_NAUJASTESTAS_H__
#define __INCLUDED_NAUJASTESTAS_H__
#pragma once

class wxGauge;
class wxTextCtrl;

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
		bool m_bAddComments;
 };
class CSettingsReader
{
   public:
		static void ReadFromCSimpleIni(CSettings &settings);
		static void Control(const char *Path,const char *dPath, wxGauge *&gauge ,wxTextCtrl *&PrgDlg);
};
#endif