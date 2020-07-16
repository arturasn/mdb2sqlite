#define _AFXDLL
#include "stdafx.h"
#include "RelationshipStatements.h"
#include "FieldStatements.h"
#include <afxdao.h>
#include "mdb2sqlite.h"
#include <wx/textctrl.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

void CRelationships::Relationhips(CDaoDatabase &db, std::vector<CString> &RelationFields, const short nRelationCount)
{
		CString sStatement;
		CString temp;
		CDaoRelationInfo relationinfo;
		for( int i1 = 0; i1 < nRelationCount; ++i1 )
		{
			db.GetRelationInfo(i1,relationinfo,AFX_DAO_ALL_INFO);
			if( relationinfo.m_strTable.Find('{') != -1 || relationinfo.m_strForeignTable.Find('{') != -1 || relationinfo.m_strName.Find('{') != -1 || relationinfo.m_strTable.Find('}') != -1 || relationinfo.m_strForeignTable.Find('}') != -1 || relationinfo.m_strName.Find('}') != -1)
				continue;
			if(relationinfo.m_lAttributes & dbRelationDeleteCascade)
			{
				short nRelationFields = relationinfo.m_nFields;
				sStatement = _T("CREATE TRIGGER ");
				sStatement += relationinfo.m_strName;
				sStatement += _T("Delete AFTER DELETE ON ");
				sStatement +=  relationinfo.m_strTable;
				sStatement += _T(" FOR EACH ROW BEGIN");
				for( int i2 = 0; i2 < nRelationFields; ++i2 )
				{ 
					sStatement += _T(" DELETE FROM ");
					sStatement += relationinfo.m_strForeignTable;
					sStatement += _T(" WHERE ");
					sStatement += relationinfo.m_pFieldInfos[i2].m_strForeignName;
					sStatement += _T(" = OLD.");
					sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
					sStatement += _T(";");
				}
				sStatement += _T(" END; ");
				RelationFields.push_back(sStatement);
			}
		}
		for( int i1 = 0; i1 < nRelationCount; ++i1 )
		{
				db.GetRelationInfo(i1,relationinfo,AFX_DAO_ALL_INFO);
				if( relationinfo.m_strTable.Find('{') != -1 || relationinfo.m_strForeignTable.Find('{') != -1 || relationinfo.m_strName.Find('{') != -1 || relationinfo.m_strTable.Find('}') != -1 || relationinfo.m_strForeignTable.Find('}') != -1 || relationinfo.m_strName.Find('}') != -1)
				   continue;
				if(relationinfo.m_lAttributes & dbRelationUpdateCascade)
				{
					short nRelationFields = relationinfo.m_nFields;
					sStatement = _T("CREATE TRIGGER ");
					sStatement += relationinfo.m_strName;
					sStatement += _T("Update AFTER UPDATE OF ");
					for( int i2 = 0; i2 < nRelationFields; ++i2 )
					{
						sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
						if( i2 != nRelationFields-1 )
							sStatement += _T(", ");
					}
					sStatement += _T(" ON ");
					sStatement += relationinfo.m_strTable;
					sStatement += _T(" BEGIN ");
					for( int i2 = 0; i2 < nRelationFields; ++i2 )
					{
						sStatement += _T("UPDATE ");
						sStatement += relationinfo.m_strForeignTable;
						sStatement += _T(" SET ");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strForeignName;
						sStatement += _T(" = new.");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
						sStatement += _T(" WHERE ");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strForeignName;
						sStatement += _T(" = OLD.");
						sStatement += relationinfo.m_pFieldInfos[i2].m_strName;
						sStatement += _T("; ");
					}
					sStatement += _T("END; ");
					RelationFields.push_back(sStatement);
				}
		}		  
}
void CRelationships::ForeignKeySupport(CDaoDatabase &db, const unsigned &nRelationCount, std::vector<CString> &TableField, std::vector<CString> &ForeignKeySupportinfo, 
									   std::vector<CString> &sTableNames, std::vector<CString> &statements, std::vector<int> &beginning, std::vector<int> &end, std::vector<CString> &InsertStatements,
									   bool &isPossibleToAddForeignKeys, unsigned &nWarningCount, const bool &m_bForeignKeyPrimary, CUIObs *pObs)
{
	CString temp;
	CDaoRelationInfo relationinfo;
	std::vector <int> priority;
	bool DoContinue;
	unsigned nTreeSize = statements.size();
	int** tree = new int*[nTreeSize];
    for(unsigned i = 0; i < nTreeSize; ++i)
       tree[i] = new int[nTreeSize];
	Preparation(tree,nTreeSize);
	for( unsigned i = 0; i < nRelationCount; ++i )
	{
		db.GetRelationInfo(i, relationinfo, AFX_DAO_ALL_INFO);
		if (relationinfo.m_lAttributes & dbRelationDontEnforce) {
			continue;
		}
	    DoContinue = false;
		if( m_bForeignKeyPrimary )
		{
			unsigned nVectorLength = TableField.size();
			short nRelationFields = relationinfo.m_nFields;
			for(short i3 = 0; i3 < nRelationFields; ++i3)
			{
				CString temp2 = relationinfo.m_strTable;
						temp2 += relationinfo.m_pFieldInfos[i3].m_strName;
				CString temp3 = relationinfo.m_strForeignTable;
						temp3 += relationinfo.m_pFieldInfos[i3].m_strForeignName;

				bool FirstField = true;
				bool SecondField = true;

				for( unsigned i2 = 0; i2 < nVectorLength; ++i2 )
				{
					if( !(TableField[i2].Compare(temp2)) )
								FirstField = false;
				}

				for( unsigned i2 = 0; i2 < nVectorLength; ++i2 )
				{
					if( !(TableField[i2].Compare(temp3)) )
								SecondField = false;
				}
				if(FirstField && SecondField)
				{
					DoContinue = true;
					continue;
				}
			}
		}
		
		if (DoContinue) {
			continue;
		}

		int idx1 = CRelationships::Findind(sTableNames, relationinfo.m_strForeignTable, nTreeSize);
		int idx2 = CRelationships::Findind(sTableNames, relationinfo.m_strTable, nTreeSize);

		if (idx1 == -1 || idx2 == -1) continue;

		temp = relationinfo.m_strForeignTable;
		temp += _T("FOREIGN KEY(`");
		unsigned nRelationFields = relationinfo.m_nFields;

		for( unsigned i1 = 0; i1 < nRelationFields; ++i1)
		{
			temp += relationinfo.m_pFieldInfos[i1].m_strForeignName;
			if( i1 != nRelationFields-1 )
				temp += _T("`,`");
			else temp += "`)";
		}

		temp += _T(" REFERENCES ");
		temp += relationinfo.m_strTable;
		++tree[idx1][idx2];
		temp += _T("(");

		for( unsigned i1 = 0; i1 < nRelationFields; ++i1 )
		{
			temp += relationinfo.m_pFieldInfos[i1].m_strName;
			if( i1 != nRelationFields-1 )
				temp += _T(",");
			else temp += ")";
			CString temp2 = relationinfo.m_strTable;
			temp2 += relationinfo.m_pFieldInfos[i1].m_strName;
			unsigned nVectorLength = TableField.size();
			for(unsigned i2 = 0; i2 < nVectorLength; ++i2)
			{
				if(!(TableField[i2].Compare(temp2)))
				{
					++nWarningCount;
					std::wstring sWarningMessage = L"WARNING: Foreign key of table: ";
                    sWarningMessage += CFieldStatements::CstringToWxString(relationinfo.m_strForeignTable);
                    sWarningMessage += L" References table: ";
                    sWarningMessage += CFieldStatements::CstringToWxString(relationinfo.m_strTable);
                    sWarningMessage += L" field: ";
                    sWarningMessage += CFieldStatements::CstringToWxString(relationinfo.m_pFieldInfos[i1].m_strName);
                    sWarningMessage += L" which is not a PRIMARY KEY.\n";
					pObs->SetDefaultStyle(wxTextAttr (wxNullColour, *wxYELLOW));
					pObs->WriteText(sWarningMessage);
					pObs->SetDefaultStyle(wxTextAttr(wxNullColour));
					break;
				}
			}
		}  

		ForeignKeySupportinfo.push_back(temp);
	}
	CRelationships::ForeignKeyPriority(nTreeSize, tree, priority);

	for(unsigned i = 0; i < nTreeSize; ++i)
      delete [] tree[i];
    delete [] tree;

	if(priority.size() != nTreeSize)
	{
		std::wstring sWarningMessage = L"WARNING: Sqlite foreing key support cannot be added for the current database as there is a loop between the table references";
		sWarningMessage += L"\n";
		pObs->SetDefaultStyle(wxTextAttr (wxNullColour, *wxRED));
		pObs->WriteText(sWarningMessage);
		pObs->SetDefaultStyle(wxTextAttr (wxNullColour));
		isPossibleToAddForeignKeys = false;
		return;
	}

	CRelationships::CreatePriority(statements, priority, sTableNames, beginning, end, InsertStatements);
}
void CRelationships::ForeignKeyPriority(const unsigned &nTreeSize, int **&tree, std::vector<int> &priority)
{
	bool isempty;
	for( unsigned i3 = 0; i3 < nTreeSize; ++i3 )
	{
		for( unsigned i = 0; i < nTreeSize; ++i )
		{
			if( Repeat(i,priority) )
				isempty = false;
			else isempty = true;
			for( unsigned i1 = 0; i1 < nTreeSize; ++i1 )
			{
				if( tree[i][i1] )
				{
					isempty = false;
					break;
				}
				if( !isempty )
					break;
			}
			if( isempty )
			{
				for( unsigned i2 = 0; i2 < nTreeSize; ++i2 )
					tree[i2][i] = 0;
				priority.push_back(i);
				if( priority.size() == nTreeSize )
					return;
			}
		}
	}
}
int CRelationships::Findind(std::vector<CString> &sTableNames, CString &sSearchedTable, const unsigned &nTreeSize)
{
	for(unsigned i3 = 0; i3 < nTreeSize; ++i3)
	{
		if( !(sTableNames[i3].Compare(sSearchedTable)) ) {
			return i3;
		}
	}
	
	//MS added system tables
	return -1;
}

