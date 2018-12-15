//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr.cpp
//	DESC		: Implementation part of CSiegeRecallMgr class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files
#include "./SiegeRecallMgr.h"

#include "..\[CC]Header\GameResourceStruct.h"
#include "..\[CC]Header\GameResourceManager.h"

#include "../[CC]RecallInfo/SiegeRecall/Siege_AddObj.h"

#include "./Network.h"

#include "./Object.h"

#include "./UserTable.h"

#include "./RegenManager.h"

#include "./AISystem.h"

#include "./MapDBMsgParser.h"

#include "./SiegeWarfareMgr.h"

#include "./Monster.h"
#include "./Player.h"







//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeRecallMgr
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::CSiegeRecallMgr(void)
{
	// 081006 LYW --- SiegeRecallMgr : 공성 후 소환물이 소환되었는지 여부를 담을 플래그 추가.
	m_wRecallCount = 0 ;
	m_byReadyToUseWaterSeed = FALSE ;

	// 081226 LYW --- SiegeRecallMgr : 유니크 인덱스 추가.
	m_byUniqueIdx = 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ~CSiegeRecallMgr
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
CSiegeRecallMgr::~CSiegeRecallMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Initialieze
//	DESC		: The function to initialize recall manager.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Initialieze()
{
	// Resetting siege obj info.
	Reset_SiegeObjInfo() ;

	// Load object info.
	//Request_ObjFromDB() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Reset_SiegeObjInfo
//	DESC		: The function to resetting.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Reset_SiegeObjInfo() 
{
	// 081006 LYW --- SiegeRecallMgr : 공성 후 소환물이 소환되었는지 여부를 담을 플래그 추가.
	m_byReadyToUseWaterSeed = FALSE ;


	// Remove all obj info.
	M_MADDOBJ::iterator it ;
	
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		m_mAddObj.erase(it) ;
	}

	SIEGEWARFAREMGR->Set_LoadSummonFlag( FALSE ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Get_AddObjIdx
//	DESC		: The function to return add object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
CSiege_AddObj* CSiegeRecallMgr::Get_AddObjIdx(DWORD dwObjIdx) 
{
	// Check index.
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it)
	{
		//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
		if( it->second.Get_ObjInfo()->dwObjectIdx != dwObjIdx ) continue ;

		return &it->second ;
	}


	// Return null.
	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: NetworkMsgParser
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::NetworkMsgParser(void* pMsg )
{
	// Check a parameter.
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
	case MP_SIEGERECALL_ADDOBJ_SYN :								Add_Object( pMsg ) ;					break ;

	case MP_SIEGERECALL_ADDCHECKOBJDIE_SYN :						Add_CheckObjDie( pMsg ) ;				break ;

	case MP_SIEGERECALL_USEITEM_SYN :								Add_CheckUseItem( pMsg ) ;				break ;

	case MP_SIEGERECALL_ADDCOMMAND_SYN :							Add_Command( pMsg ) ;					break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_ALLOBJ_SYN :			Remove_AllMapAllObj( pMsg ) ;			break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_ALLMAP_SPECIFYOBJ_SYN :		Remove_AllMapSpecifyObj( pMsg ) ;		break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_ALLOBJ_SYN :		Remove_SpecifyMapAllObj( pMsg ) ;		break ;

	case MP_SIEGERECALL_NOTICE_REMOVE_SPECIFYMAP_SPECIFYOBJ_SYN :	Remove_SpecifyMapSpecifyObj( pMsg ) ;	break ;

	case MP_SIEGERECALL_REQUEST_OBJINFO_ACK :						Set_LoadSummonFlag( pMsg ) ;			break ;

	// 081006 LYW --- SiegeRecallMgr : 소환해야 할 소환 카운트를 받는다.
	case MP_SIEGERECALL_REQUEST_RECALLCOUNT_ACK :					Set_RecallCount( pMsg ) ;				break ;

	// 081015 LYW --- SiegeRecallMgr : 공성 소환물의 완전한 킬 상태를 로딩하여 정보가 넘어왔을 때 처리하는 작업 추가.
	case MP_SIEGERECALL_LOAD_RECALLCOUNT_ACK :						Load_RecallCount( pMsg ) ;				break ;
	case MP_SIEGE_CHEAT_NEXTSTEP_SYN :								Cheat_ExcuteNextStep( pMsg ) ;			break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Object
//	DESC		: The function constructor.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Object(void* pMsg)
{
	// 소환이 완료 된 상태면, 더이상 소환 명령을 받지 않는다.
	//if( SIEGEWARFAREMGR->Is_LoadedSummon() ) return ;

	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() );
	if( !IsSiegeWarfareZone ) return ;

	// Check a aprarmeter.
	if(!pMsg)
	{
		Throw_Error("Invalid a parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check clone object info.
	st_SIEGEOBJ* pCheckObjInfo ;
	M_MADDOBJ::iterator it ;
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		pCheckObjInfo = NULL ;
		pCheckObjInfo = it->second.Get_ObjInfo() ;

		if(!pCheckObjInfo) continue ;

		if(	pCheckObjInfo->wThemeIdx	== pmsg->siegeObj.wThemeIdx		&&
			pCheckObjInfo->mapNum		== pmsg->siegeObj.mapNum		&&
			pCheckObjInfo->byStepIdx	== pmsg->siegeObj.byStepIdx		&&
			pCheckObjInfo->byAddObjIdx	== pmsg->siegeObj.byAddObjIdx	&&
			pCheckObjInfo->byComKind	== pmsg->siegeObj.byComKind		&&
			pCheckObjInfo->byComIndex	== pmsg->siegeObj.byComIndex	&&
			pCheckObjInfo->fXpos		== pmsg->siegeObj.fXpos			&&
			pCheckObjInfo->fZpos		== pmsg->siegeObj.fZpos ) 
		{
			return ;
		}
	}


	// Check map.
	if( pmsg->siegeObj.mapNum != g_pServerSystem->GetMapNum() ) 
	{
		return ;
	}

	VECTOR3 vRecallPos ;

	BASE_MONSTER_LIST* pList = NULL ;

	BYTE byRadius = pmsg->siegeObj.byRadius ;

	// 반경이 0이 아니면,
	if( pmsg->siegeObj.byUseRandomPos )
	{
		// 리콜 위치를 랜덤하게 세팅한다.
		vRecallPos.x = pmsg->siegeObj.fXpos + (rand()%(byRadius*2)) - byRadius ;
		vRecallPos.z = pmsg->siegeObj.fZpos + (rand()%(byRadius*2)) - byRadius ;
	}
	// 반경이 0이면, 
	else
	{
		// 리콜 위치를 세팅한다.
		vRecallPos.x = pmsg->siegeObj.fXpos ;
		vRecallPos.z = pmsg->siegeObj.fZpos ;
	}

	// 몬스터 타입에 따른 몬스터 리스트를 받는다.
	pList = NULL ;
	pList = GAMERESRCMNGR->GetMonsterListInfo(pmsg->siegeObj.dwObjectIdx) ;
	
	// 몬스터 리스트가 유효하다면,
	if(pList)
	{
		// 리젠 매니져로 오브젝트를 리젠 한다.
		CMonster* pMonster = NULL ;
		pMonster = REGENMGR->RegenObject(
			g_pAISystem.GeneraterMonsterID(),
			0,
			1,
			pList->ObjectKind,
			pList->MonsterKind,
			&vRecallPos,
			DWORD(EVENTRECALL_GROUP_ID),
			0,
			0,
			FALSE,
			TRUE);

		if( pMonster )
		{
			// 몬스터 리스트에 추가/
			CSiege_AddObj newObj ;
			// 081226 LYW --- SiegeRecallMgr : 인덱스 세팅을 Map컨테이너의 사이즈로 하면 문제 발생.
			// 유니크 인덱스로 설정할 수 있도록 한다.
			newObj.Set_Index( m_byUniqueIdx ) ;
			//newObj.Set_Index( m_mAddObj.size() ) ;
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			newObj.Set_MosterID( pMonster->GetID() ) ;
			newObj.Init_ObjInfo(&pmsg->siegeObj) ;

			// 081226 LYW --- SiegeRecallMgr : 유니크 인덱스를 증가한다.
			++m_byUniqueIdx ;

			m_mAddObj.insert( std::make_pair( newObj.Get_Index(), newObj ) ) ;

			if( pmsg->siegeObj.byComKind == e_CK_ADD_OBJECT )
			{
				Add_ObjToDB( pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.mapNum, g_pServerSystem->GetMapNum(), 
							 pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, e_CK_ADD_OBJECT, 0, 1 ) ;


				// 081028 LYW --- SiegeRecallMgr : 캐터펄트 소환 성공 메시지 로그를 남긴다.
				if( IsSiegeWarfareZone )
				{
					char szLog[1024] = {0, } ;
					sprintf(szLog, "Recall Catapult - %d %d %d %d %d %d %d %d \n", pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.mapNum, 
						g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, e_CK_ADD_OBJECT, 0, 1 ) ;
					WriteCatapultLog( szLog ) ;
				}

			}
			else if( pmsg->siegeObj.byComKind == e_CK_COMMAND )
			{
				Add_ObjToDB( pmsg->siegeObj.wThemeIdx, pmsg->wExValue, g_pServerSystem->GetMapNum(), 
							 pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, e_CK_COMMAND, pmsg->siegeObj.byComIndex, 3 ) ;

				// 081028 LYW --- SiegeRecallMgr : 캐터펄트 소환 성공 메시지 로그를 남긴다.
				if( IsSiegeWarfareZone )
				{
					char szLog[1024] = {0, } ;
					sprintf(szLog, "Recall Catapult - %d %d %d %d %d %d %d %d \n", pmsg->siegeObj.wThemeIdx, 
						pmsg->wExValue, g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, 
						e_CK_COMMAND, pmsg->siegeObj.byComIndex, 3 ) ;
					WriteCatapultLog( szLog ) ;
				}
			}
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_CheckObjDie
//	DESC		: The function to add check routine for object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_CheckObjDie(void* pMsg)
{
	// Check parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid message parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_CHECK_OBJ_DIE* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_CHECK_OBJ_DIE*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	if( pmsg->p_mapNum != g_pServerSystem->GetMapNum() ) return ;


	// Check check type.
	switch(pmsg->byCheckKind)
	{
	// Add start term.
	case e_SIEGE_BASEKIND_START_TERM : 
		{
			// Check add obj info.
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive start term count.
			BYTE byStermCount = pAddObj->Get_StartTermCount() ;

			// Setting obj info.
			st_CHECK_OBJ_DIE objInfo ;

			objInfo.mapNum		= pmsg->check_mapNum ;
			objInfo.byObjectIdx	= pmsg->check_ObjIdx ;

			// Check start term count.
			if( byStermCount == 0)
			{
				// Setting info index.
				objInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckObjDie( &objInfo ) ;

				pAddObj->Insert_Sterm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_StartTermInfo( byStermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				objInfo.byIdx = pTermInfo->Get_ChildCount() ;

				pTermInfo->Insert_CheckObjDie( &objInfo ) ;
			}
		}
		break ;

	case e_SIEGE_BASEKIND_END_TERM : 
		{
			// Check add obj info.
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive start term count.
			BYTE byEtermCount = pAddObj->Get_EndTermCount() ;

			// Setting obj info.
			st_CHECK_OBJ_DIE objInfo ;

			objInfo.mapNum		= pmsg->check_mapNum ;
			objInfo.byObjectIdx	= pmsg->check_ObjIdx ;

			// Check start term count.
			if( byEtermCount == 0)
			{
				// Setting info index.
				objInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckObjDie( &objInfo ) ;

				pAddObj->Insert_ETerm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_EndTermInfo( byEtermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				objInfo.byIdx = pTermInfo->Get_ChildCount() ;

				pTermInfo->Insert_CheckObjDie( &objInfo ) ;
			}
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_CheckUseItem
//	DESC		: The function to add check routine for use item.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_CheckUseItem(void* pMsg) 
{
	// Check parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid message parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_CHECK_USE_ITEM* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_CHECK_USE_ITEM*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	if( pmsg->p_mapNum != g_pServerSystem->GetMapNum() ) return ;


	// Check check type.
	switch(pmsg->byCheckKind)
	{
	// Add start term.
	case e_SIEGE_BASEKIND_START_TERM : 
		{
			// Check add obj info.
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive start term count.
			BYTE byStermCount = pAddObj->Get_StartTermCount() ;

			// Setting item info.
			st_CHECK_USE_ITEM itemInfo ;

			itemInfo.dwItemIdx	= pmsg->dwItemIdx ;
			itemInfo.wUseCount	= pmsg->wItemCount ;

			// Check start term count.
			if( byStermCount == 0)
			{
				// Setting info index.
				itemInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckUseItem( &itemInfo ) ;

				// Insert term info.
				pAddObj->Insert_Sterm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				// Receive term info.
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_StartTermInfo( byStermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				itemInfo.byIdx = pTermInfo->Get_ChildCount() ;

				// Insert info.
				pTermInfo->Insert_CheckUseItem( &itemInfo ) ;
			}
		}
		break ;

	case e_SIEGE_BASEKIND_END_TERM : 
		{
			// Check add obj info.
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;

			// Receive add obj info.
			CSiege_AddObj* pAddObj = NULL ;
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

			if(!pAddObj) return ;

			// Receive end term count.
			BYTE byEtermCount = pAddObj->Get_EndTermCount() ;

			// Setting item info.
			st_CHECK_USE_ITEM itemInfo ;

			itemInfo.dwItemIdx	= pmsg->dwItemIdx ;
			itemInfo.wUseCount	= pmsg->wItemCount ;

			// Check end term count.
			if( byEtermCount == 0)
			{
				// Setting info index.
				itemInfo.byIdx = 0 ;

				// Setting term info.
				CSiegeTerm newTerm ;

				newTerm.Set_Index( 0 ) ;

				newTerm.Insert_CheckUseItem( &itemInfo ) ;

				// Insert term info.
				pAddObj->Insert_ETerm( pAddObj->Get_ChildCount(), &newTerm ) ;
			}
			else
			{
				// Receive term info.
				CSiegeTerm* pTermInfo = NULL ;
				pTermInfo = pAddObj->Get_EndTermInfo( byEtermCount-1 ) ;

				if( !pTermInfo )
				{
					Throw_Error( "Invalid add obj Info!!", __FUNCTION__ ) ;
					return ;
				}

				itemInfo.byIdx = pTermInfo->Get_ChildCount() ;

				// Insert info.
				pTermInfo->Insert_CheckUseItem( &itemInfo ) ;
			}
		}
		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_Command
//	DESC		: he function to add command.
//	PROGRAMMER	: Yongs Lee
//	DATE		: JAugust 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Add_Command(void* pMsg)
{
	// Check parameter.
	if(!pMsg)
	{
		Throw_Error("Invalid message parameter!!", __FUNCTION__) ;
		return ;
	}


	// Convert a message.
	MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_COMMAND_DIE_RECALL_OBJ*) pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// Check map.
	if( pmsg->p_mapNum != g_pServerSystem->GetMapNum() ) return ;


	// Check add obj info.
	//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
	if(!Check_MyAddObjInfo( pmsg->p_byStepIdx, pmsg->p_dwObjectIdx )) return ;


	// Receive add obj info.
	CSiege_AddObj* pAddObj = NULL ;
	//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
	pAddObj = Get_AddObjIdx( pmsg->p_dwObjectIdx ) ;

	if(!pAddObj) return ;
 

	// Receive command count.
	BYTE byCommandCount = pAddObj->Get_CommandCount() ;


	// Check command count.
	if( byCommandCount == 0)
	{
		// Setting command info.
		CSiegeCommand newCommand ;

		newCommand.Set_Index( 0 ) ;

		newCommand.Add_Die_Recall_Obj( &pmsg->dieRecallObj ) ;

		// Insert command info.
		pAddObj->Insert_Command( pAddObj->Get_ChildCount(), &newCommand ) ;
	}
	else
	{
		// Receive command info.
		CSiegeCommand* pCommand = NULL ;
		pCommand = pAddObj->Get_CommandInfo( byCommandCount-1 ) ;

		if( !pCommand )
		{
			Throw_Error( "Invalid command Info!!", __FUNCTION__ ) ;
			return ;
		}

		pCommand->Add_Die_Recall_Obj( &pmsg->dieRecallObj ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Add_ObjToDB
//	DESC		: The function to add object info to database.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Add_ObjToDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx, WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx)
void CSiegeRecallMgr::Add_ObjToDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx, 
								  BYTE byAddObjIdx, BYTE byComKind, BYTE byComIdx, BYTE byRecallStep )
{
	// 임시 버퍼를 선언한다.
	char txt[128] = {0, } ;


	// 쿼리문을 작성한다.
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
		wTheme, wMapNum, wRecallMap, byStepIdx, byAddObjIdx, byComKind, byComIdx, byRecallStep, 200) ;


	// 쿠리문을 실행한다.
	g_DB.Query(eQueryType_FreeQuery, eSiegeREcallInsert, 0, txt) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_ObjFromDB
//	DESC		: The function to remove object info from database.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 11, 2008
//-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Remove_ObjFromDB(WORD wTheme, WORD wMapNum, BYTE byStepIdx,  WORD wAddObjIdx, BYTE IsParent, BYTE byChildIdx)
void CSiegeRecallMgr::Remove_ObjFromDB(WORD wTheme, WORD wMapNum, WORD wRecallMap, BYTE byStepIdx,  
									   BYTE byAddObjIdx, BYTE byComKind, BYTE byComIndex, BYTE byRecallStep )
{
	// 임시 버퍼를 선언한다.
	char txt[128] = {0, } ;


	// 쿼리문을 작성한다.
	//sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_REMOVE, FALSE, wTheme, wMapNum, wRecallMap, byStepIdx, byAddObjIdx, byComKind, byComIndex) ;
	sprintf(txt, "EXEC  %s %d, %d, %d, %d, %d, %d, %d, %d, %d",	MP_SIEGERECALL_INSERT, 
		wTheme, wMapNum, wRecallMap, byStepIdx, byAddObjIdx, byComKind, byComIndex, byRecallStep, m_wRecallCount) ;


	// 쿼리문을 실행한다.
	//g_DB.Query(eQueryType_FreeQuery, eSiegeRecallRemove, 0, txt, 0) ;
	g_DB.Query(eQueryType_FreeQuery, eSiegeREcallInsert, 0, txt, 0) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Request_ObjFromDB
//	DESC		: The function to load object info from database.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 12, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Request_ObjFromDB()
{
	// 소환 오브젝트 정보가 남아있으면 모두 비운다.
	if( m_mAddObj.size() != 0 )
	{
		M_MADDOBJ::iterator it ;

		for( BYTE byCount = 0 ; byCount < m_mAddObj.size() ; ++byCount )
		{
			it = m_mAddObj.find(byCount) ;
			if( it == m_mAddObj.end() ) continue ;

			m_mAddObj.erase(it) ;
		}
	}


	// 080919 LYW --- SiegeRecallMgr : 쿼리문 실행은 맵서버에서 하자.
	// 오브젝트 정보를 DB에 요청한다.
	MSG_WORD msg ;

	msg.Category	= MP_SIEGERECALL ;
	msg.Protocol	= MP_SIEGERECALL_REQUEST_OBJINFO_SYN ;

	// State 를 담아서 현재 상태를 체크하여 에이전트에 알려준다.
	msg.dwObjectID	= SIEGEWARFAREMGR->GetState() ;
	msg.wData		= g_pServerSystem->GetMapNum() ;

	g_Network.Send2AgentServer( (char*)&msg, sizeof(MSG_WORD) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_MyAddObjInfo
//	DESC		: The function to check addObjInfo.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
BYTE CSiegeRecallMgr::Check_MyAddObjInfo(BYTE byStepNum, DWORD dwObjIdx)
{
	// Check mapNum.
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		if( it->second.Get_ParentStepIdx() != byStepNum ) continue ;

		if( it->second.Get_ObjInfo()->dwObjectIdx != dwObjIdx ) continue ;

		return TRUE ;
	}


	// Return false.
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Check_ObjectDie
//	DESC		: The function to check object die.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
//void CSiegeRecallMgr::Check_ObjectDie(DWORD dwObjectID, BYTE IsCheatMode)
void CSiegeRecallMgr::Check_ObjectDie(CObject* pKiller, DWORD dwObjectID, BYTE IsCheatMode)
{
	// Killer가 NULL로 들어올 수 있음을 유의한다.

	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() ) ;
	if( !IsSiegeWarfareZone ) return ;


	// 081215 LYW --- SiegeRecallMgr : 마을 타입은 처리를 하지 않는다.(마을에는 케터펄트가 소환되지 않음)
	if( SIEGEWARFAREMGR->GetMapInfo() == eNeraCastle_Village || 
		SIEGEWARFAREMGR->GetMapInfo() >= eSiegeWarfare_Map_Max )
		return ;

	// 081215 LYW --- SiegeRecallMgr : 몬스터 타입이 캐터펄트가 아니라면, return 처리를 한다.
	CMonster* pDelMonster = (CMonster*)g_pUserTable->FindUser( dwObjectID ) ;
	if( !pDelMonster ) return ;
	// 캐터펄트의 몬스터 타입은 352~367이다.
	WORD wMonsterKind = pDelMonster->GetMonsterKind() ;
	if( wMonsterKind < 352 || wMonsterKind > 367 ) return ;



	// 081028 LYW --- SiegeRecallMgr : 몬스터 삭제 처리 함수가 호출되었는지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		char szLogCall[1024] = {0, } ;
		DWORD dwKillerID = 0 ;
		if( pKiller ) dwKillerID = pKiller->GetID() ;

		sprintf( szLogCall, "Call - Check_ObjectDie : Player : %d Monster : %u IsCheatMode : %d\n", 
			dwKillerID, dwObjectID, IsCheatMode ) ;
		WriteCatapultLog( szLogCall ) ;
	}

	//if( pKiller->GetObjectKind() == eObjectKind_Player )
	if( pKiller && pKiller->GetObjectKind() == eObjectKind_Player )	// 081212 NYJ - 킬러포인터가 유효한 경우만 체크
	{
		CPlayer* player = ( CPlayer* )pKiller;

		// 081027 LUJ, 로그
		InsertLogSiege(
			eSiegeLogDestoryCatapult,
			player->GetGuildIdx(),
			player->GetID() );
	}
	else
	{
		// 081027 LUJ, 로그
		InsertLogSiege(
			eSiegeLogDestoryCatapult,
			0,
			0,
			"by no player" );
	}

	// Declare temp variables.
	st_SIEGEOBJ* pObjInfo ;

	CSiegeTerm* pSTerm ;
	CSiegeTerm* pETerm ;
	CSiegeCommand* pCommand ;

	BYTE bySTermCount ;
	BYTE byETermCount ;
	BYTE byCommandCount ;

	M_MADDOBJ::iterator it ;


	// 081028 LYW --- SiegeRecallMgr : 몬스터 삭제 처리 함수가 호출되었는지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		char szCheckBefor[1024] = {0, } ;
		sprintf( szCheckBefor, "Befor Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckBefor ) ;
	}

	// Check add object.
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		// Receive object info.
		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;


		// Check object info.
		if(!pObjInfo) continue ;


		// Check object index.
		//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
		if( it->second.Get_MonsterID() != dwObjectID )
			continue;


		// Check start term.
		bySTermCount = 0 ;
		bySTermCount = it->second.Get_StartTermCount() ;

		for(BYTE bySCount = 0 ; bySCount < bySTermCount ; ++bySCount)
		{
			pSTerm = NULL ;
			pSTerm = it->second.Get_StartTermInfo( bySCount ) ;

			if( !pSTerm ) continue ;

			// Send start term info.
			Send_StartTermInfo( it->second.Get_ParentStepIdx(), pSTerm, bySCount ) ;
		}


		// Check end term.
		byETermCount = 0 ;
		byETermCount = it->second.Get_EndTermCount() ;

		for(BYTE byECount = 0 ; byECount < byETermCount ; ++byECount)
		{
			pETerm = NULL ;
			pETerm = it->second.Get_EndTermInfo( byECount ) ;

			if( !pETerm ) continue ;

			// Send start term info.
			Send_EndTermInfo( it->second.Get_ParentStepIdx(), pETerm, byECount ) ;
		}


		// Check command.
		byCommandCount = 0 ;
		byCommandCount = it->second.Get_CommandCount() ;

		for(BYTE byComCount = 0 ; byComCount < byCommandCount ; ++byComCount)
		{
			pCommand = NULL ;
			pCommand = it->second.Get_CommandInfo( byComCount ) ;

			if( !pCommand ) continue ;

			// Send start term info.
			//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
			Send_CommandInfo( it->second.Get_ParentStepIdx(), pCommand, it->second.Get_ObjInfo()->byAddObjIdx ) ;
		}

		if( IsSiegeWarfareZone )
		{
			WriteCatapultLog( "Ready to delete!\n" ) ;
		}


		// Remove obj info from database.
		if(pObjInfo->byComKind == e_CK_ADD_OBJECT)
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->mapNum, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 2) ;

			// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
			DWORD dwKillerID = 0 ;
			char killerName[ MAX_NAME_LENGTH+1 ] = {0, } ;

			if( pKiller )
			{
				dwKillerID = pKiller->GetID() ;
				SafeStrCpy( killerName, pKiller->GetObjectName(), MAX_NAME_LENGTH ) ;
			}
			else
			{
				SafeStrCpy( killerName, "Use Cheat Command!!\n", MAX_NAME_LENGTH ) ;
			}

			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %d %s %u\n", pObjInfo->wThemeIdx, 
					pObjInfo->mapNum, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 2, dwKillerID, killerName, pObjInfo->dwObjectIdx ) ;
				WriteCatapultLog( szLog ) ;
			}

			// 081029 LYW --- SiegeRecallMgr : 캐터펄트 소환 안되는 버그 수정.
			// 치트키 인경우...  
			if( IsCheatMode )
			{
				//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
				g_pServerSystem->RemoveMonster( it->second.Get_MonsterID() ) ;
			}

			// Delete obj info.
			m_mAddObj.erase( it ) ;
		}
		else
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->wParentMap, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 4) ;

			// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
			DWORD dwKillerID = 0 ;
			char killerName[ MAX_NAME_LENGTH+1 ] = {0, } ;

			if( pKiller )
			{
				dwKillerID = pKiller->GetID() ;
				SafeStrCpy( killerName, pKiller->GetObjectName(), MAX_NAME_LENGTH ) ;
			}
			else
			{
				SafeStrCpy( killerName, "Use Cheat Command!!\n", MAX_NAME_LENGTH ) ;
			}

			// 081028 LYW --- SiegeRecallMgr : 캐터펄트 소환 성공 메시지 로그를 남긴다.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %d %s %u\n", pObjInfo->wThemeIdx, 
					pObjInfo->wParentMap, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 4, dwKillerID, killerName, pObjInfo->dwObjectIdx ) ;
				WriteCatapultLog( szLog ) ;
			}

			// 081029 LYW --- SiegeRecallMgr : 캐터펄트 소환 안되는 버그 수정.
			// 치트키 인경우...  
			if( IsCheatMode )
			{
				//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
				g_pServerSystem->RemoveMonster( it->second.Get_MonsterID() ) ;
			}

			// Delete obj info.
			m_mAddObj.erase( it ) ;
		}

		break ;
	}


	// 081028 LYW --- SiegeRecallMgr : 몬스터 삭제 처리 함수가 호출되었는지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		char szCheckAfter[1024] = {0, } ;
		sprintf( szCheckAfter, "After Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckAfter ) ;
	}

	// 몹이 소환 된 상태이고, 소환 된 몹을 모두 잡았으면, 워터시드 각인이 가능한 상태로 상태를 변경한다.
	if( SIEGEWARFAREMGR->GetState() == eSiegeWarfare_State_Start && m_mAddObj.size() == 0 )
	{
		SIEGEWARFAREMGR->SetState(eSiegeWarfare_State_First) ;

		if( IsSiegeWarfareZone )
		{
			char szLog[1024] = {0, } ;
			sprintf( szLog, "Ready To WaterSeed Use - %d", g_pServerSystem->GetMapNum() ) ;
			WriteCatapultLog( szLog ) ;
		}
	}

	// 081222 LYW --- SiegeRecallMgr : 모든 몬스터를 잡았는지 db에 개수 요청을 한다.
	RequestKillCountToDB() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Send_StartTermInfo
//	DESC		: The function to send start term info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Send_StartTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) 
{
	// Check parameter.
	if(!pTerm)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check term kind.
	BYTE byKind ;
	BYTE byChildCount = pTerm->Get_ChildCount() ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_TERM_NONE ;
		byKind = pTerm->Get_ChildKind( byCount ) ;

		switch( byKind )
		{
		case e_TERM_DIE_OBJ :
			{
				// Notice check obj die by start term.
				st_CHECK_OBJ_DIE* pInfo = NULL ;
				pInfo = pTerm->Get_ChkObjDieInfo(byIdx) ;

				if( !pInfo )
				{
					Throw_Error("Invalid die obj info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Remove_CheckObjDie( pInfo ) ;
			}
			break ;

		case e_TERM_USE_ITEM : 
			{
				// Notice check use item by start term.
				st_CHECK_USE_ITEM* pInfo = NULL ;
				pInfo = pTerm->Get_ChkUseItemInfo(byIdx) ;

				if(!pInfo)
				{
					Throw_Error("Invalid use item info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Decrease_UseItemCount(pInfo) ;
			}
			break ;

		case e_TERM_NONE : 
		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Send_EndTermInfo
//	DESC		: The function to send end term info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Send_EndTermInfo(BYTE byParentStepIdx, CSiegeTerm* pTerm, BYTE byIdx) 
{
	// Check parameter.
	if(!pTerm)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check term kind.
	BYTE byKind ;
	BYTE byChildCount = pTerm->Get_ChildCount() ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_TERM_NONE ;
		byKind = pTerm->Get_ChildKind( byCount ) ;

		switch( byKind )
		{
		case e_TERM_DIE_OBJ :
			{
				// Notice check obj die by start term.
				st_CHECK_OBJ_DIE* pInfo = NULL ;
				pInfo = pTerm->Get_ChkObjDieInfo(byIdx) ;

				if( !pInfo )
				{
					Throw_Error("Invalid die obj info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Remove_CheckObjDie( pInfo ) ;

				//if( pTerm->Get_CheckObjDieCount() == 0 )
				//{
				//	// Setting message.
				//	MSG_WORD3 msg ;

				//	msg.Category	= MP_SIEGERECALL ;
				//	msg.Protocol	= MP_SIEGERECALL_NOTICE_ETERM_CHKOBJDIE_COMPLETE_SYN ;

				//	msg.dwObjectID	= 0 ;

				//	msg.wData1		= g_pServerSystem->GetMapNum() ;
				//	msg.wData2		= (WORD)byParentStepIdx ;
				//	msg.wData3		= pTerm->Get_ParentIdx() ;

				//	// Send message.
				//	g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
				//}
			}
			break ;

		case e_TERM_USE_ITEM : 
			{
				// Notice check use item by start term.
				st_CHECK_USE_ITEM* pInfo = NULL ;
				pInfo = pTerm->Get_ChkUseItemInfo(byIdx) ;

				if(!pInfo)
				{
					Throw_Error("Invalid use item info!!", __FUNCTION__) ;
					return ;
				}

				pTerm->Decrease_UseItemCount(pInfo) ;

				//if( pTerm->Get_CheckUseItemCount() == 0 )
				//{
				//	// Setting message.
				//	MSG_WORD3 msg ;

				//	msg.Category	= MP_SIEGERECALL ;
				//	msg.Protocol	= MP_SIEGERECALL_NOTICE_ETERM_USEITEM_COMPLETE_SYN ;

				//	msg.dwObjectID	= 0 ;

				//	msg.wData1		= g_pServerSystem->GetMapNum() ;
				//	msg.wData2		= (WORD)byParentStepIdx ;
				//	msg.wData3		= pTerm->Get_ParentIdx() ;

				//	// Send message.
				//	g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
				//}
			}
			break ;

		case e_TERM_NONE : 
		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Send_CommandInfo
//	DESC		: The function to send command info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 04, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Send_CommandInfo(BYTE byParentStepIdx, CSiegeCommand* pCommand, BYTE byIdx) 
{
	// Check parameter.
	if(!pCommand)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// Check term kind.
	BYTE byKind ;
	BYTE byChildCount = pCommand->Get_ChildCount() ;

	for( BYTE byCount = 0 ; byCount < byChildCount ; ++byCount )
	{
		byKind = e_TERM_NONE ;
		byKind = pCommand->Get_ChildKind( byCount ) ;

		switch( byKind )
		{
		case e_COMMAND_RECALL_DIE_OBJ :
			{
				// Notice check obj die by start term.

				st_DIE_RECALL_OBJ* pRecallObj = NULL ;
				pRecallObj = pCommand->Get_DieRecallObjInfo(byCount) ;

				if(!pRecallObj) return ;

				MSG_WORD3 msg ;

				msg.Category	= MP_SIEGERECALL ;
				msg.Protocol	= MP_SIEGERECALL_NOTICE_DIERECALLOBJ_READY_SYN ;

				msg.dwObjectID	= 0 ;

				msg.wData1		= g_pServerSystem->GetMapNum() ;
				msg.wData2		= (WORD)byParentStepIdx ;
				msg.wData3		= byIdx ;

				// Send message.
				g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
			}
			break ;

		default : break ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_AllMapAllObj
//	DESC		: 모든 맵 / 모든 오브젝트를 삭제해야 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_AllMapAllObj( void* pMsg )
{
	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() ) ;
	if( !IsSiegeWarfareZone ) return ;

	// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		WriteCatapultLog( "Call - Remove_AllMapAllObj" ) ;
	}

	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지로 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}

	if( IsSiegeWarfareZone )
	{
		char szCheckBefor[1024] = {0, } ;
		sprintf( szCheckBefor, "Befor Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckBefor ) ;
	}

	for(M_MADDOBJ::iterator it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		st_SIEGEOBJ* pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		if( pObjInfo->byComKind == e_CK_ADD_OBJECT )
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->mapNum, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 0) ;


			// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pObjInfo->wThemeIdx, 
					pObjInfo->mapNum, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 0,  pObjInfo->dwObjectIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}
		else
		{
			Remove_ObjFromDB( pObjInfo->wThemeIdx, pObjInfo->wParentMap, g_pServerSystem->GetMapNum(), 
			pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, pObjInfo->byComKind, pObjInfo->byComIndex, 0) ;

			// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pObjInfo->wThemeIdx, 
					pObjInfo->mapNum, g_pServerSystem->GetMapNum(), pObjInfo->byStepIdx, pObjInfo->byAddObjIdx, 
					pObjInfo->byComKind, pObjInfo->byComIndex, 0,  pObjInfo->dwObjectIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;
	}

	m_mAddObj.clear() ;

	// 081028 LYW --- SiegeRecallMgr : 몬스터 삭제 처리 함수가 호출되었는지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		char szCheckAfter[1024] = {0, } ;
		sprintf( szCheckAfter, "After Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckAfter ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_AllMapSpecifyObj
//	DESC		: 모든 맵 / 지정 된 오브젝트를 삭제해야 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_AllMapSpecifyObj( void* pMsg )
{
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
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// 소환 된 오브젝트를 확인한다.
	M_MADDOBJ::iterator it ;

	st_SIEGEOBJ* pObjInfo ;

	CObject* pObject ;

	while( m_mAddObj.size() != 0 )
	{
		it = m_mAddObj.find( BYTE(m_mAddObj.size() - 1) ) ;

		if( it == m_mAddObj.end() ) break ;

		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		pObject = NULL ;
		pObject = g_pUserTable->FindUser(pObjInfo->dwObjectIdx) ;

		if(!pObject) continue ;

		if( pObject->GetObjectKind() != pmsg->wData ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;

		m_mAddObj.erase( it ) ;
	}
}






//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_SpecifyMapAllObj
//	DESC		: 지정 된 맵 / 모든 오브젝트를 삭제해야 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_SpecifyMapAllObj( void* pMsg ) 
{
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
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// 맵 인덱스를 확인한다.
	if( g_pServerSystem->GetMapNum() != pmsg->wData ) return ;


	// 소환 된 오브젝트를 확인한다.
	M_MADDOBJ::iterator it ;
	st_SIEGEOBJ* pObjInfo ;

	while( m_mAddObj.size() != 0 )
	{
		it = m_mAddObj.find( BYTE( m_mAddObj.size() - 1) ) ;

		if( it == m_mAddObj.end() ) break ;

		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;

		m_mAddObj.erase( it ) ;
	}
}






//-------------------------------------------------------------------------------------------------
//	NAME		: Remove_SpecifyMapSpecifyObj
//	DESC		: 지정 된 맵 / 지정 된 오브젝트를 삭제해야 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 06, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Remove_SpecifyMapSpecifyObj( void* pMsg ) 
{
	// 함수 인자 확인.
	if(!pMsg)
	{
		Throw_Error("Invalid parameter!!", __FUNCTION__) ;
		return ;
	}


	// 원본 메시지로 변환.
	MSG_WORD2* pmsg = NULL ;
	pmsg = (MSG_WORD2*)pMsg ;

	if(!pmsg)
	{
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// 맵 인덱스를 확인한다.
	if( g_pServerSystem->GetMapNum() != pmsg->wData1 ) return ;


	// 소환 된 오브젝트를 확인한다.
	M_MADDOBJ::iterator it ;

	st_SIEGEOBJ* pObjInfo ;

	CObject* pObject ;

	while( m_mAddObj.size() != 0 )
	{
		it = m_mAddObj.find( BYTE( m_mAddObj.size() - 1 ) ) ;

		if( it == m_mAddObj.end() ) break ;

		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ;

		if( !pObjInfo ) continue ;

		pObject = NULL ;
		pObject = g_pUserTable->FindUser(pObjInfo->dwObjectIdx) ;

		if(!pObject) continue ;

		if( pObject->GetObjectKind() != pmsg->wData2 ) continue ;

		if( pObjInfo->dwObjectIdx == 0 )
		{
			Throw_Error("Invalid object index!!", __FUNCTION__) ;
			return ;
		}

		g_pServerSystem->RemoveMonster( pObjInfo->dwObjectIdx ) ;

		m_mAddObj.erase( it ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_LoadSummonFlag
//	DESC		: Setting flag.
//	PROGRAMMER	: Yongs Lee
//	DATE		: September 01, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Set_LoadSummonFlag( void* pMsg )
{
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
		Throw_Error("Failed to convert a message!!", __FUNCTION__) ;
		return ;
	}


	// 맵 인덱스를 확인한다.
	if( g_pServerSystem->GetMapNum() != pmsg->wData ) return ;


	// 로드 완료 세팅을 한다.
	SIEGEWARFAREMGR->Set_LoadSummonFlag(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Throw_Error
//	DESC		: The function to process error message.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 28, 2008
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
//	DATE		: July 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Map%d-SiegeRecallMgr.log", g_pServerSystem->GetMapNum() ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime) ;
		fclose(fp) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: WriteCatapultLog
//	DESC		: The function to create a error log for catapult.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 28, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::WriteCatapultLog( char* pMsg )
{
	// 함수 인자 확인.
	if( !pMsg ) return ;


	// 로그를 남긴다.
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	TCHAR szFile[_MAX_PATH] = {0, } ;
	sprintf( szFile, "Log/Siege_Catapult_%d_%04d%02d%02d.log", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay ) ;

	FILE *fp = fopen(szFile, "a+") ;
	if (fp)
	{
		fprintf(fp, "[%s]\t%s", szTime, pMsg) ;
		fclose(fp) ;
	}
}

void CSiegeRecallMgr::EndSiegeWar_ClearRecallInfo()
{
	DWORD dwRemoveObjectID ;
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		dwRemoveObjectID = 0 ;
		//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
		dwRemoveObjectID =	it->second.Get_MonsterID() ;

		g_pServerSystem->RemoveMonster( dwRemoveObjectID ) ;
	}

	m_mAddObj.clear() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsRecallMonster
//	DESC		: 인자로 넘어온 아이디의 몬스터가 소환 된 몬스터인지 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 6, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeRecallMgr::IsRecallMonster(DWORD dwID) 
{
	st_SIEGEOBJ* pObjInfo ;
	M_MADDOBJ::iterator it ;

	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		pObjInfo = NULL ;
		pObjInfo = it->second.Get_ObjInfo() ; 

		if( !pObjInfo ) continue ;

		if( pObjInfo->dwObjectIdx == dwID ) return TRUE ;
	}

	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Set_RecallCount
//	DESC		: 소환해야 할 소환 카운트를 받는다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 7, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Set_RecallCount( void* pMsg )
{
	// 함수 인자 확인
	if( !pMsg ) return ;


	// 원본 메시지 전환.
	MSG_WORD2* pmsg = NULL ;
	pmsg = (MSG_WORD2*)pMsg ;

	if( !pmsg ) return ;


	// 맵 서버 확인
	if( pmsg->wData1 != g_pServerSystem->GetMapNum() ) return ;


	m_wRecallCount = pmsg->wData2 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Load_RecallCount
//	DESC		: 공성 소환물의 완전한 킬 상태를 로딩하여 정보가 넘어왔을 때 처리하는 작업 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 15, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Load_RecallCount( void* pMsg )
{
	// 함수 인자 확인
	if( !pMsg ) return ;


	// 원본 메시지 전환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	if( !pmsg ) return ;


	// 소환 몬스터를 완전히 제거 한 수를 받는다.
	m_wRecallCount = (WORD)pmsg->bData ;


	// 모든 몬스터를 다 제거했다면, 워터시드 사용 가능 상태로 세팅한다.
	if( (WORD)pmsg->bData >= m_wRecallCount )
	{
		m_byReadyToUseWaterSeed = TRUE ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsReadyToUseWaterSeed
//	DESC		: 워터시드 사용울 위한 준비가 되었는지 여부를 반환하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 8, 2008
//-------------------------------------------------------------------------------------------------
BYTE CSiegeRecallMgr::IsReadyToUseWaterSeed()
{
	if( m_byReadyToUseWaterSeed ) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: RequestKillCountToDB
//	DESC		: 소환 된 캐터펄트가 모두 삭제 되었는지 DB상에 확인 요청하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 22, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::RequestKillCountToDB()
{
	// 임시 버퍼를 선언한다.
	char txt[128] = {0, } ;

	sprintf(txt, "EXEC  %s %d",	MP_SIEGERECALL_KILLCOUNT, g_pServerSystem->GetMapNum()) ;

	// 쿼리문을 실행한다.
	g_DB.Query(eQueryType_FreeQuery, eRequuestKillCount, 0, txt) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: CheckKillCount
//	DESC		: 캐터펄트를 모두 잡았는지 결과 값을 받아서 설정하는 함수 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 8, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::CheckKillCount(int nKillCount)
{
	// 모든 몬스터를 다 제거했다면, 워터시드 사용 가능 상태로 세팅한다.
	if( nKillCount >= m_wRecallCount )
	{
		m_byReadyToUseWaterSeed = TRUE ;
	}
	else
	{
		m_byReadyToUseWaterSeed = FALSE ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: Cheat_ExcuteNextStep
//	DESC		: 공성 소환물의 소환 상태를 다음 단계로 넘기는 치트 관련 추가.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 11, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeRecallMgr::Cheat_ExcuteNextStep( void* pMsg )
{
	// 함수 인자 확인.
	if( !pMsg ) return ;


	// 원본 메시지 변환.
	MSG_SIEGERECALL_OBJINFO* pmsg = NULL ;
	pmsg = (MSG_SIEGERECALL_OBJINFO*)pMsg ;

	if( !pmsg ) return ;

	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() );
	if( !IsSiegeWarfareZone ) return ;

	// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		WriteCatapultLog( "Call - Cheat_ExcuteNextStep" ) ;
	}

	if( pmsg->siegeObj.mapNum != g_pServerSystem->GetMapNum() ) return ;

	// 081028 LYW --- SiegeRecallMgr : 몬스터 삭제 처리 함수가 호출되었는지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		char szCheckBefor[1024] = {0, } ;
		sprintf( szCheckBefor, "Befor Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckBefor ) ;
	}


	// 서버에 일치하는 정보가 있는지 확인하고, 있으면 삭제 처리한다.
	M_MADDOBJ::iterator it ;
	it = m_mAddObj.find( pmsg->siegeObj.byAddObjIdx ) ;

	if( it != m_mAddObj.end() )
	{
		// 081028 LYW --- SiegeRecallMgr : 킬러 아이디 로그를 남기기 위해 함수를 수정함.
		//Check_ObjectDie( it->second.Get_ObjInfo()->dwObjectIdx, TRUE ) ;
		//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
		Check_ObjectDie( NULL, it->second.Get_MonsterID(), TRUE ) ;
	}
	else
	{
		if(pmsg->siegeObj.byComKind == e_CK_ADD_OBJECT)
		{
			Remove_ObjFromDB( pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.mapNum, g_pServerSystem->GetMapNum(), 
			pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 2) ;

			// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pmsg->siegeObj.wThemeIdx, 
					pmsg->siegeObj.mapNum, g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, 
					pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 2, pmsg->siegeObj.byAddObjIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}
		else
		{
			Remove_ObjFromDB( pmsg->siegeObj.wThemeIdx, pmsg->siegeObj.wParentMap, g_pServerSystem->GetMapNum(), 
			pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 4) ;

			// 081028 LYW --- SiegeRecallMgr : 캐터펄트 삭제 성공 메시지 로그를 남긴다.
			if( IsSiegeWarfareZone )
			{
				char szLog[1024] = {0, } ;
				sprintf(szLog, "Delete Catapult - %d %d %d %d %d %d %d %d %u %s\n", pmsg->siegeObj.wThemeIdx, 
					pmsg->siegeObj.wParentMap, g_pServerSystem->GetMapNum(), pmsg->siegeObj.byStepIdx, pmsg->siegeObj.byAddObjIdx, 
					pmsg->siegeObj.byComKind, pmsg->siegeObj.byComIndex, 4, pmsg->siegeObj.byAddObjIdx, "Use Cheat Command!!" ) ;
				WriteCatapultLog( szLog ) ;
			}
		}

		if( pmsg->siegeObj.byComKind == e_CK_ADD_OBJECT )
		{
			MSG_WORD3 msg ;

			msg.Category	= MP_SIEGERECALL ;
			msg.Protocol	= MP_SIEGERECALL_NOTICE_DIERECALLOBJ_READY_SYN ;

			msg.dwObjectID	= 0 ;

			msg.wData1		= pmsg->siegeObj.mapNum ;
			msg.wData2		= pmsg->siegeObj.byStepIdx ;
			msg.wData3		= e_STEP_ADD_OBJ ;

			// Send message.
			g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) ) ;
		}

		// Delete obj info.
		m_mAddObj.erase( it ) ;
	}

	// 081028 LYW --- SiegeRecallMgr : 몬스터 삭제 처리 함수가 호출되었는지 로그를 남긴다.
	if( IsSiegeWarfareZone )
	{
		char szCheckAfter[1024] = {0, } ;
		sprintf( szCheckAfter, "After Count - %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szCheckAfter ) ;
	}
}


// 081012 LYW --- SiegeRecall : 소환물 초기화 함수 추가.
void CSiegeRecallMgr::Clear_RecallInfo()
{
	BYTE IsSiegeWarfareZone = BYTE( SIEGEWARFAREMGR->IsSiegeWarfareZone() );
	if( !IsSiegeWarfareZone ) return ;

	char szLog[1024] = {0, } ;

	// 081028 LYW --- SiegeRecallMgr : 
	if( IsSiegeWarfareZone )
	{
		sprintf( szLog, "Currnet Recall Count : %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szLog ) ;
	}

	M_MADDOBJ::iterator it ;
	
	for( it = m_mAddObj.begin() ; it != m_mAddObj.end() ; ++it )
	{
		//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
		g_pServerSystem->RemoveMonster( it->second.Get_MonsterID() ) ;

		// Delete obj info.
		m_mAddObj.erase( it ) ;
	}

	if( IsSiegeWarfareZone )
	{
		sprintf( szLog, "After clear recall info : %d\n", m_mAddObj.size() ) ;
		WriteCatapultLog( szLog ) ;
	}

	// 081226 LYW --- SiegeRecallMgr : 유니크 인덱스 추가 처리.
	m_byUniqueIdx = 0 ;
}