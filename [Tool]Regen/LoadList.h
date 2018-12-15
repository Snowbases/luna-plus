// LoadList.h: interface for the CLoadList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADLIST_H__7DB30271_0CE8_4AA9_AE43_E00854FB0381__INCLUDED_)
#define AFX_LOADLIST_H__7DB30271_0CE8_4AA9_AE43_E00854FB0381__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DefineStruct.h"
#include "MHFile.h"
#include "./Engine/EngineObject.h"

class CLoadList  
{
	TCHAR m_szTileName[MAX_MAP_NUM][MAX_FILE_NAME];
	typedef DWORD MonsterKind;
	typedef std::map< MonsterKind, BASE_MONSTER_LIST > MonsterContainer;
	MonsterContainer mMonsterContainer;

public:
	CLoadList();
	virtual ~CLoadList();
	void SetMapCombo(CComboBox&);
	void LoadMonsterList();
	const BASE_MONSTER_LIST& GetMonster(MonsterKind) const;
	void SetMonsterCombo(CComboBox&);
	char* GetTileName(WORD MapNum){return m_szTileName[MapNum];}
};

extern CLoadList g_pLoadList;
#endif // !defined(AFX_LOADLIST_H__7DB30271_0CE8_4AA9_AE43_E00854FB0381__INCLUDED_)