void CRelationships::Preparation(int **&tree, const unsigned &nTreeSize)
{
	for( unsigned i = 0; i < nTreeSize; ++i )
	 for( unsigned i1 = 0; i1 < nTreeSize; ++i1 )
	 {
		 tree[i][i1] = 0;
	 }
}
bool CRelationships::Repeat(int index, std::vector<int> &priority)
{
	auto end_it = priority.end();
	for( auto it = priority.begin(); it != end_it; ++it )
	{
		if(*it == index)
			return true;
	}
	return false;
}
void CRelationships::CreatePriority(std::vector<CString> &statements, std::vector<int> &priority, std::vector<CString> &sTableNames, std::vector<int> &beginning, 
	                                std::vector<int> &end, std::vector <CString> &InsertStatements)
{
	unsigned nTableCount = priority.size();
	unsigned nInsertCount = InsertStatements.size();
	CString *temp = new CString[nTableCount];
	CString *temp2 = new CString[nTableCount];
	CString *temp3 = new CString[nInsertCount];
	for(unsigned i = 0; i < nTableCount; ++i)
	{
		temp2[i] = sTableNames[i];
		temp[i] = statements[i];
	}
	for(unsigned i = 0; i < nTableCount; ++i)
	{
		statements[i] = temp[priority[i]];
		sTableNames[i] = temp2[priority[i]];
	}
	unsigned nInsertIndex = 0;
	for(unsigned i = 0; i < nInsertCount; ++i)
	   temp3[i] = InsertStatements[i];
	for(unsigned i = 0; i < nTableCount; ++i)
	{
		for(int i1 = beginning[priority[i]]; i1 <= end[priority[i]]; ++i1)
			InsertStatements[nInsertIndex++] = temp3[i1];
	}
	delete [] temp;
	delete [] temp2;
	delete [] temp3;
}