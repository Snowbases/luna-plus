// SiegeWarfareMgr.h: interface for the CSiegeWarfareMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIEGEDUNGEONMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)
#define AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum eDeathPanalty
{
	eDeathPanalty_DecreaseExp,
	eDaathPanalty_CanItemLooting,
	eDeathPanalty_Max
};

enum eSiegeDungeonType
{
	eSiegeDungeonType_Nera,
	eSiegeDungeonType_Max
};

struct stSiegeDungeonDeathPanaltyInfo
{
	DWORD	DecreaseExp;
	BOOL	CanItemLooting;
};

#define SIEGEDUNGEONMGR CSiegeDungeonMgr::GetInstance()

class CSiegeDungeonMgr
{
	CSiegeDungeonMgr();
	~CSiegeDungeonMgr();

	WORD	m_wMapType;
	DWORD	m_dwKindMapList[eSiegeDungeonType_Max];
	stSiegeDungeonDeathPanaltyInfo	m_stPanalty[eSiegeDungeonType_Max];

public:
	MAKESINGLETON( CSiegeDungeonMgr );
	// 맵 정보를 가져온다.
	VOID	LoadInfo();
	// 성 던전인가?
	// 081009 LYW --- SiegeDungeonMgr : 공성소스 Merge
	BOOL	IsSiegeDungeon(MAPTYPE MapNum);
	stSiegeDungeonDeathPanaltyInfo* GetPanalty(MAPTYPE MapNum);
};

#endif // !defined(AFX_SIEGEDUNGEONMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)