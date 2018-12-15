
#ifndef __AGENTDBMSGPARSER_H__
#define __AGENTDBMSGPARSER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "db.h"

class CSHFamily;

enum BasicCharacterInfo
{
	eCL_ObjectID = 0, eCL_StandIndex, eCL_ObjectName, eCL_BodyType, eCL_HeadType, 
	eCL_Glove, eCL_Dress, eCL_shoes, eCL_Weapon, 
	eCL_Grade, eCL_Map, eCL_Gender, eCL_Race, 
	eCL_JobGrade, eCL_Job1, eCL_Job2, eCL_Job3, eCL_Job4, eCL_Job5, eCL_Job6, 
	eCL_GuildName, eCL_AuthKey,
};

enum Friendinfo
{
	eFr_TargetID=0, eFr_FromID, eFr_ToPlayerName, eFr_FromPlayerName, eFr_Err, 
};
enum Friendisvalidtarget
{
	eFr_vtFromid , eFr_vtTargetid , eFr_vtFromname , eFr_vtToname, eFr_vtvailderr, 
};
enum Frienddel
{
	eFr_targetname, eFr_IsSuccess, 
};
enum Friendadd 
{
	eFr_addToErr, eFr_addToIDX, eFr_addToName, eFr_addFromErr, eFr_addFromIDX, eFr_addFromName, 
};

enum Friendlogin
{
	eFr_LTargetID, eFr_LLoggedname,
};
enum Friendloginedfriend
{
	eFr_LFFriendID, eFr_LFFriendName,
};
enum 
{
	eFr_FLFriendid, eFr_FLFriendname, eFr_FLIsLogin, eFr_FLTotalPage,
};
enum Friendsendnote
{
	eFr_NErr, eFr_NToName, eFr_NToId,
};
enum FriendNotelist
{
	eFr_NNoteID, eFr_NSender, eFr_NSentDate, eFr_NSentTime, eFr_NTotalpage, eFr_NbIsRead, eFr_NTitle, eFr_NPackageItemIdx, eFr_NPackageMoney, eFr_NTotalmsg,
};
enum FriendReadnote
{
	eFr_NRNNoteID, eFr_NRNSender, eFr_NRNNote, eFr_NRNItemIdx, eFr_NRNPackageItem, eFr_NRNPackageMoney, eFr_NRNPackageItemInfo,
};
enum FriendDeletenote
{
	eFr_NdNoteID, eFr_NdbLast,
};

enum SiegeRecallLoad_ObjInfo
{
	eSRLoad_ThemeIdx = 0,
	eSRLoad_MapIdx,
	eSRLoad_RecallMap,
	eSRLoad_StepIdx,
	eSRLoad_AddObjIdx,
	eSRLoad_ComKind,
	eSRLoad_ComIndex,
	eSRLoad_RecallStep,
	eSRLoad_RequestMap,
};

enum 
{
	eCharacterBaseQuery = 1,		/// 횆횋쨍짱횇횒 짹창쨘쨩횁짚쨘쨍쨍짝 쩐챵쩐챤쩔횂쨈횢
	eCreateCharacter,			/// 횆쨀쨍짱쨩첵쩌쨘
	eLoginCheckDelete,			/// 횁짖쩌횙횄쩌횇짤쨩챔횁짝
	eDeleteCharacter,			/// 횆쨀쨍짱횇횒 쨩챔횁짝
	eNewCharacterCheckName,     /// 쨩천쨌횓쩔챤 횆쨀쨍짱횇횒 ?횑쨍짠 횁횩쨘쨔횄쩌횇짤 
	eForcedDisconnect,          /// 째짯횁짝쨌횓 짹창횁쨍?짱?첬쨍짝 짼첨?쩍.
	eSetCurMaxCount,			/// 횉철?챌?짱?첬쩔횒 Max?짱?첬쩌철 짹창쨌횕
	eLoginMapInfoSearchForName, /// 횆쨀쨍짱쨍챠?쨍쨌횓 횁짖쩌횙쩔짤쨘횓쨔횞 쨍횎쨔첩횊짙 쩐챵쩐챤쩔?짹창
	eSavePoint,					/// SavePoint 
	eUnRegistLoginMapInfo,		/// 쨍횎쩔징쩌짯 쨀짧째징쨍챕 쩍횉횉횪횉횗쨈횢
	eMapUserUnRegistMapInfo,	/// 쨍횎쩌짯쨔철 횁쩐쨌찼쩍횄 쩍횉횉횪
	eFriendGetTargetMemeberIDX,	/// 횆짙짹쨍 쨉챤쨌횕/쨩챔횁짝쩍횄 횇쨍째횢 횉횄쨌쨔?횑쩐챤 쩐횈?횑쨉챨 쩐챵쩐챤쩔횒쩌짯 쨍횧쩍횄횁철 
	eFriendAdd,					/// 횆짙짹쨍 쨉챤쨌횕
	eFriendIsValidTarget,		/// 횆짙짹쨍 쨉챤쨌횕 횉횘쩌철 ?횜쨈횂횁철

