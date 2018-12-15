#include "stdafx.h"
#include "ObjectBalloon.h"
#include "OBalloonName.h"
#include "OBalloonChat.h"
#include "StreetStallTitleTip.h"
//#include "Object.h"
#include "GameResourceManager.h"
#include "GuildMarkManager.h"
#include "GuildMark.h"
#include "Player.h"
#include "ChatManager.h"
#include "GuildUnionMarkMgr.h"
//#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
//#include "Npc.h"

#include "FamilyMarkManager.h"
#include "FamilyMark.h"

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.09.20	2007.10.01
#include "../hseos/Common/SHProgressBar.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"
#include "ObjectManager.h"
#include "GameIn.h"
// E 농장시스템 추가 added by hseos 2007.09.20	2007.10.01

// 080827 LYW --- ObjectBalloon : 공성전 매니져 헤더 호출.
#include "./SiegeWarfareMgr.h"

extern int g_nServerSetNum;

/*
WORD NPCIMAGE[] = {
	24,16,14,15,17,19,24,24,20,24,
	18,33,23,21,23,22,24,24,24,24,
	24,24,24,24,24,24,24,24,24,24,
	24,24,24,24,24,24,24,24,24,24,
	24,24,24
};
*/

CObjectBalloon::CObjectBalloon()
{
	m_pName			= NULL;
	m_pChat			= NULL;
	m_pSSTitle		= NULL;
	m_pSBSTitle		= NULL;
	m_pObject		= NULL;
	m_pGuildMark	= NULL;
//	m_pNpcMark		= NULL;

	/*
	m_pBack				= NULL ;
	m_pFront			= NULL ;

	m_bBackReady		= FALSE ;

	m_BackCurSpriteIdx	= 0 ;
	m_BackLastTick		= 0 ;
	m_BackCurTick		= 0 ;

	m_bFrontReady		= FALSE ;

	m_FrontCurSpriteIdx	= 0 ;
	m_FrontLastTick		= 0 ;
	m_FrontCurTick		= 0 ;
	*/

	// 080827 LYW --- ObjectBalloon : 공성중인지 여부를 설정한다.
	m_byIsSiegeWarfare = FALSE ;
}
 
CObjectBalloon::~CObjectBalloon()
{
	Release();
}

void CObjectBalloon::InitBalloon(CObject * pParent, LONG tall, BYTE flag)
{
	cWindow::Init(0,0,0,0,NULL,pParent->GetID());

	m_pObject = pParent;

	if( pParent->GetObjectKind() == eObjectKind_Player )
	{
		tall = (LONG)(tall * ((CPlayer*)pParent)->GetCharacterTotalInfo()->Height);
	}
/*
	else if( pParent->GetObjectKind() == eObjectKind_Npc )
	{
		m_pNpcMark = new cImage;
		SCRIPTMGR->GetImage(NPCIMAGE[((CNpc*)pParent)->GetNpcKind()],m_pNpcMark,PFT_MINIMAPPATH);
	}
*/	
	m_lTall = tall;

	if(flag & ObjectBalloon_Name)
	{
		ASSERT(!m_pName);
		m_pName = new COBalloonName;

   		m_pName->Set_PParentsObjectKind( pParent->GetObjectKind() ) ;
	}

	if(flag & ObjectBalloon_Chat)
	{
		ASSERT(!m_pChat);
		m_pChat = new COBalloonChat;
	}
	
	if(flag & ObjectBalloon_Title)
	{
		ASSERT(!m_pSSTitle);
		ASSERT(!m_pSBSTitle);
		m_pSSTitle = new CStreetStallTitleTip;
		m_pSBSTitle = new CStreetStallTitleTip;
	}

	m_bActive	= TRUE;

	WINDOWMGR->AddChatTooltip( this );

	// 080827 LYW --- ObjectBalloon : 공성전인지 여부를 가지고 있는다.
   	//if( SIEGEWARFAREMGR->Get_IsSiegeMap() ) m_byIsSiegeWarfare = TRUE ;
   	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() ) m_byIsSiegeWarfare = TRUE ;
}

