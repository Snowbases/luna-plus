#ifndef __STREETSTALL_MANAGER__
#define __STREETSTALL_MANAGER__

 
//#define STREETSTALLMGR cStreetStallManager::GetInstance()
#define STREETSTALLMGR USINGTON(cStreetStallManager)


class cStreetStall;
class CPlayer;
struct sCELLINFO;

class cStreetStallManager 
{
private:
	BOOL CanBuyItem(CPlayer* pOwner, CPlayer *pGuest, sCELLINFO* pItemInfo, WORD* EmptyCellPos, STREETSTALL_BUYINFO* pBuyInfo, ITEMBASE* pStallItemBase, DWORD dwBuyNum );
	BOOL CanSellItem(CPlayer* pOwner, CPlayer *pGuest, sCELLINFO* pItemInfo, WORD* EmptyCellPos, STREETSTALL_BUYINFO* pBuyInfo, ITEMBASE* pStallItemBase, DWORD dwBuyNum, WORD& result );
	void AssertBuyErr(WORD Err);
	void AssertSellErr(WORD Err);
protected:
	// 090923 ONS 메모리풀 교체
	CPool<cStreetStall>*				m_mpStreetStall;
	CYHHashTable<cStreetStall>			m_StallTable;

public:
	cStreetStallManager();
	virtual ~cStreetStallManager();

	//MAKESINGLETON( cStreetStallManager );

	void StreetStallMode( CPlayer* pPlayer, WORD StallKind, char* title, BOOL bMode );

	cStreetStall* CreateStreetStall( CPlayer* pOwner, WORD StallKind, char* title );
	void DeleteStreetStall( CPlayer* pOwner );
	cStreetStall* FindStreetStall( CPlayer* pOwner );
	
//	BOOL GuestIn( CPlayer* pOwner, CPlayer* pGuest );
//	void GuestOut( DWORD OwnerId, CPlayer* pGuest );
	void UserLogOut( CPlayer* pPlayer );

	BOOL BuyItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pBuyInfo );
	BOOL BuyDupItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pBuyInfo );
	BOOL SellItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pSellInfo );
	BOOL SellDupItem( CPlayer* pOwner, cStreetStall* pStall, CPlayer* pGuest, STREETSTALL_BUYINFO* pSellInfo );

	void CreateDupItem( DWORD dwObjectId, ITEMBASE* pItemBase, DWORD FromChrID );

	BOOL IsExist( cStreetStall* pStall );

	void NetworkMsgParse( BYTE Protocol, void* pMsg );
	void SendNackMsg(CPlayer* pPlayer, BYTE Protocol);
};
EXTERNGLOBALTON(cStreetStallManager);

#endif // __STREETSTALL_MANAGER__
