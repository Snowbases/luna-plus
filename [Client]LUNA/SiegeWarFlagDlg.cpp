// 080929 LYW --- SiegeWarFlagDlg : 공성 깃발 정보 다이얼로그 추가.
#include "stdafx.h"
#include ".\siegewarflagdlg.h"

#include "WindowIDEnum.h"

#include "./Interface/cStatic.h"

CSiegeWarFlagDlg::CSiegeWarFlagDlg(void)
{
	m_pFlagName		= NULL ;
	m_pMasterName	= NULL ;
	m_pGuildName	= NULL ;
}

CSiegeWarFlagDlg::~CSiegeWarFlagDlg(void)
{
}

void CSiegeWarFlagDlg::Linking()
{
	m_pFlagName		= (cStatic*)GetWindowForID(SW_FLAG_FLAGNAME) ;
	m_pMasterName	= (cStatic*)GetWindowForID(SW_FLAG_MASTERNAME) ;
	m_pGuildName	= (cStatic*)GetWindowForID(SW_FLAG_GUILDNAME) ;

	if( !m_pFlagName || !m_pMasterName || !m_pGuildName )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Failed to linking SiegeWarFlagDlg!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
		return ;
	}
}


void CSiegeWarFlagDlg::SetFlagName(char* pFlagName)
{
	if( !pFlagName )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Invalid parameter!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
		return ;
	}

	if( !m_pFlagName )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Invalid child control!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
		return ;
	}

	const char* pStr = pFlagName ;

	m_pFlagName->SetStaticText( pStr ) ;
}

void CSiegeWarFlagDlg::SetMasterName(char* pMasterName)
{
	if( !pMasterName )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Invalid parameter!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
		return ;
	}

	if( !m_pMasterName )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Invalid child control!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
		return ;
	}

	const char* pStr = pMasterName ;

	m_pMasterName->SetStaticText( pStr ) ;
}

void CSiegeWarFlagDlg::SetGuildName(char* pGuildName)
{
	if( !pGuildName )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Invalid parameter!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
		return ;
	}

	if( !m_pGuildName )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "child control!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
		return ;
	}

	const char* pStr = pGuildName ;

	m_pGuildName->SetStaticText( pStr ) ;
}