void CObjectBalloon::Release()
{
	WINDOWMGR->RemoveChatTooltipWindow( this );

	SAFE_DELETE(m_pName);
	SAFE_DELETE(m_pChat);
	SAFE_DELETE(m_pSSTitle);
	SAFE_DELETE(m_pSBSTitle);
	
	m_pObject		= NULL;
	m_pGuildMark	= NULL;
}

	
void CObjectBalloon::Render()
{
	if( !	m_pObject									||
		!	m_pObject->IsInited()						||
		!	m_pObject->GetEngineObject()				||
		!	m_pObject->GetEngineObject()->IsVisible()	||
		!	m_bActive )
	{
		return;
	}
	
#ifndef _GMTOOL_
	if( m_pObject->GetObjectKind() == eObjectKind_Player )
	{
		// 투명인간은 채팅내용 안보임
		//GM은 봄
		if(((CPlayer*)m_pObject)->GetCharacterTotalInfo()->bVisible == FALSE)
			return;
	}
#endif

	BOOL bRender = FALSE;
	
	if( ( m_pName && m_pName->IsShow() ) ||
		( m_pChat && m_pChat->IsShow() ) )
	{
		bRender = TRUE;
	}

	LONG lTall = m_lTall + 20;	//머리위부터 20
	
	if( m_pSSTitle	&& m_pSSTitle->IsShow() )
	{
		bRender = TRUE;
		
		lTall	= m_lTall - 50;
//		lX		= 100;
	}
	if( m_pSBSTitle && m_pSBSTitle->IsShow() )
	{
		bRender = TRUE;
		
		lTall	= m_lTall - 50;
		//		lX		= 100;
	}
		
	if( m_pObject->GetState() == eObjectState_Die )
		lTall = m_lTall - 100;

	if( !bRender ) return;
	
	VECTOR3 OutPos3, inPos3;
	m_pObject->GetPosition(&inPos3);

	if( m_pObject->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)m_pObject;

		// 091109 ShinJS --- HERO가 고정형에 탑승한 경우 고정형에 탑승한 모든 플레이어의 이름을 출력하지 않는다.
		if( HERO &&
			HERO->IsGetOnVehicle() && 
			pPlayer->IsGetOnVehicle() )
		{
			CVehicle* pHeroVehicle = (CVehicle*)OBJECTMGR->GetObject( HERO->GetVehicleID() );
			CVehicle* pPlayerVehicle = (CVehicle*)OBJECTMGR->GetObject( pPlayer->GetVehicleID() );
			if( pHeroVehicle && pPlayerVehicle &&
				pHeroVehicle->DoGetMoveSpeed() <= 0.0f &&
				pPlayerVehicle->DoGetMoveSpeed() <= 0.0f )
			{
				return;
			}
		}

		MATRIX4 mat;
		CEngineObject* pEngineObj = pPlayer->GetEngineObject();
		if( pEngineObj )
		{
			// 카메라의 위치를 정한 Player의 본의 위치와 일치해야함.
			g_pExecutive->GXOGetWorldMatrixByName( pEngineObj->GetGXOHandle(), &mat, "Bip01 Spine1" );

			// 탈것/가구에 탑승중인 경우
			if( pPlayer->IsGetOnVehicle()|| pPlayer->IsRideFurniture() )	//090603 pdy 하우징 탑승버그 수정
			{
				inPos3.x = mat._41;
				inPos3.y = mat._42;
				inPos3.z = mat._43;

				lTall -= 60;
			}
		}
	}
	
	inPos3.y += lTall;

	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(), 0, &inPos3, &OutPos3);
	if(OutPos3.x < 0 || OutPos3.x > 1 || OutPos3.y < 0 || OutPos3.y > 1) return;
	m_absPos.x = (GAMERESRCMNGR->m_GameDesc.dispInfo.dwWidth*OutPos3.x) + 0.25f;
	m_absPos.y = (GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight*OutPos3.y);

    // 091111 ShinJS --- 이름출력 좌표를 부드럽게 움직이도록 한다.
	{
		// 이전좌표와 현재좌표와의 거리를 가속으로 사용. 경과시간만큼 현재좌표로 이동시킨다.
		float fMultipleTick = 0.001f;
		float fChangeDist = sqrt( pow(m_absPos.x - m_OldAbsPos.x, 2) + pow(m_absPos.y - m_OldAbsPos.y, 2) );
		float fTick = gTickTime * fMultipleTick * fChangeDist;

		if( fTick > 1 || fChangeDist > 1000 )
			fTick = 1;

		m_OldAbsPos.x = m_OldAbsPos.x + (m_absPos.x - m_OldAbsPos.x) * fTick;
		m_OldAbsPos.y = m_OldAbsPos.y + (m_absPos.y - m_OldAbsPos.y) * fTick;
		m_absPos = m_OldAbsPos;
	}

	if( m_pSSTitle &&
		m_pSSTitle->Render((long)m_absPos.x, (long)(m_absPos.y), 0 ) )
	{
		m_absPos.y -= 25.0f + 15.0f*m_pSSTitle->GetLineCount();
	}

	if( m_pSBSTitle &&
		m_pSBSTitle->Render((long)m_absPos.x, (long)(m_absPos.y), 1 ) )
	{
		m_absPos.y -= 25.0f + 15.0f*m_pSBSTitle->GetLineCount();
	}
	
	if( m_pChat &&
		m_pChat->Render((long)m_absPos.x,(long)(m_absPos.y) ) )
	{
		m_absPos.y -= m_pChat->GetBalloonHeight();
	}
	
	if( m_pName	&&
		m_pName->Render((long)m_absPos.x,(long)m_absPos.y) )
	{
		switch( m_pObject->GetObjectKind() )
 		{
		case eObjectKind_Player:
			{
 				CPlayer* player = ( CPlayer* )m_pObject;
				
				const float lineSize		= 15.0f;
				const float markSpace		= 2.0f;
				const float markSize		= GUILDMARKWIDTH + markSpace;
				cImageSelf*	familyMark		= player->GetFamilyEmblem();
				bool		isFamilyName	= false;

				{
					const char* name	= player->GetFamilyNickName();

					// 패밀리가 없는 경우에는 이름이 ""으로, 처음 생성된 경우에는 " " 상태로 세팅된다. 둘다 호칭이 없는 것이며, 따라서 두 상태를 체크해줘야 한다.
					// 패밀리 마크가 있을 경우에도 증가시키자.
					if( *name && strcmp( " ", name ) )
					{
						isFamilyName = true;
					}
				}

				if( !SIEGEWARFAREMGR->IsSiegeWarfareZone() )
				{
					if( familyMark )
					{
						VECTOR2 position;

						if( isFamilyName )
						{
							position.x = m_absPos.x + m_pName->GetFamilyNickPosX() - markSize;
							position.y = m_absPos.y + m_pName->GetTall() - lineSize - markSpace * 2;
						}
						else
						{
							//position.x = m_absPos.x + m_pName->GetPosX() - markSize;
							position.x = m_absPos.x + m_pName->GetPosX() + m_pName->GetNameLength();
							position.y = m_absPos.y + m_pName->GetTall() - markSpace * 2;
						}
						
						VECTOR2 scale;
						scale.x = 1;
						scale.y = 1;

						familyMark->RenderSprite( &scale, NULL, 0 , &position, 0xffffffff );
					}				
				}
				// E 패밀리 추가 added by hseos 2007.07.17

				// 길드 관련된 마크를 표시한다.
				{
					CGuildUnionMark*			unionMark	= GUILDUNIONMARKMGR->GetGuildUnionMark( 
						g_nServerSetNum,
						player->GetGuildUnionIdx(),
						player->GetGuildUnionMarkIdx() );

					float guildMarkX;
					float unionMarkX;

					{
						// 080827 LYW --- ObjectBalloon : 공성전일 때, 길드/연합마크 출력 위치를 보정한다.
						//if( m_byIsSiegeWarfare && SIEGEWARFAREMGR->Is_SiegeState() )
						if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
						{
							guildMarkX	= m_absPos.x + m_pName->GetPosX() - (markSize/2) ;

							if( m_pGuildMark && unionMark )
							{
								unionMarkX	= guildMarkX + markSpace + markSize ;
							}
							else
							{
								unionMarkX	= guildMarkX;
							}
						}
						else
						{
							guildMarkX	= m_absPos.x + m_pName->GetPosX() - markSpace - markSize;

							if( m_pGuildMark && unionMark )
							{
								unionMarkX	= guildMarkX - markSpace - markSize;
							}
							else
							{
								unionMarkX	= guildMarkX;
							}
						}
					}

					VECTOR2 position = {0};
					position.y = m_absPos.y - markSpace * 2;

					// 길드 마크를 찍는다. 길드 호칭 좌측에.
					if( m_pGuildMark )
					{
						position.x = guildMarkX;
						
						m_pGuildMark->Render( &position );
					}
					
					// 연합 마크를 찍는다. 길드 호칭 우측에
					if( unionMark )
					{
						position.x = unionMarkX;
						
						unionMark->Render( &position );
					}
				}
				
				if (m_csProgressBar.CLI_IsStarted() && player->GetID() == gHeroID)
				{
					VECTOR2 stPos;
 					stPos.x = m_absPos.x;
					stPos.y = m_absPos.y + 15;

					m_csProgressBar.CLI_Render(&stPos);
				}
				// E 농장시스템 추가 added by hseos 2007.09.20	2007.10.01

				break;
			}
		}
	}
}

