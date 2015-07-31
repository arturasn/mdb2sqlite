#ifndef __INCLUDED_RELATIONSHIPSTATEMENTS_H__
#define __INCLUDED_RELATIONSHIPSTATEMENTS_H__

#pragma once

class CDaoDatabase;
class wxTextCtrl;

#include <vector>

class CRelationships
{
	public: 
		static void Relationhips(CDaoDatabase &db, std::vector<CString> &RelationFields, const short nRelationCount);
		static void ForeignKeySupport(CDaoDatabase &db, const unsigned &nRelationCount, std::vector<CString> (&TableField)[2], std::vector<CString> &RelationFields, wxTextCtrl *PrgDlg = NULL);
};
#endif