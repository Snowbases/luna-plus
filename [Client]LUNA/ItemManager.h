#pragma once


#define ITEMMGR CItemManager::GetInstance()


#include "item.h"


class CHero;
class CPlayer;
class cImage;
class cWindow;
class CVirtualItem;
class CItemShow;
class cQuickItem;
class CExchangeItem;
class CBuyItem;

// 090114 LYW --- ItemManager : 장비 착용 가능 여부 결과를 분류한다.
enum RESULT_CANEQUIP
{
	eEquip_Disable = 0,				// 착용불가
	eEquip_Able,					// 착용가능
	eEquip_RaceFail,				// 종족 제한으로 착용불가
	eEquip_GenderFail,				// 성별 제한으로 착용불가
	eEquip_LevelFail,				// 제벨 제한으로 착용불가
	eEquip_FishingLevelFail,		// 낚시 숙련도 부족으로 착용불가
	eEquip_CookingLevelFail,		// 요리 등급 부족으로 착용불가
};

struct EnchantEffect
{
	DWORD	ItemIdx;

	WORD	Effect[13][3];
};

struct EVENTINFO
{
	DWORD	Index;
	DWORD	Delay;
	DWORD	Next;
	DWORD	Item;
	DWORD	Count;
	DWORD	Window;
	char*	Msg;
};

// 090812 ShinJS --- 조합: 결과 Item으로 기본재료 Item Index를 구하기 위한 정보
struct stMixResultItemInfo
{
	DWORD dwResultItemIdx;		// 조합 결과물 Item Index
	DWORD dwBaseItemIdx;		// 결과 Item Index 와 매칭되는 재료 Item Index
	BOOL bOverlappedResult;		// 결과물이 중복된 경우(체크 된 경우 카테고리 조합창 Update시 표시하지 않는다)
};

class CItemManager  
{
	DWORD m_GiftEventTime;
	CYHHashTable<EVENTINFO>	mEventInfoTable;
	CYHHashTable<CItem> m_ItemHash;

	CYHHashTable<ITEM_INFO> m_ItemInfoList;

	CYHHashTable<EnchantEffect> mEffectList;
	// 100511 ShinJS --- 전체 툴팁
	std::map< DWORD, std::string > m_ToolTipTextMap;
	// 100511 ShinJS --- 타이틀 길이(작은경우 특정 길이) 만큼 잘려진 툴팁
	std::map< DWORD, std::vector< std::string > > m_CutToolTipText;

	// 키: 아이템 DB 인덱스
	typedef stdext::hash_map< DWORD, ITEM_OPTION* >	OptionMap;
	OptionMap										mOptionMap;
	//CYHHashTable<SHOPITEMBASE>		m_UsedItemList;

	CIndexGenerator m_IconIndexCreator;
	BOOL			m_bAddPrice;
	ITEMBASE		m_TempDeleteItem;

	//---KES ItemDivide Fix 071020
	POSTYPE			m_DividePos;
	int				m_DivideDur;
	//----------------------------

	// 071127 LYW --- ItemManager : 삭제 아이템 ( 봉인 관련 ) 위치 저장 변수 추가.
	int			m_nSealItemDelPos ;

public:
	static CItemManager* GetInstance();
	CItemManager();
	virtual ~CItemManager();

	//---KES ItemDivide Fix 071020
	void SetDivideItemInfo( POSTYPE pos, int dur )	{ m_DividePos = pos; m_DivideDur = dur; }
	POSTYPE GetDivideItemInfoPos()					{ return m_DividePos; }
	int		GetDivideItemInfoDur()					{ return m_DivideDur; }
	//----------------------------

	void				AddOption	( const ITEM_OPTION& );
	const ITEM_OPTION&	GetOption	( const ITEMBASE& ) const;
	const ITEM_OPTION&	GetOption	( DWORD itemDbIndex ) const;
	void				RemoveOption( DWORD itemDbIndex );


	void Release();

	CItem* MakeNewItem( const ITEMBASE*, const char* strKind);

	void ItemDelete(CItem* pItem);
	void FakeDeleteItem(CItem* pItem);
	void SendDeleteItem();

