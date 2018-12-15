#include "StdAfx.h"
#include "RegenTool.h"
#include "MainFrm.h"
#include "RegenToolDoc.h"
#include "RegenToolView.h"
#include "MHMap.h"
#include "MHFile.h"
#include "SheetPageMonster.h"
#include "LoadList.h"

IMPLEMENT_DYNCREATE(CRegenToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CRegenToolDoc, CDocument)
	//{{AFX_MSG_MAP(CRegenToolDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegenToolDoc construction/destruction

CRegenToolDoc::CRegenToolDoc()
{
}

CRegenToolDoc::~CRegenToolDoc()
{
}


void CRegenToolDoc::LoadRegenScript(LPCTSTR path)
{
	mScriptMap.clear();

	CMHFile file;
	file.Init(
		path,
		"rb");

	const DWORD oneMinute = 60 * 1000;
	DWORD groupIndex = 0;

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));
		LPCTSTR seperator = " \t{}$#";
		LPCTSTR token = _tcstok(buffer, seperator);

		if(0 == token)
		{
			continue;
		}
		else if('@' == token[0])
		{
			continue;
		}
		else if(0 == _tcsicmp("group", token))
		{
			LPCTSTR indexText = _tcstok(0, seperator);
			groupIndex = _ttoi(indexText ? indexText : "");
			Script& script = mScriptMap[groupIndex];
			script.mGroupIndex = groupIndex;
		}
		else if(0 == _tcsicmp("groupname", token))
		{
			LPCTSTR groupName = _tcstok(0, seperator);
			Script& script = mScriptMap[groupIndex];
			script.mGroupName = groupName;
		}
		else if(0 == _tcsicmp("unique", token))
		{
			LPCTSTR indexText = _tcstok(0, seperator);
			groupIndex = _ttoi(indexText ? indexText : "");
			Script& script = mScriptMap[groupIndex];
			script.mType = Script::TypeUnique;
			script.mGroupIndex = groupIndex;
		}
		else if(0 == _tcsicmp("addCondition", token))
		{
			LPCTSTR targetGroupText = _tcstok(0, seperator);
			LPCTSTR ratioText = _tcstok(0, seperator);
			LPCTSTR delayText = _tcstok(0, seperator);
			LPCTSTR regenText = _tcstok(0, seperator);
			LPCTSTR rangeText = _tcstok(0, seperator);

			Script::Condition condition;
			ZeroMemory(&condition, sizeof(condition));
			condition.mGroupIndex = _ttoi(targetGroupText ? targetGroupText : "");
			condition.mRatio = float(_tstof(ratioText ? ratioText : ""));
			condition.mDelay = _ttoi(delayText ? delayText : "");
			condition.mIsRegen = _ttoi(regenText ? regenText : "");
			condition.mRange = _ttoi(rangeText ? rangeText : "");

			Script& script = mScriptMap[groupIndex];
			condition.dwConditionIdx = script.mConditionList.size() + 1;

			script.mConditionList.push_back(condition);
		}
		else if(0 == _tcsicmp("uniqueAddCondition", token))
		{
			LPCTSTR targetGroupText = _tcstok(0, seperator);
			LPCTSTR ratioText = _tcstok(0, seperator);
			LPCTSTR delayText = _tcstok(0, seperator);
			LPCTSTR regenText = _tcstok(0, seperator);
			LPCTSTR rangeText = _tcstok(0, seperator);

			Script::Condition condition;
			ZeroMemory(&condition, sizeof(condition));
			condition.mGroupIndex = _ttoi(targetGroupText ? targetGroupText : "");
			condition.mRatio = float(_tstof(ratioText ? ratioText : ""));
			condition.mDelay = _ttoi(delayText ? delayText : "");
			condition.mIsRegen = _ttoi(regenText ? regenText : "");
			condition.mRange = _ttoi(rangeText ? rangeText : "");

			Script& script = mScriptMap[groupIndex];
			condition.dwConditionIdx = script.mConditionList.size() + 1;

			script.mConditionList.push_back(condition);
		}
		else if(0 == _tcsicmp("add", token))
		{
			LPCTSTR objectKindText = _tcstok(0, seperator);
			_tcstok(0, seperator);
			LPCTSTR monsterKindText = _tcstok(0, seperator);
			LPCTSTR xText = _tcstok(0, seperator);
			LPCTSTR zText = _tcstok(0, seperator);
			LPCTSTR finiteStateMachine = _tcstok(0, seperator);

			Script& script = mScriptMap[groupIndex];

			Script::Monster monster;
			monster.mObjectKind = EObjectKind(_ttoi(objectKindText ? objectKindText : ""));
			monster.mMonsterKind = WORD(_ttoi(monsterKindText ? monsterKindText : ""));
			monster.mPosition.x = float(_tstof(xText ? xText : ""));
			monster.mPosition.z = float(_tstof(zText ? zText : ""));
			monster.mFiniteStateMachine = (finiteStateMachine ? finiteStateMachine : "");

			script.mMonsterList.push_back(monster);
		}
		else if(0 == _tcsicmp("uniqueAdd", token))
		{
			LPCTSTR objectKindText = _tcstok(0, seperator);
			_tcstok(0, seperator);
			LPCTSTR monsterKindText = _tcstok(0, seperator);
			LPCTSTR xText = _tcstok(0, seperator);
			LPCTSTR zText = _tcstok(0, seperator);
			LPCTSTR finiteStateMachine = _tcstok(0, seperator);

			Script& script = mScriptMap[groupIndex];
			Script::Monster monster;
			monster.mObjectKind = EObjectKind(_ttoi(objectKindText ? objectKindText : ""));
			monster.mMonsterKind = WORD(_ttoi(monsterKindText ? monsterKindText : ""));
			monster.mPosition.x = float(_tstof(xText ? xText : ""));
			monster.mPosition.z = float(_tstof(zText ? zText : ""));
			monster.mFiniteStateMachine = (finiteStateMachine ? finiteStateMachine : "");

			script.mMonsterList.push_back(monster);
		}
		else if(0 == _tcsicmp("fieldBossRegenPosition", token))
		{
			LPCTSTR xText = _tcstok(0, seperator);
			LPCTSTR zText = _tcstok(0, seperator);

			VECTOR3 position = {0};
			position.x = float(_tstof(xText ? xText : ""));
			position.z = float(_tstof(zText ? zText : ""));

			Script& script = mScriptMap[groupIndex];
			script.mFieldBossPositionList.push_back(position);
		}
		else if(0 == _tcsicmp("uniqueRegenDelay", token))
		{
			LPCTSTR secondText = _tcstok(0, seperator);
			const DWORD tickCount = _ttoi(secondText ? secondText : "");

			Script::Delay delay = {0};
			delay.mMaxTick = tickCount * oneMinute;
			delay.mMinTick = tickCount * oneMinute;

			Script& script = mScriptMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
		else if(0 == _tcsicmp("randomRegenDelay", token))
		{
			LPCTSTR minSecondText = _tcstok(0, seperator);
			LPCTSTR maxSecondText = _tcstok(0, seperator);

			const DWORD minSecond = _ttoi(minSecondText ? minSecondText : "");
			const DWORD maxSecond = _ttoi(maxSecondText ? maxSecondText : "");

			Script::Delay delay = {0};
			delay.mMinTick = minSecond * oneMinute;
			delay.mMaxTick = maxSecond * oneMinute;

			Script& script = mScriptMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
		else if(0 == _tcsicmp("uniqueRandomRegenDelay", token))
		{
			LPCTSTR minSecondText = _tcstok(0, seperator);
			_tcstok(0, seperator);

			const DWORD minSecond = _ttoi(minSecondText ? minSecondText : "");

			Script::Delay delay = {0};
			delay.mMinTick = oneMinute;
			delay.mMaxTick = minSecond * oneMinute;

			Script& script = mScriptMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
		else if(0 == _tcsicmp("uniqueRandomRegenDelay2", token))
		{
			LPCTSTR minSecondText = _tcstok(0, seperator);
			LPCTSTR maxSecondText = _tcstok(0, seperator);

			const DWORD minSecond = _ttoi(minSecondText ? minSecondText : "");
			const DWORD maxSecond = _ttoi(maxSecondText ? maxSecondText : "");

			Script::Delay delay = {0};
			delay.mMinTick = minSecond * oneMinute;
			delay.mMaxTick = maxSecond * oneMinute;

			Script& script = mScriptMap[groupIndex];
			script.mDelayList.push_back(delay);
		}
	}

	// Script 정보로 Data Update, GXO 생성/배치
	UpdateDataFromScript();
}

void CRegenToolDoc::UpdateDataFromScript()
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )				return;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return;
	pMonsterPage->ResetTree();

	for(ScriptMap::iterator scriptIterator = mScriptMap.begin() ; mScriptMap.end() != scriptIterator ; ++scriptIterator)
	{
		const DWORD groupIndex = scriptIterator->first;
		Script& script = scriptIterator->second;
		Script::ConditionList& conditionList = script.mConditionList;
		
		Script::MonsterList& monsterList = script.mMonsterList;

		// Tree에 Group 정보 추가
		pMonsterPage->AddGroupInTree( groupIndex );

		// Condition 정보 추가
		DWORD dwConditionIdx = 1;
		for( Script::ConditionList::iterator conditionIter = conditionList.begin() ; conditionIter != conditionList.end() ; ++conditionIter, ++dwConditionIdx )
		{
			pMonsterPage->AddConditionInTree( groupIndex, dwConditionIdx );
		}


		// Monster 정보 추가
		for(Script::MonsterList::iterator monsterIterator = monsterList.begin() ; monsterList.end() != monsterIterator ; ++monsterIterator)
		{
			Script::Monster& monster = *monsterIterator;

			DIRECTORYMGR->SetLoadMode( eLM_Monster );
			const BASE_MONSTER_LIST& mlist = g_pLoadList.GetMonster( monster.mMonsterKind );
			monster.hGXO = g_pExecutive->CreateGXObject( LPSTR(mlist.ChxName), MHPlayerPROC, 0, 0 );

			if(0 == monster.hGXO)
			{
				monster.hGXO = g_pExecutive->CreateGXObject( "L008.chx", MHPlayerPROC, 0, 0 );
			}

			const BASE_MONSTER_LIST& baseMonsterInfo = g_pLoadList.GetMonster( monster.mMonsterKind );
			// Tree에 Monster 정보 추가
			pMonsterPage->AddMonsterInTree( groupIndex, baseMonsterInfo.Name, monster.hGXO );

			g_pExecutive->GXOEnableHFieldApply( monster.hGXO );
			VECTOR3 TargetPos = monster.mPosition;
			g_pExecutive->GXOSetPosition( monster.hGXO, &TargetPos, FALSE );
			VECTOR3 vecScale = { baseMonsterInfo.Scale, baseMonsterInfo.Scale, baseMonsterInfo.Scale };
			g_pExecutive->GXOSetScale( monster.hGXO, &vecScale );

			DIRECTORYMGR->SetLoadMode(eLM_Root);
		}
	}

	pMonsterPage->ExpandTree();
}

