#ifndef __STREETSTALLMANAGER__
#define __STREETSTALLMANAGER__

// LYJ 051017 구입노점상 추가
#include "StallKindSelectDlg.h"

#define STREETSTALLMGR USINGTON(CStreetStallManager)

#define STREETSTALL_MAX_LINKITEM 5

class CItem;
class CExchangeItem;
class CBuyItem;
class CObject;
// LYJ 051017 구입노점상 추가
class CStallKindSelectDlg;

class CStreetStallManager 
{
	CIndexGenerator m_IconIndexCreator;
	WORD m_wStallKind;
protected:
	int m_nLinkedItemNum;				// 등록 아이템 갯수
//	int m_selectedQuickAbsPos;
	
	BOOL m_bOpenMsgBox;

//	STREETSTALL_BUYINFO m_BuyMsg;
	
	char m_strOldTitle[MAX_STREETSTALL_TITLELEN + 1];

//KES
//	POSTYPE	m_posWantBuy;
	DWORD	m_dwWantBuyNum;
	DWORD	m_dwWantSellNum;

//---KES 상점검색 2008.3.11
	char	m_strSearchWord[64];

public:
	void SetSearchWord( char* pSearchWord ) { SafeStrCpy( m_strSearchWord, pSearchWord, 64 ); }
	char* GetSearchWord() { return m_strSearchWord; }
	BOOL IsSearchWordIn( const char* strSrc );

	void SearchWordInArea();
//---------------

public:
	//MAKESINGLETON(CStreetStallManager);

	CStreetStallManager();
	virtual ~CStreetStallManager();
	
	void Init();

	void SetOpenMsgBox(BOOL bOpen) { m_bOpenMsgBox = bOpen; }
	
	BOOL LinkItem( CItem* pItem, ITEMBASE* pBase );
	BOOL LinkItem(POSTYPE pos, ITEMBASE ItemInfo, WORD Volume, DWORD Money);
	void UnlinkItem( CExchangeItem* pItem );
	void UnlinkItem( CBuyItem* pItem );
	void LinkBuyItem( STREETSTALL_INFO* pStallInfo );
	void LinkSellItem( STREETSTALL_INFO* pStallInfo );
	int GetLinkedItemNum() { return m_nLinkedItemNum; }
	
	void ToggleHero_StreetStallMode( BOOL bOpen );
	void Toggle_StreetStallMode( CObject* pObject, BOOL bStreet);

	void ChangeDialogState();

	// 네트워크
	void OpenStreetStall();
	void CloseStreetStall();
	void EnterStreetStall( DWORD OwnerId );

	void RegistItemEx( CItem* pItem , DWORD money );
	void RegistItemEx( ITEMBASE ItemInfo, WORD wAbsPosition, WORD Volume , DWORD money );

	void ItemStatus( CExchangeItem* pItem, BOOL bLock );
	void ItemStatus( POSTYPE pos, BOOL bLock );
	void EditTitle();
	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	void BuyItem();
	void BuyItemDur( POSTYPE pos, DWORD dwNum );

	void SendBuyItemMsg();

	BOOL SellItem();
	BOOL SellItemDur( DWORD dwNum );
	
	BOOL SendSellItemMsg();

	WORD GetStallKind() { return m_wStallKind; }
	void SetStallKind(WORD wStallKind) { m_wStallKind = wStallKind; }
	// 070205 LYW --- Add functions to process network msg.
public :
	void StreetStall_Open_Ack( void* pMsg ) ;
	void StreetStall_Open_Nack() ;
	void StreetStall_Close_Ack() ;
	void StreetStall_Fakeregistitem_Ack( void* pMsg ) ;
	void StreetStall_Fakeregistitem_Nack( void* pMsg ) ;
	void StreetStall_Fakeregistitem( void* pMsg ) ;
	void StreetStall_Fakeregistbuyitem_Ack( void* pMsg ) ;
	void StreetStall_Fakeregistbuyitem_Nack( void* pMsg ) ;
	void StreetStall_Fakeregistbuyitem( void* pMsg ) ;
	void StreetStall_Lockitem_Ack( void* pMsg ) ;
	void StreetStall_Lockitem_Nack() ;
	void StreetStall_Lockitem( void* pMsg ) ;
	void StreetStall_Unlockitem_Ack( void* pMsg ) ;
	void StreetStall_Unlockitem_Nack() ;
	void StreetStall_Unlockitem( void* pMsg ) ;
	void StreetStall_Edittitle_Ack( void* pMsg ) ;
	void StreetStall_Edittitle_Nack() ;
	void StreetStall_Edittitle( void* pMsg ) ;
	void StreetStall_Buyitem_Ack( void* pMsg ) ;
	void StreetStall_Deleteitem_Ack( void* pMsg ) ;
	void StreetStall_Deleteitem( void* pMsg ) ;
	void StreetStall_Buyitem_Nack( void* pMsg ) ;
	void StreetStall_Sellitem_Ack( void* pMsg ) ;
	void StreetStall_Sellitem_Nack( void* pMsg ) ;
	void StreetStall_Sellitem( void* pMsg ) ;
	void StreetStall_Sellitem_error( void* pMsg ) ;
	void StreetStall_Buyitem( void* pMsg ) ;
	void StreetStall_Buyitem_error( void* pMsg ) ;
	void StreetStall_Updateitem( void* pMsg ) ;
	void StreetStall_Guestin_Ack( void* pMsg ) ;
	void StreetStall_Guestin_Nack() ;
	void StreetStall_Guestout_Ack() ;
	void StreetStall_Guestout_Nack() ;
	void StreetStall_Start( void* pMsg ) ;
	void StreetStall_End( void* pMsg ) ;
	void StreetStall_Close( void* pMsg ) ;
	void StreetStall_Update( void* pMsg ) ;
	void StreetStall_UpdateEnd() ;
	void StreetStall_Update_Ack( void* pMsg ) ;
	void StreetStall_Update_Nack() ;
	void StreetStall_Message( void* pMsg ) ;
};
EXTERNGLOBALTON(CStreetStallManager);

#endif //__STREETSTALLMANAGER__
