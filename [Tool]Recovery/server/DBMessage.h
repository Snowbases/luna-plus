#pragma once

// 로그인
void RLoginOperator( LPMIDDLEQUERY, LPDBMESSAGE );

// 로그인 ID를 키워드로 유저 인덱스를 얻어낸다
void RGetUserList( LPMIDDLEQUERY, LPDBMESSAGE );

// 접속자 인덱스를 가져온다
void RGetUserIndex( LPMIDDLEQUERY, LPDBMESSAGE );

// 접속자 정보를 가져온다
void RGetUserData( LPMIDDLEQUERY, LPDBMESSAGE );

// 접속자의 플레이어 목록을 가져온다
void RGetUserPlayerList( LPQUERY, LPDBMESSAGE );

// 사용자를 추가한다
void UserAdd( DWORD connectionIndex, const CHARACTERMAKEINFO& );
void RUserAdd( LPMIDDLEQUERY, LPDBMESSAGE );

// 사용자가 보유한 특정 플레이어를 복구한다
void RestoreUser( DWORD connectionIndex, DWORD playerIndex, const char* playerName );
void RRestoreUser( LPMIDDLEQUERY, LPDBMESSAGE );

// 사용자가 보유한 특정 플레이어를 삭제한다
void DeleteUser( DWORD connectionIndex, DWORD playerIndex );
void RDeleteUser( LPMIDDLEQUERY, LPDBMESSAGE );

// 키워드로 플레이어 인덱스와 이름을 얻어낸다
void GetPlayerList( DWORD connectionIndex, DWORD userIndex, const char* userName, const char* keyword, DWORD beginPlayerIndex );
void RGetPlayerList( LPQUERY, LPDBMESSAGE );

// 플레이어의 접속 ID를 가져온다
void GetPlayerLoginId( DWORD connectionIndex, DWORD userIndex, DWORD playerIndex );
void RGetPlayerLoginId( LPMIDDLEQUERY, LPDBMESSAGE );

// 플레이어 상세 정보를 얻어낸다
void RGetPlayerData( LPQUERY, LPDBMESSAGE );

// 문자열 등 큰 필드값만 여기서 얻어낸다
void RGetPlayerExtendedData( LPMIDDLEQUERY, LPDBMESSAGE );

// 080328 LUJ, 플레이어가 소속된 패밀리 이름을 얻어낸다
void RGetPlayerFamilyData( LPMIDDLEQUERY, LPDBMESSAGE );

// 캐릭터 확장 정보를 갱신한다
//void SetPlayerExtendedData( DWORD connectionIndex, const MSG_RM_PLAYER_EXTENDED_DATA& );
void RSetPlayerExtendedData( LPMIDDLEQUERY, LPDBMESSAGE );

// 주민증 정보를 읽는다
void RGetPlayerLicense( LPMIDDLEQUERY, LPDBMESSAGE );

// 주민증 정보를 갱신한다
//void PlayerSetLicense( DWORD connectionIndex, DWORD playerIndex, const DATE_MATCHING_INFO& );
void RPlayerSetLicense( LPMIDDLEQUERY, LPDBMESSAGE );

// 아이템 정보를 읽는다
void GetItemListOnInventory( DWORD connectionIndex, DWORD playerIndex, DWORD beginItemDbIndex );
void GetItemListOnStorage( DWORD connectionIndex, DWORD userIndex, DWORD beginItemDbIndex );
void GetItemListOnShop( DWORD connectionIndex, DWORD userIndex, DWORD beginItemDbIndex );
// 080716 LUJ, 펫이 가진 아이템을 쿼리한다
// 080730 LUJ, 사용자 번호 대신 펫 번호를 인자로 쓴다
void GetItemListOnPet( DWORD connectionIndex, DWORD petIndex, DWORD beginItemDbIndex );

void RGetItemList( LPQUERY, LPDBMESSAGE );

// 아이템 소유자를 찾는다
void FindItemOwner( DWORD connectionIndex, DWORD itemIndex, DWORD playerIndex );
void RFindItemOwner( LPQUERY, LPDBMESSAGE );

// 아이템 추가
void RAddItem( LPMIDDLEQUERY, LPDBMESSAGE );

// 아이템 사용 종료 시점 설정
void RSetItemEndTime( LPMIDDLEQUERY, LPDBMESSAGE );

// 스킬 정보를 획득한다
void GetSkillList( DWORD connectionIndex, DWORD playerIndex );
void RGetSkillList( LPQUERY, LPDBMESSAGE );

