#pragma once

#define ITEMMGR CItemManager::GetInstance()

class CItemSlot;
class CBackupSlot;

struct DealerItem
{
	BYTE Tab;
	BYTE Pos;
	DWORD ItemIdx;
};

typedef void (*DBResult)(void * pPlayer, WORD ArrayID);

struct ITEMOBTAINARRAYINFO
{
	DWORD wObtainArrayID;
	DBResult CallBack;
	DWORD dwFurnisherIdx;
	eLogitemmoney wType;
	ITEMOBTAINARRAY	ItemArray;
};

struct DealerData
{
	BOOL FindItem(DWORD ItemIdx)
	{
		POS pos = m_DealItemList.GetFirstPos();
		DealerItem* pItem = NULL;
		while((pItem = m_DealItemList.GetNextPos(pos) ) != NULL)
		{
			if(pItem->ItemIdx == ItemIdx)
				return TRUE;
		}

		return FALSE;
	}

	cLinkedList<DealerItem> m_DealItemList;
};

// 071028 LYW --- ItemManager : Add enum code for error of discard item from quest group.
enum QG_DISCARD_ERR															// 퀘스트 그룹에서 아이템 버리기를 할 때 발생하는 에러 정의.
{
	err_No_Error = 10,														// 에러 없음.
	err_Invalid_CItemSlot,													// 아이템 슬롯 포인터 에러.
	err_Invalid_ITEMBASE,													// 아이템 기본 포인터 에러.
	err_Not_Same_ItemIdx,													// 아이템 인덱스 일치 하지 않음.
	err_Not_StackItem,														// 스택 아이템이 아닐경우 에러.
	err_Not_NormalItem,														// 일반 아이템이 아닐경우 에러.
	err_Over_ItemCount,														// 최대 스택 카운트 오버 에러.
	err_Invalid_RemainCount,												// 남은 아이템 수 에러.
	err_Failed_UpdateItem,													// 아이템 업데이트 에러.
	err_Failed_DeleteItem,													// 아이템 삭제 에러.
};

class CMHFile;

class CItemManager  
{
	// desc_hseos_체인지아이템01
	// S 체인지아이템 추가 added by hseos 2007.07.29
	enum CHANGEITEM_RESULT_KIND										// 체인지 아이템 결과 종류
	{
		TRK_ITEM = 0,												// ..아이템
		TRK_MONEY,													// ..돈
	};

	struct stCHANGEITEM_RESULT_BASE
	{
		CHANGEITEM_RESULT_KIND	eResultKind;						// ..결과 종류
		DWORD					nValue01;							// ..수치1
		DWORD					nValue02;							// ..수치2
		DWORD					nValue03;							// ..수치3
		DWORD					nProbaValue;						// ..확률
	};

	struct stCHANGEITEM_RESULT
	{
		int						  nResultBaseNum;
		stCHANGEITEM_RESULT_BASE* stResultBase;
	};

	struct stCHANGEITEM												// 체인지 아이템 구조
	{
		int						nChangeItemID;						// ..체인지 아이템 ID
		int						nResultNum;							// ..결과 개수
		stCHANGEITEM_RESULT*	stResult;							// ..결과
	};
	// E 체인지아이템 추가 added by hseos 2007.07.29

	CYHHashTable<DealerData>	m_DealerTable;
	stCHANGEITEM*			m_stChangeItemInfo;
	int						m_nChangeItemInfoNum;
	// E 체인지아이템 추가 added by hseos 2007.07.29

public:
	CItemManager();
	virtual ~CItemManager();
	//GETINSTANCE(CItemManager);

	static CItemManager* GetInstance();

	void LoadDealerItem();
	DealerData * GetDealer(WORD wDealerIdx);
	int DivideItem( CPlayer*, DWORD ItemIdx,  POSTYPE FromPos, POSTYPE ToPos, DURTYPE FromDur, DURTYPE ToDur);
	int CombineItem( CPlayer*, DWORD ItemIdx, POSTYPE FromPos, POSTYPE ToPos, DURTYPE & FromDur, DURTYPE & ToDur);
	int MoveItem( CPlayer*, DWORD FromItemIdx, POSTYPE FromPos, DWORD ToItemIdx, POSTYPE& ToPos);
	int MovePetItem( CPlayer*, DWORD FromItemIdx, POSTYPE FromPos, DWORD ToItemIdx, POSTYPE& ToPos);
	int DiscardItem(CPlayer*, POSTYPE whatPos, DWORD whatItemIdx, DURTYPE whatItemNum);

