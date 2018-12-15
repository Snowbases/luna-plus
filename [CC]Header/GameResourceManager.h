// GameResourceManager.h: interface for the CGameResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMERESOURCEMANAGER_H__8C774E0A_9D8C_4535_A3B2_F847517FA424__INCLUDED_)
#define AFX_GAMERESOURCEMANAGER_H__8C774E0A_9D8C_4535_A3B2_F847517FA424__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameResourceStruct.h"
//#include "MapChange.h"
class CMHFile;
#include "ptrlist.h"
#include <vector>


#define GAMERESRCMNGR	USINGTON(CGameResourceManager)

// 070121 LYW --- Define radius of potal.
#define POTAL_RAD	308
#define MAX_OBJECTNAME_SIZE	128

///////////////////////////////////////////////////////////////////////////////
// 06. 05. PreLoadData 추가기능 - 이영준
// 이펙트 PreLoad
struct sPRELOAD_EFFECT_INFO{
	int MapNum;
	int Count;

	cPtrList Effect;
};
///////////////////////////////////////////////////////////////////////////////



class CGameResourceManager  
{
	NPC_LIST m_NpcList[MAX_NPC_NUM];
	BOOL m_bMonsterRegenLoad;
	BOOL m_bMonsterListLoad;
	BOOL m_bMonsterDropItem;
	
	DWORD m_MaxNpcChxList;
	CHXLIST* m_NpcChxList;
	
	typedef stdext::hash_map< DWORD, EnchantScript > EnchantScriptMap;
	EnchantScriptMap mEnchantScriptMap;

	typedef stdext::hash_map< EWEARED_ITEM, EnchantScript > SlotToEnchantScript;
	SlotToEnchantScript	mSlotToEnchantScript;
	EnchantProtection mEnchantProtection;
	// 인챈트시 골드소비
	stEnchantGoldRate m_EnchantGoldRateInfo;
	// 인챈트 레벨에 대한 보너스적용 수치
	typedef std::map< BYTE, stEnchantLvBonusRate > EnchantBonusContainer;
	EnchantBonusContainer m_EnchantLvBonusInfoList;
	// 인챈트 실패시의 적용 정보
	typedef std::map< ITEM_INFO::eKind, stEnchantFailSetting > EnchantFailContainer;
	EnchantFailContainer m_htEnchantFailSetting;
	// 100503 ShinJS --- 인챈트 성공 확률
	typedef std::map< BYTE, float > EnchantSuccessRateContainer;
	EnchantSuccessRateContainer m_mapEnchantSuccessRate;

	typedef stdext::hash_map< LONGLONG, const DropOptionScript* > DropOptionScriptMap;
	DropOptionScriptMap	mDropOptionScriptMap;
	typedef std::list< DropOptionScript > DropOptionScriptList;
	DropOptionScriptList mDropOptionScriptList;

	typedef stdext::hash_map< DWORD, ApplyOptionScript > ApplyOptionScriptMap;
	ApplyOptionScriptMap mApplyOptionScriptMap;

	// 키: 아이템 인덱스
	typedef stdext::hash_map< DWORD, ReinforceScript > ReinforceScriptMap;
	ReinforceScriptMap mReinforceScriptMap;
	// 080929 LUJ, 강화 보조 스크립트 컨테이너
	ReinforceSupportScriptMap mReinforceSupportScriptMap;
	// 080228 LUJ, 강화 보호 아이템 관리
	typedef stdext::hash_set< DWORD > ReinforceProtectionItemSet;
	ReinforceProtectionItemSet mReinforceProtectionItemSet;

