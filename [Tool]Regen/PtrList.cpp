// PtrList.cpp: implementation of the cPtrList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <assert.h>
#include "PtrList.h"

void* pNullPoint	= NULL;

//////////////////////////////////////////////////////////////////
//				CYHPlex	class									//
//////////////////////////////////////////////////////////////////
CYHPlex* PASCAL CYHPlex::Create(CYHPlex*& pHead, UINT nMax, UINT cbElement)
{
//	ASSERT(nMax > 0 && cbElement > 0);
	CYHPlex* p = (CYHPlex*) new BYTE[sizeof(CYHPlex) + nMax * cbElement];
	p->pNext = pHead;
	pHead = p;
	return p;
}

void CYHPlex::FreeDataChain()
{
	CYHPlex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		CYHPlex* pNext = p->pNext;
		delete[] bytes;
		p = pNext;
	}
}


cPtrList::cPtrList(int nBlockSize)
{
//	ASSERT(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

void cPtrList::RemoveAll()
{
//	ASSERT_VALID(this);
//	ASSERT(this);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

cPtrList::~cPtrList()
{
	RemoveAll();
//	ASSERT(m_nCount == 0);
}


cPtrList::CYHNode*
cPtrList::NewNode(cPtrList::CYHNode* pPrev, cPtrList::CYHNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CYHPlex* pNewBlock = CYHPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CYHNode));

		// chain them into free list
		CYHNode* pNode = (CYHNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
//	ASSERT(m_pNodeFree != NULL);

	cPtrList::CYHNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
//	ASSERT(m_nCount > 0);  

	pNode->data = NULL;

	return pNode;
}

void cPtrList::FreeNode(cPtrList::CYHNode* pNode)
{
	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
//	ASSERT(m_nCount >= 0);  


	if (m_nCount == 0)
		RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////

PTRLISTPOS cPtrList::AddHead(void* newElement)
{
//	ASSERT_VALID(this);
//	ASSERT(this);

	CYHNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (PTRLISTPOS) pNewNode;
}

PTRLISTPOS cPtrList::AddTail(void* newElement)
{
//	ASSERT_VALID(this);
//	ASSERT(this);

	CYHNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (PTRLISTPOS) pNewNode;
}

void cPtrList::AddHead(cPtrList* pNewList)
{
//	ASSERT_VALID(this);
//	ASSERT(this);
//	ASSERT(pNewList != NULL);
//	ASSERT_KINDOF(cPtrList, pNewList);
//	ASSERT(cPtrList, pNewList);
//	ASSERT_VALID(pNewList);
//	ASSERT(pNewList);


	PTRLISTPOS pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

void cPtrList::AddTail(cPtrList* pNewList)
{
//	ASSERT_VALID(this);
//	ASSERT(this);
//	ASSERT(pNewList != NULL);
//	ASSERT_KINDOF(cPtrList, pNewList);
//	ASSERT_VALID(pNewList);

	// add a list of same elements
	PTRLISTPOS pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

void* cPtrList::RemoveHead()
{
//	ASSERT_VALID(this);
//	ASSERT(m_pNodeHead != NULL);  
//	ASSERT(AfxIsValidAddress(m_pNodeHead, sizeof(CYHNode)));

	CYHNode* pOldNode = m_pNodeHead;
	void* returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

void* cPtrList::RemoveTail()
{
//	ASSERT_VALID(this);
//	ASSERT(m_pNodeTail != NULL);  
//	ASSERT(AfxIsValidAddress(m_pNodeTail, sizeof(CYHNode)));
	if( this == NULL)
		return pNullPoint;
	if( m_pNodeTail == NULL)
		return pNullPoint;

	CYHNode* pOldNode = m_pNodeTail;
	void* returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

PTRLISTPOS cPtrList::InsertBefore(PTRLISTPOS position, void* newElement)
{
//	ASSERT_VALID(this);

	if (position == NULL)
		return AddHead(newElement); 

	// Insert it before position
	CYHNode* pOldNode = (CYHNode*) position;
	CYHNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pPrev, sizeof(CYHNode)));
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
//		ASSERT(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (PTRLISTPOS) pNewNode;
}

PTRLISTPOS cPtrList::InsertAfter(PTRLISTPOS position, void* newElement)
{
//	ASSERT_VALID(this);

	if (position == NULL)
		return AddTail(newElement);

	// Insert it before position
	CYHNode* pOldNode = (CYHNode*) position;
//	ASSERT(AfxIsValidAddress(pOldNode, sizeof(CYHNode)));
	CYHNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pNext, sizeof(CYHNode)));
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
//		ASSERT(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (PTRLISTPOS) pNewNode;
}

void cPtrList::RemoveAt(PTRLISTPOS& position)
{
//	ASSERT_VALID(this);

	CYHNode* pOldNode = (CYHNode*) position;
//	ASSERT(AfxIsValidAddress(pOldNode, sizeof(CYHNode)));
	if( pOldNode == NULL)
		return;

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
		position	= (PTRLISTPOS)m_pNodeHead;
	}
	else
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pPrev, sizeof(CYHNode)));
		pOldNode->pPrev->pNext = pOldNode->pNext;
		position	= (PTRLISTPOS)pOldNode->pPrev->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pNext, sizeof(CYHNode)));
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}