	WORD GetItemStackNum(DWORD dwItemIndex);
	int QG_Discard_StackItem(CPlayer*, POSTYPE, DWORD whatItemIdx, int* nGiveNum);
	int QG_Discard_NormalItem(CPlayer*, POSTYPE, DWORD whatItemIdx, int* nGiveNum);
	void SendErrorMessage(CObject*, BYTE protocol, int errorCode);
	int ObtainItemEx(CPlayer*, ITEMOBTAINARRAYINFO*, DWORD whatItemIdx, WORD whatItemNum, WORD* EmptyCellPos, WORD EmptyCellNum, WORD ArrayInfoUnitNum, ITEM_SEAL_TYPE);	
	int SellItem(CPlayer*, POSTYPE whatPos, DWORD wSellItemIdx, DURTYPE sellItemDur, DWORD DealerIdx );
	int BuyItem(CPlayer*, DWORD buyItemIdx, WORD buyItemNum, WORD DealerIdx );
	void MonsterObtainItem(CPlayer*, DWORD obtainItemIdx, DWORD dwFurnisherIdx, WORD ItemNum = 1);
	int CheatObtainItem(CPlayer*, DWORD obtainItemIdx, WORD ItemNum);
	int ObtainMonstermeterItem(CPlayer*, DWORD obtainItemIdx, WORD ItemNum);
	int ObtainGeneralItem(CPlayer*, DWORD obtainItemIdx, WORD ItemNum, eLogitemmoney, MP_PROTOCOL_ITEM);
    static void ObtainItemDBResult(CPlayer*, WORD ArrayID);
	static void BuyItemDBResult(CPlayer*, WORD ArrayID );	
	static void DivideItemDBResult(CPlayer*, WORD ArrayID);
	//SW050920 Rare
	//static void RareItemDBResult( CPlayer*, WORD ArrayID, ITEM_RARE_OPTION_INFO* );

	void NetworkMsgParse( DWORD dwConnectionIndex, BYTE Protocol, void* );

	void LoadItemList();
	void ReloadItemList();
	void SetItemInfo(DWORD ItemIdx,ITEM_INFO*, CMHFile*);
	ITEM_INFO* GetItemInfo(DWORD wItemIdx);
	eItemUse_Err UseItem(CPlayer*, POSTYPE, DWORD wItemIdx);
	ITEM_INFO* GetUseItemInfo(CPlayer*, POSTYPE, DWORD wItemIdx);
	ITEMOBTAINARRAYINFO* Alloc(CPlayer*, MP_CATEGORY, BYTE protocol, DWORD dwObjectID, DWORD dwFurnisherIdx, eLogitemmoney, WORD ObtainNum, DBResult);
	void Free(CPlayer*, ITEMOBTAINARRAYINFO*);
	BOOL IsDupItem(DWORD wItemIdx);
	BOOL IsTwoHand(DWORD wItemIdx);
	BOOL IsRare(const ITEM_INFO&) const;
	void UpdateResult(CPlayer&, CItemSlot&, const CBackupSlot&, ITEM_UPDATE_RESULT&, MP_PROTOCOL_ITEM, eLogitemmoney logForRemove, eLogitemmoney logForInsert, const ICONBASE& itemForMixSupport);
	BOOL ItemMoveUpdateToDBbyTable(CPlayer*, DWORD dwfromDBIdx, DWORD dwfromIconIdx,  POSTYPE frompos, DWORD dwtoDBIdx, POSTYPE topos);
	void ItemUpdatebyTable(CPlayer*, DWORD dwDBIdx, DWORD wIconIdx, DURTYPE FromDur, POSTYPE Position, POSTYPE QuickPosition);
	const ITEMBASE * GetItemInfoAbsIn(CPlayer*, POSTYPE Pos);

	void ItemMoveLog(POSTYPE FromPos, POSTYPE ToPos, CPlayer*, ITEMBASE* pItem);

	// 071211 KTH -- CPlayer 추가
	unsigned int GetTotalEmptySlotNum(CItemSlot*, CPlayer*); //060612 Add by wonju

	// desc_hseos_체인지아이템01
	// S 체인지아이템 추가 added by hseos 2007.07.29	2007.08.03
	BOOL LoadScriptFileDataChangeItem();
	BOOL ProcChangeItem(CPlayer* pPlayer, DWORD nItemID);
	BOOL IsChangeItem(DWORD nItemID);
	// E 체인지아이템 추가 added by hseos 2007.07.29	2007.08.03