//---

void CObjectBalloon::SetOverInfoOption( DWORD dwOption )
{
	if( dwOption )	//차후 옵션에 맞게 수정									// 옵션값이 1이면,
	{
		if( m_pName )		m_pName->SetActive( TRUE );						// 이름 정보가 유효하면, 활성화 시킨다.
		if( m_pChat )		m_pChat->SetActive( TRUE );						// 채팅 정보가 유효하면, 활성화 시킨다.
		if( m_pSSTitle )	m_pSSTitle->SetActive( TRUE );					// 노점 정보가 유효하면, 활성화 시킨다.
		if( m_pSBSTitle )	m_pSBSTitle->SetActive( TRUE );					// 노점 정보가 유효하면, 활성화 시킨다.

	}
	else																	// 옵션값이 1이 아니면,
	{
		if( m_pName )		m_pName->SetActive( FALSE );					// 이름 정보가 유효하면, 비활성화 시킨다.
		if( m_pChat )		m_pChat->SetActive( FALSE );					// 채팅 정보가 유효하면, 비활성화 시킨다.
		if( m_pSSTitle )	m_pSSTitle->SetActive( FALSE );					// 노점 정보가 유효하면, 비활성화 시킨다.
		if( m_pSBSTitle )	m_pSBSTitle->SetActive( FALSE );				// 노점 정보가 유효하면, 비활성화 시킨다.
	}
}