	cImage* GetIconImage(DWORD ItemIdx, cImage * pImageGet);
	CItem* GetItem(DWORD dwDBIdx);


public:
	void AddToolTip( cWindow*, DWORD toolTipIndex, DWORD color = TTTC_DESCRIPTION );
	void AddToolTip( cIcon* );
	void AddToolTip( CVirtualItem* );
	void AddToolTip( CItemShow* );
	void AddToolTip( CItem* );
	void AddToolTip( cQuickItem* );
	void AddToolTip( CExchangeItem* );
	void AddToolTip( CBuyItem* );

	// 071125 LYW --- ItemManager : 시간 툴팁 전용 설정함수 추가.
	void AddTimeToolTip( cIcon* icon, const ITEM_OPTION&, BOOL isFirst ) ;

private:
	void AddNametoolTip			( cIcon&, const ITEMBASE& );
	void AddOptionToolTip		( cIcon&, const ITEMBASE& );
	void AddFunctionToolTip		( cIcon&, const ITEMBASE& );
	void AddMakerToolTip		( cIcon&, const ITEMBASE& );
	void AddSetToolTip			( cIcon&, const ITEMBASE& );
	void AddPetToolTip			( cIcon&, const ITEMBASE& );
	void AddDescriptionToolTip	( cIcon&, const ITEMBASE&, DWORD color = TTTC_DESCRIPTION );
	
	// 071115 LYW --- ItemManager : 남은 사용시간 툴팁 추가.
	void AddRemainTimeToolTip( cIcon&, const ITEMBASE&, BOOL bFirst );

	// 071126 LYW --- ItemManager : 남은 사용시간 툴팁 추가.
	void AddUseableTimeToolTip( cIcon&, const ITEMBASE& );

	// 090430 pdy 하우징 가구 상태에 따른 툴팁 추가 
	void AddFurnitureStateToolTip	( cIcon&, const ITEMBASE& );

	// 090430 pdy 하우징 가구 옵션에 따른 툴팁 추가
	void AddFurnitureOptionToolTip	( cIcon&, const ITEMBASE& );

	// 090430 pdy 하우징 가구 속성에 따른 툴팁 추가 
	void AddFurnitureFunctionToolTip( cIcon&, const ITEMBASE& );


public:
	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	void MoveItem( MSG_ITEM_MOVE_ACK*);
	void PetMoveItem( MSG_ITEM_MOVE_ACK*);
	void DivideItem(ITEMOBTAINARRAY*);
	void MoveItemToGuild(MSG_ITEM_MOVE_ACK*);
	void MoveItemToStorage(MSG_ITEM_MOVE_ACK*);
	void MoveItemToShop(MSG_ITEM_MOVE_ACK*);
	void CombineItem(MSG_ITEM_COMBINE_ACK*);
	void DeleteItem(POSTYPE, CItem**);
	BOOL AddItem(CItem*);
	eITEMTABLE GetTableIdxForAbsPos(POSTYPE);
	CItem* GetItemofTable(eITEMTABLE, POSTYPE);
	const ITEMBASE* GetItemInfoAbsIn(CPlayer*, POSTYPE);
	void DeleteItemofTable(eITEMTABLE, POSTYPE, BOOL bSellDel);
	BOOL IsEqualTableIdxForPos(eITEMTABLE, POSTYPE);
	void RefreshAllItem();
	void RefreshItem( CItem* pItem );
	void RefreshItemToolTip(DWORD ItemDBIdx);
	RESULT_CANEQUIP CanEquip(DWORD wItemIdx);
	BOOL CanUse(DWORD wItemIdx);
	void LoadItemList();
	void LoadItemToolTipList();
	void LoadItemEffectList();
	void LoadGiftEvent();

	EnchantEffect* GetItemEffect(DWORD ItemIdx);
	ITEM_INFO* GetItemInfo(DWORD wItemIdx);
	const char* GetName(EWEARED_ITEM) const;
	const char* GetName(ITEM_OPTION::Drop::Key) const;
	void SetPreItemData(sPRELOAD_INFO*, int* Level, int Count);
	DWORD m_dwStateParam;

	//SW050920 Rare
	//BOOL IsRareOptionItem( DWORD wItemIdx, DWORD dwRareIdx );
	BOOL IsRare( const ITEM_INFO* ) const;
	BOOL IsDupItem( DWORD wItemIdx );
	
