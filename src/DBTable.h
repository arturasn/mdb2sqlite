#ifndef __DB_TABLE_H__
#define __DB_TABLE_H__
#pragma once

#ifndef __DB_FIELD_H__
	#include "DBField.h"
#endif

class CDBTable
{
public:
	CDBTable(CString sName, std::vector<CDBFieldIndex> &fields, std::vector<CDBFieldIndex> &indexes) : m_fields(fields), m_indexes(indexes), m_sName(sName)
	{ }
	virtual ~CDBTable() { }

public:
	std::vector<CDBFieldIndex> & GetFields()	{ return m_fields;	}
	std::vector<CDBFieldIndex> & GetIndexes()	{ return m_indexes; }
	CString GetName() const						{ return m_sName;	}

private:
	std::vector<CDBFieldIndex> m_fields;
	std::vector<CDBFieldIndex> m_indexes;
	CString m_sName;
};

#endif