Script* CRegenToolDoc::GetScriptInfo( DWORD dwGroupIdx )
{
	ScriptMap::iterator scriptIter = mScriptMap.find( dwGroupIdx );
	if( scriptIter == mScriptMap.end() )
		return NULL;

	Script& script = scriptIter->second;

	return &script;
}

Script* CRegenToolDoc::GetScriptInfoFromGXOHandle( GXOBJECT_HANDLE hGXO )
{
	for(ScriptMap::iterator scriptIter = mScriptMap.begin() ; mScriptMap.end() != scriptIter ; ++scriptIter)
	{
		Script& script = scriptIter->second;
		Script::MonsterList& monsterList = script.mMonsterList;

		for(Script::MonsterList::iterator monsterIter = monsterList.begin() ; monsterList.end() != monsterIter ; ++monsterIter)
		{
			Script::Monster& monster = *monsterIter;
			if( monster.hGXO == hGXO )
				return &script;
		}
	}

	return NULL;
}

Script::Condition* CRegenToolDoc::GetConditionInfo( DWORD dwGroupIdx, int nIndex )
{
	Script* const script = GetScriptInfo(dwGroupIdx);

	if(0 == script)
	{
		return 0;
	}

	Script::ConditionList& conditionList = script->mConditionList;

	for( Script::ConditionList::iterator conditionIter = conditionList.begin() ; conditionIter != conditionList.end() ; ++conditionIter )
	{
		if( conditionIter->dwConditionIdx == DWORD(nIndex))
		{
			Script::Condition& condition = *conditionIter;
			return &condition;
		}
	}

	return NULL;
}

