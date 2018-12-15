#pragma once


#include "cImageSelf.h"

class CGuildUnionMark
{
protected:
	cImageSelf	m_Img;
	BOOL		m_bLoaded;
public:
	CGuildUnionMark();
	virtual ~CGuildUnionMark();

	BOOL	LoadNewGuildUnionMark( char* pName );
	void	UpdateGuildUnionMark( char* pName );
	void	Render( VECTOR2* pos, DWORD dwColor = 0xffffffff );
};


#define GUILDUNIONMARKMGR USINGTON(CGuildUnionMarkMgr)

class CGuildUnionMarkMgr  
{
protected:
	CYHHashTable<CGuildUnionMark>	m_MarkTable;

	CGuildUnionMark*	LoadNewGuildUnionMark( int nServerSetNum, DWORD dwGuildUnionIdx, DWORD dwMarkIdx );

public:
	CGuildUnionMarkMgr();
	virtual ~CGuildUnionMarkMgr();
	CGuildUnionMark* GetGuildUnionMark( int nServerSetNum, DWORD dwGuildUnionIdx, DWORD dwMarkIdx );
	BOOL RegistGuildUnionMark(int nServerSetNum, DWORD dwGuildUnionIdx, const char* pBmpName);
	void SaveGuildUnionMark( int nServerSetNum, DWORD dwGuildUnionIdx, DWORD dwMarkIdx, char* pImgData );

};

EXTERNGLOBALTON(CGuildUnionMarkMgr)