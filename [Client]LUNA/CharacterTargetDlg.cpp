#include "stdafx.h"
#include "CharacterTargetDlg.h"
#include "WindowIDEnum.h"
#include "Player.h"
#include "./Interface/cStatic.h"
#include "ChatManager.h"
#include "Player.h"
#include "cCheckBox.h"

#include "GuildMark.h"
#include "GuildMarkManager.h"

cCharacterTargetDlg::cCharacterTargetDlg()	:	cMonsterTargetDlg()
{
	//Ä³¸¯ÅÍ ¾ó±¼
	m_pFaceElfMan		=	NULL;
	m_pFaceElfWoman		=	NULL;
	m_pFaceHumanMan		=	NULL;
	m_pFaceHumanWoman	=	NULL;
	m_pFaceDevilMan		=	NULL;
	m_pFaceDevilWoman	=	NULL;
	m_pCurFace			=	NULL;

	m_pGuildName		=	NULL;
	m_pGuildMark		=	NULL;

	m_pGuildImage		=	NULL;
}

cCharacterTargetDlg::~cCharacterTargetDlg()
{

}

void cCharacterTargetDlg::Render()
{
	cMonsterTargetDlg::Render();

	VECTOR2	vecGuildMark	=	{ 0, };
	vecGuildMark.x	=	m_pGuildMark->GetAbsX();
	vecGuildMark.y	=	m_pGuildMark->GetAbsY();
	if( m_pGuildImage )
		m_pGuildImage->Render( &vecGuildMark );
}

void cCharacterTargetDlg::SetMonsterUIActive( bool val )
{
	cMonsterTargetDlg::SetMonsterUIActive( val );

	if( val == FALSE )
	{
		m_pGuildMark->SetActive( val );
		
		m_pFaceElfMan->SetActive( val );
		m_pFaceElfWoman->SetActive( val );
		m_pFaceHumanMan->SetActive( val );
		m_pFaceHumanWoman->SetActive( val );
		m_pFaceDevilMan->SetActive( val );
		m_pFaceDevilWoman->SetActive( val );
		m_pGuildImage	=	NULL;
	}

}


void cCharacterTargetDlg::TargetDlgLink()
{	
	m_pName		=	(cStatic*)GetWindowForID(CG_PLAYERGUAGENAME);
	m_pLevel	=	(cStatic*)GetWindowForID(CG_PLAYERGUAGELEVEL);
	m_pGuage	=	(CObjectGuagen*)GetWindowForID(CG_PLAYERGUAGE_HP);
	m_pFace		=	(cStatic*)GetWindowForID(CG_ST_PLAYER_DUMMY_FACE);

	m_pGuageMain_Left	=	(cStatic*)GetWindowForID(CG_PLAYERGUAGE_MAIN_LEFT);
	m_pGuageMain_Middle	=	(cStatic*)GetWindowForID(CG_PLAYERGUAGE_MAIN_MIDDLE);
	m_pGuageMain_Right	=	(cStatic*)GetWindowForID(CG_PLAYERGUAGE_MAIN_RIGHT);

	//Ä³¸¯ÅÍ ¹öÇÁ, ½Ã¼± Ã¼Å©¹Ú½º
	m_pBuffCheckBox		=	(cCheckBox*)GetWindowForID(CG_PLAYER_BUFFICON_CHECKBOX);
	m_pSightCheckBox	=	(cCheckBox*)GetWindowForID(CG_PLAYER_SIGHTICON_CHECKBOX);

	//Ä³¸¯ÅÍ Å¸°Ù

	m_pVSMain_Left		=	(cStatic*)GetWindowForID(CG_PLAYERVERSUS_MAIN_LEFT);
	m_pVSMain_Right		=	(cStatic*)GetWindowForID(CG_PLAYERVERSUS_MAIN_RIGHT);
	m_pVS				=	(cStatic*)GetWindowForID(CG_PLAYERVERSUS_IMAGE);
	m_pVSGuage			=	(CObjectGuagen*)GetWindowForID(CG_PLAYERVERSUS_LIFEGUAGE);
	m_pVSName			=	(cStatic*)GetWindowForID(CG_PLAYERVERSUS_NAME);

	//Ä³¸¯ÅÍ LifePoint
	m_pLifePoint_Empty	=	(cStatic*)GetWindowForID(CG_PLAYER_LIFEPOINT_EMPTY);

	m_pLifePoint[0]		=	(cStatic*)GetWindowForID(CG_PLAYER_LIFEPOINT_1);
	m_pLifePoint[1]		=	(cStatic*)GetWindowForID(CG_PLAYER_LIFEPOINT_2);
	m_pLifePoint[2]		=	(cStatic*)GetWindowForID(CG_PLAYER_LIFEPOINT_3);
	m_pLifePoint[3]		=	(cStatic*)GetWindowForID(CG_PLAYER_LIFEPOINT_4);
	m_pLifePoint[4]		=	(cStatic*)GetWindowForID(CG_PLAYER_LIFEPOINT_5);

	//Ä³¸¯ÅÍ ¾ó±¼
	m_pFaceElfMan		=	(cStatic*)GetWindowForID(CG_ST_PLAYER_ELF_MAN);
	m_pFaceElfWoman		=	(cStatic*)GetWindowForID(CG_ST_PLAYER_ELF_WOMAN);
	m_pFaceHumanMan		=	(cStatic*)GetWindowForID(CG_ST_PLAYER_HUMAN_MAN);
	m_pFaceHumanWoman	=	(cStatic*)GetWindowForID(CG_ST_PLAYER_HUMAN_WOMAN);
	m_pFaceDevilMan		=	(cStatic*)GetWindowForID(CG_ST_PLAYER_DEVIL_MAN);
	m_pFaceDevilWoman	=	(cStatic*)GetWindowForID(CG_ST_PLAYER_DEVIL_WOMAN);

	m_pGuildName		=	(cStatic*)GetWindowForID(CG_PLAYER_GUILD_NAME);
	m_pGuildMark		=	(cCheckBox*)GetWindowForID(CG_PLAYER_GUILD_MARK);

	m_pSpecies			=	(cStatic*)GetWindowForID(CG_PLAYER_SPECIES),

	m_eTargetType		=	CHARACTER;

	m_pSpecies->SetActive( false );
}