	WORD CheckExtraSlot(CPlayer*, CItemSlot*, DWORD whatItemIdx, DURTYPE whatItemNum, WORD * EmptyCellPos, WORD & EmptyCellNum);
	WORD GetCanBuyNumInSpace(CPlayer*, CItemSlot*, DWORD whatItemIdx, DURTYPE whatItemNum, WORD * EmptyCellPos, WORD & EmptyCellNum);
private:
	
	WORD GetCanBuyNumInMoney(CPlayer*, WORD butNum, MONEYTYPE Price);
	WORD GetCanBuyNumInFishingPoint(CPlayer*, WORD buyNum, MONEYTYPE Point);
	BOOL CanMovetoGuildWare(POSTYPE FromPos, POSTYPE ToPos, CPlayer*);
	BOOL CanbeMoved(DWORD wIconIdx,POSTYPE pos, CPlayer*);
	BOOL CanEquip(ITEM_INFO*, CPlayer*);
	void ClearQuickPos(ITEMBASE*, POSTYPE ToPos);

	CYHHashTable<ITEM_INFO> m_ItemInfoList;
	// 090923 ONS 메모리풀 교체
	CPool<ITEMOBTAINARRAYINFO> m_ItemArrayPool;
	CIndexGenerator		m_ArrayIndexCreator;

public:
	int	ObtainItemFromQuest( CPlayer*, DWORD wItemKind, DWORD dwItemNum );
	eWeaponType	GetWeaponKind(DWORD wWeapon);
	int	ObtainItemFromChangeItem( CPlayer*, DWORD wItemKind, WORD wItemNum );
	BOOL CheckHackNpc( CPlayer*, WORD wNpcIdx, WORD wParam=0 );	
	BOOL CheckHackItemMove( CPlayer*, CItemSlot* pFromSlot, CItemSlot* pToSlot );
	BOOL IsValidItem(DWORD nItemID)	{ if (m_ItemInfoList.GetData(nItemID)) return TRUE ; return FALSE; }
	int GetSetItemSize(CPlayer*, const SetScript*);
	eLogitemmoney Reinforce(CPlayer*, const MSG_ITEM_REINFORCE_SYN&, const CBackupSlot*);
	MP_PROTOCOL_ITEM Enchant(CPlayer*, const MSG_ITEM_ENCHANT_SYN*, eLogitemmoney&);
	void ApplyOption(const ITEMBASE& sourceItem, const ITEMBASE& targetItem, ITEM_OPTION&);

private:
	eLogitemmoney Mix(CPlayer*, const MSG_ITEM_MIX_SYN2&, const CBackupSlot*);
	const MixScript* GetMixScript(DWORD itemIndex ) const;
	BOOL AddMixOption(CPlayer&, const ITEMBASE& mixedItem, const ICONBASE& supportItem);
	void Compose(const MSG_ITEM_COMPOSE_SYN&);
	void LoadMixScript();
	void LoadMixSetting();
	// 080925 LUJ, 조합 보조 정보를 가져온다
	const MixSupportScript* GetMixSupportScript( DWORD itemIndex ) const;

	// 키: 아이템 인덱스
	typedef stdext::hash_map< DWORD, MixScript >	MixScriptMap;
	MixScriptMap									mMixScriptMap;
	// 080925 LUJ, 선언을 CommonStruct.h로 이동시킴
	MixSetting mMixSetting;

	// 080228 LUJ, 조합 보호 아이템 여부
	BOOL IsMixProtectionItem( const ITEMBASE& ) const;

	// 080228 LUJ, 조합 보호 아이템 관리
	typedef stdext::hash_set< DWORD >	MixProtectionItemSet;
	MixProtectionItemSet				mMixProtectionItemSet;
	// 080925 LUJ, 조합 보조 정보 관리. 키: 아이템 인덱스
	typedef stdext::hash_map< DWORD, MixSupportScript >	MixSupportScriptMap;
	MixSupportScriptMap									mMixSupportScriptMap;

	eLogitemmoney Dissolve(CPlayer&, const MSG_ITEM_DISSOLVE_SYN&, const CBackupSlot&);
	const DissolveScript* GetDissolveScript(DWORD itemIndex) const;
	void LoadDissolveScript();

	// 키: 아이템 인덱스
	typedef std::map< DWORD, DissolveScript >	DissolveScriptMap;
	DissolveScriptMap							mDissolveScriptMap;


	class CRandom
	{
	public:
		CRandom() :
		mValue( rand() )
		{}

		DWORD GetDword()
		{
			return mValue = mValue * 1103515245 + rand();
		}

		float GetFloat()
		{
			return float( GetDword() ) / UINT_MAX;
		}

	private:
		DWORD mValue;
	}
	mRandom;