// 080731 LUJ, 버프 스킬 정보를 획득한다
void GetBuffList( DWORD connectionIndex, DWORD playerIndex, DWORD lastSkillIndex );
void RGetBuffList( LPMIDDLEQUERY, LPDBMESSAGE );

// 스킬 레벨을 바꾼다
void SetSkillLevel( DWORD connectionIndex, DWORD playerIndex, DWORD skillDbIndex, DWORD level );
void RSetSkillLevel( LPMIDDLEQUERY, LPDBMESSAGE );

// 스킬을 추가한다
void AddSkillData( DWORD connectionIndex, DWORD playerIndex, DWORD skillIndex );
void RAddSkillData( LPMIDDLEQUERY, LPDBMESSAGE );

// 스킬을 삭제한다
void RemoveSkillData( DWORD connectionIndex, DWORD skillDbIndex );
void RRemoveSkillData( LPMIDDLEQUERY, LPDBMESSAGE );

// 패밀리를 검색한다
void RGetFamilyList( LPQUERY, LPDBMESSAGE );
// 패밀리 일반 정보를 읽는다
void RGetFamilyData( LPMIDDLEQUERY, LPDBMESSAGE );
// 패밀리 멤버 정보를 읽는다
void RGetFamilyMember( LPQUERY, LPDBMESSAGE );
// 패밀리 농장 정보를 읽는다
void RGetFamilyFarm( LPMIDDLEQUERY, LPDBMESSAGE );
// 농작물 정보를 읽는다
void RGetFamilyCrop( LPMIDDLEQUERY, LPDBMESSAGE );
// 080731 LUJ, 가축 정보를 읽는다
void RGetFamilyLivestock( LPMIDDLEQUERY, LPDBMESSAGE );

// 메인 퀘스트 정보를 읽는다
void GetQuestMainData( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex );
void RGetQuestMainData( LPQUERY, LPDBMESSAGE );

// 서브 퀘스트 정보를 읽는다
void GetQuestSubData( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex );
void RGetQuestSubData( LPQUERY, LPDBMESSAGE );

// 퀘스트 아이템 정보를 읽는다
void GetQuestItemList( DWORD connectionIndex, DWORD playerIndex, DWORD questIndex );
void RGetQuestItemList( LPQUERY, LPDBMESSAGE );

// 서브퀘스트를 완료한다
void EndSubQuest( DWORD connectionIndex, DWORD playerIndex, DWORD mainQuestIndex, DWORD subQuestIndex, QSTATETYPE state );
void REndSubQuest( LPMIDDLEQUERY, LPDBMESSAGE );

// 퀘스트를 삭제한다(다시 진행할 수 있도록)
void RemoveQuestData( DWORD connectionIndex, DWORD playerIndex, DWORD mainQuestIndex );
void RRemoveQuestData( LPMIDDLEQUERY, LPDBMESSAGE );

// 퀘스트 아이템 수량을 변경한다
void RUpdateQuestItem( LPMIDDLEQUERY, LPDBMESSAGE );

// 퀘스트 아이템을 삭제한다
void DeleteQuestItem( DWORD connectionIndex, DWORD playerIndex, DWORD questMainIndex, DWORD itemIndex );
void RDeleteQuestItem( LPMIDDLEQUERY, LPDBMESSAGE );

// 퀘스트 로그를 가져온다
void RGetQuestLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetQuestLog( LPQUERY, LPDBMESSAGE );

// 길드를 검색한다
void GetGuildList( DWORD connectionIndex, const char* keyword, bool isDismissed );
void RGetGuildList( LPQUERY, LPDBMESSAGE );

// 길드 기본 정보를 얻는다
void RGetGuildData( LPMIDDLEQUERY, LPDBMESSAGE );

// 080425 LUJ, 길드 스킬 정보를 얻는다
void RGetGuildSkill( LPMIDDLEQUERY, LPDBMESSAGE );

