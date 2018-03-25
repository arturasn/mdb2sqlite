#ifndef __DB_FIELD_H__
#define __DB_FIELD_H__
#pragma once

#include "vector"

class CDBFieldIndex
{
public:
	CDBFieldIndex(CString sName, std::vector<CString> &constraints) : m_sName(sName), m_constraints(constraints) {}
	virtual ~CDBFieldIndex() {}

public:
	CString GetName() { return m_sName; }
	std::vector<CString> & GetConstraints() { return m_constraints; }

private:
	CString m_sName;
	std::vector<CString> m_constraints;

};

#endif