/////////////////////////////////////////////////////////////////////////////

PTRLISTPOS cPtrList::FindIndex(int nIndex) const
{
//	ASSERT_VALID(this);

	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CYHNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
//		ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
		pNode = pNode->pNext;
	}
	return (PTRLISTPOS) pNode;
}

PTRLISTPOS cPtrList::Find(void* searchValue, PTRLISTPOS startAfter) const
{
//	ASSERT_VALID(this);

	CYHNode* pNode = (CYHNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
//		ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
		pNode = pNode->pNext; 
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (pNode->data == searchValue)
			return (PTRLISTPOS) pNode;
	return NULL;
}


void*& cPtrList::GetHead()
{
//	ASSERT(m_pNodeHead != NULL);
	return m_pNodeHead->data;
}

void* cPtrList::GetHead() const
{
//	ASSERT(m_pNodeHead != NULL);
	return m_pNodeHead->data;
}

void*& cPtrList::GetTail()
{
	//ASSERT(m_pNodeTail == NULL);
	if( m_pNodeTail == NULL)
	{
		return pNullPoint;
	}
	return m_pNodeTail->data;
}

void* cPtrList::GetTail() const
{
//	ASSERT(m_pNodeTail != NULL);
	return m_pNodeTail->data;
}

PTRLISTPOS cPtrList::GetHeadPosition() const
{
	return (PTRLISTPOS) m_pNodeHead;
}

PTRLISTPOS cPtrList::GetTailPosition() const
{
	return (PTRLISTPOS) m_pNodeTail;
}

void*& cPtrList::GetNext(PTRLISTPOS& rPosition) // return *Position++
{
	CYHNode* pNode = (CYHNode*) rPosition;
//	ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
	if( pNode == NULL)
		return pNullPoint;

	rPosition = (PTRLISTPOS) pNode->pNext;
	return pNode->data;
}

void* cPtrList::GetNext(PTRLISTPOS& rPosition) const // return *Position++
{
	CYHNode* pNode = (CYHNode*) rPosition;
//	ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
	rPosition = (PTRLISTPOS) pNode->pNext;
	return pNode->data;
}

void*& cPtrList::GetPrev(PTRLISTPOS& rPosition) // return *Position--
{
	CYHNode* pNode = (CYHNode*) rPosition;
//	ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
	if( pNode == NULL)
		return pNullPoint;

	rPosition = (PTRLISTPOS) pNode->pPrev;
	return pNode->data;
}

void* cPtrList::GetPrev(PTRLISTPOS& rPosition) const
{
	CYHNode* pNode = (CYHNode*) rPosition;
//	ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
	if( pNode == NULL)
		return pNullPoint;

	rPosition = (PTRLISTPOS) pNode->pPrev;
	return pNode->data;
}

void*& cPtrList::GetAt(PTRLISTPOS position)
{
	CYHNode* pNode = (CYHNode*) position;
//	ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
	return pNode->data;
}

void* cPtrList::GetAt(PTRLISTPOS position) const
{
	CYHNode* pNode = (CYHNode*) position;
//	ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
	return pNode->data;
}

void cPtrList::SetAt(PTRLISTPOS pos, void* newElement)
{
	CYHNode* pNode = (CYHNode*) pos;
//	ASSERT(AfxIsValidAddress(pNode, sizeof(CYHNode)));
	pNode->data = newElement;
}


BOOL cPtrList::Remove(void* pRemoveValue)
{
	PTRLISTPOS findpos = NULL;
	CYHNode* pNode = m_pNodeHead;  // start at head
	for (; pNode != NULL; pNode = pNode->pNext)
	{
		if (pNode->data == pRemoveValue)
			findpos = (PTRLISTPOS) pNode;
	}
	if(findpos == NULL)
		return FALSE;
	
	RemoveAt(findpos);
	return TRUE;
}