	void SetPriceToItem( BOOL bAddPrice );
	void SetDisableDialog(BOOL val, BYTE TableIdx);

	// 타이틀길이(짧은 경우 특정 길이)만큼 잘려진 툴팁 반환
	const std::vector< std::string >& GetToolTipMsg( DWORD dwIndex );
	// 전체 툴팁 반환
	const std::string& GetTotalToolTipMsg( DWORD dwIndex );
	// 전체 툴팁에서 특정 길이만큼 잘라서 전달받은 컨테이너에 저장한다.
	void CutToolTipMsg( const std::string totalToolTip, std::vector< std::string >& cutToolTipList, const int nCutWidth );
	ITEM_INFO* FindItemInfoForName(char*);

	void ReLinkQuickPosition(CItem* pItem);	
	void Process();
	void ItemDropEffect( DWORD wItemIdx );

#ifdef _GMTOOL_
	void SetItemIfoPositionHead()	{ m_ItemInfoList.SetPositionHead(); }
	ITEM_INFO* GetItemInfoData()	{ return m_ItemInfoList.GetData(); }
#elif _TESTCLIENT_
	void SetItemIfoPositionHead()	{ m_ItemInfoList.SetPositionHead(); }
	ITEM_INFO* GetItemInfoData()	{ return m_ItemInfoList.GetData(); }
#endif

private:
	void Item_TotalInfo_Local( void* pMsg ) ;
	void Item_Storage_Item_Info_Ack( void* pMsg ) ;
	//void Item_Monster_RareObtain_Notify( void* pMsg ) ;
	void Item_ReInForce_Ack( void* pMsg ) ;
	void Item_Reinforce_Option_Ack( void* );
	void Item_ReInForce_Nack( void* pMsg ) ;
	void Item_Move_Ack( void* pMsg ) ;
	void Item_Pet_Move_Ack( void* pMsg ) ;
	void Item_Combine_Ack( void* pMsg ) ;
	void Item_Divide_Ack( void* pMsg ) ;
	void Item_Monster_Obtain_Notify( void* pMsg ) ;	
	void Item_HouseRestored( void* pMsg ) ;//090529 pdy 하우징 보관해제 추가 
	void Item_HouseBonus( void* pMsg ) ; //090618 pdy 하우징 꾸미기 보너스 아이템보상 추가 
	void Item_Move_Nack( void* pMsg ) ;
	void Item_Divite_Nack( void* pMsg ) ;
	void Item_Combine_Nack( void* pMsg ) ;
	void Item_Discard_Nack( void* pMsg ) ;
	void Item_Discard_Ack( void* pMsg ) ;
	void Item_Use_Notify( void* pMsg ) ;
	void Item_Use_Ack( void* pMsg ) ;



private:
	void Item_Cook_Ack( void* pMsg ) ;
	void Item_Cook_GetItem(void*);

