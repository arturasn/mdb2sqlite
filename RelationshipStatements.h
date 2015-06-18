#ifndef INCLUDED_RELATIONSHIPSTATEMENTS_H
#define INCLUDED_RELATIONSHIPSTATEMENTS_H
#include "stdafx.h"
#include "naujastestas.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>

class CRelationships
{
	public: 
		void Relationhips(CDaoDatabase &db, std::vector <CString> &RelationFields, const short nRelationCount);
};
#endif