Script::Monster* CRegenToolDoc::GetMonsterInfo( DWORD dwGroupIdx, GXOBJECT_HANDLE hGXO )
{
	ScriptMap::iterator scriptIter = mScriptMap.find( dwGroupIdx );
	if( scriptIter == mScriptMap.end() )
		return NULL;

	Script& script = scriptIter->second;
	Script::MonsterList& monsterList = script.mMonsterList;
	for(Script::MonsterList::iterator monsterIter = monsterList.begin() ; monsterList.end() != monsterIter ; ++monsterIter)
	{
		Script::Monster& monster = *monsterIter;
		if( monster.hGXO == hGXO )
			return &monster;
	}

	return NULL;
}

Script::Monster* CRegenToolDoc::GetMonsterInfo( GXOBJECT_HANDLE hGXO )
{
	for(ScriptMap::iterator scriptIter = mScriptMap.begin() ; mScriptMap.end() != scriptIter ; ++scriptIter)
	{
		Script& script = scriptIter->second;
		Script::MonsterList& monsterList = script.mMonsterList;

		for(Script::MonsterList::iterator monsterIter = monsterList.begin() ; monsterList.end() != monsterIter ; ++monsterIter)
		{
			Script::Monster& monster = *monsterIter;
			if( monster.hGXO == hGXO )
				return &monster;
		}
	}

	return NULL;
}

