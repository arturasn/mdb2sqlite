// IniFile.h: interface for the CIniReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__99976B4B_DBA1_4D1E_AA14_CBEB63042FD1__INCLUDED_)
#define AFX_INIFILE_H__99976B4B_DBA1_4D1E_AA14_CBEB63042FD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcoll.h>

class CIniReader  
{
public:
	// method to set INI file name, if not already specified 
	void setINIFileName(CString strINIFile);
	
	// methods to return the lists of section data and section names
	CStringList* getSectionData(CString strSection);
	CStringList* getSectionNames();
	
	// check if the section exists in the file
	BOOL sectionExists(CString strSection);

	// updates the key value, if key already exists, else creates a key-value pair
	long setKey(CString strValue, CString strKey, CString strSection);

	// give the key value for the specified key of a section
	CString getKeyValue(CString strKey,CString strSection);
	
	// default constructor
	CIniReader()
	{
		m_sectionList = new CStringList();
		m_sectionDataList = new CStringList();
	}
	
	CIniReader(CString strFile)
	{
		m_strFileName = strFile;
		m_sectionList = new CStringList();
		m_sectionDataList = new CStringList();
	}
	
	~CIniReader()
	{
		delete m_sectionList;
		delete m_sectionDataList;
	}
	
private:
	// lists to keep sections and section data
	CStringList *m_sectionDataList;
	CStringList *m_sectionList;
	
	CString m_strSection;
	long m_lRetValue;
	
	// ini file name 
	CString m_strFileName;
};

#endif // !defined(AFX_INIFILE_H__99976B4B_DBA1_4D1E_AA14_CBEB63042FD1__INCLUDED_)