// 창고 정보를 얻는다
void GetGuildStore( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildStore( LPQUERY, LPDBMESSAGE );

// 회원 정보를 얻는다
void GetGuildMember( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildMember( LPQUERY, LPDBMESSAGE );

// 아군 정보를 얻는다
void GetGuildFriendly( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildFriendly( LPQUERY, LPDBMESSAGE );

// 적군 정보를 얻는다
void GetGuildEnemy( DWORD connectionIndex, DWORD guildIndex );
void RGetGuildEnemy( LPQUERY, LPDBMESSAGE );

// 길드 정보를 갱신한다
void RGuildUpdateData( LPMIDDLEQUERY, LPDBMESSAGE );

// 길드 직위를 변경한다
void RGuildSetRank( LPMIDDLEQUERY, LPDBMESSAGE );

// 길드에서 탈퇴시킨다
void RGuildKickMember( LPMIDDLEQUERY, LPDBMESSAGE );

// 경험치 로그 정보를 조회한다
void RGetExperienceLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetExperienceLog( LPQUERY, LPDBMESSAGE );

// 아이템 로그 조회
void RGetItemLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemLog( LPQUERY, LPDBMESSAGE );

// 스탯 로그를 가져온다
void RGetStatLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetStatLog( LPQUERY, LPDBMESSAGE );

// 스킬 로그를 가져온다
void RGetSkillLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetSkillLog( LPQUERY, LPDBMESSAGE );

// 아이템 옵션 로그를 가져온다
void RGetItemOptionLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemOptionLog( LPQUERY, LPDBMESSAGE );

// 080320 LUJ, 드롭 옵션 로그를 가져온다
void RGetItemDropOptionLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemDropOptionLog( LPQUERY, LPDBMESSAGE );

// 길드 로그 를 가져온다
void RGetGuildLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetGuildLog( LPQUERY, LPDBMESSAGE );

// 길드 창고 로그를 가져온다
void RGetGuildWarehouseLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetGuildWarehouseLog( LPQUERY, LPDBMESSAGE );

// 클래스 로그를 가져온다
void RGetJobLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetJobLog( LPQUERY, LPDBMESSAGE );

// 패밀리 농작물 로그 가져온다
void RGetFarmCropLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFarmCropLog( LPQUERY, LPDBMESSAGE );

// 패밀리 포인트 크기를 가져온다
void RGetFamilyPointLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFamilyPointLog( LPQUERY, LPDBMESSAGE );

// 길드 점수 로그를 가져온다
void RGetGuildScoreLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetGuildScoreLog( LPQUERY, LPDBMESSAGE );

// IP를 접속 허용 목록에 추가한다
void AddPermission( DWORD connectionIndex, const char* ipAddress );
void RAddPermission( LPMIDDLEQUERY, LPDBMESSAGE );

// IP를 접속 허용 목록에서 삭제한다
void RemovePermission( DWORD connectionIndex, const char* ipAddress );
void RRemovePermission( LPMIDDLEQUERY, LPDBMESSAGE );

// 李쎄퀬
void RGetStorageData( LPMIDDLEQUERY, LPDBMESSAGE );
void RSetStorageData( LPMIDDLEQUERY, LPDBMESSAGE );

// operator
void RGetOperator( LPQUERY, LPDBMESSAGE );
void RAddOperator( LPMIDDLEQUERY, LPDBMESSAGE );
void RUpdateOperator( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetOperatorIp( LPQUERY, LPDBMESSAGE );
void RAddOperatorIp( LPMIDDLEQUERY, LPDBMESSAGE );
void RRemoveOperatorIp( LPMIDDLEQUERY, LPDBMESSAGE );

// password
void RSetPassword( LPMIDDLEQUERY, LPDBMESSAGE );

// operator log
void RGetOperatorLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetOperatorLog( LPQUERY, LPDBMESSAGE );

// name log
void RGetNameLog( LPQUERY, LPDBMESSAGE );

// 080401 LUJ, 농장 로그
void RGetFarmLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFarmLog( LPQUERY, LPDBMESSAGE );

// 080630 LUJ, 농장 가축 로그
void RGetLivestockLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetLivestockLog( LPQUERY, LPDBMESSAGE );

// 080716 LUJ, 펫 로그
void RGetPetLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetPetLog( LPQUERY, LPDBMESSAGE );

// 080403 LUJ, 유료 아이템 구입 로그
void RGetItemShopLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetItemShopLog( LPQUERY, LPDBMESSAGE );

// 080523 LUJ, 낚시 로그
void RGetFishLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFishLog( LPQUERY, LPDBMESSAGE );

// 080526 LUJ, 낚시 정보를 가져온다
void RGetFishingData( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetCookData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetCookRecipeData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetHouseData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetPetData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetItemGlobalLog(LPQUERY, LPDBMESSAGE);
void RAddPet(LPMIDDLEQUERY, LPDBMESSAGE);

// 081021 LUJ, 권한 정보를 가져온다
void GetAuthorityToServer( eGM_POWER );
void RGetAuthorityToServer( LPQUERY, LPDBMESSAGE );
void GetAuthorityToUser( DWORD connectionIndex, eGM_POWER, AuthorityType );
void RGetAuthorityToUser( LPQUERY, LPDBMESSAGE );

// 081027 LUJ, 토너먼트 정보를 쿼리한다
void RGetTournamentCount( LPMIDDLEQUERY, LPDBMESSAGE );
// 081027 LUJ, 특정 회차의 정보를 쿼리한다
void GetTournamentGuild( DWORD connectionIndex, DWORD count, DWORD lastGuildIndex );
void RGetTournamentGuild( LPQUERY, LPDBMESSAGE );
// 081027 LUJ, 플레이어 정보를 쿼리한다
void RGetTournamentPlayer( LPMIDDLEQUERY, LPDBMESSAGE );

// 081027 LUJ, 怨듭꽦 濡쒓렇
void RGetSiegeLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetSiegeLog( LPQUERY, LPDBMESSAGE );
// 081127 LUJ, 버전 정보를 갱신한다. 동기화를 위해 DB에서 처리한다
void IncreaseAuthorityVersion( DWORD connectionIndex, eGM_POWER = eGM_POWER_MASTER, AuthorityType = AuthorityTypeVersion );
void RIncreaseAuthorityVersion( LPMIDDLEQUERY, LPDBMESSAGE );
// 081205 LUJ, 梨꾪똿 濡쒓렇
void RGetChatLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetChatLog( LPMIDDLEQUERY, LPDBMESSAGE );
// 081205 LUJ, 패밀리 로그
void RGetFamilyLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void RGetFamilyLog( LPQUERY, LPDBMESSAGE );
// 081224 LUJ, 오퍼레이터 로그
void LogOperator( RecoveryLog, const TCHAR* const user, const TCHAR* const key1, const TCHAR* const value1, const TCHAR* const key2 = _T( "" ), const TCHAR* const value2 = _T( "" ), const TCHAR* const key3 = _T( "" ), const TCHAR* const value3 = _T( "" ) );
// 090122 LUJ, 스크립트 변조 로그 개수를 가져온다
void RGetScriptHackLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
// 090122 LUJ, 스크립트 변조 로그 쿼리를 요청한다
void GetScriptHackLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD requestTick, DWORD queriedSize );
// 090122 LUJ, 스크립트 변조 로그 결과를 처리한다
void RGetScriptHackLog( LPQUERY, LPDBMESSAGE );
// 090406 LUJ, 돈 로그를 처리한다
void RGetMoneyLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void GetMoneyLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize );
void RGetMoneyLog( LPQUERY, LPDBMESSAGE );
// 090609 ShinJS, AutoNote 로그를 처리한다
void RGetAutoNoteLogSize( LPMIDDLEQUERY, LPDBMESSAGE );
void GetAutoNoteLog( DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize );
void RGetAutoNoteLog( LPQUERY, LPDBMESSAGE );
// 090618 ShinJS, User Punish Data Update시
void RUserDataUpdate( LPMIDDLEQUERY query, LPDBMESSAGE dbMessage );
void RGetHouseLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetHouseLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize);
void RGetHouseLog(LPQUERY, LPDBMESSAGE);
void RGetCookLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetCookLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize);
void RGetCookLog(LPQUERY, LPDBMESSAGE);
void RGetRecipeLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetRecipeLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD userIndex, DWORD requestTick, DWORD queriedSize);
void RGetRecipeLog(LPQUERY, LPDBMESSAGE);
void RGetFurniture(LPQUERY, LPDBMESSAGE);
void RGetNoteList(LPQUERY, LPDBMESSAGE);
void RGetNote(LPLARGEQUERY, LPDBMESSAGE);
void RGetConsignLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetConsignLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD itemDBIndex, DWORD consignIndex, DWORD requestTick, DWORD queriedSize);
void RGetConsignLog(LPQUERY, LPDBMESSAGE);
void RGetGameRoomPointLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetGameRoomPointLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize);
void RGetGameRoomPointLog(LPQUERY, LPDBMESSAGE);
void RGetGameRoomData(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetPartyList(LPMIDDLEQUERY, LPDBMESSAGE);
void RGetForbidChatLogSize(LPMIDDLEQUERY, LPDBMESSAGE);
void GetForbidChatLog(DWORD connectionIndex, LPCTSTR beginDate, LPCTSTR endDate, DWORD playerIndex, DWORD requestTick, DWORD queriedSize);
void RGetForbidChatLog(LPMIDDLEQUERY, LPDBMESSAGE);
