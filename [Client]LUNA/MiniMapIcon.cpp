#include "stdafx.h"
#include "MiniMapIcon.h"
#include "../[cc]header/GameResourceManager.h"
#include "Interface/cImage.h"
#include "Interface/cFont.h"
#include "Object.h"
#include "ObjectManager.h"
#include "Npc.h"
#include "cResourceManager.h"
#include "../hseos/Farm/SHFarm.h"

CMiniMapIconImage::~CMiniMapIconImage()
{
	SAFE_DELETE( m_pImage );
}


CMiniMapIcon::CMiniMapIcon()
{
	m_pIconImage = NULL;
	m_Kind = 0;
	m_Interpolation = 10;
	m_dwColor = 0xffffffff;
	m_bAlwaysTooltip = FALSE;
	m_ShowQuestMark = eQM_NONE;
	
	m_dwObjectID = 0;

	m_fImagePosRatio.x = 0.5f;
	m_fImagePosRatio.y = 0.5f;

	// 1 Frame의 시간 설정(기본 0.15초)
	m_dwFrameTick = 150;

	m_bUsePublicFrame = TRUE;
	m_dwLastCheckTime = 0;
	m_FrameIdx = 0;
}


CMiniMapIcon::~CMiniMapIcon()
{}


void CMiniMapIcon::SetMarkImage(CMiniMapIconImage* pMarkExclamation, CMiniMapIconImage* pMarkQuestion, SIZE_T size )
{
	if( pMarkQuestion )
	{
		m_pMarkQuestion.resize( size );
	}
	
	if( pMarkExclamation )
	{
		m_pMarkExclamation.resize( size );
	}

	for( SIZE_T i = 0; i < size; ++i )
	{
		if( pMarkExclamation )
		{
			m_pMarkExclamation[ i ] = pMarkExclamation	+ i;
		}

		if( pMarkQuestion )
		{
			m_pMarkQuestion[ i ]	= pMarkQuestion		+ i;
		}
	}
}

void CMiniMapIcon::Init(CMiniMapIconImage* pIconImage, int Kind )
{
	m_pIconImage = pIconImage;
	m_Kind = Kind;
}

void CMiniMapIcon::SetData( DWORD dwObjectID, VECTOR3* pObjectPos, PARTY_MEMBER* pMember, CSHFarm* pFarm )
{
	m_dwObjectID = dwObjectID;

	if( pObjectPos )
		m_ObjectPos = *pObjectPos;

	if( pMember )
		strcpy(m_MemberName, pMember->Name);

	if( pFarm )
	{
		strcpy(m_MemberName, RESRCMGR->GetMsg(799));
		m_ObjectPos = *pFarm->GetPos();
	}
}

VECTOR3 CMiniMapIcon::GetPosition()
{
	CObject* pObject = OBJECTMGR->GetObject( m_dwObjectID );
	if( pObject )
	{
		pObject->GetPosition( &m_ObjectPos );
	}
	return m_ObjectPos;
}

void CMiniMapIcon::Update(DWORD MinimapImageWidth,DWORD MinimapImageHeight)
{
	if(m_Kind == eMNMICON_PARTYMEMBER)
	{
		m_ObjectPos.x += (m_TargetPos.x - m_ObjectPos.x)/m_Interpolation;
		m_ObjectPos.z += (m_TargetPos.z - m_ObjectPos.z)/m_Interpolation;		
	}
	else if(CObject* pObject = OBJECTMGR->GetObject(m_dwObjectID))
	{
		if(eObjectKind_FarmObj != pObject->GetObjectKind())
		{
			pObject->GetPosition(
				&m_ObjectPos);
		}
	}

	m_ScreenPos.x = (m_ObjectPos.x / 51200)*MinimapImageWidth;
	m_ScreenPos.y = MinimapImageHeight - (m_ObjectPos.z / 51200)*MinimapImageHeight;
}

void CMiniMapIcon::Render(VECTOR2* pMapStartPixel,RECT* pCullRect)
{
	// m_pIconImage가 없는 경우 Render 하지 않도록 한다.
	if( !m_pIconImage )
		return;

	if( pCullRect )
	{
		VECTOR3 v1;
		VECTOR3 v2;

		v1.x = ( float )( ( pCullRect->left + pCullRect->right ) / 2 + 3);
		v1.z = ( float )( ( pCullRect->top + pCullRect->bottom ) / 2 - 3);

		v2.x = m_ScreenPos.x;
		v2.z = m_ScreenPos.y;

		float range = CalcDistanceXZ( &v1, &v2 );

		float dis = ( float )( ( pCullRect->right - pCullRect->left ) / 2 );

		if( range > dis )
			return;
	}

	VECTOR2 trans;
	trans.x = m_ScreenPos.x - m_pIconImage->m_ImageWH.x*m_fImagePosRatio.x + pMapStartPixel->x;
	trans.y = m_ScreenPos.y - m_pIconImage->m_ImageWH.y*m_fImagePosRatio.y + pMapStartPixel->y;
	
	m_pIconImage->m_pImage->RenderSprite(NULL,NULL,0,&trans,m_dwColor);
}

