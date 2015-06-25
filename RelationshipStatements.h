#ifndef __INCLUDED_RELATIONSHIPSTATEMENTS_H__
#define __INCLUDED_RELATIONSHIPSTATEMENTS_H__
#include "stdafx.h"
#include "naujastestas.h"
#include <afxdao.h>
#include <afxdb.h>
#include <vector>
#pragma once

class CRelationships
{
	public: 
		void Relationhips(CDaoDatabase &db, std::vector<CString> &RelationFields, const short nRelationCount);
};
#endif