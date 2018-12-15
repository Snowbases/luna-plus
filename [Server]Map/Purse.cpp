#include "stdafx.h"

#include "purse.h"
#include "player.h"
#include "MapDBMsgParser.h"

CPurse::CPurse()
{
	m_bInit = FALSE;
}

CPurse::~CPurse()
{
}

BOOL CPurse::InitPurse( void* pOwner, MONEYTYPE money, MONEYTYPE max )
{
	if( m_bInit ) 
	{
		return FALSE;
	}

	if( money > max )
	{
		return FALSE;
	}

	m_pOwner = pOwner;

	m_dwMoney = money;
	m_dwMaxMoney = max;
	m_bInit = TRUE;

	m_MsgMoney.Category = MP_ITEM;
	m_MsgMoney.Protocol = MP_ITEM_MONEY;

	return TRUE;
}

void CPurse::Release()
{
	m_pOwner = NULL;

	m_bInit = FALSE;
}

void CPurse::SetMoney( MONEYTYPE money, BYTE MsgFlag )
{
	m_dwMoney = money;
}

BOOL CPurse::SetMaxMoney( MONEYTYPE max )
{
	if( !m_bInit ) return FALSE;

	if( m_dwMoney > max )
	{
		return FALSE;
	}
	
	m_dwMaxMoney = max;

	return TRUE;
}

MONEYTYPE CPurse::GetMaxMoney()
{
	return m_dwMaxMoney;
}

/* ½ÇÁ¦ º¯È­·® Return */
MONEYTYPE CPurse::Addition( MONEYTYPE money, BYTE MsgFlag )
{
	if( !m_bInit ) return 0;

	if( money > m_dwMaxMoney )
	{
		//넘친경우
		money = m_dwMaxMoney - m_dwMoney;
	}
	else if( m_dwMoney > m_dwMaxMoney - money )
	{
		//합쳐서 넘친경우
		money = m_dwMaxMoney - m_dwMoney;
	}

	SetMoney(m_dwMoney + money, MsgFlag);

	return money;
}

/* ½ÇÁ¦ º¯È­·® Return */
MONEYTYPE CPurse::Subtraction( MONEYTYPE money, BYTE MsgFlag )
{
	if( !m_bInit ) return 0;

	if( !IsEnoughMoney(money) )
		return 0; // ½Çº¯È­·® ¾øÀ½.

	if( m_dwMoney - money > m_dwMoney ) // Ãß°¡ ÀÚ·áÇü¿¡ ´ëÇÑ ¿¡·¯ check
		return 0;

	SetMoney(m_dwMoney - money, MsgFlag);

	return money;
}

BOOL CPurse::IsAdditionEnough( MONEYTYPE AdditionMoney )
{
	if( m_dwMoney + AdditionMoney < m_dwMoney ||
		m_dwMoney + AdditionMoney > m_dwMaxMoney )
		return FALSE;

	return TRUE;
}

// °¨»ê ±Ý¾×°úÀÇ ºñ±³
BOOL CPurse::IsEnoughMoney( MONEYTYPE SubtractionMoney )
{
	if( m_dwMoney >= SubtractionMoney )
		return TRUE;

	return FALSE;
}

void CPurse::RSetMoney( MONEYTYPE money, BYTE flag )
{
	m_dwMoney = money;
}


/////////////////////////////////////////////////////////////////
// °³ÀÎ µ·
/////////////////////////////////////////////////////////////////
void CInventoryPurse::SetMoney( MONEYTYPE money, BYTE MsgFlag )
{
	m_dwMoney = money;

	CPlayer* pPlayer = (CPlayer*)m_pOwner;
	if( !pPlayer ) return;

	pPlayer->MoneyUpdate( m_dwMoney );

	// DB update
	if( MsgFlag != MF_OBTAIN )
		CharacterHeroInfoUpdate(pPlayer);

	// Client Msg;
	SendMoneyMsg(MsgFlag);
}

void CInventoryPurse::RSetMoney( MONEYTYPE money, BYTE flag )
{
	m_dwMoney = money;

	SendMoneyMsg( flag );
}

void CInventoryPurse::SendMoneyMsg(BYTE flag)
{
	CPlayer* pPlayer = (CPlayer*)m_pOwner;
	if( !pPlayer ) return;

	// Client¿¡°Ô º¸³¾ ¸Þ¼¼Áö.. (Ãß°¡ ÇÏ¼¼¿è!!)
	switch( flag )
	{
	case 64:
		{
			m_MsgMoney.dwObjectID = pPlayer->GetID();
			m_MsgMoney.dwTotalMoney = m_dwMoney;
			m_MsgMoney.bFlag = flag;

			pPlayer->SendMsg(&m_MsgMoney, sizeof(m_MsgMoney));
		}
		break;
	case MF_NOMAL:
		{
			m_MsgMoney.dwObjectID = pPlayer->GetID();
			m_MsgMoney.dwTotalMoney = m_dwMoney;
			m_MsgMoney.bFlag = flag;

			pPlayer->SendMsg(&m_MsgMoney, sizeof(m_MsgMoney));
		}
		break;
	case MF_OBTAIN:
		{
			m_MsgMoney.dwObjectID = pPlayer->GetID();
			m_MsgMoney.dwTotalMoney = m_dwMoney;
			m_MsgMoney.bFlag = flag;

			pPlayer->SendMsg(&m_MsgMoney, sizeof(m_MsgMoney));
		}
		break;
	case MF_LOST:
		{
			m_MsgMoney.dwObjectID = pPlayer->GetID();
			m_MsgMoney.dwTotalMoney = m_dwMoney;
			m_MsgMoney.bFlag = flag;

			pPlayer->SendMsg(&m_MsgMoney, sizeof(m_MsgMoney));

		}
		break;
	case MF_FEE:
		{
			m_MsgMoney.dwObjectID = pPlayer->GetID();
			m_MsgMoney.dwTotalMoney = m_dwMoney;
			m_MsgMoney.bFlag = flag;

			pPlayer->SendMsg(&m_MsgMoney, sizeof(m_MsgMoney));
		}
		break;
	default:
		return;
	}
}