	eFriendDel,					/// 횆짙짹쨍 쨩챔횁짝
	eFriendDelID,				/// 횆짙짹쨍 쨩챔횁짝 (쩐횈?횑쨉챨쨌횓)
	eFriendDelList,				/// 횆짙짹쨍쨍짰쩍쨘횈짰 쨩챔횁짝(횆쨀쨍짱횇횒 쨩챔횁짝쩍횄)
	eFriendNotifyLogin,			/// 쨌횓짹횞?횓 쩐횏쨍짼
	eFriendGetFriendList,		/// 횆짙짹쨍쨍짰쩍쨘횈짰
	eFriendGetLoginFriends,		/// 쨌횓짹횞?횓쨉횉쩐챤?횜쨈횂 횆짙짹쨍 쩐횏쩐횈쩔?짹창
	eNoteIsNewNote,				
	eNoteDelAll,				/// 횂횎횁철 쨍챨쨉횓 쨩챔횁짝
	eNoteDelete,				/// 횂횎횁철 쨩챔횁짝
	eNoteSave,
//For GM-Tool	
	eGM_BanCharacter,
	eGM_UpdateUserLevel,
	eGM_WhereIsCharacter,
	eGM_Login,
	eGM_GetGMPowerList,
//	
	eAgentDown,					/// 쩔징?횑?체횈짰 횇째쨍챕 횉횠쨈챌 쩔징?횑?체횈짰쩔징 쩌횙횉횗 쩐횜쨉챕 쨌횓짹횞 쩐횈쩔척 횄쨀쨍짰
	eCheckGuildMasterLogin,		// checkguildmasterlogin
	eCheckGuildFieldWarMoney,	// check guildfieldwarmoney
	eAddGuildFieldWarMoney,		// add guildfieldwarmoney
	eEventItemUse051108,			// 051108 event
	eEventItemUse2,					

	eGM_UpdateUserState,
	eLogGMToolUse,					// 
	eNProtectBlock,


	eItemInsert,
	eSkillInsert,

	// desc_hseos_紐ъ뒪?곕???1
	// S 紐ъ뒪?곕???異붽? added by hseos 2007.05.29
	eInitMonstermeterInfoOfDB,
	// E 紐ъ뒪?곕???異붽? added by hseos 2007.05.29

	// desc_hseos_?깅퀎?좏깮01
	// S ?깅퀎?좏깮 異붽? added by hseos 2007.06.15
	eGetUserSexKind,				// RGetUserSexKind
	// E ?깅퀎?좏깮 異붽? added by hseos 2007.06.15

	// desc_hseos_?⑤?由?1
	// S ?⑤?由?異붽? added by hseos 2007.07.09	2007.07.14
	eFamily_LoadInfo,
	eFamily_SaveInfo,
	eFamily_Member_LoadInfo,
	eFamily_Member_LoadExInfo01,
	eFamily_Member_SaveInfo,
	eFamily_CheckName,
	eFamily_Leave_LoadInfo,
	eFamily_Leave_SaveInfo,
	eFamily_Delete_Emblem,
	eFarm_SendNote_DelFarm,
	// E ?띿옣?쒖뒪??異붽? added by hseos 2008.01.22

//---KES PUNISH
	ePunishList_Add,	//---?쒖옱紐⑸줉

	ePunishList_Load,	//---?쒖옱紐⑸줉


	ePunishCount_Add,	//---嫄몃┛ ?뚯닔 異붽?
//-------------