	CYHHashTable<STATIC_NPCINFO>	m_StaticNpcTable;
	CYHHashTable<STATIC_NPCINFO>	m_WareNpcTable;

public:
	CGameResourceManager();
	virtual ~CGameResourceManager();
	// 0 => -5, 1 => -4, 2 => -3, 3 => -2, ....
	EXPTYPE m_PLAYERxMONSTER_POINT[MAX_PLAYERLEVEL_NUM][MAX_MONSTERLEVELPOINTRESTRICT_NUM + MONSTERLEVELRESTRICT_LOWSTARTNUM + 1];
	EXPTYPE m_CharacterExpPoint[MAX_CHARACTER_LEVEL_NUM];
	BOOL LoadExpPoint();
	EXPTYPE GetMaxExpPoint(LEVELTYPE level)
	{ 
		// 06.09.14 RaMa  수정
		if( level > 0 )			return m_CharacterExpPoint[level-1];
		else					return m_CharacterExpPoint[0];
	}
	/* ¸ó½ºÅÍ Æ÷ÀÎÅÍ */
	BOOL LoadPlayerxMonsterPoint();
	EXPTYPE GetPLAYERxMONSTER_POINT(LEVELTYPE level,int levelgap);

	// 080424 NYJ --- 낚시숙련도 추가
	EXPTYPE m_FishingExpPoint[MAX_FISHING_LEVEL];
	DWORD	m_FishingLevelUpRestrict[MAX_FISHING_LEVEL];
	DWORD	m_FishingLevelUpReward[MAX_FISHING_LEVEL];
	BOOL LoadFishingExpPoint();
	EXPTYPE GetFishingMaxExpPoint(LEVELTYPE level)
	{ 
		if( level > 0 )			return m_FishingExpPoint[level-1];
		else					return m_FishingExpPoint[0];
	}

	DWORD GetFishingLevelUpRestrict(LEVELTYPE level)
	{
		if( level > 0 )			return m_FishingLevelUpRestrict[level-1];
		else					return m_FishingLevelUpRestrict[0];
	}

	DWORD GetFishingLevelUpReward(LEVELTYPE level)
	{
		if( level > 0 )			return m_FishingLevelUpReward[level-1];
		else					return m_FishingLevelUpReward[0];
	}

	BOOL LoadMonsterList();
	CYHHashTable< BASE_MONSTER_LIST > m_SMonsterListTable;
	BASE_MONSTER_LIST* GetMonsterListInfo(int monsterKind);
	int GetMonsterIndexForName( char* pStrName );
	void ResetMonsterList();

	CYHHashTable<stMonsterEventRewardItemInfo>		m_MonsterRewardItem;
	void LoadMonsterRewardList();
	DWORD GetMonsterEventRewardItem(WORD MonsterKind);

	void GetMonsterStats(WORD wMonsterKind, monster_stats * stats);				// ¸ó½ºÅÍ °´Ã¼ ÃÊ±âÈ­µÉ¶§
	BOOL LoadNpcChxList();
	char* GetMonsterChxName(DWORD index);
	char* GetNpcChxName(DWORD index);
	BOOL LoadNpcList();
	NPC_LIST* GetNpcInfo(WORD kind) { return &m_NpcList[kind-1]; }

	BOOL	LoadStaticNpc();
	void	ReleaseStaticNpc();
	STATIC_NPCINFO*	GetStaticNpcInfo( WORD wNpcUniqueIdx );
	void	SetStaticNpcTablePositionHead() { m_StaticNpcTable.SetPositionHead(); }
	STATIC_NPCINFO* GetNextStaticNpcInfo() { return m_StaticNpcTable.GetData(); }
	BOOL	IsInWareNpcPosition( VECTOR3* pvPos );

	// 070121 LYW --- Add function to check potal position.
	WORD CheckMapChange( VECTOR3* pVector, WORD curMapNum ) ;