const DWORD CRegenToolDoc::GetUsableGroupIndex() const
{
	DWORD dwGroup = 1;
	while( mScriptMap.find( dwGroup ) != mScriptMap.end() )
	{
		++dwGroup;
	}

	return dwGroup;
}

Script* CRegenToolDoc::AddGroup()
{
	DWORD dwAddGroupIdx = GetUsableGroupIndex();
	ScriptMap::iterator scriptIter = mScriptMap.find( dwAddGroupIdx );
	// 이미 존재하는 경우(ERROR)
	if( scriptIter != mScriptMap.end() )
		return NULL;
	
	Script& script =  mScriptMap[ dwAddGroupIdx ];
	script.mGroupIndex = dwAddGroupIdx;

	return &script;
}

BOOL CRegenToolDoc::DeleteGroup( DWORD dwGroup )
{
	CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
	if( !pFrm )		return FALSE;
	CRegenToolView* pView = (CRegenToolView*)pFrm->GetActiveView();
	if( !pView )	return FALSE;
	CSheetPageMonster* pMonsterPage = (CSheetPageMonster*)pFrm->GetMonsterPage();
	if( !pMonsterPage )		return FALSE;

	ScriptMap::iterator scriptIter = mScriptMap.find( dwGroup );
	if( scriptIter == mScriptMap.end() )
		return FALSE;


	Script& script = scriptIter->second;

	// 몬스터 List 제거
	for( Script::MonsterList::const_iterator monsterIter = script.mMonsterList.begin() ; monsterIter != script.mMonsterList.end() ; ++monsterIter )
	{
		pMonsterPage->DeleteMonsterInTree( monsterIter->hGXO );
		if( pView->GetSelectedObjectHandle() == monsterIter->hGXO )
			pView->SetSelectedObjectHandle( NULL );
		g_pExecutive->DeleteGXObject( monsterIter->hGXO );
	}
	script.mMonsterList.clear();
	
	mScriptMap.erase( scriptIter );
	return TRUE;
}

const DWORD CRegenToolDoc::GetUsableConditionIndex( const DWORD dwGroup ) const
{
	ScriptMap::const_iterator scriptIter = mScriptMap.find( dwGroup );
	if( scriptIter == mScriptMap.end() )
		return 0;

	const Script& script = scriptIter->second;

	std::set<DWORD> setUsedIdx;
	for( Script::ConditionList::const_iterator conditionIter = script.mConditionList.begin() ; conditionIter != script.mConditionList.end() ; ++conditionIter )
	{
		setUsedIdx.insert( conditionIter->dwConditionIdx );
	}

	DWORD dwConditionIdx = 1;
	while( setUsedIdx.find( dwConditionIdx ) != setUsedIdx.end() )
	{
		++dwConditionIdx;
	}

	return dwConditionIdx;
}

Script::Condition* CRegenToolDoc::AddCondition( DWORD dwGroup )
{
	ScriptMap::iterator scriptIter = mScriptMap.find( dwGroup );
	if( scriptIter == mScriptMap.end() )
		return NULL;

	Script& script =  mScriptMap[ dwGroup ];
	static Script::Condition emptyCondition;
	script.mConditionList.push_back( emptyCondition );
	Script::Condition& condition = script.mConditionList.back();
	condition.dwConditionIdx = GetUsableConditionIndex( dwGroup );

	script.mConditionList.sort();

	return &condition;
}

BOOL CRegenToolDoc::DeleteCondition( DWORD dwGroup, DWORD dwConditionIdx )
{
	ScriptMap::iterator scriptIter = mScriptMap.find( dwGroup );
	if( scriptIter == mScriptMap.end() )
		return NULL;

	Script& script = mScriptMap[ dwGroup ];

	for( Script::ConditionList::iterator conditionIter = script.mConditionList.begin() ; conditionIter != script.mConditionList.end() ; ++conditionIter )
	{
		if( conditionIter->dwConditionIdx == dwConditionIdx )
		{
			script.mConditionList.erase( conditionIter );
			return TRUE;
		}
	}
	return FALSE;
}