	void Item_Mix_Ack( void* pMsg ) ;
	void Item_Mix_Nack( void* pMsg );
	void Item_Mix_GetItem( void* );
	void Item_Working_Success( void* pMsg ) ;
	void Item_Working_Fail( void* pMsg ) ;
	void Item_Working_Start( void* pMsg ) ;
	void Item_Working_StartEx( void* pMsg ) ;
	void Item_Working_Stop( void* pMsg ) ;
	void Item_TimeLimit_Item_OneMinute( void* pMsg ) ;
	void Item_ShopItem_Use_Ack( void* pMsg ) ;
	void Item_Enchant_Success_Ack( void* pMsg ) ;
	void Item_Enchant_Failed_Ack( void* pMsg );
	void Item_Enchant_Nack( void* pMsg ) ;
	void Item_Use_Nack( void* pMsg ) ;
	void Item_Sell_Ack( void* pMsg ) ;
	void Item_Sell_Nack( void* pMsg ) ;
	void Item_Buy_Ack( void* pMsg ) ;
	void Item_Buy_Nack( void* pMsg ) ;
	void Item_Buy_By_Coin( void* pMsg );				// 090227 ShinJS --- 암상인 추가작업 : Coin Item 구입시 Coin 제거 작업
	void Item_Dealer_Ack( void* pMsg ) ;
	//void Item_Appearance_Change( void* pMsg ) ;
	void Item_Appearance_Add( void* );
	void Item_Appearance_Remove( void* );
	void Item_Money( void* pMsg ) ;
	void Item_Obtain_Money( void* pMsg ) ;
	void Item_Divide_NewItem_Notify( void* pMsg ) ;
	void Item_Guild_Move_Ack( void* pMsg ) ;
	void Item_Guild_Move_Nack( void* pMsg ) ;
	//void Item_Guild_WareHouse_Info_Ack( void* pMsg ) ;
	//void Item_Guild_WareHouse_Info_Nack( void* pMsg ) ;
	void Item_DeleteFrom_GuildWare_Notify( void* pMsg ) ;
	void Item_AddTo_GuildWare_Notify( void* pMsg ) ;
	void Item_Storage_Move_Ack( void* pMsg ) ;
	void Item_Dissolve_GetItem( void* pMsg ) ;
	void Item_Dissolve_Ack( void* pMsg ) ;
	void Item_Dissolve_Nack( void* pMsg ) ;
	void Item_Error_Nack( void* pMsg ) ;
	void Item_ChangeItem_Nack( void* pMsg ) ;
	void Item_Use_ChangeItem_Ack( void* pMsg ) ;
	void Item_EventItem_Use( void* pMsg ) ;
	void Item_ShopItem_Info_Ack( void* pMsg ) ;
	// 071204 LYW --- ItemManager :
	void Item_Reset_Stats_Ack( void* pMsg ) ;
	// 071208 LYW --- ItemManager : 스탯 초기화 실패 처리.
	void Item_Reset_Stats_Nack( void* pMsg ) ;
	// 071210 LYW --- ItemManager : 인벤토리 확장 성공, 실패 함수 추가.
	void Item_Increase_Inventory_Ack( void* pMsg ) ;
	void Item_Increase_Inventory_Nack( void* pMsg ) ;
	// 071226 LYW --- ItemManager : 이름 변경 아이템 사용 성공, 실패 함수 추가.
	void Item_Change_Character_Name_Ack( void* pMsg ) ;
	void Item_Change_Character_Name_Nack( void* pMsg ) ;
	void Item_Skill_Ack(LPVOID);
	void Item_Gift_Event_Notify(LPVOID);
	void CompositString(char* inputStr, char* str, ...);

public:
	int	GetSetItemSize(CPlayer*, const SetScript*);
	const MixScript* GetMixScript( DWORD itemIndex ) const;
	const ItemMixResult* GetMixResult( DWORD itemIndex, DWORD level ) const;
	CYHHashTable<stMixResultItemInfo>& GetMixInfoForScriptTable() { return m_htMixResultItemInfo; }
	void GetMixResultFromResultItemIdx( DWORD resultItemIdx, CYHHashTable<ItemMixResult>* pEqualMixScript );		// 결과 아이템으로 조합가능한 모든 스크립트를 구한다
	const DWORD	GetMixResultPosInfo( DWORD BaseItemIdx, ItemMixResult* pMixResult );								// 기본재료 아이템으로 Mix Script List에서 현재선택된 Script의 위치를 구한다.

	// 080228 LUJ, 조합 보호 아이템 여부
	BOOL IsMixProtectionItem( const ITEMBASE& ) const;
	// 080925 LUJ, 조합 보조 정보를 가져온다
	const MixSupportScript* GetMixSupportScript( DWORD itemIndex ) const;
	void LoadMixScript();
private:
	// 키: 아이템 인덱스
	typedef stdext::hash_map< DWORD, MixScript >	MixScriptMap;
	MixScriptMap									mMixScriptMap;

	// 080228 LUJ, 조합 보호 아이템 관리
	typedef stdext::hash_set< DWORD >	MixProtectionItemSet;
	MixProtectionItemSet				mMixProtectionItemSet;
	// 080925 LUJ, 조합 보조 정보 관리. 키: 아이템 인덱스
	typedef stdext::hash_map< DWORD, MixSupportScript >	MixSupportScriptMap;
	MixSupportScriptMap									mMixSupportScriptMap;

