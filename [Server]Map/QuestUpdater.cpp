// QuestUpdater.cpp: implementation of the CQuestUpdater class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Player.h"
#include "QuestUpdater.h"
#include "MapDBMsgParser.h"
#include "Quest.h"
#include "QuestGroup.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\SubQuestInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestUpdater::CQuestUpdater()
{

}

CQuestUpdater::~CQuestUpdater()
{

}

void CQuestUpdater::StartQuest(CPlayer* player, CQuest* pQuest)
{	
	if( pQuest == NULL )	return;

	QuestMainQuestInsert( player->GetID(), pQuest->GetQuestIdx(), pQuest->GetSubQuestFlag(),
		pQuest->GetQuestRegistTime(), pQuest->IsRepeat() );

	// 071129 웅주
	InsertLogQuest(
		player,
		pQuest->GetQuestIdx(),
		0,
		0,
		0,
		eQuestLog_Main_Begin );
}

void CQuestUpdater::EndQuest(CPlayer* player, CQuest* pQuest)
{	
	// 071117 LYW --- QuestUpdate : DB 업데이트 수정.
	ASSERT( player ) ;
	ASSERT( pQuest ) ;

	EndQuest_New
	(
		player->GetID()
	,	pQuest->GetQuestIdx()
	,	pQuest->GetSubQuestFlag()
	,	1
	,	pQuest->GetQuestRegistTime()
	) ;

	// 071129 웅주
	InsertLogQuest(
		player,
		pQuest->GetQuestIdx(),
		0,
		0,
		0,
		eQuestLog_Main_End );

	//if( pQuest == NULL )	return;
	//// 확실하게 끝났다고 1로 셋팅. - RaMa 04.10.14
	//// pQuest->IsQuestComplete() 이거 좀 이상한거 같음..	
	//QuestMainQuestUpdateToDB( player->GetID(), pQuest->GetQuestIdx(), pQuest->GetSubQuestFlag(),		
	//						  1, pQuest->GetQuestTime() );
	//QuestSubQuestDeleteAll( player->GetID(), pQuest->GetQuestIdx() );
	//QuestItemDeleteAll( player->GetID(), pQuest->GetQuestIdx() );
}

void CQuestUpdater::DeleteQuest(CPlayer* player, CQuest* pQuest, DWORD param)
{
	// 071117 LYW --- QuestUpdate : DB 업데이트 수정.
	ASSERT( player ) ;
	ASSERT( pQuest ) ;

	if( !player || !pQuest )
		return;

	DeleteQuest_New( player->GetID(), pQuest->GetQuestIdx(), param ) ;

	// 071129 웅주
	InsertLogQuest(
		player,
		pQuest->GetQuestIdx(),
		0,
		0,
		0,
		eQuestLog_Main_Delete );

	/*if( pQuest == NULL )	return;
	QuestMainQuestDelete( player->GetID(), pQuest->GetQuestIdx() );
	QuestSubQuestDeleteAll( player->GetID(), pQuest->GetQuestIdx() );
	QuestItemDeleteAll( player->GetID(), pQuest->GetQuestIdx() );*/
}

void CQuestUpdater::UpdateQuest( CPlayer* player, DWORD dwFlag, DWORD dwParam, __time64_t registTime )
{
	QuestMainQuestUpdateToDB( player->GetID(), 0, dwFlag, dwParam, registTime );
}

void CQuestUpdater::StartSubQuest(CPlayer* player, CQuest* pQuest, DWORD sIndex)
{
	if( pQuest == NULL )	return;
	QuestSubQuestInsert( player->GetID(), pQuest->GetQuestIdx(), sIndex,
						 pQuest->GetSubQuestData(sIndex), pQuest->GetSubQuestTime(sIndex) );

	// 071129 웅주
	InsertLogQuest(
		player,
		pQuest->GetQuestIdx(),
		sIndex,
		0,
		0,
		eQuestLog_Sub_Begin );
}

void CQuestUpdater::EndSubQuest(CPlayer* player, CQuest* pQuest, DWORD sIndex)
{	
	// 071117 LYW --- QuestUpdate : DB 업데이트 수정.
	ASSERT( player ) ;
	ASSERT( pQuest ) ;

	EndSubQuest_New
	(
		player->GetID()
	,	pQuest->GetQuestIdx()
	,	sIndex
	,	pQuest->GetSubQuestFlag()
	,	pQuest->GetQuestRegistTime()
	) ;


	// 071129 웅주
	InsertLogQuest(
		player,
		pQuest->GetQuestIdx(),
		sIndex,
		0,
		0,
		eQuestLog_Sub_End );

	/*if( pQuest == NULL ) return;

	SYSTEMTIME time;
	char szFile[256] = {0,};
	GetLocalTime( &time );

	sprintf( szFile, "./Log/SUB_QUEST_END%02d_%4d%02d%02d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay ) ;

	FILE* fp = fopen(szFile,"a+") ;

	fprintf(fp,"%d\t%d\t%d\n", pQuest->GetQuestIdx(), sIndex, pQuest->IsQuestComplete() ) ;

	fclose(fp) ;

	QuestMainQuestUpdateToDB( player->GetID(), pQuest->GetQuestIdx(), pQuest->GetSubQuestFlag(),
							  pQuest->IsQuestComplete(), pQuest->GetQuestTime() );
	QuestSubQuestDelete(player->GetID(), pQuest->GetQuestIdx(), sIndex);*/
}

void CQuestUpdater::UpdateSubQuest(CPlayer* player, CQuest* pQuest, DWORD sIndex)
{
	if( pQuest == NULL )	return;
	QuestSubQuestUpdateToDB( player->GetID(), pQuest->GetQuestIdx(), sIndex, 
							 pQuest->GetSubQuestData(sIndex), pQuest->GetSubQuestTime(sIndex) );
}

void CQuestUpdater::GiveQuestItem( CPlayer* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	QuestItemDelete( pPlayer->GetID(), dwItemIdx );

	InsertLogQuest(
		pPlayer,
		dwQuestIdx,
		0,
		dwItemIdx,
		dwItemNum,
		eQuestLOg_ItemGive );
}

void CQuestUpdater::TakeQuestItem( CPlayer* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	QuestItemInsert( pPlayer->GetID(), dwQuestIdx, dwItemIdx, dwItemNum );

	// 071129 웅주
	InsertLogQuest(
		pPlayer,
		dwQuestIdx,
		0,
		dwItemIdx,
		dwItemNum,
		eQuestLOg_ItemTake );
}

void CQuestUpdater::UpdateQuestItem( CPlayer* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	QuestItemUpdate( pPlayer->GetID(), dwQuestIdx, dwItemIdx, dwItemNum );

	// 071129 웅주
	InsertLogQuest(
		pPlayer,
		dwQuestIdx,
		0,
		dwItemIdx,
		dwItemNum,
		eQuestLOg_ItemUpdate);
}

void CQuestUpdater::UpdateQuestCheckTime( CPlayer* player, DWORD dwQuestIdx, DWORD dwCheckType, DWORD dwCheckTime )
{
	QuestMainQuestUpdateCheckTime( player->GetID(), dwQuestIdx, dwCheckType, dwCheckTime );
}
