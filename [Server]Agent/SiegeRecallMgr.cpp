//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.cpp
//	DESC		: Implementation part of CSiegeRecallMgr class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "MHFile.h" 

#include "../[CC]ServerModule/Network.h"

#include "../[CC]RecallInfo/SiegeRecall/SiegeTheme.h" 
#include "../[CC]RecallInfo/SiegeRecall/SiegeMap.h"

#include "./SiegeRecallMgr.h"

#include "./AgentDBMsgParser.h"





//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr
//	DESC		: 생성자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::CSiegeRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeRecallMgr
//	DESC		: 소멸자 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::~CSiegeRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Initialize
//	DESC		: 초기화 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Initialize()
{
	// 공성전 소환물 정보가 담긴 스크립트를 로딩한다.
	Load_RecallInfo() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RecallInfo
//	DESC		: 소환 정보 로딩 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Load_RecallInfo()
{
	// 파일 클래스 선언.
	CMHFile file ;


	// SiegeRecall.bin 파일을 읽기 모드로 연다.
	if( !file.Init("./system/resource/SiegeRecall.bin", "rb") )
	{
		Throw_Error( "Failed to initialize SiegeRecall.bin", __FUNCTION__ ) ;
		return ;
	}

	char szBuff[257] = {0};
	BYTE byCommandKind = ERR_INVALID_COMMAND ;

	while( TRUE )
	{
		// 파일의 끝인지 체크한다.
		if( file.IsEOF() ) break ;

		// 명령어를 받는다.
		memset( szBuff, 0, strlen(szBuff) ) ;
		file.GetString(szBuff) ;

		// 주석 명령어 라면, continue 처리를 한다..
		if(strcmp( szBuff, "//" ) == 0)
		{
			memset( szBuff, 0, strlen(szBuff) ) ;
			file.GetLine(szBuff, sizeof(256)) ;
			continue ;
		}
		else if( (strcmp( szBuff, "{" ) == 0) || (strcmp( szBuff, "}" ) == 0 ) ) continue ;

		// 스크립트 명령어 타입을 받는다.
		byCommandKind = Get_CommandKind( szBuff ) ;

		// 명령어 타입에 따른 처리를 한다.
		Command_Process(byCommandKind, &file) ;
	}


	// 파일 클래스를 해제한다.
	file.Release() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_CommandKind
//	DESC		: 스크립트 명령어 타입을 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeRecallMgr::Get_CommandKind(char* pString)
{
	BYTE byCommandKind = ERR_INVALID_COMMAND ;

	// 함수 인자 확인.
	if(!pString)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// 스크립트 명령어의 스트링 사이즈 체크.
	if( strlen(pString) <= 1 )
	{
		Throw_Error("Invalid string size!!", __FUNCTION__) ;
		return byCommandKind ;
	}


	// 스크립트 명령어 타입을 설정한다.
	if( strcmp( pString, "#ADD_THEME" ) == 0 )
	{
		byCommandKind = e_CK_ADD_THEME ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_THEME_IDX" ) == 0 )
	{
		byCommandKind = e_CK_ADD_THEME_IDX ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_MAP" ) == 0 )
	{
		byCommandKind = e_CK_ADD_MAP ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_STEP" ) == 0 )
	{
		byCommandKind = e_CK_ADD_STEP ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#ADD_OBJECT" ) == 0 )
	{
		byCommandKind = e_CK_ADD_OBJECT ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#START_TERM" ) == 0 )
	{
		byCommandKind = e_CK_START_TERM ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#END_TERM" ) == 0 )
	{
		byCommandKind = e_CK_END_TERM ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#COMMAND" ) == 0 )
	{
		byCommandKind = e_CK_COMMAND ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#DIE_RECALL_OBJ" ) == 0 )
	{
		byCommandKind = e_CK_DIE_RECALL_OBJ ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#CHECK_DIE_OBJ" ) == 0 )
	{
		byCommandKind = e_CK_CHECK_DIE_OBJ ;
		return byCommandKind ;
	}

	if( strcmp( pString, "#CHECK_USE_ITEM" ) == 0 )
	{
		byCommandKind = e_CK_USE_ITEM ;
		return byCommandKind ;
	}


	// 스크립트 명령어 타입을 반환한다.
	return byCommandKind ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Command_Process
//	DESC		: 스크립트 명령어 타입에 따른 처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Command_Process(BYTE byCommandKind, CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}

	switch(byCommandKind)
	{
	case e_CK_ADD_THEME :				Add_Theme( pFile ) ;		break ;
	//	
	case e_CK_ADD_THEME_IDX :			Add_Theme_Idx( pFile ) ;	break ;

	case e_CK_ADD_MAP :					Add_Map( pFile ) ;			break ;

	case e_CK_ADD_STEP :				Add_Step( pFile ) ;			break ;

	case e_CK_ADD_OBJECT :				Add_Obj( pFile ) ;			break ;

	case e_CK_START_TERM :				Add_StartTerm( pFile ) ;	break ;
	case e_CK_END_TERM :				Add_EndTerm( pFile ) ;		break ;
	case e_CK_COMMAND :					Add_Command( pFile ) ;		break ;

	case e_CK_DIE_RECALL_OBJ :			Die_RecallObj( pFile ) ;	break ;

	case e_CK_CHECK_DIE_OBJ :			Check_Die_Obj( pFile ) ;	break ;

	case e_CK_USE_ITEM :				Check_Use_Item( pFile ) ;	break ;

	default : return ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Theme
//	DESC		: 공성 테마 정보를 추가하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Theme(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 테마 인덱스를 받는다.
	BYTE byIdx = (BYTE)m_Theme.size() ;


	// 중복 테마가 있는지 확인한다.
	M_THEME::iterator it ;
	it = m_Theme.find(byIdx) ;

	if(it != m_Theme.end())
	{
		Throw_Error("Failed add theme, Clone theme!!", __FUNCTION__) ;
		return ;
	}


	// 새로운 테마를 추가한다.
	CSiegeTheme newTheme ;
	newTheme.Set_Idx(byIdx) ;

	m_Theme.insert(std::make_pair(byIdx, newTheme)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ThemeInfo
//	DESC		: 컨테이너에 담기는 인덱스로 테마 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTheme* CSiegeRecallMgr::Get_ThemeInfo(BYTE byIdx)
{
	// 함수 인자 확인.
	M_THEME::iterator it ;
	
	for( it = m_Theme.begin() ; it != m_Theme.end() ; ++it )
	{
		if( it == m_Theme.end() ) return NULL ;

		if( it->second.Get_Idx() != byIdx ) continue ;

		// 테마 정보를 반환한다.
		return &it->second ;
	}


	// NULL을 반환한다.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_ThemeInfoByThemeIdx
//	DESC		: 테마 인덱스로 테마 정보를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
CSiegeTheme* CSiegeRecallMgr::Get_ThemeInfoByThemeIdx(WORD wIdx) 
{
	// 테마 인덱스를 확인한다.
	M_THEME::iterator it ;
	
	for( it = m_Theme.begin() ; it != m_Theme.end() ; ++it )
	{
		if( it->second.Get_ThemeIdx() != wIdx ) continue ;

		// 테마 정보를 확인한다.
		return &it->second ;
	}


	// NULL을 반환한다.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Theme_Idx
//	DESC		: 테마 인덱스를 추가하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Theme_Idx(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 테마 인덱스를 확인한다.
	BYTE byIdx = (BYTE)m_Theme.size() ;


	// 중복 테마 여부를 확인한다.
	M_THEME::iterator it ;
	it = m_Theme.find(byIdx-1) ;

	if(it == m_Theme.end() || byIdx == 0)
	{
		Throw_Error("No theme info!!", __FUNCTION__) ;
		return ;
	}


	// 테마 인덱스를 설정한다.
	it->second.Set_ThemeIdx( pFile->GetWord() ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Map
//	DESC		: 소환물들이 소환 될 맵 정보를 추가하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Map(CMHFile* pFile)
{
	// 함수 인자 확인.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 맵 번호를 읽는다.
	MAPTYPE mapNum = 0 ;
	mapNum = pFile->GetWord() ;

	if( mapNum == 0 )
	{
		Throw_Error("Failed add map, Invalid map num!!", __FUNCTION__) ;
		return ;
	}


	// 현재 테마의 인덱스를 받는다.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed add map, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// 부모 테마를 받는다.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("Failed add map, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Check clone map.
	CSiegeMap* pCloneMap = NULL ;
	pCloneMap = it->second.Get_SiegeMapByMapNum( mapNum ) ;

	if( pCloneMap )
	{
		Throw_Error("Failed add map, Clone map!!", __FUNCTION__) ;
		return ;
	}


	// Create a new siege map.
	CSiegeMap newMap ;
	newMap.Set_ThemeIdx( it->second.Get_ThemeIdx() ) ;
	newMap.Set_Idx( it->second.Get_MapCount() ) ;
	newMap.Set_MapNum(mapNum) ;


	// Add siege map to current theme.
	it->second.Add_SiegeMap( &newMap ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Step
//	DESC		: The function to add step info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 29, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Step(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed add map, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1)) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("Failed add map, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Setting step index.
	CSiegeStep newStep ;
	newStep.Set_ThemeIdx( it->second.Get_ThemeIdx() ) ;
	newStep.Set_ParentMapNum( pCurMap->Get_MapNum() ) ;
	newStep.Set_StepIndex(byStepCount) ;


	// Add step.
	pCurMap->Add_Step(byStepCount, &newStep) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Obj
//	DESC		: The function to add the object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Obj(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive recall info.
	MAPTYPE mapNum		= pFile->GetWord() ;
	DWORD dwObjectIdx	= pFile->GetDword() ;

	float fXpos			= pFile->GetFloat() ;
	float fZpos			= pFile->GetFloat() ;

	BYTE byUseRandomPos = pFile->GetByte() ;
	BYTE byRadius		= pFile->GetByte() ;


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Receive child count.
	BYTE byChildCount = 0 ;
	byChildCount = pStep->Get_ChildCount() ;


	// Setting index.
	CSiege_AddObj newAddObj ;
	newAddObj.Set_Index( byChildCount ) ;


	// Setting object info.
	//newAddObj.Init_ObjInfo(it->second.Get_ThemeIdx(), mapNum, pStep->Get_StepIndex(), dwObjectIdx, fXpos, fZpos, byUseRandomPos, byRadius, 0, 0 ) ;

	st_SIEGEOBJ objInfo ;
	memset(&objInfo, 0, sizeof(st_SIEGEOBJ)) ;

	objInfo.wThemeIdx		= it->second.Get_ThemeIdx() ;
	objInfo.mapNum			= mapNum ;
	objInfo.byStepIdx		= pStep->Get_StepIndex() ;
	objInfo.dwObjectIdx		= dwObjectIdx ;
	objInfo.fXpos			= fXpos ;
	objInfo.fZpos			= fZpos ;
	objInfo.byUseRandomPos	= byUseRandomPos ;
	objInfo.byRadius		= byRadius ;

	newAddObj.Init_ObjInfo(&objInfo) ;


	// Insert add object info.
	pStep->Insert_AddObj( byChildCount, &newAddObj ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_StartTerm
//	DESC		: The function to add start term.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_StartTerm(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pStep->Get_ChildCount() ;
	BYTE byChildKind = pStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeTerm newTerm ;
			newTerm.Set_Index( byIndex ) ;

			pAddInfo->Insert_Sterm( byIndex, &newTerm ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_EndTerm
//	DESC		: The function to add end term.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_EndTerm(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1)) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pStep->Get_ChildCount() ;
	BYTE byChildKind = pStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeTerm newTerm ;
			newTerm.Set_Index( byIndex ) ;

			pAddInfo->Insert_ETerm( byIndex, &newTerm ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Command
//	DESC		: The function to add a command info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Command(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Failed to add obj, Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;

	if( it == m_Theme.end() )
	{
		Throw_Error("Failed to add obj, No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;

	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("Failed to add obj, No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive step count.
	BYTE byStepCount = 0 ;
	byStepCount = pCurMap->Get_StepCount() ;


	// Receive step info.
	CSiegeStep* pStep = NULL ;
	pStep = pCurMap->Get_StepInfo(byStepCount-1) ;

	if(!pStep)
	{
		Throw_Error("Failed to add obj, No step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pStep->Get_ChildCount() ;
	BYTE byChildKind = pStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeCommand newCommand ;
			newCommand.Set_Index( byIndex ) ;

			pAddInfo->Insert_Command( byIndex, &newCommand ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Die_RecallObj
//	DESC		: The function to recall object when a object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Die_RecallObj(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;
	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive a step info.
	BYTE byStepCount = pCurMap->Get_StepCount() ;

	CSiegeStep* pCurStep = NULL ;
	pCurStep = pCurMap->Get_StepInfo(byStepCount-1) ;


	// Check step info.
	if(!pCurStep)
	{
		Throw_Error("No parent step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pCurStep->Get_ChildCount() ;
	BYTE byChildKind = pCurStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pCurStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			CSiegeCommand* pCurCommand = NULL ;
			pCurCommand = pAddInfo->Get_CommandInfo(byIndex-1) ;

			if(!pCurCommand)
			{
				Throw_Error("No parent command info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byCommandChildCount = pCurCommand->Get_ChildCount() ;

			st_DIE_RECALL_OBJ recallObj ;

			recallObj.byIdx = byCommandChildCount ;

			recallObj.objInfo.wThemeIdx			= it->second.Get_ThemeIdx() ;
			recallObj.objInfo.mapNum			= pFile->GetWord() ;
			recallObj.objInfo.byStepIdx			= pCurStep->Get_StepIndex() ;
			recallObj.objInfo.dwObjectIdx		= pFile->GetDword() ;
			recallObj.objInfo.fXpos				= pFile->GetFloat() ;
			recallObj.objInfo.fZpos				= pFile->GetFloat() ;
			recallObj.objInfo.byUseRandomPos	= pFile->GetByte() ;
			recallObj.objInfo.byRadius			= pFile->GetByte() ;

			pCurCommand->Add_Die_Recall_Obj( &recallObj ) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_Die_Obj
//	DESC		: The function to check whether object die or not.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Check_Die_Obj(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;
	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive a step info.
	BYTE byStepCount = pCurMap->Get_StepCount() ;

	CSiegeStep* pCurStep = NULL ;
	pCurStep = pCurMap->Get_StepInfo(byStepCount-1) ;


	// Check step info.
	if(!pCurStep)
	{
		Throw_Error("No parent step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pCurStep->Get_ChildCount() ;
	BYTE byChildKind = pCurStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pCurStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			BYTE byKind = pAddInfo->Get_ChildKind(byIndex-1) ;

			CSiegeTerm* pSiegeTerm = NULL ;

			switch(byKind)
			{
			case e_SIEGE_BASEKIND_START_TERM : 
				{
					pSiegeTerm = pAddInfo->Get_StartTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			case e_SIEGE_BASEKIND_END_TERM :
				{
					pSiegeTerm = pAddInfo->Get_EndTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			default : break ;
			}

			MAPTYPE mapNum = pFile->GetWord() ;
			BYTE byObjIdx = pFile->GetByte() ;

			BYTE bychildIndex = pSiegeTerm->Get_ChildCount() ;

			st_CHECK_OBJ_DIE checkInfo ;
			checkInfo.byIdx = bychildIndex ;
			checkInfo.mapNum = mapNum ;
			checkInfo.byObjectIdx = byObjIdx ;

			pSiegeTerm->Insert_CheckObjDie(&checkInfo) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_Use_Item
//	DESC		: The function to check use item.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Check_Use_Item(CMHFile* pFile)
{
	// Check parameter.
	if(!pFile)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Receive a index of current theme.
	int nIndex = 0 ;
	nIndex = m_Theme.size() ;

	if(nIndex <= 0)
	{
		Throw_Error("Empty theme!!", __FUNCTION__) ;
		return ;
	}


	// Check parent theme.
	M_THEME::iterator it ;
	it = m_Theme.find( (BYTE)(nIndex-1) ) ;
	if( it == m_Theme.end() )
	{
		Throw_Error("No parent theme!!", __FUNCTION__) ;
		return ;
	}


	// Receive a current siege map.
	BYTE byMapCount = it->second.Get_MapCount() ;
	CSiegeMap* pCurMap = NULL ;
	pCurMap = it->second.Get_SiegeMapByIdx(byMapCount-1) ;

	if(!pCurMap)
	{
		Throw_Error("No parent map!!", __FUNCTION__) ;
		return ;
	}


	// Receive a step info.
	BYTE byStepCount = pCurMap->Get_StepCount() ;

	CSiegeStep* pCurStep = NULL ;
	pCurStep = pCurMap->Get_StepInfo(byStepCount-1) ;


	// Check step info.
	if(!pCurStep)
	{
		Throw_Error("No parent step!!", __FUNCTION__) ;
		return ;
	}


	// Check step kind.
	BYTE byChildCount = pCurStep->Get_ChildCount() ;
	BYTE byChildKind = pCurStep->Get_ChildKind(byChildCount-1) ;

	switch(byChildKind)
	{
	case e_STEP_ADD_OBJ :
		{
			CSiege_AddObj* pAddInfo = NULL ;
			pAddInfo = pCurStep->Get_AddObjInfo( byChildCount-1 ) ;

			if(!pAddInfo)
			{
				Throw_Error("No parent addobj info!!", __FUNCTION__) ;
				return ;
			}

			BYTE byIndex = pAddInfo->Get_ChildCount() ;

			BYTE byKind = pAddInfo->Get_ChildKind(byIndex-1) ;

			CSiegeTerm* pSiegeTerm = NULL ;

			switch(byKind)
			{
			case e_SIEGE_BASEKIND_START_TERM : 
				{
					pSiegeTerm = pAddInfo->Get_StartTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			case e_SIEGE_BASEKIND_END_TERM :
				{
					pSiegeTerm = pAddInfo->Get_EndTermInfo(byIndex-1) ;

					if(!pSiegeTerm)
					{
						Throw_Error("No start term info!!", __FUNCTION__) ;
						return ;
					}
				}
				break ;

			default : break ;
			}

			DWORD  dwItemIdx = pFile->GetDword() ;
			WORD	wUseCount = pFile->GetWord() ;

			BYTE bychildIndex = pSiegeTerm->Get_ChildCount() ;

			st_CHECK_USE_ITEM checkInfo ;
			checkInfo.byIdx = bychildIndex ;
			checkInfo.dwItemIdx = dwItemIdx ;
			checkInfo.wUseCount = wUseCount ;

			pSiegeTerm->Insert_CheckUseItem(&checkInfo) ;
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: UserMsgParser
//	DESC		: The function to parsing network message from client.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::UserMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// Check parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a messaeg to base message.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check a protocol.
	switch(pmsg->Protocol)
	{
	case MP_SIEGE_CHEAT_EXECUTE_STEP_SYN :						Cheat_Execute_Step( dwIndex, pMsg ) ;		break ;

	case MP_SIEGE_CHEAT_REMOVE_ALLMAP_ALLOBJ_SYN :
	case MP_SIEGE_CHEAT_REMOVE_ALLMAP_SPECIFYOBJ_SYN :			Remove_Object( pMsg ) ;						break ;

	case MP_SIEGE_CHEAT_REMOVE_SPECIFYMAP_ALLOBJ_SYN :
	case MP_SIEGE_CHEAT_REMOVE_SPECIFYMAP_SPECIFYOBJ_SYN :		Remove_Object_SpecifyMap( pMsg ) ;			break ;

	//case MP_SIEGE_CHEAT_ADDOBJ_SYN :			Cheat_AddObj(dwIndex, pMsg) ;	break ;

	//case MP_SIEGE_CHEAT_INCREASE_STEP_SYN :		Increase_Step(dwIndex, pmsg->dwObjectID, pMsg) ;	break ;

	//case 0 :	Increase_Step(pMsg) ;	break ;

	//case 1 :	Decrease_Step(pMsg) ;	break ;

	// 081011 LYW --- Protocol : 공성 소환물의 소환 상태를 다음 단계로 넘기는 치트 관련 추가.
	case MP_SIEGE_CHEAT_NEXTSTEP_SYN :		
	case MP_SIEGE_CHEAT_NEXTSTEP_OTHER_AGENT_SYN :				Cheat_Execute_NextStep( dwIndex, pMsg ) ;	break ;

	case MP_SIEGE_CHEAT_ENDSTEP_SYN :
	case MP_SIEGE_CHEAT_ENDSTEP_OTHER_AGENT_SYN :				Cheat_Execute_EndStep( dwIndex, pMsg ) ;	break ;		
		
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ServerMsgPaser
//	DESC		: 서버로 부터의 메시지를 파싱하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::ServerMsgPaser(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 기본 메시지로 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// 프로토콜 확인.
	switch(pmsg->Protocol)
	{
	case MP_SIEGERECALL_EXECUTE_STEP_SYN :					Execute_Step( dwIndex, pMsg, dwLength ) ;		break ;	

	case MP_SIEGERECALL_ADDOBJ_ACK :						Add_Obj_Ack( dwIndex, pMsg ) ;					break ;

	case MP_SIEGERECALL_ADDOBJ_NACK :						Add_Obj_Nack( dwIndex, pMsg ) ;					break ;

	case MP_SIEGERECALL_REMOVE_ALLMAP_OBJ_SYN :				Remove_Object( pMsg ) ;							break ;

	case MP_SIEGERECALL_REMOVE_SPECIFYMAP_OBJ_SYN :			Remove_Object_SpecifyMap( pMsg ) ;				break ;

	case MP_SIEGERECALL_REQUEST_OBJINFO_SYN :				Request_ObjInfo_Syn( dwIndex, pMsg ) ;			break ;

	case MP_SIEGERECALL_NOTICE_DIERECALLOBJ_READY_SYN :		Ready_Execute_Command( dwIndex, pMsg ) ;		break ;

	// 081007 LYW --- SiegeRecallMgr : 맵 서버에서, 해당맵의 소환물이 몇개인지 요청하는 작업 추가.
	case MP_SIEGERECALL_REQUEST_RECALLCOUNT_SYN :			RequestRecallCount_Syn( dwIndex, pMsg ) ;		break ;
		
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Execute_Step
//	DESC		: 지정 된 스텝을 실행하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Execute_Step(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}


	// 테마 를 확인한다.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->wData) ;

	if(!pTheme)
	{
		Throw_Error("Invalid theme info!!", __FUNCTION__) ;
		return ;
	}


	// 맵을 확인한다.
	CSiegeMap* pMap ;
	CSiegeStep* pStep ;
	BYTE byChildCount ;
	BYTE byKind ;

	for( BYTE count = 0 ; count < pTheme->Get_MapCount() ; ++count )
	{
		pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByIdx( count ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}

		// 스텝을 확인한다.
		for( BYTE sCount = 0 ; sCount < pMap->Get_StepCount() ; ++sCount )
		{
			pStep = NULL ;
			pStep = pMap->Get_StepInfo( sCount ) ;

			if(!pStep)
			{
				Throw_Error("Invalid step info!!", __FUNCTION__) ;
				continue ;
			}

			// 스텝을 실행한다.
			byChildCount = 0 ;
			byChildCount = pStep->Get_ChildCount() ;

			for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
			{
				byKind = e_STEP_ADD_OBJ ;
				byKind = pStep->Get_ChildKind(byCount) ;

				switch(byKind)
				{
				case e_STEP_ADD_OBJ :	Execute_AddObj( dwIndex, pStep, byCount ) ;	break ;

				default : break ;
				}
			}
		}

		// 맵 서버에, 소환물 로딩 완료 메시지를 전송 한다.
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_ACK ;

		msg.dwObjectID	= 0 ;

		msg.wData		= pMap->Get_MapNum() ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}


	//// 해당 맵이 살아있는지 확인한다.
	//g_pServerTable->SetPositionHead();
	//SERVERINFO* pInfo = g_pServerTable->FindMapServer(pmsg->wData2) ;
	//if( !pInfo )
	//{
	//	// 080831 LYW --- SiegeRecallMgr : 소환 정보를 DB에 업데이트 하고, 
	//	// 결과를 받은 후, map 서버에 소환 명령을 전송하도록 한다.
	//	char txt[128] = {0, } ;

	//	// 쿼리문을 작성한다.
	//	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
	//		pmsg->wData1, pmsg->wData2, pmsg->wData2, pmsg->wData3, 0, e_CK_ADD_OBJECT, 0) ;

	//	// 쿼리문을 실행한다.
	//	g_DB.Query(eQueryType_FreeQuery, eSiegeRecallInsert, 0, txt) ;

	//	return ;
	//}


	// 맵 번호를 지정해서 사용할 경우, 이 코드를 사용한다.
	//CSiegeMap* pMap = NULL ;
	//pMap = pTheme->Get_SiegeMapByMapNum( pmsg->wData2 ) ;

	//if(!pMap)
	//{
	//	Throw_Error("Invalid map info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// 스텝을 확인한다.
	//CSiegeStep* pStep = NULL ;
	//pStep = pMap->Get_StepInfo( (BYTE)(pmsg->wData3) ) ;

	//if(!pStep)
	//{
	//	Throw_Error("Invalid step info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// 스텝을 실행한다.
	//BYTE byChildCount = 0 ;
	//byChildCount = pStep->Get_ChildCount() ;

	//BYTE byKind ;

	//for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	//{
	//	byKind = e_STEP_ADD_OBJ ;
	//	byKind = pStep->Get_ChildKind(byCount) ;

	//	switch(byKind)
	//	{
	//	case e_STEP_ADD_OBJ :	Execute_AddObj( dwIndex, pStep, byCount ) ;	break ;

	//	default : break ;
	//	}
	//}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Execute_AddObj
//	DESC		: 오브젝트 추가를 실행하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Execute_AddObj(DWORD dwIndex, CSiegeStep* pStep, BYTE byChildIdx)
{
	// 함수 인자 확인.
	if(!pStep)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 오브젝트 추가정보 확인.
	CSiege_AddObj* pAddObj = NULL ;
	pAddObj = pStep->Get_AddObjInfo(byChildIdx) ;

	if(!pAddObj)
	{
		Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
		return ;
	}


	// 기본 오브젝트 정보 확인.
	st_SIEGEOBJ* pObjInfo = NULL ;
	pObjInfo = pAddObj->Get_ObjInfo() ;

	if(!pObjInfo)
	{
		Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
		return ;
	}


	// 080916 LYW --- SiegeRecallMgr : 몬스터 추가 처리 수정.
	//////// 080831 LYW --- SiegeRecallMgr : 소환 정보를 DB에 업데이트 하고, 
	//////// 결과를 받은 후, map 서버에 소환 명령을 전송하도록 한다.
	//////char txt[128] = {0, } ;

	//////// 쿼리문을 작성한다.
	//////sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
	//////	pObjInfo->wThemeIdx, pObjInfo->mapNum, pObjInfo->mapNum, pObjInfo->byStepIdx, byChildIdx, e_CK_ADD_OBJECT, 0) ;

	//////// 쿼리문을 실행한다.
	//////g_DB.Query(eQueryType_FreeQuery, eSiegeRecallInsert, 0, txt) ;


	// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
	MSG_SIEGERECALL_OBJINFO msg ;

	msg.Category	= MP_SIEGERECALL ;
	msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	msg.dwObjectID	= dwIndex ;

	pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
	pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
	pObjInfo->byComIndex	= 0 ;
	pObjInfo->wParentMap	= 0 ;

	memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

	g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


	// 시작조건 / 종료조건 / 종료 명령어를 확인한다.
	Execute_TermsAndCommand(pStep, pAddObj) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Execute_TermsAndCommand
//	DESC		: The function to execute terms and command.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Execute_TermsAndCommand(CSiegeStep* pStep, CSiege_AddObj* pAddObj) 
{
	// Check parameter.
	if( !pAddObj )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check start terms.
	BYTE byStartTermCount = pAddObj->Get_StartTermCount() ;

	CSiegeTerm* pStartTerm ;
	for( BYTE bySCount = 0 ; bySCount < byStartTermCount ; ++bySCount )
	{
		// Receive start term info.
		pStartTerm = NULL ;
		pStartTerm = pAddObj->Get_StartTermInfo(bySCount) ;

		// Check term info.
		if(!pStartTerm) continue ;

		// Check child kind.
		switch(pStartTerm->Get_ChildKind(bySCount))
		{
		case e_TERM_NONE :		break ;

		case e_TERM_DIE_OBJ :
			{
				// Receive info.
				st_CHECK_OBJ_DIE* pCheckObjInfo = NULL ;
				pCheckObjInfo = pStartTerm->Get_ChkObjDieInfo(bySCount) ;

				// Check obj info.
				if( pCheckObjInfo )
				{
					MSG_SIEGERECALL_CHECK_OBJ_DIE msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_ADDCHECKOBJDIE_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_START_TERM ;

					msg.check_mapNum	= pCheckObjInfo->mapNum ;
					msg.check_ObjIdx	= pCheckObjInfo->byObjectIdx ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		case e_TERM_USE_ITEM :
			{
				// Receive info.
				st_CHECK_USE_ITEM* pUseItemInfo = NULL ;
				pUseItemInfo = pStartTerm->Get_ChkUseItemInfo(bySCount) ;

				// Check use item info.
				if( pUseItemInfo )
				{
					MSG_SIEGERECALL_CHECK_USE_ITEM msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_USEITEM_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_START_TERM ;

					msg.dwItemIdx		= pUseItemInfo->dwItemIdx ;
					msg.wItemCount		= pUseItemInfo->wUseCount ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		default : break ;
		}
	}


	// Check end terms.
	BYTE byEndTermCount = pAddObj->Get_EndTermCount() ;

	CSiegeTerm* pEndTerm ;
	for( BYTE byECount = 0 ; byECount < byEndTermCount ; ++byECount )
	{
		// Receive end term info.
		pEndTerm = NULL ;
		pEndTerm = pAddObj->Get_EndTermInfo(byECount) ;

		// Check term info.
		if(!pEndTerm) continue ;

		// Check child kind.
		switch(pEndTerm->Get_ChildKind(byECount))
		{
		case e_TERM_NONE :		break ;

		case e_TERM_DIE_OBJ :
			{
				// Receive info.
				st_CHECK_OBJ_DIE* pCheckObjInfo = NULL ;
				pCheckObjInfo = pEndTerm->Get_ChkObjDieInfo(byECount) ;

				// Check obj info.
				if( pCheckObjInfo )
				{
					MSG_SIEGERECALL_CHECK_OBJ_DIE msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_ADDCHECKOBJDIE_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_END_TERM ;

					msg.check_mapNum	= pCheckObjInfo->mapNum ;
					msg.check_ObjIdx	= pCheckObjInfo->byObjectIdx ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		case e_TERM_USE_ITEM :
			{
				// Receive info.
				st_CHECK_USE_ITEM* pUseItemInfo = NULL ;
				pUseItemInfo = pEndTerm->Get_ChkUseItemInfo(byECount) ;

				// Check use item info.
				if( pUseItemInfo )
				{
					MSG_SIEGERECALL_CHECK_USE_ITEM msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_USEITEM_SYN ;

					msg.dwObjectID		= 0 ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;

					msg.byCheckKind		= e_SIEGE_BASEKIND_END_TERM ;

					msg.dwItemIdx		= pUseItemInfo->dwItemIdx ;
					msg.wItemCount		= pUseItemInfo->wUseCount ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		default : break ;
		}
	}


	// Check command.
	BYTE byCommandCount = pAddObj->Get_CommandCount() ;

	CSiegeCommand* pCommand ;
	for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
	{
		// Receive start term info.
		pCommand = NULL ;
		pCommand = pAddObj->Get_CommandInfo(byCCount) ;

		// Check term info.
		if(!pCommand) continue ;

		// Check child kind.
		switch(pCommand->Get_ChildKind(byCCount))
		{
		case e_COMMAND_NONE :					break ;

		case e_COMMAND_RECALL_DIE_OBJ :
			{
				// Receive info.
				st_DIE_RECALL_OBJ* pDieRecallObjInfo = NULL ;
				pDieRecallObjInfo = pCommand->Get_DieRecallObjInfo(byCCount) ;

				// Check obj info.
				if( pDieRecallObjInfo )
				{
					MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ msg ;

					msg.Category		= MP_SIEGERECALL ;
					msg.Protocol		= MP_SIEGERECALL_ADDCOMMAND_SYN ;

					msg.dwObjectID		= 0 ;

					msg.byCommandKind	= e_COMMAND_RECALL_DIE_OBJ ;

					msg.p_mapNum		= pStep->Get_ParentMapNum() ;
					msg.p_byStepIdx		= pStep->Get_StepIndex() ;
					//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
					//msg.p_byChildIdx	= pAddObj->Get_Index() ;
					msg.p_dwObjectIdx	= pAddObj->Get_ObjInfo()->dwObjectIdx;
					
					msg.dieRecallObj.byIdx = pDieRecallObjInfo->byIdx ;
                    memcpy(&msg.dieRecallObj.objInfo, &pDieRecallObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
				}
			}
			break ;

		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_Object
//	DESC		: 모든 맵 서버의 오브젝트들을, 모두/오브젝트 타입에 따라 삭제하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_Object(char* pMsg) 
{
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_WORD2* pmsg = NULL ;
	pmsg = (MSG_WORD2*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// wData1 = 모든 / 일부 오브젝트 삭제 여부.
	// wData2 = 삭제 할 오브젝트 타입.



	// 모든 오브젝트를 삭제하는 명령어라면,
	if(pmsg->wData1 == e_SIEGERECALL_REMOVE_ALL_OBJ)
	{
		MSGBASE msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_ALLOBJ_SYN ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSGBASE) ) ;
	}
	// 지정 된 오브젝트 타입만 삭제하는 명령이라면,
	else if(pmsg->wData1 == e_SIEGERECALL_REMOVE_SPECIFY_OBJ)
	{
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_SPECIFYOBJ_SYN ;

		// 삭제 할 오브젝트 타입을 세팅한다.
		msg.wData		= pmsg->wData2 ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_Object_SpecifyMap
//	DESC		: 지정 된 맵 서버의 오브젝트들을, 모두/오브젝트 타입에 따라 삭제하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_Object_SpecifyMap(char* pMsg)
{
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지 변환.
	MSG_WORD3* pmsg = NULL ;
	pmsg = (MSG_WORD3*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// wData1 = 모든 / 일부 오브젝트 삭제 여부.
	// wData2 = 삭제를 실행 할 맵.
	// wData3 = 삭제 할 오브젝트 타입.


	// 모든 오브젝트를 삭제하는 명령어라면,
	if(pmsg->wData1 == e_SIEGERECALL_REMOVE_ALL_OBJ)
	{
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_ALLOBJ_SYN ;

		// 삭제를 실행 할 맵을 세팅한다.
		msg.wData		= pmsg->wData2 ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}
	// 지정 된 오브젝트 타입만 삭제하는 명령이라면,
	else if(pmsg->wData1 == e_SIEGERECALL_REMOVE_SPECIFY_OBJ)
	{
		MSG_WORD2 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_SPECIFYOBJ_SYN ;

		// 삭제를 실행 할 맵을 세팅한다.
		msg.wData1		= pmsg->wData2 ;

		// 삭제를 실행 할 오브젝트 타입을 세팅한다.
		msg.wData2		= pmsg->wData3 ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Request_ObjInfo_Syn
//	DESC		: 맵 서버로 부터, 오브젝트 로딩 정보가 오면 처리하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 12, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Request_ObjInfo_Syn(DWORD dwIndex, char* pMsg) 
{
	// 080916 LYW --- SiegeRecallMgr : 로딩 처리를 수정한다.
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 원본 메시지로 변환.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	//if( !pmsg->dwObjectID )
	//{
	//	// DB에 삭제 메시지를 보내고 리턴.
	//	char txt[128] = {0, } ;
	//	
	//	sprintf(txt, "EXEC  %s %d",	MP_SIEGERECALL_REMOVE, pmsg->wData) ;
	//	g_DB.Query(eQueryType_FreeQuery, eSiegeRecallRemove, 0, txt, 0) ;

	//	return ;
	//}


	// 임시 버퍼를 선언한다.
	char txt[128] = {0, } ;
	// 쿼리문을 작성한다.
	sprintf(txt, "EXEC  %s %d",	MP_SIEGERECALL_LOAD, pmsg->wData) ;
	g_DB.Query(eQueryType_FreeQuery, eSiegeRecallLoad, pmsg->wData, txt, 0) ;

	//// 함수 인자 확인.
	//if(!pMsg)
	//{
	//	Throw_Error("Invalid parameter!!", __FUNCTION__) ;
	//	return ;
	//}

	//// 원본 메시지로 변환.
	//MSG_SIEGERECALL_LOADOBJINFO* pmsg = NULL ;
	//pmsg = (MSG_SIEGERECALL_LOADOBJINFO*)pMsg ;

	//if(!pmsg)
	//{
	//	Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
	//	return ;
	//}


	//// 테마 를 확인한다.
	//CSiegeTheme* pTheme = NULL ;
	//pTheme = Get_ThemeInfoByThemeIdx((WORD)pmsg->ThemeIdx) ;

	//if(!pTheme)
	//{
	//	Throw_Error("Invalid theme info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// 맵을 확인한다.
	//CSiegeMap* pMap = NULL ;
	//pMap = pTheme->Get_SiegeMapByMapNum( pmsg->MapIdx ) ;

	//if(!pMap)
	//{
	//	Throw_Error("Invalid map info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// 스텝을 확인한다.
	//CSiegeStep* pStep = NULL ;
	//pStep = pMap->Get_StepInfo( pmsg->StepIdx ) ;

	//if(!pStep)
	//{
	//	Throw_Error("Invalid step info!!", __FUNCTION__) ;
	//	return ;
	//}


	//// 부모인지, 자식인지 확인한다.
	//BYTE byKind = e_STEP_ADD_OBJ ;
	//if(pmsg->ClassType == e_ObjParent)
	//{
	//	byKind = pStep->Get_ChildKind(pmsg->ClassIdx) ;

	//	switch(byKind)
	//	{
	//	case e_STEP_ADD_OBJ :	
	//		{
	//			// 오브젝트 추가정보 확인.
	//			CSiege_AddObj* pAddObj = NULL ;
	//			pAddObj = pStep->Get_AddObjInfo(pmsg->ClassIdx) ;

	//			if(!pAddObj)
	//			{
	//				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	//				return ;
	//			}


	//			// 기본 오브젝트 정보 확인.
	//			st_SIEGEOBJ* pObjInfo = NULL ;
	//			pObjInfo = pAddObj->Get_ObjInfo() ;

	//			if(!pObjInfo)
	//			{
	//				Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
	//				return ;
	//			}


	//			// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
	//			MSG_SIEGERECALL_OBJINFO msg ;

	//			msg.Category	= MP_SIEGERECALL ;
	//			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	//			msg.dwObjectID	= dwIndex ;

	//			msg.siegeObj.IsParent	= e_ObjParent ;
	//			msg.siegeObj.byChildIdx	= pmsg->ClassIdx ;

	//			memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

	//			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


	//			// 시작조건 / 종료조건 / 종료 명령어를 확인한다.
	//			Execute_TermsAndCommand(pStep, pAddObj) ;
	//		}
	//		break ;

	//	default : break ;
	//	}
	//}
	//else
	//{
	//	byKind = pStep->Get_ChildKind(pmsg->ClassIdx) ;

	//	switch(byKind)
	//	{
	//	case e_STEP_ADD_OBJ :	
	//		{
	//			// 오브젝트 추가정보 확인.
	//			CSiege_AddObj* pAddObj = NULL ;
	//			pAddObj = pStep->Get_AddObjInfo(pmsg->ClassIdx) ;

	//			if(!pAddObj)
	//			{
	//				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	//				return ;
	//			}

	//			st_SIEGEOBJ* pAddObjInfo = NULL ;
	//			pAddObjInfo = pAddObj->Get_ObjInfo() ;

	//			if(!pAddObjInfo)
	//			{
	//				Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
	//				return ;
	//			}

	//			char szQuery[256] = {0, } ;
	//			sprintf(szQuery, "%s %d, %d, %d, %d, %d, %d", MP_SIEGERECALL_UPDATE, pTheme->Get_ThemeIdx(), 
	//				pMap->Get_MapNum(), pStep->Get_StepIndex(), pAddObj->Get_Index(), e_ObjChild, pmsg->ClassIdx) ;

	//			g_DB.Query( eQueryType_FreeQuery, eSiegeRecallUpdate, 0, szQuery ) ;


	//			CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(pmsg->ClassIdx) ;
	//			if(!pCommand)
	//			{
	//				Throw_Error("Invalid command info!!", __FUNCTION__) ;
	//				return ;
	//			}

	//			st_DIE_RECALL_OBJ* pObjInfo ;
	//			for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
	//			{
	//				pObjInfo = NULL ;
	//				pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

	//				if(!pObjInfo)
	//				{
	//					Throw_Error("Invalid obj info!!", __FUNCTION__) ;
	//					return ;
	//				}


	//				char szQuery[256] = {0, } ;
	//				sprintf(szQuery, "%s %d, %d, %d, %d, %d, %d", MP_SIEGERECALL_UPDATE, pAddObjInfo->wThemeIdx, 
	//				pAddObjInfo->mapNum, pAddObjInfo->byStepIdx, pAddObj->Get_Index(), e_ObjChild, 0) ;

	//				g_DB.Query( eQueryType_FreeQuery, eSiegeRecallUpdate, 0, szQuery ) ;

	//				// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
	//				MSG_SIEGERECALL_OBJINFO msg ;

	//				msg.Category	= MP_SIEGERECALL ;
	//				msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	//				msg.dwObjectID	= dwIndex ;

	//				msg.siegeObj.IsParent	= e_ObjChild ;
	//				msg.siegeObj.byChildIdx	= byCount ;

	//				memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

	//				g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
	//			}
	//		}
	//		break ;

	//	default : break ;
	//	}
	//}

	////// 함수 인자 확인.
	////if(!pMsg)
	////{
	////	Throw_Error("Invalid parameter!!", __FUNCTION__) ;
	////	return ;
	////}

	////// 원본 메시지로 변환.
	////MSG_SIEGERECALL_LOADOBJINFO* pmsg = NULL ;
	////pmsg = (MSG_SIEGERECALL_LOADOBJINFO*)pMsg ;

	////if(!pmsg)
	////{
	////	Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
	////	return ;
	////}

	////// 서버 포트를 확인한다.
	////SERVERINFO* pSInfo = NULL ;
	////pSInfo = g_pServerTable->FindMapServer( pmsg->MapIdx ) ;

	////if(!pSInfo)
	////{
	////	Throw_Error("Failed to recieve server info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// 테마 를 확인한다.
	////CSiegeTheme* pTheme = NULL ;
	////pTheme = Get_ThemeInfoByThemeIdx((WORD)pmsg->ThemeIdx) ;

	////if(!pTheme)
	////{
	////	Throw_Error("Invalid theme info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// 맵을 확인한다.
	////CSiegeMap* pMap = NULL ;
	////pMap = pTheme->Get_SiegeMapByMapNum( pmsg->MapIdx ) ;

	////if(!pMap)
	////{
	////	Throw_Error("Invalid map info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// 스텝을 확인한다.
	////CSiegeStep* pStep = NULL ;
	////pStep = pMap->Get_StepInfo( pmsg->StepIdx ) ;

	////if(!pStep)
	////{
	////	Throw_Error("Invalid step info!!", __FUNCTION__) ;
	////	return ;
	////}


	////// 명령어 종류를 확인한다.
	////switch(pmsg->ComKind)
	////{
	////case e_CK_ADD_OBJECT :
	////	{
	////		// 오브젝트 추가정보 확인.
	////		CSiege_AddObj* pAddObj = NULL ;
	////		pAddObj = pStep->Get_AddObjInfo(pmsg->AddObjIdx) ;

	////		if(!pAddObj)
	////		{
	////			Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	////			return ;
	////		}


	////		// 기본 오브젝트 정보 확인.
	////		st_SIEGEOBJ* pObjInfo = NULL ;
	////		pObjInfo = pAddObj->Get_ObjInfo() ;

	////		if(!pObjInfo)
	////		{
	////			Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
	////			return ;
	////		}



	////		// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
	////		MSG_SIEGERECALL_OBJINFO msg ;

	////		msg.Category	= MP_SIEGERECALL ;
	////		msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	////		msg.dwObjectID	= 0 ;

	////		pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
	////		pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
	////		pObjInfo->byComIndex	= 0 ;
	////		pObjInfo->wParentMap	= pmsg->MapIdx ;

	////		memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

	////		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
	////		//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


	////		// 시작조건 / 종료조건 / 종료 명령어를 확인한다.
	////		Execute_TermsAndCommand(pStep, pAddObj) ;
	////	}
	////	break ;

	////case e_CK_COMMAND :
	////	{
	////		// 오브젝트 추가정보 확인.
	////		CSiege_AddObj* pAddObj = NULL ;
	////		pAddObj = pStep->Get_AddObjInfo(pmsg->AddObjIdx) ;

	////		if(!pAddObj)
	////		{
	////			Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
	////			return ;
	////		}

	////		st_SIEGEOBJ* pAddObjInfo = NULL ;
	////		pAddObjInfo = pAddObj->Get_ObjInfo() ;

	////		if(!pAddObjInfo)
	////		{
	////			Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
	////			return ;
	////		}

	////		CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(pmsg->ComIndex) ;
	////		if(!pCommand)
	////		{
	////			Throw_Error("Invalid command info!!", __FUNCTION__) ;
	////			return ;
	////		}

	////		st_DIE_RECALL_OBJ* pObjInfo ;
	////		for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
	////		{
	////			pObjInfo = NULL ;
	////			pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

	////			if(!pObjInfo)
	////			{
	////				Throw_Error("Invalid obj info!!", __FUNCTION__) ;
	////				continue ;
	////			}


	////			// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
	////			MSG_SIEGERECALL_OBJINFO msg ;

	////			msg.Category	= MP_SIEGERECALL ;
	////			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

	////			msg.dwObjectID	= 0 ;

	////			msg.wExValue	= pAddObjInfo->mapNum ;

	////			pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
	////			pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
	////			pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
	////			pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

	////			memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

	////			g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
	////			//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
	////		}
	////	}
	////	break ;
	////}

	////// 맵 서버에, 소환물 로딩 완료 메시지를 전송 한다.
	////MSG_WORD msg ;

	////msg.Category	= MP_SIEGERECALL ;
	////msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_ACK ;

	////msg.dwObjectID	= 0 ;

	////msg.wData		= pmsg->MapIdx ;

	////g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_WORD) ) ;
	//////g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Result_ObjInfo_Syn
//	DESC		: DB에 기록 된 오브젝트 정보를 참조하여, 오브젝트 리로딩 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 13, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Result_ObjInfo_Syn( LPQUERY pData, LPDBMESSAGE pMessage )
{
	WORD wMapNum = 0 ;
	SERVERINFO* pSInfo ;

	// 081015 LYW --- SiegeRecallMgr : 소환 몬스터를 완전히 제거한 수를 받을 변수 선언.
	BYTE byCompleteKillCount = 0 ;

	if( pMessage->dwResult == 0 )
	{
		BYTE byThemeIdx = 0 ;

		switch( pMessage->dwID )
		{
		case 52 :
		case 81 :
		case 82 :
		case 83 :	byThemeIdx = 0 ; break ;

		default : return ;
		}

		MSG_WORD msg ;		
		memset( &msg, 0, sizeof(MSG_WORD) ) ;

		msg.wData = byThemeIdx ;
		
		Execute_Step( 0, (char*)&msg, sizeof(MSG_WORD) ) ;
	}


	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
		WORD wRecallMap		= (WORD)atoi((char*)pData[count].Data[eSRLoad_RecallMap]) ;
		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;
		BYTE byComIndex		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComIndex]) ;
		//BYTE byResult		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_Result]) ;
		BYTE byRecallStep	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_RecallStep]) ;
		WORD wRequestMap	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_RequestMap]) ;

		// 081015 LYW --- SiegeRecallMgr : 맵 로딩시, 완전히 제거한 소환 정보를 받아서, 
		// 해당 맵 서버에게 정보를 전송한다.
		if( pMessage->dwID == (DWORD)wRecallMap )
		{
			if( byRecallStep == 4 )
			{
				++byCompleteKillCount ;
			}
		}

		wMapNum = wRequestMap ;

		pSInfo = NULL ;
		pSInfo = g_pServerTable->FindMapServer( wRecallMap ) ;

		if(!pSInfo)
		{
			Throw_Error("Failed to recieve map server info!!", __FUNCTION__) ;
			continue ;
		}


		// 테마 를 확인한다.
		CSiegeTheme* pTheme = NULL ;
		pTheme = Get_ThemeInfoByThemeIdx((WORD)byThemeIdx) ;

		if(!pTheme)
		{
			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
			continue ;
		}


		// 맵을 확인한다.
		CSiegeMap* pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}


		// 스텝을 확인한다.
		CSiegeStep* pStep = NULL ;
		pStep = pMap->Get_StepInfo( byStepIdx ) ;

		if(!pStep)
		{
			Throw_Error("Invalid step info!!", __FUNCTION__) ;
			continue ;
		}


		// 오브젝트 추가정보 확인.
		CSiege_AddObj* pAddObj = NULL ;
		pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

		if(!pAddObj)
		{
			Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
			continue ;
		}


		// 소환 스텝을 확인한다.
		switch(byRecallStep)
		{
		case 0 :
		case 4 : break ;

		case 1 : 
			{
				if( wRequestMap != wRecallMap ) continue ;

				// 기본 오브젝트 정보 확인.
				st_SIEGEOBJ* pObjInfo = NULL ;
				pObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pObjInfo)
				{
					Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
					continue ;
				}



				// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
				MSG_SIEGERECALL_OBJINFO msg ;

				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

				msg.dwObjectID	= 0 ;

				pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
				pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
				pObjInfo->byComIndex	= 0 ;
				pObjInfo->wParentMap	= wMapIdx ;

				memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

				g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
				//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


				// 시작조건 / 종료조건 / 종료 명령어를 확인한다.
				Execute_TermsAndCommand(pStep, pAddObj) ;
			}
			break ;

		case 2 :
		case 3 :
			{
				st_SIEGEOBJ* pAddObjInfo = NULL ;
				pAddObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pAddObjInfo)
				{
					Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
					continue ;
				}

				CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(byComIndex) ;
				if(!pCommand)
				{
					Throw_Error("Invalid command info!!", __FUNCTION__) ;
					continue ;
				}

				st_DIE_RECALL_OBJ* pObjInfo ;
				for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
				{
					pObjInfo = NULL ;
					pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

					if(!pObjInfo)
					{
						Throw_Error("Invalid obj info!!", __FUNCTION__) ;
						continue ;
					}

					if( wRequestMap != pObjInfo->objInfo.mapNum ) continue ;


					// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
					MSG_SIEGERECALL_OBJINFO msg ;

					msg.Category	= MP_SIEGERECALL ;
					msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

					msg.dwObjectID	= 0 ;

					msg.wExValue	= pAddObjInfo->mapNum ;

					pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
					pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
					pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
					pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

					memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

					if(byRecallStep == 2)
					{
						pSInfo = NULL ;
						pSInfo = g_pServerTable->FindMapServer( pObjInfo->objInfo.mapNum ) ;

						if(!pSInfo)
						{
							Throw_Error("Failed to recieve map server info!!", __FUNCTION__) ;
							continue ;
						}
					}

					g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
					//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
				}
			}
			break ;
		}


		//// 명령어 종류를 확인한다.
		//switch(byComKind)
		//{
		//case e_CK_ADD_OBJECT :
		//	{
		//		// 기본 오브젝트 정보 확인.
		//		st_SIEGEOBJ* pObjInfo = NULL ;
		//		pObjInfo = pAddObj->Get_ObjInfo() ;

		//		if(!pObjInfo)
		//		{
		//			Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
		//			continue ;
		//		}



		//		// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
		//		MSG_SIEGERECALL_OBJINFO msg ;

		//		msg.Category	= MP_SIEGERECALL ;
		//		msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

		//		msg.dwObjectID	= 0 ;

		//		pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
		//		pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
		//		pObjInfo->byComIndex	= 0 ;
		//		pObjInfo->wParentMap	= wMapIdx ;

		//		memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

		//		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
		//		//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


		//		// 시작조건 / 종료조건 / 종료 명령어를 확인한다.
		//		Execute_TermsAndCommand(pStep, pAddObj) ;
		//	}
		//	break ;

		//case e_CK_COMMAND :
		//	{
		//		st_SIEGEOBJ* pAddObjInfo = NULL ;
		//		pAddObjInfo = pAddObj->Get_ObjInfo() ;

		//		if(!pAddObjInfo)
		//		{
		//			Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
		//			continue ;
		//		}

		//		CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(byComIndex) ;
		//		if(!pCommand)
		//		{
		//			Throw_Error("Invalid command info!!", __FUNCTION__) ;
		//			continue ;
		//		}

		//		st_DIE_RECALL_OBJ* pObjInfo ;
		//		for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
		//		{
		//			pObjInfo = NULL ;
		//			pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

		//			if(!pObjInfo)
		//			{
		//				Throw_Error("Invalid obj info!!", __FUNCTION__) ;
		//				continue ;
		//			}


		//			// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
		//			MSG_SIEGERECALL_OBJINFO msg ;

		//			msg.Category	= MP_SIEGERECALL ;
		//			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

		//			msg.dwObjectID	= 0 ;

		//			msg.wExValue	= pAddObjInfo->mapNum ;

		//			pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
		//			pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
		//			pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
		//			pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

		//			memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

		//			g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
		//			//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
		//		}
		//	}
		//	break ;
		//}
	}


	// 맵 서버에, 소환물 로딩 완료 메시지를 전송 한다.
	{
		MSG_WORD msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_ACK ;

		msg.dwObjectID	= 0 ;

		pSInfo = NULL ;

		if( pMessage->dwResult > 0 )
		{
			msg.wData		= wMapNum ;
			pSInfo = g_pServerTable->FindMapServer( wMapNum ) ;
		}
		else
		{
			msg.wData		= (WORD)pMessage->dwID ;
			pSInfo = g_pServerTable->FindMapServer( (WORD)pMessage->dwID ) ;
		}

		if(!pSInfo)
		{
			//Throw_Error("Failed to recieve map server info!!", __FUNCTION__) ;
			return ;
		}

		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_WORD) ) ;
		//g_Network.Broadcast2MapServer( (char*)&msg, sizeof(MSG_WORD) ) ;
	}


	// 081015 LYW --- SiegeRecallMgr : 소환 몬스터를 완전히 제거한 수를 맵 서버로 전송한다.
	if( byCompleteKillCount )
	{
		MSG_BYTE msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGERECALL_LOAD_RECALLCOUNT_ACK ;

		msg.dwObjectID	= 0 ;

		msg.bData		= byCompleteKillCount ;

		pSInfo = NULL ;

		if( pMessage->dwResult > 0 )
		{
			pSInfo = g_pServerTable->FindMapServer( wMapNum ) ;
		}
		else
		{
			pSInfo = g_pServerTable->FindMapServer( (WORD)pMessage->dwID ) ;
		}

		if(!pSInfo) return ;

		g_Network.Send2Server( pSInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RSiegeRecallUpdate
//	DESC		: DB에 기록 된 오브젝트 정보를 참조하여, 오브젝트 업데이트 결과 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 2, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RSiegeRecallUpdate( LPQUERY pData, LPDBMESSAGE pMessage ) 
{
	MAPTYPE mapNum = 0 ; 

	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
		WORD wRecallMap		= (WORD)atoi((char*)pData[count].Data[eSRLoad_RecallMap]) ;
		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;
		BYTE byComKind		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComKind]) ;
		BYTE byComIndex		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComIndex]) ;

		mapNum = wRecallMap ;


		// 테마 를 확인한다.
		CSiegeTheme* pTheme = NULL ;
		pTheme = Get_ThemeInfoByThemeIdx((WORD)byThemeIdx) ;

		if(!pTheme)
		{
			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
			continue ;
		}


		// 맵을 확인한다.
		CSiegeMap* pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}


		// 스텝을 확인한다.
		CSiegeStep* pStep = NULL ;
		pStep = pMap->Get_StepInfo( byStepIdx ) ;

		if(!pStep)
		{
			Throw_Error("Invalid step info!!", __FUNCTION__) ;
			continue ;
		}


		// 명령어 종류를 확인한다.
		switch(byComKind)
		{
		case e_CK_ADD_OBJECT :
			{
				// 오브젝트 추가정보 확인.
				CSiege_AddObj* pAddObj = NULL ;
				pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

				if(!pAddObj)
				{
					Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
					continue ;
				}


				// 기본 오브젝트 정보 확인.
				st_SIEGEOBJ* pObjInfo = NULL ;
				pObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pObjInfo)
				{
					Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
					continue ;
				}



				// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
				MSG_SIEGERECALL_OBJINFO msg ;

				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

				msg.dwObjectID	= 0 ;

				pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
				pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
				pObjInfo->byComIndex	= 0 ;
				pObjInfo->wParentMap	= wMapIdx ;

				memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

				g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;


				// 시작조건 / 종료조건 / 종료 명령어를 확인한다.
				Execute_TermsAndCommand(pStep, pAddObj) ;
			}
			break ;

		case e_CK_COMMAND :
			{
				// 오브젝트 추가정보 확인.
				CSiege_AddObj* pAddObj = NULL ;
				pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

				if(!pAddObj)
				{
					Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
					continue ;
				}

				st_SIEGEOBJ* pAddObjInfo = NULL ;
				pAddObjInfo = pAddObj->Get_ObjInfo() ;

				if(!pAddObjInfo)
				{
					Throw_Error("Invalid addobj base info!!", __FUNCTION__) ;
					continue ;
				}

				CSiegeCommand* pCommand = pAddObj->Get_CommandInfo(byComIndex) ;
				if(!pCommand)
				{
					Throw_Error("Invalid command info!!", __FUNCTION__) ;
					continue ;
				}

				st_DIE_RECALL_OBJ* pObjInfo ;
				for(BYTE byCount = 0 ; byCount < pCommand->Get_ChildCount() ; ++byCount)
				{
					pObjInfo = NULL ;
					pObjInfo = pCommand->Get_DieRecallObjInfo(byCount) ;

					if(!pObjInfo)
					{
						Throw_Error("Invalid obj info!!", __FUNCTION__) ;
						continue ;
					}


					// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
					MSG_SIEGERECALL_OBJINFO msg ;

					msg.Category	= MP_SIEGERECALL ;
					msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

					msg.dwObjectID	= 0 ;

					msg.wExValue	= pAddObjInfo->mapNum ;

					pObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
					pObjInfo->objInfo.byComKind		= e_CK_COMMAND ;
					pObjInfo->objInfo.byComIndex	= pCommand->Get_Index() ;
					pObjInfo->objInfo.wParentMap	= pAddObjInfo->mapNum ;

					memcpy(&msg.siegeObj, &pObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

					g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;	
				}
			}
			break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RSiegeRecallInsert
//	DESC		: DB에 소환 오브젝트 정보 추가 후, 맵 서버에 소환 명령어를 보내는 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 4, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RSiegeRecallInsert( LPQUERY pData, LPDBMESSAGE pMessage ) 
{
	// 함수 인자 확인.
	if( !pData )
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 추가 결과를 받는다.
	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
	{
		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;

		CSiegeTheme* pTheme = Get_ThemeInfoByThemeIdx(byThemeIdx) ;

		if(!pTheme)
		{
			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
			continue ;
		}


		// 맵 번호를 지정해서 사용할 경우, 이 코드를 사용한다.
		CSiegeMap* pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			continue ;
		}


		// 스텝을 확인한다.
		CSiegeStep* pStep = NULL ;
		pStep = pMap->Get_StepInfo( (BYTE)(byStepIdx) ) ;

		if(!pStep)
		{
			Throw_Error("Invalid step info!!", __FUNCTION__) ;
			return ;
		}


		// 스텝을 실행한다.
		BYTE byChildCount = 0 ;
		byChildCount = pStep->Get_ChildCount() ;

		BYTE byKind = e_STEP_ADD_OBJ ;
		byKind = pStep->Get_ChildKind(byAddObjIdx) ;

		if( byKind == e_STEP_ADD_OBJ )
		{
			// 오브젝트 추가정보 확인.
			CSiege_AddObj* pAddObj = NULL ;
			pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;

			if(!pAddObj)
			{
				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
				continue ;
			}


			// 기본 오브젝트 정보 확인.
			st_SIEGEOBJ* pObjInfo = NULL ;
			pObjInfo = pAddObj->Get_ObjInfo() ;

			if(!pObjInfo)
			{
				Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
				continue ;
			}

			
			// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
			MSG_SIEGERECALL_OBJINFO msg ;

			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

			msg.dwObjectID	= 0 ;

			memcpy( &msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ) ) ;

			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
		}
	}
}





////-------------------------------------------------------------------------------------------------
////	NAME		: RSiegeRecallRemove
////	DESC		: DB에 소환 오브젝트 정보 삭제 후, 맵 서버에 소환 정보 삭제 명령을 보내는 처리를 한다.
////	PROGRAMMER	: Yongs Lee
////	DATE		: September 7, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::RSiegeRecallRemove( LPQUERY pData, LPDBMESSAGE pMessage )
//{
//	// 함수 인자 확인.
//	if( !pData )
//	{
//		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// 추가 결과를 받는다.
//	for( DWORD count = 0 ;  count < pMessage->dwResult ; ++count )
//	{
//		BYTE byThemeIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ThemeIdx]) ;
//		WORD wMapIdx		= (WORD)atoi((char*)pData[count].Data[eSRLoad_MapIdx]) ;
//		WORD wRecallMap		= (WORD)atoi((char*)pData[count].Data[eSRLoad_RecallMap]) ;
//		BYTE byStepIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_StepIdx]) ;
//		BYTE byAddObjIdx	= (BYTE)atoi((char*)pData[count].Data[eSRLoad_AddObjIdx]) ;
//		BYTE byComKind		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComKind]) ;
//		BYTE byComIdx		= (BYTE)atoi((char*)pData[count].Data[eSRLoad_ComIndex]) ;
//
//
//		// 테마 를 확인한다.
//		CSiegeTheme* pTheme = NULL ;
//		pTheme = Get_ThemeInfoByThemeIdx(byThemeIdx) ;
//
//		if(!pTheme)
//		{
//			Throw_Error("Invalid theme info!!", __FUNCTION__) ;
//			continue ;
//		}
//
//
//		// 맵 번호를 지정해서 사용할 경우, 이 코드를 사용한다.
//		CSiegeMap* pMap = NULL ;
//		pMap = pTheme->Get_SiegeMapByMapNum( wMapIdx ) ;
//
//		if(!pMap)
//		{
//			Throw_Error("Invalid map info!!", __FUNCTION__) ;
//			continue ;
//		}
//
//
//		// 스텝을 확인한다.
//		CSiegeStep* pStep = NULL ;
//		pStep = pMap->Get_StepInfo( (BYTE)(byStepIdx) ) ;
//
//		if(!pStep)
//		{
//			Throw_Error("Invalid step info!!", __FUNCTION__) ;
//			return ;
//		}
//
//
//		// 스텝을 실행한다.
//		BYTE byChildCount = 0 ;
//		byChildCount = pStep->Get_ChildCount() ;
//
//		BYTE byKind = e_STEP_ADD_OBJ ;
//		byKind = pStep->Get_ChildKind(byAddObjIdx) ;
//
//		if( byKind == e_STEP_ADD_OBJ )
//		{
//			// 오브젝트 추가정보 확인.
//			CSiege_AddObj* pAddObj = NULL ;
//			pAddObj = pStep->Get_AddObjInfo(byAddObjIdx) ;
//
//			if(!pAddObj)
//			{
//				Throw_Error("Invalid addobj info!!", __FUNCTION__) ;
//				continue ;
//			}
//
//
//			// 기본 오브젝트 정보 확인.
//			st_SIEGEOBJ* pObjInfo = NULL ;
//			pObjInfo = pAddObj->Get_ObjInfo() ;
//
//			if(!pObjInfo)
//			{
//				Throw_Error("Invalid base obj info!!", __FUNCTION__) ;
//				continue ;
//			}
//
//			
//			// 오브젝트 추가 정보를 현재 Agent와 연결 된, 맵 서버로 알린다.
//			MSG_SIEGERECALL_OBJINFO msg ;
//
//			msg.Category	= MP_SIEGERECALL ;
//			msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;
//
//			msg.dwObjectID	= 0 ;
//
//			memcpy( &msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ) ) ;
//
//			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
//		}
//	}
//}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Obj_Ack
//	DESC		: 오브젝트 추가 성공 메시지 처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Obj_Ack(DWORD dwIndex, char* pMsg)
{
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 원본 메시지로 변환.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}


	// 클라이언트로 진행 사항을 전송한다.
	g_Network.Send2User( dwIndex, (char*)pmsg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Obj_Nack
//	DESC		: 오브젝트 추가 실패 메시지 처리를 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Obj_Nack(DWORD dwIndex, char* pMsg)
{
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}

	// 원본 메시지로 변환.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message to original!!", __FUNCTION__) ;
		return ;
	}

	// 클라이언트로 진행 사항을 전송한다.
	g_Network.Send2User( dwIndex, (char*)pmsg, sizeof(MSG_SIEGERECALL_OBJINFO) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Ready_Execute_Command
//	DESC		: The function to execute command.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Ready_Execute_Command(DWORD dwIndex, char* pMsg) 
{
	// Check pararmeter.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_WORD3* pmsg = NULL ;
	pmsg = (MSG_WORD3*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a messsage to original!!", __FUNCTION__) ;
		return ;
	}


	// Check theme info.	
	M_THEME::iterator it_theme ;

	CSiegeMap* pMap = NULL ;

	for( it_theme = m_Theme.begin() ; it_theme != m_Theme.end() ; ++it_theme )
	{
		// Check map info.
		pMap = NULL ;
		pMap = it_theme->second.Get_SiegeMapByMapNum( pmsg->wData1 ) ;

		if(!pMap)
		{
			Throw_Error("Invalid map info!!", __FUNCTION__) ;
			return ;
		}
	}


	// Check step info.
	CSiegeStep* pStep = NULL ;
	pStep = pMap->Get_StepInfo( (BYTE)pmsg->wData2 ) ;

	if(!pStep)
	{
		Throw_Error("Invalid step info!!", __FUNCTION__) ;
		return ;
	}


	// Check child kind.
	BYTE byKind = pStep->Get_ChildKind( (BYTE)pmsg->wData3 ) ;

	switch(byKind)
	{
	case e_STEP_ADD_OBJ :
		{
			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			pAddObj = pStep->Get_AddObjInfo( (BYTE)pmsg->wData3 ) ;

			if(!pAddObj)
			{
				Throw_Error("Invalid add obj info!!", __FUNCTION__) ;
				return ;
			}

			// Check command info.
			BYTE byCommandCount = pAddObj->Get_CommandCount() ;

			CSiegeCommand* pExeCommand ;
			for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
			{
				pExeCommand = NULL ;
				pExeCommand = pAddObj->Get_CommandInfo(byCCount) ;

				if(!pExeCommand)
				{
					Throw_Error("Invalid exe command info!!", __FUNCTION__) ;
					continue ;
				}

				BYTE byChildCount = pExeCommand->Get_ChildCount() ;

				for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
				{
					switch(pExeCommand->Get_ChildKind(byCount))
					{
					case e_COMMAND_RECALL_DIE_OBJ : 
						{
							st_DIE_RECALL_OBJ* pInfo = NULL ;
							pInfo = pExeCommand->Get_DieRecallObjInfo( byCount) ;

							if(!pInfo)
							{
								Throw_Error("Invalid die recall obj info!!", __FUNCTION__) ;
								return ;
							}

							// 080916 LYW --- 소환물 명령 처리 수정.
							//////char szQuery[256] = {0, } ;
							//////sprintf(szQuery, "%s %d, %d, %d, %d, %d, %d, %d", MP_SIEGERECALL_UPDATE, pMap->Get_ThemeIdx(),
							//////	pMap->Get_MapNum(), pInfo->objInfo.mapNum, pStep->Get_StepIndex(), pAddObj->Get_Index(), e_CK_COMMAND, byCount) ;

							//////g_DB.Query( eQueryType_FreeQuery, eSiegeRecallUpdate, 0, szQuery ) ;

							// Broadcast add object info.
							MSG_SIEGERECALL_OBJINFO msg ;

							msg.Category	= MP_SIEGERECALL ;
							msg.Protocol	= MP_SIEGERECALL_ADDOBJ_SYN ;

							msg.dwObjectID	= 0 ;

							msg.wExValue	= pAddObj->Get_ObjInfo()->mapNum ;

							pInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
							pInfo->objInfo.byComKind	= e_CK_COMMAND ;
							pInfo->objInfo.byComIndex	= pExeCommand->Get_Index() ;
							pInfo->objInfo.wParentMap	= pAddObj->Get_ObjInfo()->mapNum ;

							memcpy(&msg.siegeObj, &pInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

							g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
						}
						break ;

					default : break ;
					}
				}
			}
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Throw_Error(char* szErr, char* szCaption)
{
	// Check parameter of this function.
	if(!szErr || !szCaption) return ;


	// Check err string size.
	if(strlen(szErr) <= 1)
	{
#ifdef _USE_ERRBOX_
		MessageBox( NULL, "Invalid err string size!!", __FUNCTION__, MB_OK ) ;
#else
		char tempStr[257] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid err string size!!" ) ;
		WriteLog( tempStr ) ;
#endif // _USE_ERRBOX_
	}


	// Check caption string size.
	if(strlen(szCaption) <= 1)
	{
#ifdef _USE_ERRBOX_
		MessageBox( NULL, "Invalid caption string size!!", __FUNCTION__, MB_OK ) ;
#else
		char tempStr[257] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid caption string size!!" ) ;
		WriteLog( tempStr ) ;
#endif // _USE_ERRBOX_
	}


	// Print a err message.
#ifdef _USE_ERRBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK) ;
#else
	char tempStr[257] = {0, } ;

	SafeStrCpy(tempStr, szCaption, 256) ;
	strcat(tempStr, " - ") ;
	strcat(tempStr, szErr) ;
	WriteLog(tempStr) ;
#endif // _USE_ERRBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteLog
//	DESC		: The function to create a error log for siege recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 24, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	FILE *fp = fopen("Log/Agent-SiegeRecallMgr.log", "a+") ;
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ;
		fclose(fp) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_Execute_Step
//	DESC		: The function to execute step by cheat code.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 31, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_Execute_Step(DWORD dwIndex, char* pMsg)
{
	// Check pararmeter.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_WORD3* pmsg = NULL ;
	pmsg = (MSG_WORD3*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check theme.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->wData1) ;

	if(!pTheme)
	{
		Throw_Error("Invalid theme info!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	CSiegeMap* pMap = NULL ;
	pMap = pTheme->Get_SiegeMapByMapNum( pmsg->wData2 ) ;

	if(!pMap)
	{
		Throw_Error("Invalid map info!!", __FUNCTION__) ;
		return ;
	}


	// Check step.
	CSiegeStep* pStep = NULL ;
	pStep = pMap->Get_StepInfo( (BYTE)(pmsg->wData3) ) ;

	if(!pStep)
	{
		Throw_Error("Invalid step info!!", __FUNCTION__) ;
		return ;
	}


	// Execute step.
	BYTE byChildCount = 0 ;
	byChildCount = pStep->Get_ChildCount() ;

	BYTE byKind ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_STEP_ADD_OBJ ;
		byKind = pStep->Get_ChildKind(byCount) ;

		switch(byKind)
		{
		case e_STEP_ADD_OBJ :	Execute_AddObj( dwIndex, pStep, byCount ) ;	break ;

		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RequestRecallCount_Syn
//	DESC		: 맵 서버에서, 해당맵의 소환물이 몇개인지 요청하는 작업 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 7, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RequestRecallCount_Syn( DWORD dwIndex, void* pMsg )
{
	// 함수 인자를 확인한다.
	if( !pMsg ) return ;


	// 원본 메시지 변환.
	MSG_WORD* pmsg = NULL ;
	pmsg = (MSG_WORD*)pMsg ;

	if( !pmsg ) return ;


	// 카운트 설정에 필요한 힘시 변수 선언.
	WORD wRecallCount = 0 ;

	M_THEME::iterator it_theme ;		// 테마 관련

	CSiegeMap* pMap ;					// 맵 관련
	BYTE byMapCount ;

	CSiegeStep* pStep ;					// 스텝 관련
	BYTE byStepCount ;

	CSiege_AddObj* pAddObj ;			// 오브젝트 관련
	BYTE byAddObjCount ;

	st_SIEGEOBJ* pInfo ;				// 오브젝트 소환 관련 정보

	CSiegeCommand* pComm ;				// 오브젝트의 명령어 관련
	BYTE byCommCount ;

	st_DIE_RECALL_OBJ* pRecallInfo ;	// 오브젝트 명령어의 소환 관련 정보


	// 요청이 들어온 맵과 관련 된 소환 카운트를 세팅한다.
	for( it_theme = m_Theme.begin() ; it_theme != m_Theme.end() ; ++it_theme )
	{
		// 한 테마에 해당하는 맵 정보를 확인한다.
		byMapCount = it_theme->second.Get_MapCount() ;

		for( BYTE count1 = 0 ; count1 < byMapCount ; ++count1 )
		{
			pMap = NULL ;
			pMap = it_theme->second.Get_SiegeMapByIdx( count1 ) ;

			if( !pMap ) continue ;

			// 한 맵에 해당하는 스텝 정보를 확인한다.
			byStepCount = pMap->Get_StepCount() ;

			for( BYTE count2 = 0 ;  count2 < byStepCount ; ++count2 )
			{
				pStep = NULL ;
				pStep = pMap->Get_StepInfo( count2 ) ;

				if( !pStep ) continue ;

				// 한 스텝에 해당하는 오브젝트를 확인한다.
				byAddObjCount = pStep->Get_ChildCount() ;

				for( BYTE count3 = 0 ; count3 < byAddObjCount ; ++count3 )
				{
					pAddObj = NULL ;
					pAddObj = pStep->Get_AddObjInfo( count3 ) ;

					if( !pAddObj ) continue ;

					// 오브젝트의 소환 정보를 확인한다.
					pInfo = NULL ;
					pInfo = pAddObj->Get_ObjInfo() ;

					if( !pInfo ) continue ;

					//if( pInfo->mapNum != pmsg->wData ) continue ;

					if( pInfo->mapNum == pmsg->wData ) 
					{
						++wRecallCount ;
					}

					// 한 오부젝트의 명령어 부분을 확인한다.
					byCommCount = pAddObj->Get_CommandCount() ;

					for( BYTE count4 = 0 ; count4 < byCommCount ; ++count4 )
					{
						pComm = NULL ;
						pComm = pAddObj->Get_CommandInfo( count4 ) ;

						if( !pComm ) continue ;

						// 오브젝트가 사망하였을 때, 발생하는 소환 명령어를 확인한다.
						pRecallInfo = NULL ;
						pRecallInfo = pComm->Get_DieRecallObjInfo( count4 ) ;

						if( !pRecallInfo ) continue ;

						if( pRecallInfo->objInfo.mapNum != pmsg->wData ) continue ;

						++wRecallCount ;
					}
				}
			}
		}
	}

	// 소환 카운트를 맵서버로 전송한다.
	MSG_WORD2 msg ;

	msg.Category	= MP_SIEGERECALL ;
	msg.Protocol	= MP_SIEGERECALL_REQUEST_RECALLCOUNT_ACK ;

	msg.dwObjectID	= 0 ;

	msg.wData1		= pmsg->wData ;
	msg.wData2		= wRecallCount ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof( MSG_WORD2 ) ) ;
}





////-------------------------------------------------------------------------------------------------
////	NAME		: Cheat_AddObj
////	DESC		: The function to add object by cheat code.
////	PROGRAMMER	: Yongs Lee
////	DATE		: July 28, 2008
////-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Cheat_AddObj(DWORD dwIndex, void* pMsg)
//{
//	// Check parameter.
//	if(!pMsg)
//	{
//		Throw_Error( "Invalid a message parameter!!", __FUNCTION__) ;
//		return ;
//	}
//
//
//	// Convert a message.
//	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
//	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;
//
//	if(!pmsg)
//	{
//		Throw_Error( "Failed to convert a message!!", __FUNCTION__) ;
//		return ;
//	}
//
//	MSG_SIEGERECALL_OBJINFO msg ;
//
//	msg.Category	= MP_SIEGERECALL ;
//	msg.Protocol	= MP_SIEGE_CHEAT_ADDOBJ_SYN ;
//
//	msg.dwObjectID	= pmsg->dwObjectID ;
//
//	msg.siegeObj.mapNum			= pmsg->siegeObj.mapNum ;
//	msg.siegeObj.wObjectKind	= pmsg->siegeObj.wObjectKind ;
//	msg.siegeObj.dwObjectIdx	= pmsg->siegeObj.dwObjectIdx ;
//	msg.siegeObj.fXpos			= pmsg->siegeObj.fXpos ;
//	msg.siegeObj.fZpos			= pmsg->siegeObj.fZpos ;
//	msg.siegeObj.byUseRandomPos	= pmsg->siegeObj.byUseRandomPos ;
//	msg.siegeObj.byRadius		= pmsg->siegeObj.byRadius ;
//
//
//	// Send recall message.
//	g_Network.Send2Server(dwIndex, (char*)&msg, sizeof(MSG_SIEGERECALL_OBJINFO)) ;
//}





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_Execute_NextStep
//	DESC		: 공성 소환물의 소환 상태를 다음 단계로 넘기는 치트 관련 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_Execute_NextStep( DWORD dwIndex, char* pMsg )
{
	// 함수 인자 확인.
	if( !pMsg ) return ;


	// 원본 메시지 변환.
	MSG_BYTE5* pmsg = NULL ;
	pmsg = (MSG_BYTE5*)pMsg ;

	if( !pmsg ) return ;


	// 프로토콜을 확인한다.
	if( pmsg->Protocol == MP_SIEGE_CHEAT_NEXTSTEP_SYN )
	{
		MSG_BYTE5 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_OTHER_AGENT_SYN ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData1		= pmsg->bData1 ;
		msg.bData2		= pmsg->bData2 ;
		msg.bData3		= pmsg->bData3 ;
		msg.bData4		= pmsg->bData4 ;
		msg.bData5		= pmsg->bData5 ;

		g_Network.Broadcast2AgentServerExceptSelf( (char*)&msg, sizeof(MSG_BYTE5) ) ;
	}


	// 테마 를 확인한다.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->bData1) ;

	if(!pTheme)
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidThemeIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// 맵 번호를 확인한다.
	CSiegeMap* pMap = NULL ;
	pMap = pTheme->Get_SiegeMapByIdx( pmsg->bData2 ) ;

	// 맵 번호 일치 하지 않음을 처리.
	if( !pMap )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidMapIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// 스텝을 확인한다.
	CSiegeStep* pStep = NULL ;
	pStep = pMap->Get_StepInfo( pmsg->bData3 ) ;

	// 스텝 번호 일치 하지 않음을 처리.
	if( !pStep )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidStepIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// 오브젝트 번호를 확인한다.
	CSiege_AddObj* pAddObj = NULL ;
	pAddObj = pStep->Get_AddObjInfo( pmsg->bData4 ) ;

	// 오브젝트 번호 일치 하지 않음을 처리.
	if( !pAddObj )
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidObjIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// 1차 소환몹 인가? 2차 소환몹인가?
	if( pmsg->bData5 == 1 )
	{
		// 오브젝트 정보를 받는다.
		st_SIEGEOBJ* pObjInfo = NULL ;
		pObjInfo = pAddObj->Get_ObjInfo() ;

		if(!pObjInfo)
		{
			MSG_BYTE msg ;
			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

			msg.dwObjectID	= pmsg->dwObjectID ;

			msg.bData		= eCE_InvalidObjInfo ;

			g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

			return ;
		}

		MSG_SIEGERECALL_OBJINFO msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

		msg.dwObjectID	= dwIndex ;

		pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
		pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
		pObjInfo->byComIndex	= 0 ;
		pObjInfo->wParentMap	= 0 ;

		memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

		g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
	}
	else if( pmsg->bData5 == 2 )
	{
		
		{
			// 오브젝트 정보를 받는다.
			st_SIEGEOBJ* pObjInfo = NULL ;
			pObjInfo = pAddObj->Get_ObjInfo() ;

			if(!pObjInfo)
			{
				MSG_BYTE msg ;
				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

				msg.dwObjectID	= pmsg->dwObjectID ;

				msg.bData		= eCE_InvalidObjInfo ;

				g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

				return ;
			}

			MSG_SIEGERECALL_OBJINFO msg ;

			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

			msg.dwObjectID	= dwIndex ;

			pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
			pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
			pObjInfo->byComIndex	= 0 ;
			pObjInfo->wParentMap	= 0 ;

			memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

			g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
		}

		{
			// Check command.
			BYTE byCommandCount = pAddObj->Get_CommandCount() ;

			CSiegeCommand* pCommand ;
			for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
			{
				// Receive start term info.
				pCommand = NULL ;
				pCommand = pAddObj->Get_CommandInfo(byCCount) ;

				// Check term info.
				if(!pCommand) continue ;

				// Check child kind.
				switch(pCommand->Get_ChildKind(byCCount))
				{
				case e_COMMAND_NONE :					break ;

				case e_COMMAND_RECALL_DIE_OBJ :
					{
						// Receive info.
						st_DIE_RECALL_OBJ* pDieRecallObjInfo = NULL ;
						pDieRecallObjInfo = pCommand->Get_DieRecallObjInfo(byCCount) ;

						// Check obj info.
						if( pDieRecallObjInfo )
						{
							MSG_SIEGERECALL_OBJINFO msg ;

							msg.Category	= MP_SIEGERECALL ;
							msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

							msg.dwObjectID	= dwIndex ;

							pDieRecallObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
							pDieRecallObjInfo->objInfo.byComKind	= e_CK_COMMAND ;
							pDieRecallObjInfo->objInfo.byComIndex	= 0 ;
							pDieRecallObjInfo->objInfo.wParentMap	= pAddObj->Get_ObjInfo()->mapNum ;

							memcpy(&msg.siegeObj, &pDieRecallObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

							g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
						}
					}
					break ;

				default : break ;
				}
			}
		}

	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_Execute_NextStep
//	DESC		: 공성 소환물의 소환 상태를 모두 완료하는 함수 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_Execute_EndStep( DWORD dwIndex, char* pMsg )
{
	// 함수 인자 확인.
	if( !pMsg ) return ;


	// 원본 메시지 변환.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	if( !pmsg ) return ;


	// 프로토콜을 확인한다.
	if( pmsg->Protocol == MP_SIEGE_CHEAT_ENDSTEP_SYN )
	{
		MSG_BYTE2 msg ;

		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_ENDSTEP_OTHER_AGENT_SYN ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData1		= pmsg->bData1 ;
		msg.bData2		= pmsg->bData2 ;

		g_Network.Broadcast2AgentServerExceptSelf( (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}


	// 테마 를 확인한다.
	CSiegeTheme* pTheme = NULL ;
	pTheme = Get_ThemeInfoByThemeIdx(pmsg->bData1) ;

	if(!pTheme)
	{
		MSG_BYTE msg ;
		msg.Category	= MP_SIEGERECALL ;
		msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

		msg.dwObjectID	= pmsg->dwObjectID ;

		msg.bData		= eCE_InvalidThemeIdx ;

		g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

		return ;
	}


	// 맵을 확인한다.
	CSiegeMap* pMap ;
	CSiegeStep* pStep ;
	BYTE byChildCount ;
	BYTE byKind ;

	for( BYTE count = 0 ; count < pTheme->Get_MapCount() ; ++count )
	{
		pMap = NULL ;
		pMap = pTheme->Get_SiegeMapByIdx( count ) ;

		if(!pMap) continue ;

		if( pMap->Get_MapNum() != pmsg->bData2 ) continue ;

		// 스텝을 확인한다.
		for( BYTE sCount = 0 ; sCount < pMap->Get_StepCount() ; ++sCount )
		{
			pStep = NULL ;
			pStep = pMap->Get_StepInfo( sCount ) ;

			if(!pStep)
			{
				Throw_Error("Invalid step info!!", __FUNCTION__) ;
				continue ;
			}

			// 스텝을 실행한다.
			byChildCount = 0 ;
			byChildCount = pStep->Get_ChildCount() ;

			for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
			{
				byKind = e_STEP_ADD_OBJ ;
				byKind = pStep->Get_ChildKind(byCount) ;

				switch(byKind)
				{
				case e_STEP_ADD_OBJ :
					{
						CSiege_AddObj* pAddObj = NULL ;
						pAddObj = pStep->Get_AddObjInfo( byCount ) ;

						if( !pAddObj ) continue ;

						{
							// 오브젝트 정보를 받는다.
							st_SIEGEOBJ* pObjInfo = NULL ;
							pObjInfo = pAddObj->Get_ObjInfo() ;

							if(!pObjInfo)
							{
								MSG_BYTE msg ;
								msg.Category	= MP_SIEGERECALL ;
								msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_NACK ;

								msg.dwObjectID	= pmsg->dwObjectID ;

								msg.bData		= eCE_InvalidObjInfo ;

								g_Network.Send2User( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;

								return ;
							}

							MSG_SIEGERECALL_OBJINFO msg ;

							msg.Category	= MP_SIEGERECALL ;
							msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

							msg.dwObjectID	= dwIndex ;

							pObjInfo->byAddObjIdx	= pAddObj->Get_Index() ;
							pObjInfo->byComKind		= e_CK_ADD_OBJECT ;
							pObjInfo->byComIndex	= 0 ;
							pObjInfo->wParentMap	= 0 ;

							memcpy(&msg.siegeObj, pObjInfo, sizeof(st_SIEGEOBJ)) ;

							g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
						}

						{
							// Check command.
							BYTE byCommandCount = pAddObj->Get_CommandCount() ;

							CSiegeCommand* pCommand ;
							for( BYTE byCCount = 0 ; byCCount < byCommandCount ; ++byCCount )
							{
								// Receive start term info.
								pCommand = NULL ;
								pCommand = pAddObj->Get_CommandInfo(byCCount) ;

								// Check term info.
								if(!pCommand) continue ;

								// Check child kind.
								switch(pCommand->Get_ChildKind(byCCount))
								{
								case e_COMMAND_NONE :					break ;

								case e_COMMAND_RECALL_DIE_OBJ :
									{
										// Receive info.
										st_DIE_RECALL_OBJ* pDieRecallObjInfo = NULL ;
										pDieRecallObjInfo = pCommand->Get_DieRecallObjInfo(byCCount) ;

										// Check obj info.
										if( pDieRecallObjInfo )
										{
											MSG_SIEGERECALL_OBJINFO msg ;

											msg.Category	= MP_SIEGERECALL ;
											msg.Protocol	= MP_SIEGE_CHEAT_NEXTSTEP_SYN ;

											msg.dwObjectID	= dwIndex ;

											pDieRecallObjInfo->objInfo.byAddObjIdx	= pAddObj->Get_Index() ;
											pDieRecallObjInfo->objInfo.byComKind	= e_CK_COMMAND ;
											pDieRecallObjInfo->objInfo.byComIndex	= 0 ;
											pDieRecallObjInfo->objInfo.wParentMap	= pAddObj->Get_ObjInfo()->mapNum ;

											memcpy(&msg.siegeObj, &pDieRecallObjInfo->objInfo, sizeof(st_SIEGEOBJ)) ;

											g_Network.Broadcast2MapServer( (char*)&msg, sizeof(msg) ) ;
										}
									}
									break ;

								default : break ;
								}
							}
						}
					}
					break ;

				default : break ;
				}
			}
		}
	}
}