	// 080124 웅주, 드롭 옵션 관리
public:
	void AddDropOption( const ITEMBASE&, ITEM_OPTION& );

private:
	void LoadCoolTimeScript();

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

	// 값: 쿨타임 그룹
	typedef stdext::hash_set< DWORD >					CoolTimeGroup;

	// 키: 플레이어 인덱스, 값: 사용 중인 쿨타임 아이템 그룹 인덱스
	typedef stdext::hash_map< DWORD, CoolTimeGroup >	UsingCoolTimeGroup;
	UsingCoolTimeGroup									mUsingCoolTimeGroup;

public:
	DWORD GetCoolTimeGroupIndex( DWORD itemIndex ) const;
	void RemoveCoolTime(DWORD playerIndex);
	BOOL Move_ChangeMap(CPlayer*, MAPTYPE);
	BOOL UseItem_ResetStats(CPlayer*);
	BOOL UseItem_ResetSkill(CPlayer*);
	const ITEM_OPTION& GetOption( const ITEMBASE& ) const;
	void AddOption( const ITEM_OPTION& );
	void RemoveOption( const ITEMBASE& );
	void RemoveOption( DWORD itemDbIndex );

private:
	typedef stdext::hash_map< DWORD, ITEM_OPTION* >	OptionMap;
	OptionMap										mOptionMap;
	// 090923 ONS 메모리풀 교체
	CPool< ITEM_OPTION >	mOptionPool;


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

private:	
	BodyChangeScriptListMap	mBodyChangeScriptListMap;

	void LoadBodyChangeScript();


	//081027 NYJ - 몬스터소환아이템
public:
	struct stMonSummonInfo
	{
		DWORD MonsterIdx;		// 몬스터 인덱스
		DWORD minRate;
		DWORD maxRate;

		stMonSummonInfo() {	MonsterIdx = minRate = maxRate = 0;	}
	};

	struct stMonSummonGroupInfo
	{
		WORD	GroupIdx;		// 그룹인덱스
		DWORD	dwDieTime;
		bool	bNotForbidden;
		
		std::vector<stMonSummonInfo*>	vecBossMosSummonGroup;
		std::vector<stMonSummonInfo*>	vecMonSummonGroup;


		stMonSummonGroupInfo() { GroupIdx = 0; dwDieTime =0; bNotForbidden=false;}

		~stMonSummonGroupInfo()
		{
			std::vector<stMonSummonInfo*>::iterator iter;
			for(iter=vecBossMosSummonGroup.begin(); iter!=vecBossMosSummonGroup.end(); iter++)
			{
				SAFE_DELETE(*iter);
			}

			for(iter=vecMonSummonGroup.begin(); iter!=vecMonSummonGroup.end(); iter++)
			{
				SAFE_DELETE(*iter);
			}
		}

		DWORD MonSummon(DWORD rate)
		{
			if(false == vecBossMosSummonGroup.empty())
			{
				int nBossCnt = vecBossMosSummonGroup.size() - 1;

				if(0<=nBossCnt && rate <= vecBossMosSummonGroup[nBossCnt]->maxRate)
				{
					// 확률테이블내에 위치한 보스몬스터 소환함.
					std::vector<stMonSummonInfo*>::iterator iter;
					for(iter=vecBossMosSummonGroup.begin(); iter!=vecBossMosSummonGroup.end(); iter++)
					{
						if((*iter)->minRate<=rate && rate<=(*iter)->maxRate)
							return (*iter)->MonsterIdx;
					}

					return 0;
				}
			}

			if(vecMonSummonGroup.empty())
			{
				return 0;
			}

			const int nRand = rand() % vecMonSummonGroup.size();

			return vecMonSummonGroup[nRand]->MonsterIdx;
		}
	};

	void LoadMonSummonItemInfo();

private:
	std::vector< stMonSummonGroupInfo* > m_vecMonSummonGroupInfo;
	std::set< MAPTYPE > m_vecMonSummonForbiddenMap;

	//081027 NYJ - NPC소환아이템
public:
	struct stNpcSummonInfo
	{
		DWORD SummonIdx;	// 소환인덱스
		DWORD NpcIdx;		// 몬스터 인덱스
		DWORD LifeTime;

		stNpcSummonInfo() {	SummonIdx = NpcIdx = LifeTime = 0; }
	};

	void LoadNpcSummonItemInfo();

private:
	std::map< DWORD, stNpcSummonInfo* > m_mapNpcSummonInfo;
	std::set< MAPTYPE > m_vecNpcSummonForbiddenMap;
};
