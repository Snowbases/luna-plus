
#ifndef __MAPNETWORKMSGPARSER_H__
#define __MAPNETWORKMSGPARSER_H__

void MP_POWERUPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_MAPSERVERMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHATMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHARMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_USERCONNMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_MOVEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_ITEMMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_SKILLTREEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHEATMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_QUICKMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_PARTYMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// 070124 LYW --- PeaceWar : Delete peacewar part.
//void MP_PEACEWARMODEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_STORAGEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_SkillMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_BattleMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_REVIVEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_MonitorMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
//KES EXCHANGE 030922
void MP_EXCHANGEMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_HACKCHECKMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// LBS 03.09.25 노점상
void MP_STREETSTALLMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

void MP_NPCMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

void MP_QUESTMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

void MP_PKMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength );
void MP_HACKCHECKMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GUILDMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GUILDFIELDWARMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GTOURNAMENTMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_GUILUNIONMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength );

// 080723 KTH -- 공성전
void MP_SIEGEWARFAREMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// S 농장시스템 추가 added by hseos 2007.04.11
void MP_FARM_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength );
// E 농장시스템 추가 added by hseos 2007.04.11

// desc_hseos_주민등록01
// S 주민등록 추가 added by hseos 2007.06.04
void MP_RESIDENTREGIST_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength );
// E 주민등록 추가 added by hseos 2007.06.04

void MP_FAMILYMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.19
void MP_DATE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_AUTONOTE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
//---------------