	// 090812 ShinJS --- 조합: 결과 Item으로 script를 구하기 위한 정보
	CYHHashTable<stMixResultItemInfo>	m_htMixResultItemInfo;

	// 070709 아이템 이름 색상을 가져옴
public:
	DWORD	GetItemNameColor( const ITEM_INFO& ) const;


	// 070710 웅주, 분해 아이템 관리
public:
	const DissolveScript*	GetDissolveScript( DWORD itemIndex ) const;
	void LoadDissolveScript();
private:
	// 키: 아이템 인덱스
	typedef stdext::hash_map< DWORD, DissolveScript >	DissolveScriptMap;
	DissolveScriptMap									mDissolveScriptMap;

public:
	DWORD GetCoolTimeGroupIndex( DWORD itemIndex ) const;

	// 해당 아이템이 쿨타임 중인지 체크한다.
	BOOL IsCoolTime( DWORD playerIndex, DWORD itemIndex ) const;

	// 아이템을 쿨타임 상태로 등록하거나 해제
	BOOL AddCoolTime( DWORD playerIndex, DWORD itemIndex );

	// 플레이어의 쿨타임 정보를 지운다.
	void RemoveCoolTime( DWORD playerIndex );

	// 080326 NYJ --- 쿨타임갱신
	void UpdateDlgCoolTime(DWORD itemIndex);	//HERO에게만 사용됨.
	void LoadCoolTimeScript();
private:
	void ProcessCoolTime();

	struct CoolTimeScript
	{
		struct Unit
		{
			DWORD mIndex;	// 쿨타임 그룹 인덱스
			DWORD mTime;	// 쿨타임 적용 시간: 밀리세컨드
		};

		// 키: 아이템 인덱스
		typedef stdext::hash_map< DWORD, Unit >	ItemData;
		ItemData								mItemData;

		// 값: 아이템 인덱스
		typedef stdext::hash_set< DWORD >				ItemIndex;

		// 키: 그룹 인덱스
		typedef stdext::hash_map< DWORD, ItemIndex >	GroupData;
		GroupData										mGroupData;
	}
	mCoolTimeScript;	

	// 쿨타임이 빨리 해제되는 순으로 정렬시키는데 사용하는 유틸리티 클래스.
	class CoolTimeSort
	{
	public:
		struct Tick
		{
			DWORD mPlayerIndex;		// 해당 쿨타임 유닛을 소유한 플레이어 인덱스
			DWORD mCoolTimeGroup;	// 쿨타임 그룹 인덱스
			DWORD mBegin;			// 쿨타임 시작 시간
			DWORD mEnd;				// 쿨타임 해제 시간
		};

		bool operator() ( const Tick& lhs, const Tick& rhs ) const
		{
			const DWORD	 maxTick		= UINT_MAX;
			const DWORD lhsRemainTime	= ( lhs.mBegin > lhs.mEnd ? maxTick - lhs.mBegin + lhs.mEnd : lhs.mEnd - lhs.mBegin );
			const DWORD rhsRemainTime	= ( rhs.mBegin > rhs.mEnd ? maxTick - rhs.mBegin + rhs.mEnd : rhs.mEnd - rhs.mBegin );

			return lhsRemainTime > rhsRemainTime;
		}
	};

	// 남은 시간이 적게 
	// 주의사항: 자료는 반드시 값 복사로 넣어야한다. 포인터로 넣을 경우 정렬하면서 힙 주소를 잃어버린다.
	std::priority_queue< CoolTimeSort::Tick, std::vector< CoolTimeSort::Tick >, CoolTimeSort > mCoolTimeQueue;

	// 값: 쿨타임 그룹
	typedef stdext::hash_set< DWORD >	CoolTimeGroup;

	// 키: 플레이어 인덱스, 값: 사용 중인 쿨타임 아이템 그룹 인덱스
	typedef stdext::hash_map< DWORD, CoolTimeGroup >	UsingCoolTimeGroup;
	UsingCoolTimeGroup									mUsingCoolTimeGroup;

	// 080326 NYJ --- 쿨타임정보 저장
	std::map<DWORD, CoolTimeSort::Tick> m_mapCoolTime;


	// 070927. 변환 아이템 사용시 프로그레스바 표시하도록 함
public:
	BOOL IsChangeItem( DWORD itemIndex ) const;