	eGiftEvent,

	// 080812 LYW --- AgentDBMsgParser : 소환물 db 업데이트 처리 추가.
	eSiegeRecallUpdate,

	// 080813 LYW --- AgentDBMsgParser : 소환물 db 정보 로드 처리 추가.
	eSiegeRecallLoad,

	// 080831 LYW --- AgentDBMsgParser : 소환물 db 정보 추가 처리 추가.
	eSiegeRecallInsert,

	// 080831 LYW --- AgentDBMsgParser : 소환물 db 정보 삭제 처리 추가.
	eSiegeRecallRemove,

	// 090525 ShinJS --- 이름으로 파티초대시 DB 검색 처리 추가
	eSearchCharacterForInvitePartyMember,


	MaxQuery,
};

void CheatLog(DWORD CharacterIDX,DWORD CheatKind);
void UserIDXSendAndCharacterBaseInfo(DWORD UserIDX, DWORD AuthKey, DWORD dwConnectionIndex);
void LoginCheckDelete(DWORD UserID);//, DWORD dwConnectionIndex);
//void CreateCharacter(CHARACTERMAKEINFO* pMChar, DWORD dwConnectionIndex);
void CreateCharacter(CHARACTERMAKEINFO* pMChar, WORD ServerNo, DWORD dwConnectionIndex);

//void DeleteCharacter(DWORD dwPlayerID, DWORD dwConnectionIndex);
void DeleteCharacter(DWORD dwPlayerID, WORD ServerNo, DWORD dwConnectionIndex);

void CharacterNameCheck(char* pName, DWORD dwConnectionIndex);
void SearchWhisperUserAndSend(DWORD dwPlayerID, char* CharacterName, DWORD dwKey );
void SearchPartyChatSendOnPlayer(DWORD dwPlayerID, DWORD dwKey );
//void SearchMunpaChatSendOnPlayer(DWORD dwPlayerID, DWORD dwKey );
void SaveMapChangePointUpdate(DWORD CharacterIDX, WORD MapChangePoint_Idx);
void LoadCharacterMap(DWORD CharacterIDX);
void RLoadCharacterMap(LPMIDDLEQUERY pData, LPDBMESSAGE pMessage );
void UnRegistLoginMapInfo(DWORD CharacterIDX);
void FriendGetUserIDXbyName(DWORD CharacterIDX, char* TargetName);
// 100305 ONS 허락여부를 묻지않고 친구를 추가하도록 수정.
void FriendAddFriendByName(DWORD CharacterIDX, char* TargetName);
void FriendAddFriend(DWORD CharacterIDX, DWORD TargetID);
void FriendIsValidTarget(DWORD CharacterIDX, DWORD TargetID, char* FromName);
void FriendDelFriend(DWORD CharacterIDX, char* TargetName);
void FriendDelFriendID(DWORD CharacterIDX, DWORD TargetID, DWORD bLast);
void FriendNotifyLogintoClient(DWORD CharacterIDX);
void FriendGetLoginFriends(DWORD CharacterIDX);
void FriendGetFriendList(DWORD CharacterIDX);
void NoteIsNewNote(DWORD PlayerID);
void NoteSendtoPlayer(DWORD FromIDX, char* FromName, char* ToName, char* Title, char* Note);
void NoteServerSendtoPlayer(DWORD FromIDX, char* FromName, char* ToName, char* Title, char* Note);
void NoteSendtoPlayerID(DWORD FromIDX, char* FromName, DWORD ToIDX, char* Note);
void NoteDelAll(DWORD CharacterIDX);
void NoteList(DWORD CharacterIDX, WORD Page, WORD Mode);
void NoteRead(DWORD CharacterIDX, DWORD NoteIDX, DWORD IsFront);
void NoteDelete(DWORD PlayerID, DWORD NoteID, BOOL bLast);
void NoteSave(DWORD PlayerID, DWORD NoteID, BOOL bLast);
//For GM-Tool
void GM_WhereIsCharacter(DWORD dwID, char* CharacterName, DWORD dwSeacherID );
void GM_BanCharacter(DWORD dwID, char* CharacterName, DWORD dwSeacherID );
void GM_UpdateUserLevel(DWORD dwID, DWORD dwServerGroup, char* Charactername, BYTE UserLevel);
void GM_GetGMPowerList(DWORD dwStartIdx, DWORD dwFlag = 0 );
void GM_Login( DWORD dwConnectionIdx, char* strID, char* strPW, char* strIP );

