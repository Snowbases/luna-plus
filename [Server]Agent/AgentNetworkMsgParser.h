
#ifndef __AGENTNETWORKMSGPARSER_H__
#define __AGENTNETWORKMSGPARSER_H__

//---KES 해킹 사기 방지
void MP_TEMPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
//---------------------

void MP_POWERUPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_AGENTSERVERMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_USERCONNMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_USERCONNUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_USERCONNServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHATMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHATServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

void TransToClientMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void TransToMapServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_PACKEDMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_PARTYServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_FRIENDMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_NOTEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_NOTEServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_MonitorMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHEATUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHEATServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_HACKCHECKMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GUILDServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GUILDUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GUILD_WARServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GUILD_WARUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

BOOL CheckCharacterMakeInfo( CHARACTERMAKEINFO* pmsg );

//
void MP_ITEMUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_ITEMServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

void MP_GTOURNAMENTUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GTOURNAMENTServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

void MP_SkillServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength );

// guildunion
void MP_GUILD_UNIONUserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength );
void MP_GUILD_UNIONServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength );
// 080805 KTH SiegeWarfare
void MP_SIEGEWARFAREUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_SIEGEWARFAREServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// S 주민등록 추가 added by hseos 2007.06.07	2007.06.10
void MP_RESIDENTREGIST_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_RESIDENTREGIST_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// E 주민등록 추가 added by hseos 2007.06.07	2007.06.10

// desc_hseos_패밀리01
// S 패밀리 추가 added by hseos 2007.07.09	2007.07.11
void MP_FAMILY_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_FAMILY_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// E 패밀리 추가 added by hseos 2007.07.09	2007.07.11

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.09.10
void MP_FARM_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_FARM_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// E 농장시스템 추가 added by hseos 2007.09.10

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.16
void MP_DATE_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_DATE_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
//---KES AUTONOTE
void MP_AUTONOTE_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_AUTONOTE_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
//---------------

//---KES PUNISH
void MP_PUNISH_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
//-------------

// 080407 LYW --- AgentNetworkMsgParser : Add a user message parser for chatroom system.
void MP_CHATROOM_UserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 080407 LYW --- AgentNetworkMsgParser : Add a server message parser for chatroom system.
void MP_CHATROOM_ServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 080327 NYJ --- 낚시시스템
void MP_FISHING_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_FISHING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// 081119 NYJ --- 요리
void MP_COOKING_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_COOKING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// 090402 NYJ --- 하우징
void MP_HOUSING_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_HOUSING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// 090922 NYJ --- 인던
void MP_DUNGEON_UserMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_SIEGERECALL_ServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 080728 LYW --- AgentNetworkMsgParser : 공성전 소환물 관련 유저 메시지 파싱을 하는 함수 추가.
void MP_SIEGERECALL_UserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 080819 LYW --- AgentNetworkMsgParser : 클라이언트로 부터의 npc 소환 메시지를 파싱하는 함수 추가.
void MP_RECALLNPC_UserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 080819 LYW --- AgentNetworkMsgParser : npc 소환 관련 서버 메시지를 파싱하는 함수 추가.
void MP_RECALLNPC_ServerMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
#ifdef _HACK_SHIELD_
void MP_HACKSHIELDUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
#endif

#ifdef _NPROTECT_
void MP_NPROTECTUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
#endif

