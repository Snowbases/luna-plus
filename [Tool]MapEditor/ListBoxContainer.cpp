#include "stdafx.h"
#include "ListBoxContainer.h"
#include "../4DyuchiGXGFunc/global.h"

CListBoxContainer::CListBoxContainer() :
m_pNameHash(0),
m_pDelFunc(0),
m_pListBox(0)
{}

BOOL CListBoxContainer::Initialize(DWORD dwMaxItemNum,DWORD dwMaxSize,CListBox* pListBox,DelFunc pDelFunc)
{
	m_pDelFunc = pDelFunc;
	m_pNameHash = VBHCreate();
	VBHInitialize(m_pNameHash,128,dwMaxSize,dwMaxItemNum);
	m_pListBox = pListBox;
	return TRUE;
}
BOOL CListBoxContainer::InsertItem(char* szName,LPVOID pItem)
{
	if (!VBHInsert(m_pNameHash,(DWORD)pItem,szName,lstrlen(szName)))
	{
		return FALSE;
	}

	m_pListBox->AddString(szName);
	return TRUE;
}
void* CListBoxContainer::GetSelectedItem()
{
	char	szText[_MAX_PATH];
	int		iTextLen;

	void*	pItem = NULL;
	int iIndex = m_pListBox->GetCurSel();
	if (iIndex == LB_ERR)
		goto lb_return;
	
	memset(szText,0,sizeof(szText));
	m_pListBox->GetText(iIndex,szText);
	iTextLen = m_pListBox->GetTextLen(iIndex);
	VBHSelect(m_pNameHash,(DWORD*)&pItem,1,szText,iTextLen);

lb_return:
	return pItem;
}
void CListBoxContainer::DeleteAll()
{
	char	szText[_MAX_PATH];
	memset(szText,0,sizeof(szText));
	int		iTextLen;

	while (m_pListBox->GetCount())
	{
		m_pListBox->GetText(0,szText);
		iTextLen = m_pListBox->GetTextLen(0);
		VBHDeleteWithKey(m_pNameHash,szText,iTextLen,m_pDelFunc);

		// VBHDeleteWithKey()함수는 키(이름)가 같은 아이템을 모두 지우므로 리스트박스의 아이템과 갯수가 안맞을수 있다.
		// 따라서 리스트박스의 아이템은 몽땅 지운다.
		m_pListBox->DeleteString(0);
	}
}
void* CListBoxContainer::GetItem(int iSeqIndex)
{
	char	szText[_MAX_PATH];
	int		iTextLen;

	void*	pItem = NULL;
	
	memset(szText,0,sizeof(szText));
	if (LB_ERR == m_pListBox->GetText(iSeqIndex,szText))
		goto lb_return;

	iTextLen = m_pListBox->GetTextLen(iSeqIndex);
	VBHSelect(m_pNameHash,(DWORD*)&pItem,1,szText,iTextLen);

lb_return:
	return pItem;
}
int CListBoxContainer::GetItemNum()
{
	return m_pListBox->GetCount();
}
BOOL CListBoxContainer::DeleteSelectedItem()
{
	BOOL	bResult = FALSE;
	char	szText[_MAX_PATH];
	int		iTextLen;
	int iIndex = m_pListBox->GetCurSel();

	if (iIndex == LB_ERR)
		goto lb_return;
	
	memset(szText,0,sizeof(szText));
	m_pListBox->GetText(iIndex,szText);
	iTextLen = m_pListBox->GetTextLen(iIndex);
	
	bResult = VBHDeleteWithKey(m_pNameHash,szText,iTextLen,m_pDelFunc);
	if (bResult)
		m_pListBox->DeleteString(iIndex);

lb_return:
	return bResult;
}
BOOL CListBoxContainer::DeleteItem(char* szName)
{
	return VBHDeleteWithKey(m_pNameHash,szName,lstrlen(szName),m_pDelFunc);
}

CListBoxContainer::~CListBoxContainer()
{
	if (m_pNameHash)
	{
		VBHRelease(m_pNameHash);
		m_pNameHash = NULL;
	}

}