void CObjectBalloon::ShowObjectName( BOOL bShow, DWORD dwColor )
{
	if( !m_pName ) 
	{
		return;
	}
	m_pName->SetFGColor( dwColor );
	m_pName->Show( bShow );
}

void CObjectBalloon::ShowChatBalloon( BOOL bShow, char* chatMsg, DWORD dwColor, DWORD dwAliveTime )
{
	if( !m_pChat ) return;
	m_pChat->SetFGColor( dwColor );
	m_pChat->Show( bShow );

	if( bShow )
	{
		m_pChat->SetChat( chatMsg );
		m_pChat->SetDisappearTime( dwAliveTime );
	}
}

//---KES 상점검색 2008.3.11
void CObjectBalloon::ShowStreetStallTitle( BOOL bShow, char* strTitle, DWORD dwFontColor, DWORD dwBGColor )
{
	if( !m_pSSTitle ) return;
	m_pSSTitle->SetFGColor( dwFontColor );
	m_pSSTitle->SetBGColor( dwBGColor );
	m_pSSTitle->Show( bShow );

	if( bShow )
	{
		if( strTitle )
			m_pSSTitle->SetTitle( strTitle );
		else
			m_pSSTitle->SetTitle( CHATMGR->GetChatMsg(366) );
	}
}

void CObjectBalloon::ShowStreetBuyStallTitle( BOOL bShow, char* strTitle, DWORD dwFontColor, DWORD dwBGColor )
{
	if( !m_pSBSTitle ) return;
	m_pSBSTitle->SetFGColor( dwFontColor );
	m_pSBSTitle->SetBGColor( dwBGColor );
	m_pSBSTitle->Show( bShow );

	if( bShow )
	{
		if( strTitle )
			m_pSBSTitle->SetTitle( strTitle );
		else
			m_pSBSTitle->SetTitle( CHATMGR->GetChatMsg(366) );
	}
}
//-------------------

//090116 pdy 클라이언트 최적화 작업 (Hide 노점Title)
void CObjectBalloon::HideStreetStallTitle(BOOL bHide)		//노점 타이틀 그리기를 설정할뿐 
{
	bool bActive = !bHide;
	if( m_pSSTitle )	m_pSSTitle->SetActive( bActive );							// 노점 정보가 유효하면, (활성화or비활성화 )시킨다.
	if( m_pSBSTitle )	m_pSBSTitle->SetActive( bActive );							// 노점 정보가 유효하면, (활성화or비활성화 )시킨다.
}

BOOL CObjectBalloon::SetGuildMark( DWORD GuildIdx )
{
	m_pGuildMark = GUILDMARKMGR->GetGuildMark( GuildIdx );
	if( m_pGuildMark )	return TRUE;
	else				return FALSE;
}

void CObjectBalloon::SetNickName(char* NickName)
{
	m_pName->SetNickName(NickName);	
}

BOOL CObjectBalloon::SetFamilyMark( DWORD FamilyIdx )
{
	m_pFamilyMark = FAMILYMARKMGR->GetFamilyMark( FamilyIdx );
	if( m_pFamilyMark )	return TRUE;
	else				return FALSE;
}

void CObjectBalloon::SetFamilyNickName(char* NickName)
{
	m_pName->SetFamilyNickName(NickName);	
}

void CObjectBalloon::SetGTName(DWORD NameType, char* pGuildName)
{
	if(m_pName)
		m_pName->SetGTName(NameType, pGuildName);
}

void CObjectBalloon::Add( cWindow* )
{
	ASSERT( 0 && "It never contain any child control" );
}


void CObjectBalloon::SetObjectName( const char* name )
{
	m_pName->SetName( name );
}