	BOOL LoadMapChangeArea(); //060608 - Add by wonju
	BOOL LoadMapChangeArea(MAPCHANGE_INFO* pMapChangeArea);
	BOOL LoadLoginPoint(LOGINPOINT_INFO* pLogin);
	MAPCHANGE_INFO m_MapChangeArea[MAX_POINT_NUM];
	LOGINPOINT_INFO m_LoginPoint[MAX_POINT_NUM];
	void GetRegenPoint(VECTOR3& pos,VECTOR3& RandPos,WORD LoginIndex,WORD MapchangeIndex);
	void GetRegenPointFromEventMap(VECTOR3& pos,VECTOR3& RandPos,WORD LoginIndex,WORD MapchangeIndex,DWORD dwTeam);
	MAPCHANGE_INFO* GetMapChangeInfo(WORD Kind)
	{
		for(int i = 0; i < MAX_POINT_NUM; i++)
		{
			// 090929 NYJ - 할당안된녀석은 패스~
			if(0 == m_MapChangeArea[i].Kind)
				continue;

			if(m_MapChangeArea[i].Kind == Kind)
				return &m_MapChangeArea[i];
		}
		return NULL;
	}
//KES for Cheat
	MAPCHANGE_INFO* GetMapChangeInfoForDestMapNum( WORD wDestMapNum )
	{
		for(int i = 0; i < MAX_POINT_NUM; i++)
		{
			if(m_MapChangeArea[i].MoveMapNum == wDestMapNum)
				return &m_MapChangeArea[i];
		}
		return NULL;
	}

	MAPCHANGE_INFO* GetMapChangeInfoFromToMapNum( WORD wFromMapNum, WORD wToMapNum )
	{
		for(int i = 0; i < MAX_POINT_NUM; ++i)
		{
			if(m_MapChangeArea[i].CurMapNum == wFromMapNum && m_MapChangeArea[i].MoveMapNum == wToMapNum)
				return &m_MapChangeArea[i];
		}
		return NULL;
	}
	
	LOGINPOINT_INFO* GetLoginPointInfo(DWORD Kind)
	{
		for( int i = 0; i < MAX_POINT_NUM; i++)
		{
			if(m_LoginPoint[i].Kind == Kind)
				return &m_LoginPoint[i];
		}
		return NULL;
	}
	LOGINPOINT_INFO* GetLoginPointInfoFromMapNum(WORD MapNum)
	{
		for( int i = 0; i < MAX_POINT_NUM; i++)
		{
			if(m_LoginPoint[i].MapNum == MapNum)
				return &m_LoginPoint[i];
		}
		ASSERT(0);
		return &m_LoginPoint[0];
	}
	BOOL LoadDeveloperInfo();
	void Parse(LPCTSTR, BUFF_SKILL_INFO&) const;
	void Parse(LPCTSTR, ACTIVE_SKILL_INFO&) const;

	const EnchantScript* GetEnchantScript(DWORD itemIndex) const;
	const EnchantScript* GetEnchantScript(EWEARED_ITEM) const;
	void AddEnchantValue(ITEM_OPTION&, const EnchantScript::Ability&, int value) const;
	void AddEnchantValue(PlayerStat&, const EnchantScript::Ability&, int value) const;
	const stEnchantLvBonusRate& GetEnchantLvBonusInfo(BYTE enchantLevel) const;
	const stEnchantFailSetting* GetEnchantFailSetting(ITEM_INFO::eKind) const;
	float GetEnchantSuccessRate(BYTE enchantLevel) const;
	const EnchantProtection& GetEnchantProtection() const { return mEnchantProtection; }
	MONEYTYPE GetEnchantCost(const ITEM_INFO&) const;

	const ApplyOptionScript& GetApplyOptionScript(DWORD itemIndex) const;
	BOOL IsApplyOptionItem(DWORD itemIndex) const;
	const DropOptionScript& GetDropOptionScript(const ITEM_INFO&) const;
	LONGLONG GetItemKey(EWEARED_ITEM, eWeaponType, eArmorType, eWeaponAnimationType) const;

	const ReinforceScript* GetReinforceScript(DWORD itemIndex) const;
	BOOL IsReinforceProtectionItem(const ITEMBASE&) const;
	const ReinforceSupportScript* GetReinforceSupportScript(DWORD itemIndex) const;
	typedef ReinforceScriptMap ReinforceScriptContainer;
	const ReinforceScriptContainer& GetReinforceScript() const { return mReinforceScriptMap; }