// 080327 NYJ --- 낚시시스템
void MP_FISHING_ServerMsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_COOK_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_HOUSE_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_TRIGGER_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_DUNGEON_MsgParser( DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_PET_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// 070228 LYW --- MapNetworkMsgParser : Add message parser for facial part.
void MP_FACIALMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// 070228 LYW --- MapNetworkMsgParser : Add message parser for emotion part.
void MP_EMOTIONMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 081126 LYW --- MapNetworkMsgParser : 리미트 던전 추가.
void MP_LIMITDUNGEON_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

BOOL CheckHackNpc( CPlayer* pPlayer, WORD wNpcIdx );


// 070205 LYW --- Add functions to process network msg.
/// MONITORMSGPARSER PART
void MornitorMapServer_NoticeSend_Syn() ;
void MornitorMapServer_Ping_Syn(MSGROOT* pTempMsg, DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
void MornitorMapServer_QueryUserCount_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void MornitorMapServer_Query_ChannelInfo_Syn(DWORD dwConnectionIndex) ;
void MornitorMapServer_AssertMsgBox_Syn(char* pMsg) ;
void MornitorMapServer_ServerOff_Syn() ;

/// CHARMSGPARSER PART
//void Char_LevelDown_Syn(char* pMsg) ;
void Char_PointAdd_Syn(char* pMsg) ;
void Char_State_Notify(char* pMsg) ;
//void Char_Ability_Upgrade_Syn(char* pMsg) ;
//void Char_Ability_Upgrade_SkPoint_Syn(char* pMsg) ;
//void Char_Ability_Downgrade_SkPoint_Syn(char* pMsg) ;
void Char_ExitStart_Syn(char* pMsg) ;
void Char_Emergency_Syn(char* pMsg) ;
void Char_Exit_Syn(char* pMsg) ;
/// USERCONNMSGPARSER
void UserConn_ChannelInfo_Syn(MSGBASE*, DWORD dwConnectionIndex);
void UserConn_GameIn_OtherMap_Syn(char* pMsg) ;
void UserConn_GameIn_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_GameInPos_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_GameIn_Nack(MSGBASE* pTempMsg) ;
void UserConn_GameOut_Syn(char* pMsg) ;
void UserConn_NoWaitExitPlayer(char* pMsg) ;
void UserConn_Disconnected(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_NpcAdd_Syn(char* pMsg);	//080402 NYJ
void UserConn_NpcDie_Syn(char* pMsg);	//080402 NYJ
void UserConn_ChangeMap_Nack() ;
void UserConn_Option_Syn(LPVOID);
void UserConn_Npc_ChangeMap_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_SavePoint_Syn(char* pMsg) ;
void UserConn_BackToCharSel_Syn(MSGBASE* pTempMsg, DWORD dwConnectionIndex, char* pMsg) ;
void UserConn_SetVisible(char* pMsg) ;
void UserConn_Return_Syn(DWORD dwConnectionIndex, char* pMsg) ;

/// MOVEMSGPARSER PART
void Move_Target(char* pMsg, DWORD dwLength) ;
void Move_OneTarget(char* pMsg) ;
void Move_Pet_OneTarget(char* pMsg) ;
void Move_Pet_Target(char* pMsg, DWORD dwLength) ;
void Move_Stop(char* pMsg, DWORD dwLength) ;
void Move_Pet_Stop(char* pMsg, DWORD dwLength) ;
void Move_Pet_Warp_Syn(char* pMsg, DWORD dwLength) ;
void Move_WalkMode(char* pMsg, DWORD dwLength) ;
void Move_RunMode(char* pMsg, DWORD dwLength) ;
void Move_EffectMove(char* pMsg) ;

/// CHEATMSGPARSER PART
void Cheat_WhereIs_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void Cheat_Hide_Syn(char* pMsg) ;
//	void Cheat_BanCharacter_Syn(char* pMsg) ;
void Cheat_PkAllow_Syn(CPlayer* pSender, char* pMsg) ;
void Cheat_MoveToChar_Syn(char* pMsg) ;
void Cheat_Move_Syn(char* pMsg) ;
void Cheat_MoveUser_Syn(char* pMsg) ;
void Cheat_ChangeMap_Syn(char* pMsg) ;
void Cheat_Item_Syn(char* pMsg) ;
//void Cheat_Item_Option_Syn(char* pMsg) ;
void Cheat_Mana_Syn(char* pMsg) ;
void Cheat_Update_Skill_Syn(char* pMsg) ;
void Cheat_Money_Syn(char* pMsg) ;
void Cheat_ReLoading() ;
void Cheat_LevelUp_Syn(char* pMsg) ;
void Cheat_PetLevelUp_Syn(char* pMsg) ;
void Cheat_Event_Monster_Regen(char* pMsg) ;
void Cheat_Event_Monster_Delete(char* pMsg) ;
void Cheat_Str_Syn(char* pMsg) ;
void Cheat_Dex_Syn(char* pMsg) ;
void Cheat_Vit_Syn(char* pMsg) ;
void Cheat_Wis_Syn(char* pMsg) ;
void Cheat_Int_Syn(char* pMsg) ;
void Cheat_Kill_AllMonster_Syn() ;
void Cheat_PartyInfo_Syn(char* pMsg) ;
void Cheat_Event_Syn(char* pMsg) ;
void Cheat_PlusTime_AllOff(char* pMsg) ;
void Cheat_PlusTime_On(char* pMsg) ;
void Cheat_PlusTime_Off(char* pMsg) ;
void Cheat_MunpaDateClr_Syn(char* pMsg) ;
void Cheat_GuildUnion_Clear(char* pMsg) ;
void Cheat_Mob_Chat(char* pMsg) ;
void Cheat_Pet_Stamina(char* pMsg) ;
void Cheat_Pet_FriendShip_Syn(char* pMsg) ;
void Cheat_Pet_SkillReady_Syn(char* pMsg) ;
void Cheat_Pet_Selected_FriendShip_Syn(char* pMsg) ;
void Cheat_NpcHide_Syn(char* pMsg) ;
void Cheat_SkillPoint_Syn( char *pMsg );
void Cheat_God_Syn( char *pMsg );
// 081022 LYW --- Pet : 펫 버그/기타 테스트를 위하여 Pet에도 GOD 모드를 적용한다.
void Cheat_PetGod_Syn(char* pMsg) ;
// 071212 KTH -- 확장 인벤토리의 정보를 초기화 하여준다.
void Cheat_Reset_Inventory(char* pMsg) ;
// 080115 KTH -- PVP 데미지 비율 설정
void Cheat_PvP_Damage_Rate(char* pMsg) ;
// 080213 KTH -- ClearInventory
void Cheat_Clear_Inventory(char* pMsg);

void MP_TUTORIALMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
// 080426 NYJ --- 낚시관련 치트키
void Cheat_Fishing_Exp(char* pMsg);
void Cheat_Fishing_FishPoint(char* pMsg);

// 100322 NYJ --- 던전모니터링관련 치트키
void Cheat_Dungeon_Observer_Syn(DWORD dwConnectionIndex, char* pMsg);
void Cheat_Dungeon_ObserverJoin_Syn(DWORD dwConnectionIndex, char* pMsg);

void Cheat_Dungeon_GetInfoAll_Syn(DWORD dwConnectionIndex, char* pMsg);
void Cheat_Dungeon_GetInfoOne_Syn(DWORD dwConnectionIndex, char* pMsg);

// 100611 ONS 채팅금지 관련 함수
void Cheat_ForbidChat(char* pMsg);
void Cheat_ForbidChat_Server( DWORD dwConnectionIndex, char* pMsg );
void Cheat_PermitChat( char* pMsg );
void Cheat_PermitChat_Server( DWORD dwConnectionIndex, char* pMsg );

// 080728 LYW --- MapNetworkMsgParser : Add message parser for siege recall part.
void MP_SIEGERECALL_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 08020 LYW --- MapNetworkMsgParser : Add message parser for recall npc.
void MP_RECALLNPC_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

// 080827 LYW --- MapNetworkMsgParser : Add message parser for siegewar part.
void MP_SIEGEWARFARE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;
// 090316 LUJ, 탈것 메시지 처리 함수
void MP_VEHICLE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// 100104 ShinJS, PC방 메시지 처리 함수
void MP_PCROOM_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
// 100510 NYJ 판매대행
void MP_CONSIGNMENT_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_NOTE_MsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
#endif