void RUserIDXSendAndCharacterBaseInfo(LPQUERY pData, LPDBMESSAGE pMessage);
void RCreateCharacter(LPQUERY pData, LPDBMESSAGE pMessage);
void RLoginCheck(LPQUERY pData, LPDBMESSAGE pMessage);
void RCharacterNameCheck(LPQUERY pData, LPDBMESSAGE pMessage);
void RSearchWhisperUserAndSend(LPQUERY pData, LPDBMESSAGE pMessage);
//void RSearchMunpaChatSendOnPlayer(LPQUERY pData, LPDBMESSAGE pMessage);
void RFriendDelFriend(LPQUERY pData, LPDBMESSAGE pMessage);
void RFriendDelFriendID(LPQUERY pData, LPDBMESSAGE pMessage);
void RFriendGetFriendList(LPQUERY pData, LPDBMESSAGE pMessage);
void RFriendAddFriend(LPQUERY pData, LPDBMESSAGE pMessage);
void RFriendNotifyLogintoClient(LPQUERY pData, LPDBMESSAGE pMessage);
void RNoteIsNewNote(LPQUERY pData, LPDBMESSAGE pMessage);
void RNoteSendtoPlayer(LPLARGEQUERY pData, LPDBMESSAGE pMessage);
void RNoteServerSendtoPlayer(LPLARGEQUERY pData, LPDBMESSAGE pMessage);
void RNoteList(LPMIDDLEQUERY pData, LPDBMESSAGE pMessage );
void RNoteRead(LPLARGEQUERY pData, LPDBMESSAGE pMessage);
void RNoteDelete(LPQUERY pData, LPDBMESSAGE pMessage);
void RNoteSave(LPQUERY pData, LPDBMESSAGE pMessage);
void RDeleteCharacter(LPQUERY pData, LPDBMESSAGE pMessage);
void RFriendGetLoginFriends(LPQUERY pData, LPDBMESSAGE pMessage);
void RFriendIsValidTarget(LPQUERY pData, LPDBMESSAGE pMessage);
//For GM-Tool
void RGM_WhereIsCharacter(LPQUERY pData, LPDBMESSAGE pMessage);
void RGM_BanCharacter(LPQUERY pData, LPDBMESSAGE pMessage);
void RGM_UpdateUserLevel(LPQUERY pData, LPDBMESSAGE pMessage);
void RGM_GetGMPowerList(LPQUERY pData, LPDBMESSAGE pMessage);
void RGM_Login(LPQUERY pData, LPDBMESSAGE pMessage);
//

// guildfieldwar
void CheckGuildMasterLogin( DWORD dwConnectionIdx, DWORD dwPlayerIdx, char* pSearchName, DWORD dwMoney, BYTE Protocol );
void RCheckGuildMasterLogin( LPQUERY pData, LPDBMESSAGE pMessage );
void CheckGuildFieldWarMoney( DWORD dwConnectionIndex, DWORD dwSenderIdx, DWORD dwEnemyGuildIdx, DWORD dwMoney );
void RCheckGuildFieldWarMoney( LPQUERY pData, LPDBMESSAGE pMessage );
void AddGuildFieldWarMoney( DWORD dwConnectionIndex, DWORD dwPlayerIdx, DWORD dwMoney );
void RAddGuildFieldWarMoney( LPQUERY pData, LPDBMESSAGE pMessage );
// 051108 event
void EventItemUse051108( DWORD dwUserIdx, char* sCharName, DWORD dwServerNum );
void EventItemUse2( DWORD dwUserIdx, char* sCharName, DWORD dwServerNum );
void GM_UpdateUserLevel(DWORD dwID, DWORD dwServerGroup, char* Charactername, BYTE UserState);
void RGM_UpdateUserState(LPQUERY pData, LPDBMESSAGE pMessage);


// GMlog
void LogGMToolUse( DWORD CharacterIdx, DWORD GMLogtype, DWORD Logkind, DWORD Param1, DWORD Param2 );
void InitMonstermeterInfoOfDB(DWORD nPlayerID);
void GetUserSexKind(DWORD nUserID);
void RGetUserSexKind(LPQUERY pData, LPDBMESSAGE pMessage);