// 090526 ShinJS --- 파티 메세지 함수 추가
void MP_PARTYUserMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// 070205 LYW --- Add functions to process network msg.
/// MONITORMSGPARSER PART
void MornitorMapServer_NoticeSend_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void MornitorMapServer_Ping_Syn(MSGROOT* pTempMsg, DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void MornitorMapServer_QueryUserCount_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void MornitorMapServer_AssertMsgBox_Syn(char* pMsg) ;
void MornitorMapServer_Disconnect_AllUser_Syn(DWORD dwConnectionIndex) ;
void MornitorMapServer_ServerOff_Syn() ;
void MornitorMapServer_Connected_ServerList_Syn(DWORD dwConnectionIndex) ;
void MornitorMapServer_EventNotice_On(char* pMsg) ;
void MornitorMapServer_EventNotice_Off(char* pMsg) ;

///	USERCONNMSGPARSER PART
void UserConn_Notify_UserLogin_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_NotifyToAgent_AlreadyOut(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_Notify_OverlappedLogin(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_Force_Disconnect_OverlapLogin(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_Disconnected_On_Login(char* pMsg) ;
void UserConn_Disconnect_Syn(DWORD dwConnectionIndex) ;
void UserConn_Connection_Check_Ok(DWORD dwConnectionIndex) ;
void UserConn_Characterlist_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_DirectCharacterList_Syn(DWORD dwConnectionIndex) ;
void UserConn_CharacterSelect_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_ChannelInfo_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_ChannelInfo_Ack(char* pMsg, DWORD dwLength) ;
void UserConn_ChannelInfo_Nack(char* pMsg, DWORD dwLength) ;
void UserConn_Gamein_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_Gamein_Nack(char* pMsg) ;
void UserConn_Gamein_Ack(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void UserConn_Character_Make_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_Character_NameCheck_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_Character_Remove_Syn(DWORD dwConnectionIndex, char* pMsg) ;
// desc_hseos_결혼_01
// S 결혼 추가 added by hseos 2008.01.30
// ..이동 좌표 추가
// void UserConn_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void UserConn_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength, VECTOR3* pPos = NULL) ;
// 080930 LYW --- AgentNetworkMsgParser : 페이드 인/아웃 맵이동 추가.
void UserConn_FadeInOut_ChangeMap_Syn( DWORD dwConnectionIndex, char* pMsg ) ;
// E 결혼 추가 added by hseos 2008.01.30
// 070917 LYW --- AgentNetworkMsgParser : Add function to process change map through the npc.
void UserConn_Npc_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void UserConn_Map_Out(char* pMsg) ;
void UserConn_Map_Out_WithMapNum(char* pMsg) ;
// 081010 LYW --- AgentNetworkMsgParser : 공성전에서 사용할 맵아웃 함수 추가.
void SiegeWarfare_Map_Out_WithMapNum(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
// 070917 LYW --- AgentNetworkMsgParser : Add function to process map out through the npc.
void UserConn_Npc_Map_Out(char* pMsg) ;
void UserConn_ChangeMap_Ack(MSGROOT* pTempMsg, DWORD dwLength) ;
void UserConn_SavePoint_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void UserConn_BackToCharSel_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void UserConn_BackToCharSel_Ack(char* pMsg) ;
void UserConn_Cheat_Using(char* pMsg) ;
void UserConn_LoginCheck_Delete(char* pMsg) ;
void UserConn_GameInPos_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_EventItem_Use(char* pMsg, DWORD dwLength) ;
void UserConn_EventItem_Use2(char* pMsg, DWORD dwLength) ;
void UserConn_Return_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void UserConn_Option_Syn(DWORD dwConnectionIndex, char*, DWORD dwLength);
void UserConn_HouseEntrance_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void UserConn_DungeonEntrace_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

void Chat_Whisper_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void Chat_Whisper_Gm_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void Chat_Whisper_Ack(char* pMsg, DWORD dwLength) ;
void Chat_Whisper_Nack(char* pMsg, DWORD dwLength) ;
void Chat_Party(char* pMsg) ;
void Chat_Guild(char* pMsg, DWORD dwLength) ;
// 070601 LYW --- Add function for famil chatting.
void Chat_Family( char* pMsg, DWORD dwLength ) ;
void Chat_GuildUnion(char* pMsg, DWORD dwLength) ;
void Chat_Shout_Ack(char* pMsg);
void chatmsg_FindUser(DWORD dwConnectionIndex, char*pMsg, DWORD dwLength) ;

void chatmsg_whisper_syn(DWORD dwConnectionIndex, char* pMsg) ;
void chatmsg_party(char* pMsg) ;
void chatmsg_guild(char* pMsg, DWORD dwLength) ;
// 070601 LYW --- AgentNetMsgParser : Add function for family chatting.
void chatmsg_family(char* pMsg, DWORD dwLength) ;
void chatmsg_guildunion(char* pMsg, DWORD dwLength) ;

/// FRIENDMSGPARSER	PART
void Friend_Login(MSGBASE* pTempMsg) ;
void Friend_Add_Syn(char* pMsg) ;
void Friend_Add_Accept(char* pMsg) ;
void Friend_Add_Deny(char* pMsg) ;
void Friend_Del_Syn(char* pMsg) ;
void Friend_Delid_Syn(char* pMsg) ;
void Friend_Addid_Syn(char* pMsg) ;
void Friend_Logout_Notify_To_Agent(char* pMsg) ;
void Friend_Logout_Notify_Agent_To_Agent(char* pMsg) ;
void Friend_List_Syn(char* pMsg) ;
void Friend_Add_Ack_To_Agent(char* pMsg) ;
void Friend_Add_Nack_To_Agent(char* pMsg) ;
void Friend_Add_Accept_To_Agent(char* pMsg) ;
void Friend_Add_Accept_Nack_To_Agent(char* pMsg) ;
void Friend_Login_Notify_To_Agent(char* pMsg) ;
void Friend_Add_Invite_To_Agent(char* pMsg) ;
void Friend_Add_Nack(char* pMsg) ;

/// NOTEMSGPARSER PART
void Note_SendNote_Syn(char* pMsg) ;
void Note_SendNoteId_Syn(char* pMsg) ;
void Note_ReceiveNote(char* pMsg) ;
void Note_DelAllNote_Syn(char* pMsg) ;
void Note_NoteList_Syn(char* pMsg) ;
void Note_ReadNote_Syn(char* pMsg) ;
void Note_DelNote_Syn(char* pMsg) ;
void Note_SaveNote_Syn(char* pMsg) ;
void Cheat_Gm_Login_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void Cheat_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_BanCharacter_Syn(char* pMsg) ;
void Cheat_BlockCharacter_Syn(USERINFO* pUserInfo, DWORD dwConnectionIndex, char* pMsg) ;
void Cheat_WhereIs_Syn(char* pMsg) ;
void Cheat_Event_Monster_Regen(USERINFO* pUserInfo, DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_Event_Monster_Delete(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_BanMap_Syn(char* pMsg) ;
void Cheat_AgentCheck_Syn(char* pMsg, DWORD dwLength) ;
void Cheat_PkAllow_Syn(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_Notice_Syn(char* pMsg, DWORD dwLength) ;
void Cheat_DungeonObserverJoin_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_Money_Syn(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_Event_Syn(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_EventNotify_On(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_PlusTime_On(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void Cheat_EventNotify_Off(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;	
void Cheat_PlusTime_AllOff(USERINFO* pUserInfo,DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void CheatServer_BanCharacter_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void CheatServer_BanCharacter_Ack(char* pMsg) ;
void CheatServer_WhereIs_MapServer_Ack(char* pMsg, DWORD dwLength) ;
void CheatServer_WhereIs_Nack(char* pMsg, DWORD dwLength) ;
void CheatServer_BanMap_Syn(char* pMsg) ;
void CheatServer_AgentCheck_Syn(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void CheatServer_AgentCheck_Ack(char* pMsg, DWORD dwLength) ;
void CheatServer_Notice_Syn(char* pMsg) ;
void CheatServer_Event_Syn(char* pMsg) ;
void CheatServer_EventNotify_On(char* pMsg) ;
void CheatServer_EventNotify_Off(char* pMsg) ;
void CheatServer_PlusTime_On(char* pMsg) ;
void CheatServer_PlusTime_AllOff(char* pMsg) ;

/// GTOURNAMENTUSERMSGPARSER PART
void Gtournament_MoveToBattleMap_Syn(char* pMsg, DWORD dwLength) ;
void Gtournament_StandingInfo_Syn(char* pMsg) ;
void Gtournament_ObserverJoin_Syn(char* pMsg, DWORD dwLength) ;
void Gtournament_Leave_Syn(char* pMsg) ;
void Gtournament_Cheat(char* pMsg) ;

/// GTOURNAMENTSERVERMSGPARSER PART
void GtournamentServer_Cheat(char* pMsg, DWORD dwLength) ;
void GtournamentServer_StandingInfo_Nack(char* pMsg, DWORD dwLength) ;
void GtournamentServer_StandingInfo_Ack(char* pMsg, DWORD dwLength) ;
void GtournamentServer_ReturntoMap(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void GtournamentServer_Notify_WinLose(char* pMsg, DWORD dwLength) ;
void GtournamentServer_Notify(char* pMsg, DWORD dwLength) ;
void GtournamentServer_Notify2GuildToMap(char* pMsg, DWORD dwLength) ;
void GtournamentServer_Notify2PlayerToMap(char* pMsg, DWORD dwLength) ;
void GtournamentServer_Reward(char* pMsg, DWORD dwLength) ;
void GtournamentServer_Battle_Notify(char* pMsg, DWORD dwLength) ;
void GtournamentServer_EndBattle(char* pMsg, DWORD dwLength) ;
// 091202 ONS 길토 우승골드 보상 처리 추가
void GtournamentServer_Reward_Money(char* pMsg, DWORD dwLength) ;
// 090227 ShinJS --- 암상인 추가작업 : 이동NPC 프로토콜 처리를 위해 사용
void MP_NPCMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// 100302 ONS 마족 캐릭터 생성시 생성레벨 체크처리 추가
BOOL CheckDevilMakeLevel(USERINFO*);

#endif //__AGENTNETWORKMSGPARSER_H__
