#pragma once

typedef LPVOID VBHASH_HANDLE;
typedef void (*DelFunc)(LPVOID);

class CListBoxContainer  
{
	VBHASH_HANDLE			m_pNameHash;
	DelFunc					m_pDelFunc;
	CListBox*				m_pListBox;
public:
	BOOL					Initialize(DWORD dwMaxItemNum,DWORD dwMaxSize,CListBox*,DelFunc);
	BOOL					InsertItem(char* szName,LPVOID pItem);
	void*					GetSelectedItem();
	BOOL					DeleteSelectedItem();
	BOOL					DeleteItem(char* szName);
	void					DeleteAll();
	void*					GetItem(int iSeqIndex);
	int						GetItemNum();
	
	CListBoxContainer();
	virtual ~CListBoxContainer();
};