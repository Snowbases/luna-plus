#pragma once 

#include "cComboBox.h"

class	cChannelComboBox	:	public	cComboBox
{
public:
	cChannelComboBox();
	virtual ~cChannelComboBox();

public:
	virtual void Render();

	cListItem* const GetChannelCongestion() { return &m_ChannelCongestion; }

	void InsertChannelInfo( char* pItemText, DWORD dwColor );

	void RemoveAllChannelComboItem();

	void LoadChannelInfo();

	DWORD	GetFreeCount()		{ return	m_dwFreeCnt; }
	DWORD	GetBusyCount()		{ return	m_dwBusyCnt; }

	DWORD	GetFreeColor()		{ return	m_dwFreeColor; }
	DWORD	GetBusyColor()		{ return	m_dwBusyColor; }
	DWORD	GetExceedColor()	{ return	m_dwExceedColor; }

public:
	cListItem	m_ChannelCongestion;

	DWORD		m_dwFreeCnt;
	DWORD		m_dwBusyCnt;

	DWORD		m_dwFreeColor;
	DWORD		m_dwBusyColor;
	DWORD		m_dwExceedColor;

	DWORD		m_dwTextDistance;
};