	// 071203 LYW --- ItemManager : 스탯 초기화 아이템 처리.
	void UseItem_ResetStats(CItem* pItem) ;
	// 071203 LYW --- ItemManager : 스킬 초기화 아이템 처리.
	void UseItem_ResetSkill(CItem* pItem) ;
	// 071203 LYW --- ItemManager : 맵 이동 스크롤 처리.
	void UseItem_MapMoveScroll(CItem* pItem) ;
	void Item_MapMoveScroll_Nack( void* pMsg ) ;
	// 071206 LYW --- ItemManager : 창고 확장 아이템 처리.
	void UseItem_ExtendStorage(CItem* pItem) ;
	// 080611 LYW --- 스킬 포인트 추가 주문서 처리.
	void GetSkillPointByItem(CItem* pItem) ;

	// 080826 LYW --- ItemManager : 공성전에서 사용하는 워터시드 사용함수 추가.
	void UseItem_WarterSeed(CItem* pItem) ;

	// 071210 LYW --- ItemManager : 인벤토리 확장 아이템 처리.
	void UseItem_ExtendInventory(CItem* pItem) ;
	// 071206 LYW --- ItemManager : 이미 사용하여 버프가 활성화 중인지 체크하는 함수 추가.
	BOOL IsUsingItem(CItem* pItem) ;
	// 071206 LYW --- ItemManager : 아이템몰 아이템 사용 함수 추가.
	void UseItem_ItemMallItem(CItem* pItem, CObject* pTarget) ;
	// 071228 LYW --- ItemManager : 이름 변경 아이템 사용 함수 추가.
	void UseItem_ChangeName(char* pName) ;

	// 080218 KTH -- Reset RegistResident
	void UseItem_ResetRegistResident(CItem* pItem);

	// 080312 NYJ --- ItemManager : 전체외치기 아이템 처리.
	void UseItem_WorldShout(CItem* pItem) ;
	// 080326 NYJ --- 쿨타임정보 초기화
	void ClearCoolTimeInfo() {m_mapCoolTime.clear();}
	// 080820 LUJ, 획득 시 쿨타임을 가진 아이템은 이동 시에 시간을 업데이트해줘야한다. 서버는 알아서 갱신한다
	void UpdateCoolTimeItem( CItem& );
	void LoadChangeItem();
private:
	typedef stdext::hash_set< DWORD >	ChangeItemSet;
	ChangeItemSet						mChangeItemSet;

	// 080414 LUJ, 외양 변환 스크립트 처리
public:
	struct BodyChangeScript
	{
		DWORD		mGender;
		RaceType	mRace;
		PART_TYPE	mPart;
		
		// 080414 LUJ, 텍스트 번호(InterfaceMsg.bin 참조)
		DWORD		mText;
		DWORD		mValue;
	};

	typedef std::list< BodyChangeScript >					BodyChangeScriptList;
	typedef stdext::hash_map< DWORD, BodyChangeScriptList >	BodyChangeScriptListMap;
	
	const BodyChangeScriptListMap&	GetBodyChangeScriptListMap() const;
	void LoadBodyChangeScript();
private:	
	BodyChangeScriptListMap	mBodyChangeScriptListMap;

	// 080916 LUJ, 합성 스크립트
public:
	BOOL IsGiftEvent() const { return 0 < m_GiftEventTime; }
	WORD GetItemStackNum(DWORD dwItemIndex);
	void UseItem_ChangeClass(CItem*);

// 100609 ONS +10이상 인챈트 공지기능 추가
private:
	struct EnchantNotifyInfo
	{
		DWORD	ItemIndex;
		DWORD	EnchantLevel;
		BYTE	IsSuccessed;
		char	Name[MAX_NAME_LENGTH + 1];
	};
	std::queue<EnchantNotifyInfo>			m_EnchantNotifyQueue;
	DWORD									m_ENchantNotifyCheckTime;
public:
	void EnchantNotifyAdd( void* pMsg );

public:
	WORD GetItemList(const char* pKeyWord, const char* pCategory1, const char* pCategory2, int nRareLevel, WORD wMinLevel, WORD wMaxLevel, WORD wNum, DWORD* pItemList);
};