	BOOL IsMovableMapWithScroll( MAPTYPE ) const;

public:
	// 081203 LUJ, 스킬 스크립트 로딩
	void LoadSkillScript( SkillScriptMap& );
	// 081203 LUJ, 스킬 번호로 스크립트를 반환한다
	const SkillScript& GetSkillScript( DWORD skillIndex ) const;
private:
	// 081203 LUJ, 스크립트 저장 영역
	SkillScriptMap mSkillScriptMap;

	// 090114 LUJ, 맵 이동 스크롤 스크립트 로딩
private:
	typedef stdext::hash_set< MAPTYPE > MovableMapSet;
	MovableMapSet mMovableMapSet;

	void LoadMovableMapWithScroll(MovableMapSet&);
	void LoadEnchantScript();
	void LoadDropOptionScript();
	void LoadApplyOptionScript();
	void LoadReinforceScript();

	typedef stdext::hash_set< DWORD > BuyItemSet;
	BuyItemSet mBuyItemSet;
	// 090114 LUJ, 구매 아이템 정보를 읽는다

	void LoadBuyItem( BuyItemSet& );
public:
	// 090114 LUJ, 구매 아이템 여부를 반환한다
	BOOL IsBuyItem( DWORD itemIndex ) const;

	// 081231 LUJ, 세트 스크립트를 관리한다. ItemManager 클래스에서 이동시킴
public:
	// 081231 LUJ, 아이템 인덱스에 해당하는 세트 스크립트를 반환한다
	const SetScript* const GetSetScript( DWORD itemIndex ) const;
	// 081231 LUJ, 아이템 번호로 세트 스크립트에서 정의하는 추가 속성 정의를 반환한다
	const SetScript::Element& GetItemElement( DWORD itemIndex ) const;
private:
	// 081231 LUJ, 아이템 번호로 세트 아이템 스크립트의 포인터를 반환
	//	키: 아이템 인덱스, 값: 세트 아이템 스크립트 포인터

	typedef stdext::hash_map< DWORD, const SetScript* >	SetScriptMap;
	SetScriptMap										mSetScriptMap;
	// 081231 LUJ, 세트 아이템 스크립트를 로딩한다
	void LoadSetScript( SetScriptMap& );
	typedef stdext::hash_map< DWORD, std::string > SetItemName;
	// 081231 LUJ, 세트 아이템 스크립트 하나를 파싱해서 메모리에 담는다

	void ParseSetScript( std::list< std::string >& textList, SetScript&, const SetItemName& );
	// 081231 LUJ, 세트 아이템 스크립트를 보관한다
	std::list< SetScript >	mSetScriptList;

	// 090129 LUJ, 합성 스크립트를 관리한다. ItemManager 클래스에서 이동시킴
public:
	// 080916 LUJ, 아이템에 해당하는 합성 스크립트를 반홚한다
	const ComposeScript* GetComposeScript( DWORD itemIndex ) const;
	BOOL IsComposeKeyItem( DWORD keyItemIndex ) const;
private:
	// 080916 LUJ, 키: 아이템 번호, 값: 합성 스크립트
	typedef stdext::hash_map< DWORD, const ComposeScript* >	ItemComposeScriptMap;
	ItemComposeScriptMap									mItemComposeScriptMap;
	// 080916 LUJ, 합성 스크립트를 읽는다

	void LoadComposeScript( ItemComposeScriptMap& );
	// 080916 LUJ, 합성 스크립트가 저장되는 컨테이너
	typedef std::list< ComposeScript >	ComposeScriptList;
	ComposeScriptList					mComposeScriptList;
	// 080916 LUJ, 시작 아이템과 스크립트가 연결되어 있다
	typedef std::set< ComposeScript* >					ComposeScriptSet;
	typedef stdext::hash_map< DWORD, ComposeScriptSet > KeyItemComposeScriptMap;
	KeyItemComposeScriptMap								mKeyItemComposeScriptMap;