Script::Monster* CRegenToolDoc::AddMonster( DWORD dwGroupIdx )
{
	ScriptMap::iterator scriptIter = mScriptMap.find( dwGroupIdx );
	if( scriptIter == mScriptMap.end() )
		return NULL;

	Script& script =  mScriptMap[ dwGroupIdx ];
	static Script::Monster emptyMonster;
	script.mMonsterList.push_back(emptyMonster);
	Script::Monster& monster = script.mMonsterList.back();
	return &monster;
}

BOOL CRegenToolDoc::DeleteMonster( GXOBJECT_HANDLE hGXO )
{
	for(ScriptMap::iterator scriptIter = mScriptMap.begin() ; mScriptMap.end() != scriptIter ; ++scriptIter)
	{
		Script& script = scriptIter->second;
		Script::MonsterList& monsterList = script.mMonsterList;

		for(Script::MonsterList::iterator monsterIter = monsterList.begin() ; monsterList.end() != monsterIter ; ++monsterIter)
		{
			Script::Monster& monster = *monsterIter;
			if( monster.hGXO == hGXO )
			{
				CMainFrame* pFrm = (CMainFrame*)AfxGetMainWnd();
				if( !pFrm )		return FALSE;
				CRegenToolView* pView = (CRegenToolView*)pFrm->GetActiveView();
				if( !pView )	return FALSE;

				if( pView->GetSelectedObjectHandle() == monster.hGXO )
					pView->SetSelectedObjectHandle( NULL );

				monsterList.erase( monsterIter );
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CRegenToolDoc::DataToScript( CString& text )
{
	TCHAR textComputerName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
	DWORD textSize = sizeof(textComputerName) / sizeof(*textComputerName);
	GetComputerName( textComputerName, &textSize );
	TCHAR textUserName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
	GetUserName( textUserName, &textSize );
	SYSTEMTIME systemTime = {0};
	GetLocalTime( &systemTime );

	text += _T("@\r\n");
	
	CString buffer;
	buffer.Format( _T("@ designer: %s\r\n" ), textUserName );
	text += buffer;

	buffer.Format( _T("@ computer: %s\r\n"), textComputerName );
	text += buffer;

	buffer.Format(
		_T("@ date: %04d.%02d.%02d %02d:%02d:%02d\r\n"),
		systemTime.wYear,
		systemTime.wMonth,
		systemTime.wDay,
		systemTime.wHour,
		systemTime.wMinute,
		systemTime.wSecond);
	text += buffer;
	text += _T("@\r\n");

	for( ScriptMap::const_iterator scriptIter = mScriptMap.begin() ; scriptIter != mScriptMap.end() ; ++scriptIter )
	{
		const DWORD groupIndex = scriptIter->first;
		const Script& script = scriptIter->second;
		const Script::ConditionList& conditionList = script.mConditionList;
		const Script::MonsterList& monsterList = script.mMonsterList;

		switch(script.mType)
		{
		case Script::TypeNone:
			{
				buffer.Format( "$Group %d\r\n{\r\n", groupIndex );
				text += buffer;
				break;
			}
		case Script::TypeUnique:
			{
				buffer.Format( "$Unique %d\r\n{\r\n", groupIndex );
				text += buffer;
				break;
			}
		}

		buffer.Format( "\t#GROUPNAME\t%s\r\n", script.mGroupName );
		text += buffer;

		for( Script::ConditionList::const_iterator conditionIter = conditionList.begin() ; conditionIter != conditionList.end() ; ++conditionIter )
		{
			const Script::Condition& condition = *conditionIter;
			buffer.Format( "\t#ADDCONDITION\t%d\t%.1f\t%d\t%d\t%d\r\n",
				condition.mGroupIndex,
				condition.mRatio,
				condition.mDelay,
				condition.mIsRegen,
				condition.mRange );
			text += buffer;
		}

		for(Script::MonsterList::const_iterator monsterIter = monsterList.begin() ; monsterIter != monsterList.end() ; ++monsterIter)
		{
			const Script::Monster& monster = *monsterIter;
			buffer.Format(
				"\t#ADD\t%d\t%d\t%d\t%.0f\t%.0f\t%s\r\n",
				monster.mObjectKind,
				0,
				monster.mMonsterKind,
				monster.mPosition.x,
				monster.mPosition.z,
				monster.mFiniteStateMachine);
			text += buffer;
		}

		text += "}\r\n\r\n";
	}
}

BOOL CRegenToolDoc::OnSaveDocument(LPCTSTR path) 
{
	CString text;
	DataToScript( text );
	CMHFile::Save( path, text );

	return TRUE;
}