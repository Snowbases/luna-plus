#include "stdafx.h"
#include "BaseItem.h"
#include "cQuickItem.h"


CBaseItem::CBaseItem() :
mIsVisibleDurability( TRUE )
{}


CBaseItem::~CBaseItem()
{}


void CBaseItem::RemoveQuickLink()
{
	PTRLISTPOS pos = mQuickList.GetHeadPosition();
	cQuickItem* pQuickItem = NULL;

	WORD tab = 0;
	WORD qpos = 0;

	while(pos)
	{
		pQuickItem = ( cQuickItem* )mQuickList.GetNext( pos );
		if( pQuickItem )
		{
			pQuickItem->GetPosition( tab, qpos );

			MSG_QUICK_REMOVE_SYN msg;

			msg.Category = MP_QUICK;
			msg.Protocol = MP_QUICK_REMOVE_SYN;
			msg.dwObjectID = HEROID;
			msg.tabNum = ( BYTE )( tab );
			msg.pos = ( BYTE )( qpos );

			NETWORK->Send( &msg, sizeof( msg ) );

			pQuickItem->ClearLink();
		}

	}

	mQuickList.RemoveAll();
}


BOOL CBaseItem::IsVisibleDurability() const
{
	return mIsVisibleDurability;
}


void CBaseItem::SetVisibleDurability( BOOL isVisible )
{
	mIsVisibleDurability = isVisible;
}