	// 090316 LUJ, 탈것 스크립트 관리

public:
	// 090316 LUJ, 탈것 스크립트를 반환한다
	const VehicleScript& GetVehicleScript( DWORD monsterKind );
	LPCTSTR GetRiderAttachObjName( DWORD monsterKind, WORD pos );
private:
	// 090316 LUJ, 탈것 스크립트를 읽는다

	void LoadVehicleScript();
private:
	CYHHashTable< VehicleScript > mVehicleScriptHashTable;

	//090318 pdy Housing Script작업 추가
private:
	CYHHashTable<stFunitureInfo>			m_FunitureList;				//가구 정보리스트

	CYHHashTable<stHouseBonusInfo>			m_HouseBonusList;			//보너스 리스트 
	CYHHashTable<stHouseActionGroupInfo>	m_HouseActionGroupList;		//액션 그룹리스트

	CYHHashTable<stHouseActionInfo>			m_HouseActionList;			//액션 리스트
	CYHHashTable<stDungeonKey>		m_DungeonKeyList;	// 인던 키 리스트

public:
	CYHHashTable<stHouseBonusInfo>*	GetHouseBonusList(){return &m_HouseBonusList;}

public:
	void LoadHousing_AllList();
	void LoadHousing_FurnitureList();
	void LoadHousing_BonuseList();
	void LoadHousing_ActionGroupList();
	void LoadHousing_ActionList();

	void ReleaseHousing_AllList();
	void ReleaseHousing_FurnitureList();
	void ReleaseHousing_BonusList();
	void ReleaseHousing_ActionGroupList();
	void ReleaseHousing_ActionList();

public:
	//가구인덱스로 가구정보를 가져온다.
	stFunitureInfo* GetFunitureInfo(DWORD FunitureIndex);

	//가구인덱스로 가구모델파일 정보를 가져온다.
	stFurnitureFileInfo* GetModelFileInfo(DWORD FunitureIndex);

	//보너스인덱스로 보너스정보를 가져온다

	stHouseBonusInfo*	GetHouseBonusInfo(DWORD dwBonusIndex);

	//보너스인덱스로 보너스정보를 가져온다

	stHouseActionGroupInfo*	GetHouseActionGroupInfo(DWORD dwGroupIndex);

	//보너스인덱스로 보너스정보를 가져온다

	stHouseActionInfo*		GetHouseActionInfo(DWORD dwActionIndex);
	void LoadDungeonKeyList();
	void ReleaseDungeonKeyList();
	stDungeonKey* GetDungeonKey(DWORD dwKeyIndex);

	// 090227 ShinJS --- 암상인추가작업 (이동NPC 정보 별도생성:[서버:맵에 로그인시 활용], [클라이언트:이동NPC정보 파악])
private:
	CYHHashTable<STATIC_NPCINFO>	m_htMoveNpcStaticInfo;										// 이동NPC의 StaticNPC 정보
	void LoadMoveNPCInfo();																		// StaticNpc.bin에서 이동NPC 정보만 저장한다
public:	
	STATIC_NPCINFO* GetMoveNpcInfo( WORD wNpcUniqueIdx ) { return (STATIC_NPCINFO*)m_htMoveNpcStaticInfo.GetData( wNpcUniqueIdx ); }

