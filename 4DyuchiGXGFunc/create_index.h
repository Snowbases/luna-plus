#pragma once

#include "type.h"
#include "../4DyuchiGRX_Common/stdafx.h"

struct INDEX_DESC
{
	DWORD			m_dwIndex;
	INDEX_DESC*		m_pNext;
};

#define		INDEX_DESC_SIZE		8

class CIndexCreator
{
public:
	
	INDEX_DESC*			m_pIndexList;
	INDEX_DESC*			m_pBaseDesc;			// 4
	INDEX_DESC**		m_ppIndexDescTable;		// 8
	DWORD				m_dwIndexNum;			// 12
	DWORD				m_dwMaxIndexNum;		// 16
	INDEX_DESC*			m_pTailDesc;			// 20
	
public:
	CIndexCreator();
	~CIndexCreator();
};