void cCharacterTargetDlg::SetTargetGuageInfo( CObject* pObject )
{
	CPlayer* pPlayer = (CPlayer*)pObject;

	m_dwLifePoint	=	0;

	float	fGuage	=	(float)pPlayer->GetLife()/(float)pPlayer->DoGetMaxLife();
	SetMonsterLife( fGuage );
	UpdateLifePoint();
}

void cCharacterTargetDlg::SetTargetGuildInfo( char* pGuildName )
{
	CPlayer* pTargetPlayer	=	(CPlayer*)m_pCurTargetObject;

	char guildHead[128]	=	{ 0, };

	if( pTargetPlayer )
	{
		sprintf( guildHead, CHATMGR->GetChatMsg(2325), pGuildName );

		m_pGuildImage	=	GUILDMARKMGR->GetGuildMark( pTargetPlayer->GetGuildMarkName() );
		m_pGuildMark->SetToolTip( guildHead, RGBA_MAKE( 255, 255, 255, 255 ) );
		m_pGuildMark->SetActive( true );
	}
}

void cCharacterTargetDlg::SelectTargetGender( CPlayer* pPlayer )
{
	CHARACTER_TOTALINFO* pTargetTotalInfo	=	pPlayer->GetCharacterTotalInfo();

	m_pFace->SetActive( false );
	if( pTargetTotalInfo->Gender == 0 ) //³²ÀÚ
	{
		if( pTargetTotalInfo->Race == 0 )//ÈÞ¸Õ
		{
			m_pFace	=	m_pFaceHumanMan;
		}
		else if( pTargetTotalInfo->Race == 1 )//¿¤ÇÁ
		{
			m_pFace	=	m_pFaceElfMan;
		}
		else if( pTargetTotalInfo->Race == 2 )//¸¶Á×
		{
			m_pFace	=	m_pFaceDevilMan;
		}
	}
	else if( pTargetTotalInfo->Gender == 1 )// ¿©ÀÚ
	{
		if( pTargetTotalInfo->Race == 0 )//ÈÞ¸Õ
		{
			m_pFace	=	m_pFaceHumanWoman;
		}
		else if( pTargetTotalInfo->Race == 1 )//¿¤ÇÁ
		{
			m_pFace	=	m_pFaceElfWoman;
		}
		else if( pTargetTotalInfo->Race == 2 )//¸¶Á×
		{
			m_pFace	=	m_pFaceDevilWoman;
		} 
	}
	m_pFace->SetActive( true );

}