	// 091106 LUJ, 리미트 던전 정보 관리
public:
	typedef std::pair< MAPTYPE, DWORD > LimitDungeonKey;
	typedef std::map< LimitDungeonKey, LimitDungeonScript > LimitDungeonScriptMap;
	const LimitDungeonScriptMap& GetLimitDungeonScriptMap() const { return mLimitDungeonScriptMap; }
	void LoadLimitDungeonScript();
	const LimitDungeonScript& GetLimitDungeonScript(MAPTYPE, DWORD channelIndex) const;
private:
	LimitDungeonScriptMap mLimitDungeonScriptMap;

// 100128 ONS DualWeapon처리를 위해 파일명저장 처리 변경
private:
	typedef std::pair<DWORD, BYTE>									DualWeaponModListKey;			// modfile인덱스와 종족정보를 키로 사용한다.	
	typedef std::map<DualWeaponModListKey, std::string>				DualWeaponModListMap;			 
	DualWeaponModListMap											m_DualWeaponModList;			 		
public:
	void ParseModFileName( char* pInString, char* pRObjectName, DWORD wIndex, BYTE byRaceType, BYTE byGender );		// '/'로 구분지어진 양손무기 modfile명을 파싱한다.
	const char* GetDualWeaponLObject( WORD wIndex, BYTE byRaceType, BYTE byGender );										// 인덱스, 종족, 성별정보를 통해서 해당 modfile명을 반환한다.


	// 100225 ShinJS --- 캐릭터들의 초기 스탯 정보를 가져온다.
private:
	typedef std::pair<RaceType, BYTE>								CharBaseStatusInfoKey;
	typedef std::map<CharBaseStatusInfoKey, PlayerStat>				CharBaseStatusInfoMap;
	CharBaseStatusInfoMap											m_CharBaseStatusInfoMap;
public:
	void LoadCharBaseStatusInfo();
	const PlayerStat& GetBaseStatus( RaceType race, BYTE byClass );

public:
//-----------------------------------------------------------------------------------------------------------//
//		Å¬¶óÀÌ¾ðÆ®¸¸ »ç¿ëÇÏ´Â ºÎºÐ
#ifdef _CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//

	MOD_LIST			m_ModFileList[RaceType_Max][GENDER_MAX];
	MOD_LIST			m_HairModList[RaceType_Max][GENDER_MAX];
	MOD_LIST			m_FaceModList[RaceType_Max][GENDER_MAX];
	BOOL LoadModFileList(MOD_LIST pModList[RaceType_Max][GENDER_MAX]);
	BOOL LoadHairModFileList(MOD_LIST pHairModList[RaceType_Max][GENDER_MAX]);
	BOOL LoadFaceModFileList(MOD_LIST pFaceModList[RaceType_Max][GENDER_MAX]); //pjscode
	BOOL LoadBodyModFileList(MOD_LIST pBodyModList[RaceType_Max][GENDER_MAX]);

	MOD_LIST			mPetModList;
	BOOL LoadPetModList();

	GAMEDESC_INI m_GameDesc;
	const DISPLAY_INFO& GetResolution() const { return m_GameDesc.dispInfo; }
	void SetDispInfo( DISPLAY_INFO* dispInfo ) { memcpy(&m_GameDesc.dispInfo, dispInfo, sizeof(DISPLAY_INFO)) ; }
	BOOL LoadGameDesc();
	
	void LoadPreDataTable();
	void DeletePreLoadData();
	void LoadPreMonsterData(sPRELOAD_INFO* pPreLoadInfo, MAPTYPE MapNum);	
	CYHHashTable<sPRELOAD_INFO>		m_PreLoadDataTable;
	sPRELOAD_INFO*	GetPreDataInfo(int MapNum)		{	return m_PreLoadDataTable.GetData(MapNum);	}
	cPtrList m_PreLoadEfflist;
	cPtrList* GetPreLoadEfflist()	{	return &m_PreLoadEfflist;	}
	cPtrList m_PreLoadModlist;
	cPtrList* GetPreLoadModlist()	{	return &m_PreLoadModlist;	}
	///////////////////////////////////////////////////////////////////////////////
	// 06. 04. PreLoadData 추가기능 - 이영준
	WORD* m_PreLoadItemlist;
	WORD* GetPreLoadItemlist()	{	return m_PreLoadItemlist;	}
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	// 06. 05. PreLoadData 추가기능 - 이영준
	CYHHashTable<sPRELOAD_EFFECT_INFO>		m_PreLoadEffectTable;
	sPRELOAD_EFFECT_INFO*	GetPreEffectInfo(int MapNum)		{	return m_PreLoadEffectTable.GetData(MapNum);	}
	///////////////////////////////////////////////////////////////////////////////

