#ifndef _EXCHANGEMANAGER_H
#define _EXCHANGEMANAGER_H

// 070620 LYW --- ExchangeRoom : Modified exchange count.
//#define MAX_EXCHANGEITEM	10
#define MAX_EXCHANGEITEM	12

#define EXCHANGEMGR USINGTON(CExchangeManager)

class cIcon;
class CItem;
class CExchangeItem;
class CPlayer;



enum eEXCHANGE_ERROR
{
	eEE_OK					= 0,
	eEE_USERCANCEL,
	eEE_USERLOGOUT,
	eEE_USERDIE,		//»ó´ë°¡ Á×´Ù
	eEE_DIE,			//³»°¡Á×´Ù
	eEE_NOTENOUGHMONEY,
	eEE_NOTENOUGHSPACE,
	eEE_MAXMONEY,	
	eEE_ERROR,
};


class CExchangeManager
{

protected:

	int					m_nLinkedItemNum;
	CIndexGenerator		m_IconIndexCreator;
	
	BOOL				m_IsExchanging;
//	int					m_nExchangeState;

	BOOL				m_bLockSyncing;
	BOOL				m_bExchangeSyncing;
	BOOL				m_bInventoryActive;

public:

	CExchangeManager();
	virtual ~CExchangeManager();

	void Init();
	void ExchangeStart();
	void ExchangeEnd();
	BOOL LinkItem( CItem* pItem );
	void UnLinkItem( int nTypeIndex, cIcon* pItem );
	int GetLinkedItemNum() { return m_nLinkedItemNum; }
	
//±³È¯½ÅÃ»
	BOOL ApplyExchange();
//±³È¯½ÅÃ»Ãë¼Ò
	void CancelApply();
//±³È¯Çã¶ô
	BOOL CanAcceptExchange( CPlayer* pAccepter );
	void AcceptExchange( BOOL bAccept );

	void Lock( BOOL bLock );
	void Exchange();

//parsing
	void NetworkMsgParse( BYTE Protocol, void* pMsg );
	
	BOOL IsLockSyncing()		{ return m_bLockSyncing; }
	BOOL IsExchangeSyncing()	{ return m_bExchangeSyncing; }
	
	BOOL IsExchanging()			{ return m_IsExchanging; }

	// 070205 LYW --- Add functions to process network msg.
public :
	void Exchange_Apply( void* pMsg ) ;
	void Exchange_Apply_Ack( void* pMsg ) ;
	void Exchange_Apply_Nack() ;
	void Exchange_Accept_Nack( void* pMsg ) ;
	void Exchange_Accept( void* pMsg ) ;
	void Exchange_Reject( void* pMsg ) ;
	void Exchange_Cantapply( void* pMsg ) ;
	void Exchange_Waiting_Cancel( void* pMsg ) ;
	void Exchange_Waiting_Cancel_Ack() ;
	void Exchange_Waiting_Cancel_Nack() ;
	void Exchange_Start() ;
	void Exchange_Additem_Ack( void* pMsg ) ;
	void Exchange_Additem_Nack( void* pMsg ) ;
	void Exchange_Additem( void* pMsg ) ;		
	void Exchange_Delitem_Ack( void* pMsg ) ;
	void Exchange_Delitem_Nack( void* pMsg ) ;
	void Exchange_Delitem( void* pMsg ) ;
	void Exchange_Inputmoney_Ack( void* pMsg ) ;
	void Exchange_Inputmoney_Nack() ;
	void Exchange_Inputmoney( void* pMsg ) ;
	void Exchange_Lock_Ack() ;
	void Exchange_Lock_Nack() ;
	void Exchange_Lock() ;
	void Exchange_Unlock_Ack() ;
	void Exchange_Unlock_Nack() ;
	void Exchange_Exchange() ;
	void Exchange_Exchange_Nack() ;
	void Exchange_Cancel_Ack() ;	
	void Exchange_Cancel_Nack() ;
	void Exchange_Cancel( void* pMsg ) ;
	void Exchange_Setmoney( void* pMsg ) ;
	void Exchange_Insert( void* pMsg ) ;
	void Exchange_Remove( void* pMsg ) ;
};

EXTERNGLOBALTON(CExchangeManager)

#endif