void Family_SaveInfo(DWORD nMasterID, char* pszFamilyName, int nHonorPoint, BOOL bNicknameON, int nSaveType = 0);
void Family_SaveEmblemInfo(DWORD nPlayerID, DWORD nFamily, char* pcImg);
void RFamily_SaveInfo(LPQUERY pData, LPDBMESSAGE pMessage);
void Family_LoadInfo(DWORD nPlayerID);
void RFamily_LoadInfo(LPQUERY pData, LPDBMESSAGE pMessage);
void Family_LoadEmblemInfo(DWORD nPlayerID, DWORD nFamilyID);
void RFamily_LoadEmblemInfo(LPLARGEQUERY pData, LPDBMESSAGE pMessage);
void Family_Member_SaveInfo(DWORD nFamilyID, DWORD nMemberID, char* pszMemberNickname);
void Family_Member_LoadInfo(DWORD nPlayerID, DWORD nFamilyID);
void Family_Member_LoadExInfo01(DWORD nPlayerID, DWORD nMemberID);
void RFamily_Member_LoadInfo(LPQUERY pData, LPDBMESSAGE pMessage);
void RFamily_Member_LoadExInfo01(LPQUERY pData, LPDBMESSAGE pMessage);
void Family_CheckName(DWORD nPlayerID, char* pszName);
void RFamily_CheckName(LPQUERY pData, LPDBMESSAGE pMessage);
void Family_Leave_SaveInfo(DWORD nPlayerID, int nLeaveKind, BOOL bInit, BOOL bBreakUp = FALSE);
void Family_Leave_LoadInfo(DWORD nPlayerID);
void RFamily_Leave_LoadInfo(LPQUERY pData, LPDBMESSAGE pMessage);
// 091126 ONS 패밀리장 이양 기능 추가
void Family_ChangeMaster( DWORD dwMaster, DWORD dwNewMaster, DWORD dwFamilyID);
void RFamily_ChangeMaster(LPMIDDLEQUERY, LPDBMESSAGE);
void Family_DeleteEmblem(DWORD nPlayerID, DWORD nFamilyID);
void Farm_SendNote_DelFarm(DWORD nFarmOwnerID);
void RFarm_SendNote_DelFarm(LPQUERY pData, LPDBMESSAGE pMessage);
// 081205 LUJ, 패밀리 로그
void InsertLogFamily( eFamilyLog, DWORD familyIndex, DWORD playerIndex, const char* memo = "" );
void InsertLogFamilyPoint( CSHFamily*, eFamilyLog );

//---KES PUNISH
void PunishListAdd( DWORD dwUserIdx, int nPunishKind, DWORD dwPunishTime );
void PunishListLoad( DWORD dwUserIdx );
void RPunishListLoad( LPQUERY pData, LPDBMESSAGE pMessage );

void PunishCountAdd( DWORD dwUserIdx, int nPunishKind, int nPeriodDay );
void RPunishCountAdd( LPQUERY pData, LPDBMESSAGE pMessage );
//-------------

void TransToClientMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void TransToMapServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

#ifdef _NPROTECT_
void NProtectBlock(DWORD UserIdx, DWORD CharIdx, char* IP, DWORD BlockType);
#endif

void GiftEvent( DWORD dwUserIdx );
void RGiftEvent( LPQUERY pData, LPDBMESSAGE pMessage );

void RSiegeRecallUpdate( LPQUERY pData, LPDBMESSAGE pMessage );
void RSiegeRecallLoad( LPQUERY pData, LPDBMESSAGE pMessage );
void RSiegeRecallInsert(LPQUERY pData, LPDBMESSAGE pMessage );

// 090525 ShinJS --- 이름 검색으로 파티초대 추가
void SearchCharacterForInvitePartyMember( DWORD dwRequestPlayerID, char* pInvitedName );
void RSearchCharacterForInvitePartyMember( LPQUERY pData, LPDBMESSAGE pMessage );

// 웹이벤트
void WebEvent( DWORD dwUserIdx, DWORD type );

#endif //__AGENTBMSGPARSER_H__