void CMiniMapIcon::RenderQuestMark( VECTOR2* pMapStartPixel, RECT* pCullRect )
{
	if( m_ShowQuestMark == eQM_NONE )
	{
		return;
	}

	if( pCullRect )
	{
		VECTOR3 v1;
		v1.x = ( float )( ( pCullRect->left + pCullRect->right ) / 2 + 3);
		v1.z = ( float )( ( pCullRect->top + pCullRect->bottom ) / 2 - 3);

		VECTOR3 v2;
		v2.x = m_ScreenPos.x;
		v2.z = m_ScreenPos.y;

		const float range	= CalcDistanceXZ( &v1, &v2 );
		const float dis		= ( float )( ( pCullRect->right - pCullRect->left ) / 2 );

		if( range > dis )
		{
			return;
		}
	}

	// 091005 ShinJS --- 같은 프레임을 사용하는 경우
	static SIZE_T index;
	static DWORD dwTime = gCurTime;

	if( m_bUsePublicFrame )
	{
		// 특정 시간이 될때마다 인덱스를 0~2로 순차 변경한다.
		if( gCurTime - dwTime > m_dwFrameTick )
		{
			static const SIZE_T size = max( m_pMarkExclamation.size(), m_pMarkQuestion.size() );

			if( size > 0 )
				index	= ++index % size;
			dwTime	= gCurTime;
		}

		m_FrameIdx = index;
	}	
	else
	{
		if( gCurTime - m_dwLastCheckTime > m_dwFrameTick )
		{
			SIZE_T size = max( m_pMarkExclamation.size(), m_pMarkQuestion.size() );

			if( size > 0 )
				m_FrameIdx	= ++m_FrameIdx % size;
			m_dwLastCheckTime = gCurTime;
		}
	}
	
	switch( m_ShowQuestMark )
	{
	case eQM_EXCLAMATION:
	case eQM_EXCLAMATION_HAS_TIMEQUEST:
		{
			if( m_FrameIdx < m_pMarkExclamation.size() )
			{
				CMiniMapIconImage* icon = m_pMarkExclamation[ m_FrameIdx ];

				VECTOR2 trans;
				trans.x = m_ScreenPos.x - icon->m_ImageWH.x * m_fImagePosRatio.x + pMapStartPixel->x - 1;
				trans.y = m_ScreenPos.y - icon->m_ImageWH.y * m_fImagePosRatio.y + pMapStartPixel->y - 1;

				icon->m_pImage->RenderSprite( NULL, NULL, 0.0f, &trans, m_dwColor );		
			}

			break;
		}
	case eQM_QUESTION:
	case eQM_QUESTION_HAS_TIMEQUEST:
		{
			if( m_FrameIdx < m_pMarkQuestion.size() )
			{
				CMiniMapIconImage* icon = m_pMarkQuestion[ m_FrameIdx ];

				VECTOR2 trans;
				trans.x = m_ScreenPos.x - icon->m_ImageWH.x * m_fImagePosRatio.x + pMapStartPixel->x - 1;
				trans.y = m_ScreenPos.y - icon->m_ImageWH.y * m_fImagePosRatio.y + pMapStartPixel->y - 1;

				icon->m_pImage->RenderSprite( NULL, NULL, 0.0f, &trans, m_dwColor );				
			}

			break;
		}
	}
}

int CMiniMapIcon::GetToolTip(int x,int y,VECTOR2* pMapStartPixel,char* pOutText, float* distOut )
{
	// m_pIconImage가 없는 경우 ToolTip을 그리지 않도록 한다.
	if( !m_pIconImage )
		return 0;

	if( m_dwObjectID == 0 && m_Kind != eMNMICON_PARTYMEMBER && m_Kind != eMNMICON_FARM)
		return 0;

	if( pMapStartPixel == NULL ) return 0;

	int gapx = x - int(m_ScreenPos.x+pMapStartPixel->x);
	int gapy = y - int(m_ScreenPos.y+pMapStartPixel->y);
	int halfx = int(m_pIconImage->m_ImageWH.x*m_fImagePosRatio.x);
	int halfy = int(m_pIconImage->m_ImageWH.y*m_fImagePosRatio.y);
		
	if(gapx > halfx || gapx < -halfx)
		return 0;
	if(gapy > halfy || gapy < -halfy)
		return 0;

	if( distOut )
		*distOut = sqrtf( (float)(gapx*gapx + gapy*gapy) );

	switch( m_Kind )
	{
	case eMNMICON_PARTYMEMBER:
		{
			strcpy(pOutText, m_MemberName);
		}
		return 2;
	case eMNMICON_FARM:
		{
			strcpy(pOutText, m_MemberName);
		}
		return 3;
	default:
		{
			char* npcName = NULL;
			WORD wNpcKind = 0;
			CObject* pObject = OBJECTMGR->GetObject( m_dwObjectID );
			STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo( (WORD)m_dwObjectID );
			if( pObject != NULL )
			{
				npcName = pObject->GetObjectName();
				if( pObject->GetObjectKind() == eObjectKind_Npc )
				{
					wNpcKind = ((CNpc*)pObject)->GetNpcKind();
				}
			}
			else if( pStaticNpcInfo != NULL )
			{
				npcName = pStaticNpcInfo->NpcName;
				wNpcKind = pStaticNpcInfo->wNpcJob;
			}

			if( !npcName )
				return 0;

			NPC_LIST* pInfo = GAMERESRCMNGR->GetNpcInfo( wNpcKind );
			if( pInfo != NULL && pInfo->ShowJob )
			{
				wsprintf( pOutText, "%s(%s)", npcName, pInfo->Name );
			}
			else
			{
				wsprintf( pOutText, "%s", npcName );
			}
		}
		break;
	}

	return 1;
}