	void PreLoadData();	
	
	// serverlist
	SEVERLIST	m_ServerList[30];
	int			m_nMaxServerList;
	BOOL		LoadServerList();
	char*		GetServerSetName();			// 현재서버셋의 이름을 리턴

#ifdef _TESTCLIENT_
	TESTCLIENTINFO m_TestClientInfo;
	BOOL LoadTestClientInfo();
#endif

//-----------------------------------------------------------------------------------------------------------//
#endif //_CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------------//
//		¼­¹ö¸¸ »ç¿ëÇÏ´Â ºÎºÐ
#ifdef _SERVER_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
	
//	void GetMonsterStats(WORD wMonsterKind, MONSTER_STATS * stats);

	VECTOR3 m_RevivePoint;
	// 071201 KTH - SafetyZone 정보를 담고 있는 Map
	std::map<MAPTYPE, VECTOR3> m_RevivePointMap;

	void LoadRevivePoint();
	// 090603 LUJ, 반복 코드 제거
	inline VECTOR3* GetRevivePoint() { return GetRevivePoint( m_LoadingMapNum ); }
	// 071201 KTH - 해당 맵의 SaftyZone을 가져옴.
	// 090603 LUJ, 부활 지점이 없더라도 종료되지 않도록, 기본값을 반환하도록 함
	VECTOR3* GetRevivePoint( MAPTYPE mapIndex )
	{
		static VECTOR3 emptyPosition;
		std::map< MAPTYPE, VECTOR3 >::iterator it = m_RevivePointMap.find( mapIndex );
		
		return m_RevivePointMap.end() == it ? &emptyPosition : &( it->second );
	}

	MAPTYPE m_LoadingMapNum;
	MAPTYPE GetLoadMapNum() const { return m_LoadingMapNum; }
	void SetLoadMapNum(MAPTYPE);
	NPC_LIST* GetNpcList(WORD JobKind);

	// 06. 05 HIDE NPC - 이영준
	CYHHashTable<HIDE_NPC_INFO>		m_HideNpcTable;
	BOOL	LoadHideNpcList();
	HIDE_NPC_INFO* GetHideNpcInfo(WORD UniqueIdx) { return m_HideNpcTable.GetData(UniqueIdx); }
	CYHHashTable<HIDE_NPC_INFO>* GetHideNpcTable() { return &m_HideNpcTable; }

//---KES 스킬초기화돈	071207
private:
	CYHHashTable<SKILL_MONEY>	m_htSkillMoney;
public:
	BOOL LoadSkillMoney();
	DWORD GetSkillMoney( DWORD SkillIdx );
//----------------------------

	// 090227 ShinJS --- 암상인추가작업 (StaticNPC를 추가/삭제할수 있도록 함, 암상인 추가정보 별도생성:맵에 로그인시 활용)
public:
	CYHHashTable<STATIC_NPCINFO>* GetMoveNpcInfoTable() { return &m_htMoveNpcStaticInfo; }
	void AddMoveNpcInfo( WORD wNpcIdx, WORD wNpcKindIdx, WORD wXPos, WORD wZPos, WORD wDir );	// 이동NPC 추가
	void RemoveMoveNpcInfo( WORD wNpcUniqueIdx );												// 이동NPC 제거
	void RemoveStaticNpcInfo( WORD wNpcUniqueIdx );	

//-----------------------------------------------------------------------------------------------------------//
#endif //_SERVER_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
};
EXTERNGLOBALTON(CGameResourceManager);
#endif // !defined(AFX_GAMERESOURCEMANAGER_H__8C774E0A_9D8C_4535_A3B2_F847517FA424__INCLUDED_)
