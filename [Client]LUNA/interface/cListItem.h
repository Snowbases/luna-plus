// cListItem.h: interface for the cListItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLISTITEM_H__EFECF4A3_00CC_4F48_A2CA_775C28D96531__INCLUDED_)
#define AFX_CLISTITEM_H__EFECF4A3_00CC_4F48_A2CA_775C28D96531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "cWindowDef.h"

class cListItem
{
public:
	cListItem():m_wMaxLine(0)
	{
	}
	virtual ~cListItem()
	{
		RemoveAll();
	}
	// 070409 LYW --- cListItem : Add function to return item.
	ITEM* GetItem( LONG idx )
	{
		PTRLISTPOS p = m_ListItem.FindIndex( idx ) ;

		if( p )
		{
			return (ITEM*)m_ListItem.GetAt( p ) ;
		}
		else
		{
			return NULL ;
		}
	}

	LONG GetItemCount()
	{
		return m_ListItem.GetCount();
	}
	virtual void AddItem(ITEM* pItem)			// pItem의 메모리 관리는 밖에서 할당해서 넣고, cListItem에서 해제한다.
	{
		if(m_wMaxLine < 1)	return;
		if(m_wMaxLine <= GetItemCount())		// 꽉 찼을 경우, 헤드 아이템을 지워서 공간 확보.
		{
			ITEM* item = (ITEM*)m_ListItem.GetHead();
			SAFE_DELETE(item);					// 이쪽에서 지운다. 구조 별로 안좋음. -_-;
			m_ListItem.RemoveHead();
		}
		m_ListItem.AddTail( pItem );
	}
	virtual void AddItem(ITEM* pItem, LONG idx)		// 인덱스로 지정할 경우, 인덱스가 맥스라인만큼 사용 되고 있는것을 전제로 하므로 웬만하면 쓰지 말자.
	{
		_asm int 3;
		if(m_wMaxLine < 1) return;
		// 위와 비슷한 위험요소 있음. 그리고 index 지정해서 넣을 경우라도 꽉 차면 해당 인덱스를 교체하는게 아니라 헤드를 날려버리므로 수정요. - 이진영.
		if(m_wMaxLine <= GetItemCount())
		{
			_asm int 3;
			ITEM* item = (ITEM*)m_ListItem.GetHead();
			SAFE_DELETE(item);
			m_ListItem.RemoveHead();
		}
	
		PTRLISTPOS p = m_ListItem.FindIndex(idx);
		if( p == NULL ) return;								// 인덱스가 맥스라인만큼 사용되지 안을 경우 이라인에서 문제가 된다. 그냥 나가버리면 어쩌라고. -_-;

		m_ListItem.InsertAfter(p, pItem);
	}
	
	virtual void RemoveAll()
	{
		PTRLISTSEARCHSTART(m_ListItem,ITEM*,item)				// 포인터 리스트의 아이템들을 해제하고, (구조 안좋음. -_-;)
			delete item;
		PTRLISTSEARCHEND

		m_ListItem.RemoveAll();									// 포인터 리스트 자체를 해제.
	}
	virtual void RemoveItem(LONG idx)
	{
		PTRLISTPOS p = m_ListItem.FindIndex( idx );

		if( p )
		{
			ITEM * item = (ITEM*)m_ListItem.GetAt( p );
			SAFE_DELETE(item);
			m_ListItem.RemoveAt(p);
		}
	}
	
	int RemoveItem(char * str)
	{
		int nIndex = -1;
		PTRLISTPOS p  = m_ListItem.GetHeadPosition();
		while( p )
		{
//			ITEM* item = (ITEM*)m_ListItem.GetNext( p );
			ITEM* item = (ITEM*)m_ListItem.GetAt( p );

			if( item )
			{
				nIndex++;
				if(strcmp(item->string, str) == 0)
				{
					SAFE_DELETE(item);
					m_ListItem.RemoveAt(p);
					break;
				}
			}

			m_ListItem.GetNext( p );
		}		
		
		return nIndex;
	}

	void SetMaxLine(WORD maxLine)	{ m_wMaxLine = maxLine; }
	WORD GetMaxLine()				{ return m_wMaxLine; }

	// 061129 LYW --- Add Function that Return cPtrList.
	cPtrList* GetListItem() { return &m_ListItem ; }

protected:

	WORD		m_wMaxLine;
	cPtrList	m_ListItem;
};

/*
class cListItem
{
public:
	cListItem():m_wMaxLine(0)
	{
		m_pItemList = new cLinkedList<ITEM>;
	}
	~cListItem()
	{
		RemoveAll();
		SAFE_DELETE(m_pItemList);
	}
	LONG GetItemCount()
	{
		return m_pItemList->GetCount();
	}
	void AddItem(char * str)
	{
		ITEM * item = new ITEM;
		strcpy(item->string, str);
		item->rgb = RGB_HALF(255,255,255);
		m_pItemList->AddTail(item);
		item->wType = 0;
	}
	
	void AddItem(char * str, LONG idx)
	{
		if(m_wMaxLine < 1) return;
		if(m_wMaxLine < GetItemCount())
			m_pItemList->DeleteHead();
		
		ITEM * item = new ITEM;
		strcpy(item->string, str);
		item->rgb = RGB_HALF(255,255,255);
		item->wType = 0;
		m_pItemList->InsertpAfter(item, idx);
	}
	
	void AddItem(char * str, DWORD color)
	{
		if(m_wMaxLine < 1) return;
		if(m_wMaxLine < GetItemCount())
			m_pItemList->DeleteHead();
		ITEM * item = new ITEM;
		strcpy(item->string, str);
		item->rgb = color;
		item->wType = 0;
		
		m_pItemList->AddTail(item);
	}

	void AddItem( WORD wType, char * str)
	{
		ITEM * item = new ITEM;
		strcpy(item->string, str);
		item->rgb = RGB_HALF(255,255,255);
		m_pItemList->AddTail(item);
		item->wType = wType;
	}

	void RemoveAll()
	{
		m_pItemList->DeleteAll();
	}
	
	void RemoveItem(LONG idx)
	{
		ITEM * item = m_pItemList->DeleteAt(idx);			// return 값 이상해-_-;
		SAFE_DELETE(item);
	}
	
	void RemoveItem(char * str)
	{
		POS pos = m_pItemList->GetFirstPos();
		ITEM * item = NULL;
		while (pos)
		{
			item = m_pItemList->GetNextPos(pos);			
			if(strcmp(item->string, str) == 0)
			{
				m_pItemList->GetPrevPos(pos);
				m_pItemList->DeletePos(pos);			// return 값 이상해-_-;
				SAFE_DELETE(item);
			}
		}
	}
	void SetMaxLine(WORD maxLine) { m_wMaxLine = maxLine; }
protected:

	WORD m_wMaxLine;								// 상속받은 클래스에서 초기화한다.
	cLinkedList<ITEM> * m_pItemList;
};
*/
#endif // !defined(AFX_CLISTITEM_H__EFECF4A3_00CC_4F48_A2CA_775C28D96531__INCLUDED_)
