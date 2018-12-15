#pragma once
#include "stdafx.h"
#include "CommonStruct.h"

#define DungeonMGR USINGTON(CDungeonMgr)

class CDungeonMgr
{
private:
	CYHHashTable<stWarpInfo>		m_WarpList;
	CYHHashTable<stSwitchNpcInfo>	m_SwitchNpcList;
	stDungeon						m_CurDungeon;
	std::multimap< DWORD, DWORD >	m_mapEntranceNpcJob;

public:
	CDungeonMgr(void);
	virtual ~CDungeonMgr(void);

	void	Init();
	void	Process();
	void	NetworkMsgParse(BYTE Protocol,void* pMsg);	


	void	LoadDungeonInfo();
	void	SetWarp(DWORD dwIndex, BOOL bActive);
	void	SetSwitchNpc(WORD dwIndex, BOOL bActive);

	BOOL	IsDungeonMap(MAPTYPE map);

	void	ZoneBlock(float posx, float posz, float range);
	void	ZoneFree(float posx, float posz, float range);
	void	EdgeBlock(float posx, float posz, float range);
	void	EdgeFree(float posx, float posz, float range);

	// 091119 ONS 인던 페이드무브후 처리함수 추가
	void	AfterWarpAction( DWORD dwWarpIndex );

	void	ClearCurDungeon();

	// 인던 포털 입장을 위해 입장 NPC정보 추가
	void	AddEntranceNpc( DWORD dwDungeonKey, DWORD dwNpcJob );
	BOOL	IsDungeonEntranceNpc( const DWORD dwNpcJob ) const;
	std::multimap< DWORD, DWORD >& GetEntranceNpcJobMap() { return m_mapEntranceNpcJob; }
};

EXTERNGLOBALTON(CDungeonMgr)