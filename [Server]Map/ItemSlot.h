#pragma once

enum UPDATE_BIT{ UB_DBIDX = 1, UB_ICONIDX=2, UB_ABSPOS=4, UB_QABSPOS=8, UB_DURA=16, UB_ALL=63, };
enum ERROR_ITEM { EI_TRUE = 0, EI_OUTOFPOS = 1, EI_NOTEQUALDATA, EI_EXISTED, EI_NOTEXIST, EI_LOCKED, EI_NOTENOUGHMONEY, EI_NOSPACE, EI_MAXMONEY };

// SS_LOCKOMIT 을 수행한 함수 후에 락이 풀린다. 
// 주의 : Updateitemabs도 락이 풀린다!
//---KES StreetStall Fix 071020
enum SLOT_STATE { SS_NONE = 0, SS_PREINSERT=1, SS_LOCKOMIT = 2, SS_CHKDBIDX = 4,};
//----------------------------

class CPlayer;
class CPurse;
// 071226 LUJ
class CBackupSlot;

class CItemSlot  
{
	friend class CBackupSlot;

public:
	CItemSlot();
	virtual ~CItemSlot();

	virtual void Init(POSTYPE startAbsPos, POSTYPE slotNum, ITEMBASE * pInfo, SLOTINFO * pSlotInfo);
	void Release();
	virtual ITEMBASE* GetItemInfoAbs(POSTYPE);
	void GetItemInfoAll(ITEMBASE*, size_t) const;
	void SetItemInfoAll(ITEMBASE*);
	virtual ERROR_ITEM UpdateItemAbs(CPlayer*, POSTYPE whatAbsPos, DWORD dwDBIdx, DWORD wItemIdx, POSTYPE position, POSTYPE quickPosition, DURTYPE Dur, WORD flag=UB_ALL, WORD state=SS_NONE);
	virtual ERROR_ITEM InsertItemAbs(CPlayer*, POSTYPE absPos, ITEMBASE * pItem, WORD state = SS_NONE );
	virtual ERROR_ITEM DeleteItemAbs(CPlayer*, POSTYPE absPos, ITEMBASE * pItemOut, WORD state = SS_NONE);

	// 071212 LUJ, DB에서 처리된 결과를 업데이트하는 함수
	virtual ERROR_ITEM InsertItemAbsFromDb( CPlayer*, const ITEMBASE& );
	
	POSTYPE GetStartPos()	const { return m_StartAbsPos; }
	POSTYPE GetSlotNum()	const { return m_SlotNum; }
	inline virtual BOOL IsEmpty(POSTYPE absPos);

	CPurse*	GetPurse() { return m_pPurse; }
	MONEYTYPE GetMoney();
	BOOL CreatePurse(CPurse* pNewPurse, void* pOwner, MONEYTYPE money, MONEYTYPE max = MAXMONEY);
	virtual BOOL IsAddable(POSTYPE, ITEMBASE*) {	return TRUE; }
	virtual WORD GetEmptyCell(WORD * EmptyCellPos, WORD NeedCount = 1) { return 0; }
	virtual void SetLock(POSTYPE absPos, BOOL val);
	virtual BOOL IsLock(POSTYPE absPos);

	virtual WORD GetItemCount();

	// RaMa	
	virtual BOOL CheckQPosForItemIdx( DWORD ItemIdx )		{	return TRUE;	}
	virtual BOOL CheckItemLockForItemIdx( DWORD ItemIdx )	{	return TRUE;	}


	// 071225 KTH -- 최대 슬롯 개수를 설정 할수 있도록 수정.
	void SetSlotNum(POSTYPE slotNum)	{	m_SlotNum = slotNum;	}

	// 080621 LYW --- ItemSlot : 아이템과 슬롯 정보를 초기화 하는 함수 추가.
	void ClearItemBaseAndSlotInfo(POSTYPE absPos) ;

	void ForcedUnseal(POSTYPE absPos);
	// 090122 LUJ, 봉인 가능한 아이템을 봉인한다
	void ForcedSeal(POSTYPE absPos);
	
protected:
	virtual inline BOOL IsPosIn(POSTYPE);
	ITEMBASE *		m_ItemBaseArray;
	SLOTINFO *		m_SlotInfoArray;
	POSTYPE			m_StartAbsPos;
	POSTYPE			m_SlotNum;						// Slot의 갯수

	/* purse value */
	CPurse*			m_pPurse;
};


// 071226 LUJ, 슬롯 정보를 복사해두고 복구하거나 비교하기 위해 사용
class CBackupSlot
{
public:
	CBackupSlot( CItemSlot& slot ) :
		mSlot( slot ),
		mItemArray		( new ITEMBASE[ slot.GetSlotNum() ] ),
		mSlotInfoArray	( new SLOTINFO[ slot.GetSlotNum() ] )
	{
		memcpy( mItemArray,		slot.m_ItemBaseArray, sizeof( *slot.m_ItemBaseArray ) * slot.m_SlotNum );
		memcpy( mSlotInfoArray,	slot.m_SlotInfoArray, sizeof( *slot.m_SlotInfoArray ) * slot.m_SlotNum );
	}

	~CBackupSlot()
	{
		SAFE_DELETE_ARRAY( mItemArray );
		SAFE_DELETE_ARRAY( mSlotInfoArray );
	}

	void Restore() const
	{
		memcpy( mSlot.m_ItemBaseArray, mItemArray,		sizeof( *mSlot.m_ItemBaseArray ) * mSlot.m_SlotNum );
		memcpy( mSlot.m_SlotInfoArray, mSlotInfoArray,	sizeof( *mSlot.m_SlotInfoArray ) * mSlot.m_SlotNum );
	}

	const ITEMBASE* GetItem( POSTYPE position ) const
	{
		if( ! mSlot.IsPosIn( position ) )
		{
			return 0;
		}

		// 이 포인터에 대한 널 검사는 의미가 없음
		const ITEMBASE* item = mItemArray + position;

		return item->dwDBIdx ? item : 0;
	}


private:
	CItemSlot&	mSlot;
	ITEMBASE*	mItemArray;
	SLOTINFO*	mSlotInfoArray;
};
