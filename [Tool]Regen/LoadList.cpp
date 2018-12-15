// LoadList.cpp: implementation of the CLoadList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegenTool.h"
#include "LoadList.h"
#include "./Engine/EngineObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CLoadList g_pLoadList;

CLoadList::CLoadList()
{
}

CLoadList::~CLoadList()
{
}

void CLoadList::SetMapCombo(CComboBox& comboBox)
{
	comboBox.Clear();

	typedef std::map< MAPTYPE, CString > NameContainer;
	NameContainer nameContainer;

	CMHFile file;
	file.Init(
		"system\\resource\\MapMoveList.bin",
		"rb");
	comboBox.AddString(
		_T("맵을 선택하세요"));
	comboBox.AddString(
		_T("---------------"));

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = "\t ";
		_tcstok(
			buffer,
			seperator);
		LPCTSTR textName = _tcstok(
			0,
			seperator);
		_tcstok(
			0,
			seperator);
		LPCTSTR textIndex = _tcstok(
			0,
			seperator);

		const MAPTYPE mapType = MAPTYPE(_ttoi(textIndex ? textIndex : ""));
		TCHAR mapName[MAX_PATH] = {0};
		_parsingKeywordString(
			textName ? textName : "",
			mapName);

		nameContainer.insert(
			std::make_pair(mapType, mapName));
	}

	for(NameContainer::const_iterator iterator = nameContainer.begin();
		nameContainer.end() != iterator;
		++iterator)
	{
		const MAPTYPE mapType = iterator->first;
		const CString& mapName = iterator->second;

		CString text;
		text.Format(
			_T("%3d %s"),
			mapType,
			mapName);
		comboBox.AddString(
			text);
	}

	comboBox.SetCurSel(0);
}

void CLoadList::LoadMonsterList()
{
	CMHFile file;
	file.Init(
		"system/Resource/MonsterList.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH * 2] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = " \t";
		LPCTSTR textKind = _tcstok(buffer, seperator);
		LPCTSTR textName = _tcstok(0, seperator);
		LPCTSTR textEnglishName = _tcstok(0, seperator);
		LPCTSTR textChxName = _tcstok(0, seperator);
		LPCTSTR textDamage1DramaName = _tcstok(0, seperator);
		LPCTSTR textDie1DramaName = _tcstok(0, seperator);
		LPCTSTR textDie2DramaName = _tcstok(0, seperator);
		LPCTSTR textSpecialType = _tcstok(0, seperator);
		LPCTSTR textObjectKind = _tcstok(0, seperator);
		LPCTSTR textMonsterTargetType = _tcstok(0, seperator);		// TargetDlg 분류
		LPCTSTR textMonsterType = _tcstok(0, seperator);			// 유형분류
		LPCTSTR textMonsterAttribute = _tcstok(0, seperator);		// 속성
		LPCTSTR textMonsterRace = _tcstok(0, seperator);			// 대표 종족 코드
		LPCTSTR textLevel = _tcstok(0, seperator);					// 몬스터레벨	
		LPCTSTR textExpPoint = _tcstok(0, seperator);				// 경험치
		LPCTSTR textLife = _tcstok(0, seperator);					// 체력
		LPCTSTR textBaseReinforce = _tcstok(0, seperator);
		LPCTSTR textScale = _tcstok(0, seperator);					// 크기	



		const WORD monsterKind = WORD( _ttoi(textKind ? textKind : "") );
		const WORD objectKind = WORD( _ttoi(textObjectKind ? textObjectKind : "") );
		const float fScale = float( _tstof(textScale ? textScale : "") );

		BASE_MONSTER_LIST& monster = mMonsterContainer[monsterKind];
		ZeroMemory( &monster, sizeof(monster) );
		_parsingKeywordString( textName ? textName : "", monster.Name );
		_tcsncpy( monster.EngName, textEnglishName ? textEnglishName : "", sizeof(monster.EngName) / sizeof(*monster.EngName) );
		_tcsncpy( monster.ChxName, textChxName ? textChxName : "", sizeof(monster.ChxName) / sizeof(*monster.ChxName) );

		monster.MonsterKind = monsterKind;
		monster.ObjectKind = objectKind;
		monster.Scale = fScale;
	}
}

const BASE_MONSTER_LIST& CLoadList::GetMonster(MonsterKind monsterKind) const
{
	const MonsterContainer::const_iterator iterator = mMonsterContainer.find(monsterKind);

	if(mMonsterContainer.end() == iterator)
	{
		static BASE_MONSTER_LIST emptyMonster = {0};
		return emptyMonster;
	}

	return iterator->second;
}

void CLoadList::SetMonsterCombo(CComboBox& comboBox)
{
	comboBox.Clear();
	
	for(MonsterContainer::const_iterator iterator = mMonsterContainer.begin();
		mMonsterContainer.end() != iterator;
		++iterator)
	{
		const DWORD monsterKind = iterator->first;
		const BASE_MONSTER_LIST& baseMonsterList = iterator->second;

		CString text;
		text.Format(
			_T("%4d %s"),
			monsterKind,
			baseMonsterList.Name);
		comboBox.AddString(
			text);
	}

	comboBox.SetCurSel(0);
}