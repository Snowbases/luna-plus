// TileManager.cpp: implementation of the CTileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileManager.h"
#include "Object.h"
#include "MHTimeManager.h"

///
/// 길찾기용 수학함수.
///

H2DLine::H2DLine( VECTOR2 p_P1, VECTOR2 p_P2 )
{
	// 직선이 x = b 형태일때 체크
	if ( (p_P1.x - p_P2.x) == 0 )
	{
		m_Flag_x_equal_b = TRUE;
		m_a = 0;
		m_b = p_P1.x;
	}
	else
	{
		m_Flag_x_equal_b = FALSE;
		m_a = (p_P1.y - p_P2.y) / (p_P1.x - p_P2.x);
		m_b = p_P1.y - (m_a * p_P1.x);
	}
}

eH2DLineSide H2DLine::CheckSide( VECTOR2 p_Point )
{
	if ( m_Flag_x_equal_b == TRUE )
	{
		if ( p_Point.x < m_b )
		{
			return eH2DLineSide_Up_Left;
		}
		else if (p_Point.x == m_b)
		{
			return eH2DLineSide_Equal;
		}
		else
		{
			return eH2DLineSide_Down_Right;
		}
	}
	else
	{
		float t_Y = (m_a * p_Point.x) + m_b;

		if ( t_Y < p_Point.y )
		{
			return eH2DLineSide_Up_Left;
		}
		else if (t_Y == p_Point.y)
		{
			return eH2DLineSide_Equal;
		}
		else
		{
			return eH2DLineSide_Down_Right;
		}
	}
}

BOOL CollisionCheck_2D_Segment_Line( VECTOR2 p_SegA, VECTOR2 p_SegB, VECTOR2 p_LineA, VECTOR2 p_LineB )
{
	// 직선을 만듦
	H2DLine l_Line = H2DLine(p_LineA, p_LineB);

	return CollisionCheck_2D_Segment_Line(p_SegA, p_SegB, l_Line);
}

BOOL CollisionCheck_2D_Segment_Line( VECTOR2 p_SegA, VECTOR2 p_SegB, H2DLine& p_Line )
{
	// 직선과 점과 체크
	// 위쪽인지 아래쪽인지, 아니면 겹치는지! 체크
	eH2DLineSide l_SideA = p_Line.CheckSide(p_SegA);
	eH2DLineSide l_SideB = p_Line.CheckSide(p_SegB);

	// 선분의 두 개의 점이 다른 방향에 있다면 선분과 직선이 교차하는 것
	// 같은 방향에 있다면 교차하지 않음
	// 점이 직선 위에 있다면 교차로 인정

	// 한 점이라도 선분 위에 있다면 교차함
	if (l_SideA == eH2DLineSide_Equal || l_SideB == eH2DLineSide_Equal)
	{
		return TRUE;
	}
	// 각 점의 방향이 다르면 교차함
	else if (l_SideA == l_SideB)
	{
		// 점이 직선 위에 있는 경우 위에서 걸렀다
		// 또한 두 점이 직선 위에 있는 경우는 일치하는 경우다(교차가 의미가 없다)
		return FALSE;
	}

	return TRUE;
}

CTileManager::CTileManager() :
m_pAllNode1x(0),
m_pAllNode3x(0)
{
	m_pTile = NULL;
}

CTileManager::~CTileManager()
{
	SAFE_DELETE_ARRAY(m_pAllNode1x);
	SAFE_DELETE_ARRAY(m_pAllNode3x);
}

CTile* CTileManager::GetTile(DWORD dwX, DWORD dwZ)
{
	if(!(dwX < m_dwTileWidth && dwZ < m_dwTileHeight))
		return NULL;

	return (m_pTile + (m_dwTileHeight * dwZ + dwX));
}

CTile* CTileManager::GetTile(float fx, float fz)
{
	if(!(fx >= 0 && fz >= 0))
		return NULL;	

	DWORD z = DWORD(fz / fTILE_WIDTH);
	DWORD x = DWORD(fx / fTILE_HEIGHT);

	return GetTile(x, z);
}

BOOL CTileManager::LoadTileInfo(char* TileFile)
{
	DWORD dwRead = 0;
	HANDLE hFile = CreateFile(TileFile,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	ReadFile(hFile, &m_dwTileWidth, sizeof(m_dwTileWidth), &dwRead, NULL);
	ReadFile(hFile, &m_dwTileHeight, sizeof(m_dwTileHeight), &dwRead, NULL);
	
	m_pTile = new CTile[ m_dwTileWidth * m_dwTileHeight ];
	CTile* l_Node = NULL;
	TILE_ATTR l_Attrib;

	for (DWORD j=0; j<m_dwTileHeight; j++)
	{
		for (DWORD i=0; i<m_dwTileWidth; i++)
		{
			ReadFile(hFile, &l_Attrib , sizeof(TILE_ATTR), &dwRead, NULL);
			l_Node = m_pTile + (m_dwTileWidth * j) + i;

			if (l_Node)
			{
				l_Node->SetPosX(i);
				l_Node->SetPosY(j);
				l_Node->SetTileAttrib(l_Attrib);
			}
		}
	}

	CloseHandle(hFile);


	/// 1배 레이어 셋팅
	m_dwWidth1x = m_dwTileWidth;
	m_dwHeight1x = m_dwTileHeight;
	m_pAllNode1x = m_pTile;
	

	/// 3배 타일 레이어 계산

	// 가로세로
	m_dwWidth3x = m_dwTileWidth/3;
	m_dwHeight3x = m_dwTileHeight/3;

	// 1x짜리 타일을 참조해서 큰 타일 레이어를 생성한다
	m_pAllNode3x = new CTile[ m_dwWidth3x * m_dwHeight3x ];

	for (DWORD t_Y=0; t_Y < m_dwHeight3x; t_Y++ )
	{
		for (DWORD t_X=0; t_X < m_dwWidth3x; t_X++ )
		{
			// 9개의 하부 타일이 모두 갈 수 있는 곳일 경우만 Ok
			CTile* t_Tile_1 = GetNode(t_X*3, t_Y*3);
			CTile* t_Tile_2 = GetNode(t_X*3+1, t_Y*3);
			CTile* t_Tile_3 = GetNode(t_X*3+2, t_Y*3);
			CTile* t_Tile_4 = GetNode(t_X*3, t_Y*3+1);
			CTile* t_Tile_5 = GetNode(t_X*3+1, t_Y*3+1);
			CTile* t_Tile_6 = GetNode(t_X*3+2, t_Y*3+1);
			CTile* t_Tile_7 = GetNode(t_X*3, t_Y*3+2);
			CTile* t_Tile_8 = GetNode(t_X*3+1, t_Y*3+2);
			CTile* t_Tile_9 = GetNode(t_X*3+2, t_Y*3+2);

			if ( t_Tile_1 && t_Tile_2 && t_Tile_3 && t_Tile_4 &&
				t_Tile_5 && t_Tile_6 && t_Tile_7 && t_Tile_8 && t_Tile_9 )
			{
				if ( !t_Tile_1->IsCollisonTile() &&
					!t_Tile_2->IsCollisonTile() &&
					!t_Tile_3->IsCollisonTile() &&
					!t_Tile_4->IsCollisonTile() &&
					!t_Tile_5->IsCollisonTile() &&
					!t_Tile_6->IsCollisonTile() &&
					!t_Tile_7->IsCollisonTile() &&
					!t_Tile_8->IsCollisonTile() &&
					!t_Tile_9->IsCollisonTile() )
				{
					TILE_ATTR l_Attrib;
					l_Attrib.uAttr = 0;
					l_Attrib.uSkillObjectBlock = 0;
					m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetTileAttrib(l_Attrib);
					m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosX(t_X);
					m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosY(t_Y);

				}
				else
				{
					TILE_ATTR l_Attrib;
					l_Attrib.uAttr = 1;
					l_Attrib.uSkillObjectBlock = 0;
					m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetTileAttrib(l_Attrib);
					m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosX(t_X);
					m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosY(t_Y);
				}

			}
		}
	}

	return TRUE;
}


BOOL CTileManager::IsInTile(int cellX, int cellY, MAPTYPE MapNum,CObject* pObject)
{
	if( cellX < 0 || cellY < 0 || cellX >= (int)m_dwTileWidth || cellY >= (int)m_dwTileHeight )
			return FALSE;
		return TRUE;
}
BOOL CTileManager::CollisionTile( int x, int y, MAPTYPE MapNum,CObject* pObject)
{ 
	CTile *pTile = GetTile((DWORD)x, (DWORD)y);
	if(pTile)
		return pTile->IsCollisonTile();
	else
	{
		char temp[256];
		sprintf(temp,"%d %d %s",x,y,pObject->GetObjectName());
		ASSERTMSG(0,temp);
		return TRUE;
	}
}	
BOOL CTileManager::CollisionLine(VECTOR3* pSrc,VECTOR3* pDest, VECTOR3 * pTarget, MAPTYPE MapNum,CObject* pObject)
{
	BOOL bEscape = FALSE;

	int x1 = int(pSrc->x / TILECOLLISON_DETAIL);
	int y1 = int(pSrc->z / TILECOLLISON_DETAIL);
	int x2 = int(pDest->x / TILECOLLISON_DETAIL);
	int y2 = int(pDest->z / TILECOLLISON_DETAIL);

	int dx = x2 - x1;
	int dy = y2 - y1;
	
	int absDX = abs(dx);
	int absDY = abs(dy);
	int MaxDelta = max(absDX, absDY);
	
	int CellX = x1;
	int CellY = y1;
	int x = 0;
	int y = 0;

	int signDX = sign(dx);
	int signDY = sign(dy);
	
	int PrevCellX = 0;
	int PrevCellY = 0;

	for( int i = 0 ; i <= MaxDelta ; ++i )
	{
		PrevCellX = CellX;
		PrevCellY = CellY;

		x += absDX;  
		y += absDY;
		
		if( x > MaxDelta)  
		{  
			x -= MaxDelta;
			CellX += signDX;
		}
		
		if( y > MaxDelta ) 
		{  
			y -= MaxDelta;  
			CellY += signDY;  
		}
		CTile *pTile = GetTile((DWORD)CellX, (DWORD)CellY);
		if(pTile == NULL)
		{
			if(pTarget)
			{
				pTarget->x = PrevCellX*TILECOLLISON_DETAIL;
				pTarget->y = 0;
				pTarget->z = PrevCellY*TILECOLLISON_DETAIL;
			}
			
			char temp[256];
			sprintf(temp,"%d %d %s",CellX,CellY,pObject->GetObjectName());
			ASSERTMSG(0,temp);
			
			return TRUE;
		}
		if(pTile->IsCollisonTile())
		{
			/// 현재위치가 비정상적인 위치라면 탈출을 시도한다.
			if( i == 0 )
			{
				bEscape = TRUE;
				continue;
			}
			/// 탈출을 시도할때는 현재위치와 바로 앞 위치를 무시한다.
			if( bEscape && i < 2)
			{
				continue;
			}
			if(pTarget)
			{
				pTarget->x = PrevCellX*TILECOLLISON_DETAIL;
				pTarget->y = 0;
				pTarget->z = PrevCellY*TILECOLLISON_DETAIL;
				/*
				CTile *p2Tile = GetTile((DWORD)PrevCellX, (DWORD)PrevCellY);
								if(p2Tile->IsCollisonTile())
								{
									int a = 3;
								}*/
				
			}
			return TRUE;
		}
	}

	if(pTarget)
	{
		pTarget->x = x2*TILECOLLISON_DETAIL;
		pTarget->y = 0;
		pTarget->z = y2*TILECOLLISON_DETAIL;
	}
	return FALSE;
}

BOOL CTileManager::NonCollisionLine( VECTOR3* pStart, VECTOR3* pEnd, VECTOR3* pRtNonCollisonPos, MAPTYPE MapNum, CObject* pObj )
{
	int x1 = int(pStart->x / TILECOLLISON_DETAIL);
	int y1 = int(pStart->z / TILECOLLISON_DETAIL);
	int x2 = int(pEnd->x / TILECOLLISON_DETAIL);
	int y2 = int(pEnd->z / TILECOLLISON_DETAIL);

	int dx = x1 - x2;
	int dy = y1 - y2;
	
	int absDX = abs(dx);
	int absDY = abs(dy);
	int MaxDelta = max(absDX, absDY);
	
	int CellX = x1;
	int CellY = y1;

	CTile *pCurTile = GetTile((DWORD)CellX, (DWORD)CellY);
	if(pCurTile == NULL)
	{
		if(pRtNonCollisonPos)
		{
			pRtNonCollisonPos->x = CellX * TILECOLLISON_DETAIL;
			pRtNonCollisonPos->y = 0;
			pRtNonCollisonPos->z = CellY * TILECOLLISON_DETAIL;
		}

		char temp[256] = {0,};
		sprintf(temp, "%d %d %s", CellX, CellY, pObj->GetObjectName() );
		ASSERTMSG(0,temp);

		return FALSE;
	}
	if(FALSE == pCurTile->IsCollisonTile())	//현재 위치가 충돌 타일이 아니면 이 함수의 역할과 무관
	{
		if(pRtNonCollisonPos)
		{
			pRtNonCollisonPos->x = CellX * TILECOLLISON_DETAIL;
			pRtNonCollisonPos->y = 0;
			pRtNonCollisonPos->z = CellY * TILECOLLISON_DETAIL;
		}

		return FALSE;
	}

	int x = 0;
	int y = 0;

	int signDX = -sign(dx);
	int signDY = -sign(dy);
	
	int PrevCellX = 0;
	int PrevCellY = 0;

#define THROUGH_PERMIT_TILE_INDEX 2

	for( int i = 0 ; i <= MaxDelta ; ++i )
	{
		if( i > THROUGH_PERMIT_TILE_INDEX )
		{
			if(pRtNonCollisonPos)
			{
				pRtNonCollisonPos->x = CellX*TILECOLLISON_DETAIL;
				pRtNonCollisonPos->y = 0;
				pRtNonCollisonPos->z = CellY*TILECOLLISON_DETAIL;
			}

			return FALSE;
		}

		PrevCellX = CellX;
		PrevCellY = CellY;

		x += absDX;  
		y += absDY;
		
		if( x > MaxDelta)  
		{  
			x -= MaxDelta;
			CellX += signDX;
		}
		
		if( y > MaxDelta ) 
		{  
			y -= MaxDelta;  
			CellY += signDY;  
		}
		CTile *pTile = GetTile((DWORD)CellX, (DWORD)CellY);
		if(pTile == NULL)
		{
			if(pRtNonCollisonPos)
			{
				pRtNonCollisonPos->x = PrevCellX*TILECOLLISON_DETAIL;
				pRtNonCollisonPos->y = 0;
				pRtNonCollisonPos->z = PrevCellY*TILECOLLISON_DETAIL;
			}
			
			char temp[256];
			sprintf(temp,"%d %d %s",CellX,CellY,pObj->GetObjectName());
			ASSERTMSG(0,temp);
			
			return FALSE;
		}
		if( FALSE == pTile->IsCollisonTile() )
		{
			if(pRtNonCollisonPos)
			{
				pRtNonCollisonPos->x = CellX*TILECOLLISON_DETAIL;
				pRtNonCollisonPos->y = 0;
				pRtNonCollisonPos->z = CellY*TILECOLLISON_DETAIL;
			}
			return TRUE;
		}
	}

	if(pRtNonCollisonPos)
	{
		pRtNonCollisonPos->x = x1*TILECOLLISON_DETAIL;
		pRtNonCollisonPos->y = 0;
		pRtNonCollisonPos->z = y1*TILECOLLISON_DETAIL;
	}
	return FALSE;
}

// pObj ´Â µð¹ö±×¿ë

BOOL CTileManager::CollisonCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos,CObject* pObj)
{	
	if( pStart->x >= 51200 || pStart->x < 0 ||
		pStart->z >= 51200 || pStart->z < 0 )
	{
		ASSERTMSG(0,"ÀÌµ¿¿µ¿ªÀ» ¹þ¾î³µ½À´Ï´Ù.");
		pRtCollisonPos->x = 25000;
		pRtCollisonPos->z = 25000;
		return TRUE;
	}

	int x0 = int(pStart->x / fTILE_WIDTH);
	int y0 = int(pStart->z / fTILE_HEIGHT);
	int x1 = int(pEnd->x / fTILE_WIDTH);
	int y1 = int(pEnd->z / fTILE_HEIGHT);
	
	int dx = x1-x0,dy = y1-y0;
	int sx,sy;
	if(dx >= 0)
	{
		sx = 1;
	}
	else
	{
		sx = -1;
		dx *= -1;
	}
	if(dy >= 0)
	{
		sy = 1;
	}
	else
	{
		sy = -1;
		dy *= -1;
	}
	
	int ax = 2*dx,ay = 2*dy;

	int x = x0, y = y0;
	int lastx = x, lasty = y;
	
	BOOL bFirst = TRUE;
	if(dx == 0 && dy == 0)
	{
		//bFirst = FALSE;
		return FALSE;	//CSW
	}
	
	if(dx >= dy)
	{
		for(int desc = ay - dx; ;x += sx,desc += ay)
		{
			if(bFirst == TRUE)
			{
				bFirst = FALSE;
				continue;
			}

			CTile *pTile = GetTile((DWORD)x, (DWORD)y);
			
#ifdef _DEBUG
			if(pTile == NULL)
			{
				//LOGFILE("TILENOTFOUND\t%s",OBJECTLOG(pObj));
				//LOGFILE("StartPos\t%4.2f\t%4.2f\t\tEndPos\t%4.2f\t%4.2f",
				//	pStart->x,pStart->z,pEnd->x,pEnd->z);
				ASSERTMSG(0,"ÀÌ ¿¡·¯ ¸Þ¼¼Áö º¸½Ã¸é À±È£ÇÑÅ× ²À! ¿¬¶ôÇØÁÖ¼¼¿ä");
				break;
			}			
#endif
			if(pTile == NULL || pTile->IsCollisonTile())
			{
				pRtCollisonPos->x = (float)(lastx*fTILE_WIDTH);
				pRtCollisonPos->y = 0;
				pRtCollisonPos->z = (float)(lasty*fTILE_HEIGHT);
				return TRUE;
			}

			if(x == x1)
				break;

			lastx = x;
			lasty = y;

			if(desc > 0)
			{
				y += sy;
				desc -= ax;
			}
		}
	}
	else
	{
		for(int desc = ax - dy; ;y += sy,desc += ax)
		{
			if(bFirst == TRUE)
			{
				bFirst = FALSE;
				continue;
			}

			CTile *pTile = GetTile((DWORD)x, (DWORD)y);
			
#ifdef _DEBUG
			if(pTile == NULL)
			{
				//LOGFILE("TILENOTFOUND\t%s",OBJECTLOG(pObj));
				//LOGFILE("StartPos\t%4.2f\t%4.2f\t\tEndPos\t%4.2f\t%4.2f",
				//	pStart->x,pStart->z,pEnd->x,pEnd->z);
				ASSERTMSG(0,"ÀÌ ¿¡·¯ ¸Þ¼¼Áö º¸½Ã¸é À±È£ÇÑÅ× ²À! ¿¬¶ôÇØÁÖ¼¼¿ä");
				break;
			}			
#endif
			if(pTile == NULL || pTile->IsCollisonTile())
			{
				pRtCollisonPos->x = (float)(lastx*fTILE_WIDTH);
				pRtCollisonPos->y = 0;
				pRtCollisonPos->z = (float)(lasty*fTILE_HEIGHT);
				return TRUE;
			}

			if(y == y1)
				break;
			
			lastx = x;
			lasty = y;

			if(desc > 0)
			{
				x += sx;
				desc -= ay;
			}
		}
	}
	return FALSE;
}

void CTileManager::AddTileAttrByAreaData(CObject* Object,VECTOR3* pPos,AREATILE areatile)
{
	CTile *pTile = GetTile(pPos->x, pPos->z);
	if(pTile == 0)
	{
		ASSERT(0);
		return;
	}
	pTile->AddTileAttrib(areatile);

	// 091208 ShinJS --- 3배속 타일도 수정한다.
	UpdateNodeInfo( pPos );
}

void CTileManager::RemoveTileAttrByAreaData(CObject* Object,VECTOR3* pPos,AREATILE areatile)
{
	CTile *pTile = GetTile(pPos->x, pPos->z);
	if(pTile == 0)
	{
		ASSERT(0);
		return;
	}
	pTile->RemoveTileAttrib(areatile);

	// 091208 ShinJS --- 3배속 타일도 수정한다.
	UpdateNodeInfo( pPos );
}


void CTileManager::SetPathFindMode( ePathFindLayer p_PathFindMode )
{
	switch(p_PathFindMode)
	{
	case ePathFindLayer_1x:
		{
			m_dwTileWidth = m_dwWidth1x;
			m_dwTileHeight = m_dwHeight1x;
			m_pTile = m_pAllNode1x;
			m_NodeRatio = 50;
			break;
		}
	case ePathFindLayer_3x:
		{
			m_dwTileWidth = m_dwWidth3x;
			m_dwTileHeight = m_dwHeight3x;
			m_pTile = m_pAllNode3x;
			m_NodeRatio = 150;
			break;
		}
	default:
		{
			break;
		}
	}

}

ePathFindLayer CTileManager::GetPathFindMode()
{
	if (m_pTile == m_pAllNode1x) return ePathFindLayer_1x;
	else if (m_pTile == m_pAllNode3x) return ePathFindLayer_3x;

	return ePathFindLayer_1x;
}

///--------------------------------------------------
///	길찾기 (090907 ShinJS --- 5팀 Soruce 적용)
///--------------------------------------------------
// 인접정보는 상수접근시간으로 노드 접근한다(상하좌우)

///
/// 길찾기를 하기 위한 중간 데이터(각종 리스트 등)를 리셋
void CTileManager::AStarDataReset()
{
	// 모든 노드의 부모를 없애주고, A-Star상태도 비워준다
	/// ttb가 1024*1024개이므로 리셋하는데도 꽤 큰 시간이 들 것이다
	/// 리셋 시간도 재 보고, 적당한 방법을 찾자
	for(std::multimap<float, IPathNode*>::iterator it_NowNode = m_map_OpenList.begin();
		it_NowNode != m_map_OpenList.end();
		++it_NowNode)
	{
		IPathNode* t_Node = it_NowNode->second;
		t_Node->Reset();
	}

	for (UINT i = 0; i<m_vec_ClosedList.size(); i++)
	{
		m_vec_ClosedList[i]->Reset();
	}
	for (UINT i = 0; i<m_AStarPath.size(); i++)
	{
		m_AStarPath[i]->Reset();
	}
	for (UINT i = 0; i<m_AStarPath_Optimize.size(); i++)
	{
		m_AStarPath_Optimize[i]->Reset();
	}

	// 리스트 리셋
	m_map_OpenList.clear();
	m_vec_ClosedList.clear();
	m_AStarPath.clear();
	m_AStarPath_Optimize.clear();

	m_StartNode = NULL;
	m_DestNode = NULL;
	m_NowNode = NULL;

	// 외부 내보내기용, 디버그 그리기용 리스트 리셋
	m_PathResult.clear();
	m_PathResult_Optimize.clear();
	m_PathResult_Optimize_Debug.clear();
}

/// 시작 노드와 끝 노드를 입력
/// 현재 노드가 무엇이냐 하는것과, 목표 노드가 중요하다!
BOOL CTileManager::InputPoint( IPathNode* p_StartNode, IPathNode* p_DestNode )
{
	// 시작 노드나 끝 노드가 없거나 같으면 길찾기를 하지 않는다
	if ( p_StartNode == NULL || p_DestNode == NULL )
	{
		HTRACE("시작 노드나 도착 노드가 없습니다. 길찾기를 하지 않습니다\n");
		return FALSE;
	}

	if ( p_StartNode == p_DestNode )
	{
		HTRACE("시작 노드와 도착 노드가 같습니다. 길찾기를 하지 않습니다\n");
		return FALSE;
	}

	// 시작 노드와 끝 노드를 입력해 놓아야 비용 계산이 가능하다
	m_StartNode = p_StartNode;
	m_DestNode = p_DestNode;

	// 휴리스틱을 계산하기 위한 설정을 한다
	// ->ttb는 정방형격자이므로 특별한 셋팅이 없이 노드간 거리계산만으로 충분하다

	// 열린 노드에 시작지점(현재 위치)를 넣는다
	m_NowNode = m_StartNode;
	m_NowNode->Calc_Cost(m_DestNode);
	m_map_OpenList.insert( std::pair<float, IPathNode*>( m_NowNode->GetFCost(), m_NowNode ) );
	
	return TRUE;
}

BOOL CTileManager::InputPoint( const VECTOR3& p_StartPosition, const VECTOR3& p_DestPosition )
{
	return InputPoint( GetMovableNode(p_StartPosition.x, p_StartPosition.z),
					   GetMovableNode(p_DestPosition.x, p_DestPosition.z) );
}

BOOL CTileManager::PathFind_AStar( UINT p_LoopCount )
{
	while(false == m_map_OpenList.empty())
	{
		if(0 == --p_LoopCount)
		{
			break;
		}

		/// 열린 목록에서 가장 F비용이 낮은 노드를 찾아 현재 선택된 노드로 한다
		// (모든 열린 노드는 이미 G값과 H값이 계산된 상태)
		// 현재 선택 노드(맨 처음에는 시작 노드가 된다)
		// 현재 선택 노드를 열린 목록에서 제거
		std::multimap<float, IPathNode*>::iterator it_NowNode = m_map_OpenList.begin();
		m_NowNode = it_NowNode->second;
		m_map_OpenList.erase( it_NowNode );

		/// 현재 선택 노드를 닫힌 목록에 추가
		m_vec_ClosedList.push_back( m_NowNode );
		m_NowNode->SetAStarStatus( eClosed );

		/// 현재 노드의 인접 노드를 얻어낸다
		IPathNode* t_ProximityNode = NULL;

		/// 인접 노드를 돌면서 조사한다
		for (int i=0; i<4; i++)
		{
			if ( i == 0 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX(), m_NowNode->GetPosY()-1 );	// 상
			if ( i == 1 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX(), m_NowNode->GetPosY()+1 );	// 하
			if ( i == 2 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX()-1, m_NowNode->GetPosY() );	// 좌
			if ( i == 3 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX()+1, m_NowNode->GetPosY() );	// 우
			/// 타일 이동과 속도상 문제로 대각선의 길은 찾지 않는다
			//if ( i == 4 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX()-1, m_NowNode->GetPosY()-1 );	// 좌상
			//if ( i == 5 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX()+1, m_NowNode->GetPosY()-1 );	// 우상
			//if ( i == 6 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX()-1, m_NowNode->GetPosY()+1 );	// 좌하
			//if ( i == 7 ) t_ProximityNode = GetMovableNode( m_NowNode->GetPosX()+1, m_NowNode->GetPosY()+1 );	// 우하

			if (t_ProximityNode)
			{
				/// 인접 노드 t_Proximity에 대해서 처리:
				if ( t_ProximityNode->GetAStarStatus() == eUnidentified )
				{
					// 1. 닫힌 목록상에 있거나 열린 목록상에 있지 않다면 추가.
					//		추가되는 노드의 F = G+H를 저장해준다
					//		추가되는 노드들은 현재 선택 노드를 부모로 해 준다
					t_ProximityNode->Astar_MakeOpened( m_NowNode, m_DestNode );
					m_map_OpenList.insert( std::pair<float, IPathNode*>(t_ProximityNode->GetFCost(), t_ProximityNode) );
				}
				else if ( t_ProximityNode->GetAStarStatus() == eOpened )
				{
					// 2. 이미 열린 목록상에 있었다면 G비용을 재서 현재 선택 노드를 통해서 
					//		가는 것이 더 좋은가 알아보고, 더 좋다면 부모를 바꾼다
					//		(t_NowSelectedFace의 G값과 t_Proximity과의 이동 경로를 더해서
					//		t_Proximity의 G값과 비교, 더 작다면 t_Proximity의 부모를 t_NowSelectedFace로 바꿈)
					//		또한 G값과 F값을 다시 계산하고, 열린목록을 재정렬해줘야한다.
					t_ProximityNode->Astar_CompareParentCost( m_NowNode, i );
				}

				// 3. (처리된)인접 노드가 목표 노드라면 구하기 성공한 것
				if (t_ProximityNode == m_DestNode) 
				{
					// 마지막 결과 노드를 닫힌 노드에 넣어준다.
					m_vec_ClosedList.push_back( m_DestNode );
					return TRUE;
				}
			}
		}

		// 4. 열린 노드가 비어있게 되는 경우 실패한 것임 - 아마도 모두 닫힌 노드로 되고, 이 경우 길이 모두 막힌거다
		if(m_map_OpenList.empty())
		{
			HTRACE("CTileManager - 길이 없습니다! (모든 노드가 닫힌 노드로 됐음)");
			return FALSE;
		}


	}	// while

	///--------------------------------------------------
	///	Description
	///--------------------------------------------------

	/// 논리구조...

	/// 처음 열린 목록은 시작점 A

	/// 시작 지점을 잡고,
	///		A를 열린 목록에서 빼고, 닫힌 목록에 추가

	/// 1. A의 인접노드의 F = G + H를 구한다
	//		G는 있고, H는 구한다... 
	/// 2. 인접노드의 부모지정을 A로 한다

	/// 3. 인접노드의 각 F가 나왔다면 제일 작은 F를 가진곳으로 이동
	//		이것을 열린목록에서 빼서 닫힌 목록으로 이동
	/// 새 이동지에서 인접한 모든 노드를 검색, 닫힌목록에 있지 않는것중에서
	// 열린목록에 들어가 있지 않은 것들을 열린목록에 추가
	// 선택되었던 사각형을 새로운 사각형들의 부모로 만든다
	/// 만약 인접한 사각형중에 이미 열린목록에 있는 사각형이 있다면
	// 현재 선택된 사각형보다 G비용이 더 작은지를 검사해 보고, 
	// 새로운 길이 G비용이 더 작다면 선택되었던 사각형의 인접사각형들의 부모를 새로운 사각형으로 바꿔라


	// 특정 루프를 돌아도 안끝난 경우
	HTRACE("CTileManager - 루프를 다 돌았지만, 길을 찾지 못했습니다. 길찾기 루프 수에 비해서 타일이 큽니다!");

	return FALSE;
}

///
/// 길찾기 완료되었고, Path를 역추적, 만들어서 리턴
BOOL CTileManager::MakeAStarPath()
{
	// Closed List를 거꾸로 타고 올라가서, 결과적으로 Path를 만들어준다
	IPathNode* t_NowNode = m_DestNode;
	m_AStarPath.push_front( t_NowNode );

	// 역추적...
	while(TRUE)
	{
		if ( t_NowNode->GetAstarParentNode() )
		{
			// 부모 노드에 다음 노드 속성을 연결해준다
			t_NowNode->GetAstarParentNode()->SetAstarNextNode( t_NowNode );

			// 부모 노드로 이동
			t_NowNode = t_NowNode->GetAstarParentNode();
			m_AStarPath.push_front( t_NowNode );

			// 출발점을 찾았다
			if ( t_NowNode == m_StartNode ) break;
		}
		else
		{
			ASSERT( 0 && "HPathFinder::MakeAPath() - 부모 노드가 없습니다!");
			break;
		}
	}


	return TRUE;
}

///
/// A-Star를 통해서 나온 위치값들을 가시성 판단, 직선화 실시
/// 몇 번 돌리면 아주 심플하게 된다.
BOOL CTileManager::Optimize()
{
	std::deque<IPathNode*> t_deq_Original;
	std::deque<IPathNode*> t_deq_Optimize;
	std::deque<IPathNode*> t_deq_Optimize2;

	HTR_S(eRT_Optimize1);
	Optimize(m_AStarPath, t_deq_Optimize);
	HTR_E(eRT_Optimize1);

	HTR_S(eRT_Optimize2);
	Optimize(t_deq_Optimize, t_deq_Optimize2);
	HTR_E(eRT_Optimize2);

	// 최종적으로 최적화된 리스트 (값전달)
	m_AStarPath_Optimize = t_deq_Optimize2;


	return TRUE;
}

// A-Star를 통해서 나온 위치값들을 가시성 판단, 직선화 하는 세부 함수
BOOL CTileManager::Optimize(std::deque<IPathNode*>& p_deq_AStarPath, std::deque<IPathNode*>& p_deq_AStarPath_Optimize)
{
	/// 도착 노드에서 시작, 시작 노드까지의 가시성 판단을 한다

	// 중간 변수들
	DWORD t_TempStartX, t_TempStartY;				// 시작 좌표
	DWORD t_NowX, t_NowY;							// 현재 좌표
	IPathNode* t_StartNode =p_deq_AStarPath.front();// 시작 노드
	IPathNode* t_DestNode = p_deq_AStarPath.back();	// 마지막 노드
	IPathNode* t_LastNode = t_StartNode;			// 마지막 직선화 성공 노드
	IPathNode* t_NowNode = NULL;					// 현재 탐색 노드


	// 맨 처음은 시작 노드다. 한칸씩 전진하면서 가시성을 확보한다
	t_TempStartX = t_StartNode->GetPosX();
	t_TempStartY = t_StartNode->GetPosY();
	p_deq_AStarPath_Optimize.push_back(t_StartNode);


	// 시작노드 다음부터 갈 수 있는 가장 먼 노드를 찾는다.
	// 찾았다면 그곳을 넣고, 시작노드로 정함
	// 못찾았다면 현재 노드를 넣고, 다음 노드를 시작으로 정함
	for (UINT i=1; i<p_deq_AStarPath.size(); i++)
	{
		t_NowNode = p_deq_AStarPath[i];
		t_NowX = t_NowNode->GetPosX();
		t_NowY = t_NowNode->GetPosY();

		// 가시성 판단을 한다
		if ( CollisionCheck_OneLine_New(t_TempStartX, t_TempStartY, t_NowX, t_NowY) == TRUE )
		{
			/// 직선상으로 움직일 수 있다는 말이므로 계속 노드 전진
			// 현재 노드를 이전 노드에 기록
			t_LastNode = t_NowNode;
		}
		else
		{
			/// 직선상으로 움직이지 못하는 녀석을 만나면, 기록되었던 LastNode를 적는다
			// ※그런데 시작 위치와 이전 노드의 위치가 같다면 무한 루프에 빠지게 되는 상황,
			// 현재 노드를 기록하고, 현재 노드를 LastNode로 놓는다, 시작 위치도 현재 노드.
			if ( t_LastNode->GetPosX() == t_TempStartX && t_LastNode->GetPosY() == t_TempStartY )
			{
				HTRACE("Optimize() - 발생하면 안됨! 직선상으로 움직이지 못하는데 시작위치와 이전 노드의 위치가 같다\n");
				p_deq_AStarPath_Optimize.push_back(t_NowNode);
				t_TempStartX = t_NowX;
				t_TempStartY = t_NowY;
				t_LastNode = t_NowNode;
			}
			else
			{
				// 직선상으로 움직이지 못하는 녀석을 만나면, 기록되었던 LastNode를 적는다
				p_deq_AStarPath_Optimize.push_back(t_LastNode);

				// 그리고 이제 LastNode부터 다시 찾는다
				t_TempStartX = t_LastNode->GetPosX();
				t_TempStartY = t_LastNode->GetPosY();

				// 찾던 위치를 한칸 뒤로 보내야 한다
				i--;
				/// LastNode는 그냥 유지한 채 가야 한다
			}

		}	// 가시성 판단을 한다

	}

	// (위의 로직에서 들어가지 않으므로 목적지 노드를 넣어준다)
	p_deq_AStarPath_Optimize.push_back(t_DestNode);


	return TRUE;
}

IPathNode* CTileManager::GetMovableNode( DWORD p_PosX, DWORD p_PosY )
{
	// 범위 안에 있다면
	if ( (p_PosX >= 0) && (p_PosX < m_dwTileWidth) &&
		(p_PosY >= 0) && (p_PosY < m_dwTileHeight)	)
	{
		CTile* l_Node  = m_pTile + (p_PosY * m_dwTileWidth) + p_PosX;
		if (l_Node)
		{
			if(FALSE == l_Node->IsCollisonTile())
			{
				return l_Node;
			}
		}
	}

	return NULL;
}

IPathNode* CTileManager::GetMovableNode( float p_PosX, float p_PosY )
{
	if (m_NodeRatio == 0)
	{
		return NULL;
	}

	return GetMovableNode((DWORD)(p_PosX/m_NodeRatio), (DWORD)(p_PosY/m_NodeRatio) );
}
CTile* CTileManager::GetNode( DWORD p_PosX, DWORD p_PosY )
{
	// 범위 안에 있다면
	if ( (p_PosX >= 0) && (p_PosX < m_dwTileWidth) && 
		 (p_PosY >= 0) && (p_PosY < m_dwTileHeight)	)
	{
		CTile* l_Node = m_pTile + (p_PosY * m_dwTileWidth) + p_PosX;
		if (l_Node)
		{
			return l_Node;
		}
	}

	return NULL;
}

CTile* CTileManager::GetNode( float p_PosX, float p_PosY )
{
	return GetNode((DWORD)(p_PosX/m_NodeRatio), (DWORD)(p_PosY/m_NodeRatio) );
}

CTile* CTileManager::GetNode1x( DWORD p_PosX, DWORD p_PosY )
{
	// 범위 안에 있다면
	if ( (p_PosX >= 0) && (p_PosX < m_dwWidth1x) && 
		(p_PosY >= 0) && (p_PosY < m_dwHeight1x)	)
	{
		CTile* l_Node = m_pAllNode1x + (p_PosY * m_dwWidth1x) + p_PosX;
		if (l_Node)
		{
			return l_Node;
		}
	}

	return NULL;
}

CTile* CTileManager::GetNode1x( float p_PosX, float p_PosY )
{
	return GetNode1x((DWORD)(p_PosX/eDefaultTileRatio), (DWORD)(p_PosY/eDefaultTileRatio) );
}

/// 완료된 데이터들을 바깥쪽으로 뺌
std::vector<VECTOR3> CTileManager::GetPathResult( float p_Ratio )
{
	m_PathResult.clear();
	for (UINT i=0; i<m_AStarPath.size(); i++)
	{
		VECTOR3 t_Vector;
		t_Vector.x = (float)m_AStarPath[i]->GetPosX() * p_Ratio;
		t_Vector.y = 0;
		t_Vector.z = (float)m_AStarPath[i]->GetPosY() * p_Ratio;
		m_PathResult.push_back(t_Vector);
	}

	return m_PathResult;
}

std::deque<VECTOR3> CTileManager::GetPathResult_Optimize( float p_Ratio )
{
	// 실제 타일용 데이터로 바꿔준다. 첫번째 데이터는 현 위치므로 웨이포인트에서 뺀다
	m_PathResult_Optimize.clear();
	for (UINT i=1; i<m_AStarPath_Optimize.size(); i++)
	{
		VECTOR3 t_Vector;
		t_Vector.x = (float)m_AStarPath_Optimize[i]->GetPosX() * p_Ratio + p_Ratio * 0.5f;
		t_Vector.y = 0;
		t_Vector.z = (float)m_AStarPath_Optimize[i]->GetPosY() * p_Ratio + p_Ratio * 0.5f;
		m_PathResult_Optimize.push_back(t_Vector);
	}

	return m_PathResult_Optimize;
}

std::deque<VECTOR3> CTileManager::GetPathResult_Optimize_Debug( float p_Ratio )
{
	// 디버그용 데이터는 첫번째 위치까지 포함해서 보낸다
	m_PathResult_Optimize_Debug.clear();
	for (UINT i=0; i<m_AStarPath_Optimize.size(); i++)
	{
		VECTOR3 t_Vector;
		t_Vector.x = (float)m_AStarPath_Optimize[i]->GetPosX() * p_Ratio + p_Ratio * 0.5f;
		t_Vector.y = 0;
		t_Vector.z = (float)m_AStarPath_Optimize[i]->GetPosY() * p_Ratio + p_Ratio * 0.5f;
		m_PathResult_Optimize_Debug.push_back(t_Vector);
	}

	return m_PathResult_Optimize_Debug;
}

BOOL CTileManager::FindClosestTile( const VECTOR3& p_Position, IPathNode*& p_pClosestNode)
{
	ePathFindLayer l_LayerInfo = GetPathFindMode();

	SetPathFindMode(ePathFindLayer_3x);

	CTile* l_Node = GetNode(p_Position.x, p_Position.z);
	if (l_Node)
	{
		if (l_Node->IsCollisonTile() == FALSE)
		{
			// 움직일 수 있는 4x타일이라면 그것을 리턴
			p_pClosestNode = l_Node;
			SetPathFindMode(l_LayerInfo);
			return TRUE;
		}
		else
		{
			// 아니라면 인접한 타일 중 움직임 가능한 것을 찾음 (아무거나 찾아도 된다)
			DWORD l_NowPosX = l_Node->GetPosX();
			DWORD l_NowPosY = l_Node->GetPosY();

			DWORD l_StartPosX = max(0, l_NowPosX-1);
			DWORD l_StartPosY = max(0, l_NowPosY-1);
			DWORD l_EndPosX = min(m_dwTileWidth, l_NowPosX+1);
			DWORD l_EndPosY = min(m_dwTileHeight, l_NowPosY+1);
			for (DWORD l_TempPosY = l_StartPosY; l_TempPosY <= l_EndPosY; l_TempPosY++)
			{
				for (DWORD l_TempPosX = l_StartPosX; l_TempPosX <= l_EndPosX; l_TempPosX++)
				{
					CTile* l_TempNode = GetNode(l_TempPosX, l_TempPosY);
					if (l_TempNode && (l_TempNode->IsCollisonTile() == FALSE))
					{
						p_pClosestNode = l_TempNode;
						SetPathFindMode(l_LayerInfo);
						return TRUE;
					}
				}
			}
			// 돌아도 인접 타일 중 움직일 수 있는 것이 없을 경우
			p_pClosestNode = NULL;
		}
	}
	else
	{
		// 현 포지션에 타일이 없을 경우
		p_pClosestNode = NULL;
	}


	SetPathFindMode(l_LayerInfo);	
	return FALSE;
}

// 개량된 (정확한) 가시성 판단 함수
/// 대각선인경우, 양쪽을 모두 계산하고 둘 다 되면 대각 진행, 안되면 포기
BOOL CTileManager::CollisionCheck_OneLine_New( DWORD p_StartX, DWORD p_StartY, DWORD p_EndX, DWORD p_EndY )
{
	/// 입력체크

	// 직선을 하나 만들고
	VECTOR2 l_LineStart, l_LineEnd;
	l_LineStart.x = (float)p_StartX + 0.5f;
	l_LineStart.y = (float)p_StartY + 0.5f;
	l_LineEnd.x = (float)p_EndX + 0.5f;
	l_LineEnd.y = (float)p_EndY + 0.5f;
	H2DLine l_Line = H2DLine(l_LineStart, l_LineEnd);


	// 현재 타일
	DWORD l_NowPosX = p_StartX;
	DWORD l_NowPosY = p_StartY;

	// 현재 타일의 4개 선분
	VECTOR2 l_TopSegmentS;
	VECTOR2 l_TopSegmentE;
	VECTOR2 l_LeftSegmentS;
	VECTOR2 l_LeftSegmentE;
	VECTOR2 l_RightSegmentS;
	VECTOR2 l_RightSegmentE;
	VECTOR2 l_BottomSegmentS;
	VECTOR2 l_BottomSegmentE;

	// 직선의 기울기를 판단, 어느 쪽 2개 선분을 얻을 것인가를 선택함
	float l_DeltaX = l_LineEnd.x - l_LineStart.x;
	float l_DeltaY = l_LineEnd.y - l_LineStart.y;

	// 대각선의 경우를 체크하기 위해서
	BOOL l_SideA = FALSE; BOOL l_SideB = FALSE;

	/// 카운트 제한을 두자
	int l_Count = 0;
	CTile* l_NowNode = NULL;
	CTile* l_TempNodeA = NULL;
	CTile* l_TempNodeB = NULL;

	while( l_Count < 50000 )
	{
		l_Count++;
		
		l_SideA = FALSE;
		l_SideB = FALSE;

		/// 타일 영역도 조사(잘못된 포인터를 참조하지 않도록)

		// 현 타일 조사(갈 수 있는 곳인가)
		l_NowNode = GetNode(l_NowPosX, l_NowPosY);
		if (l_NowNode)
		{
			if (l_NowNode->IsCollisonTile())
			{
				// 막혀있는 타일이라면 실패
				return FALSE;
			}
		}
		else
		{
			OutputDebugString(
				"노드가 없습니다! 잘못된 위치를 참조한 것 같습니다\n");
			return FALSE;
		}

		// 막힌 노드가 아니라면 조건별 선분을 가져와서 교차체크 후 이동해야한다
		l_TopSegmentS.x		=	(float)l_NowPosX;
		l_TopSegmentS.y		=	(float)l_NowPosY;
		l_TopSegmentE.x		=	(float)l_NowPosX + 1;
		l_TopSegmentE.y		=	(float)l_NowPosY;
		l_LeftSegmentS.x	=	(float)l_NowPosX;	
		l_LeftSegmentS.y	=	(float)l_NowPosY;
		l_LeftSegmentE.x	=	(float)l_NowPosX;
		l_LeftSegmentE.y	=	(float)l_NowPosY + 1;
		l_RightSegmentS.x	=	(float)l_NowPosX + 1;
		l_RightSegmentS.y	=	(float)l_NowPosY;
		l_RightSegmentE.x	=	(float)l_NowPosX + 1;
		l_RightSegmentE.y	=	(float)l_NowPosY + 1;
		l_BottomSegmentS.x	=	(float)l_NowPosX;
		l_BottomSegmentS.y	=	(float)l_NowPosY + 1;
		l_BottomSegmentE.x	=	(float)l_NowPosX + 1;
		l_BottomSegmentE.y	=	(float)l_NowPosY + 1;

		// 현 타일이 목적 타일과 같다면 성공
		if (l_NowPosX == p_EndX && l_NowPosY == p_EndY)
		{
			return TRUE;
		}

		// 반직선이 오른쪽으로 향한다면
		if (l_DeltaX > 0 && l_DeltaY == 0)
		{
			// 오른쪽 선분 체크, 교차한다면 오른쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_RightSegmentS, l_RightSegmentE, l_Line) == TRUE)
			{
				l_NowPosX++;
				continue;
			}
		}
		// 반직선이 왼쪽으로 향한다면
		else if (l_DeltaX < 0 && l_DeltaY == 0)
		{
			// 왼쪽 선분 체크, 교차한다면 왼쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_LeftSegmentS, l_LeftSegmentE, l_Line) == TRUE)
			{
				l_NowPosX--;
				continue;
			}
		}
		// 반직선이 위쪽으로 향한다면
		else if (l_DeltaX == 0 && l_DeltaY < 0)
		{
			// 위쪽 선분 체크, 교차한다면 위쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_TopSegmentS, l_TopSegmentE, l_Line) == TRUE)
			{
				l_NowPosY--;
				continue;
			}
		}
		// 반직선이 아래쪽으로 향한다면
		else if (l_DeltaX == 0 && l_DeltaY > 0)
		{
			// 아래쪽 선분 체크, 교차한다면 아래쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_BottomSegmentS, l_BottomSegmentE, l_Line) == TRUE)
			{
				l_NowPosY++;
				continue;
			}
		}
		// 반직선이 왼쪽 위로 향한다면
		else if (l_DeltaX < 0 && l_DeltaY < 0)
		{
			// 왼쪽 선분 체크, 교차한다면 왼쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_LeftSegmentS, l_LeftSegmentE, l_Line) == TRUE)
			{
				l_SideA = TRUE;
			}
			// 위쪽 선분 체크, 교차한다면 위쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_TopSegmentS, l_TopSegmentE, l_Line) == TRUE)
			{
				l_SideB = TRUE;
			}

			// 어느 한 쪽만 교차하는 경우는 그쪽으로 이동
			if (l_SideA == TRUE && l_SideB == FALSE)
			{
				l_NowPosX--;
				continue;
			}
			else if (l_SideA == FALSE && l_SideB == TRUE)
			{
				l_NowPosY--;
				continue;
			}
			// 양쪽으로 교차하는 경우는(대각) 둘 다 이동가능하면 이동, 아니면 포기
			else if (l_SideA == TRUE && l_SideB == TRUE)
			{
				l_TempNodeA = GetNode(l_NowPosX-1, l_NowPosY);
				l_TempNodeB = GetNode(l_NowPosX, l_NowPosY-1);
				if (l_TempNodeA && l_TempNodeB &&
					!l_TempNodeA->IsCollisonTile() && !l_TempNodeB->IsCollisonTile() )
				{
					l_NowPosX--;
					l_NowPosY--;
					continue;
				}
				else
				{
					return FALSE;
				}
			}

		}
		// 반직선이 오른쪽 위로 향한다면
		else if (l_DeltaX > 0 && l_DeltaY < 0)
		{
			// 오른쪽 선분 체크, 교차한다면 오른쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_RightSegmentS, l_RightSegmentE, l_Line) == TRUE)
			{
				l_SideA = TRUE;
			}
			// 위쪽 선분 체크, 교차한다면 위쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_TopSegmentS, l_TopSegmentE, l_Line) == TRUE)
			{
				l_SideB = TRUE;
			}

			// 어느 한 쪽만 교차하는 경우는 그쪽으로 이동
			if (l_SideA == TRUE && l_SideB == FALSE)
			{
				l_NowPosX++;
				continue;
			}
			else if (l_SideA == FALSE && l_SideB == TRUE)
			{
				l_NowPosY--;
				continue;
			}
			// 양쪽으로 교차하는 경우는(대각) 둘 다 이동가능하면 이동, 아니면 포기
			else if (l_SideA == TRUE && l_SideB == TRUE)
			{
				l_TempNodeA = GetNode(l_NowPosX+1, l_NowPosY);
				l_TempNodeB = GetNode(l_NowPosX, l_NowPosY-1);
				if (l_TempNodeA && l_TempNodeB &&
					!l_TempNodeA->IsCollisonTile() && !l_TempNodeB->IsCollisonTile() )
				{
					l_NowPosX++;
					l_NowPosY--;
					continue;
				}
				else
				{
					return FALSE;
				}
			}

		}
		// 반직선이 왼쪽 아래로 향한다면
		else if (l_DeltaX < 0 && l_DeltaY > 0)
		{
			// 왼쪽 선분 체크, 교차한다면 왼쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_LeftSegmentS, l_LeftSegmentE, l_Line) == TRUE)
			{
				l_SideA = TRUE;
			}
			// 아래쪽 선분 체크, 교차한다면 아래쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_BottomSegmentS, l_BottomSegmentE, l_Line) == TRUE)
			{
				l_SideB = TRUE;
			}

			// 어느 한 쪽만 교차하는 경우는 그쪽으로 이동
			if (l_SideA == TRUE && l_SideB == FALSE)
			{
				l_NowPosX--;
				continue;
			}
			else if (l_SideA == FALSE && l_SideB == TRUE)
			{
				l_NowPosY++;
				continue;
			}
			// 양쪽으로 교차하는 경우는(대각) 둘 다 이동가능하면 이동, 아니면 포기
			else if (l_SideA == TRUE && l_SideB == TRUE)
			{
				l_TempNodeA = GetNode(l_NowPosX-1, l_NowPosY);
				l_TempNodeB = GetNode(l_NowPosX, l_NowPosY+1);
				if (l_TempNodeA && l_TempNodeB &&
					!l_TempNodeA->IsCollisonTile() && !l_TempNodeB->IsCollisonTile() )
				{
					l_NowPosX--;
					l_NowPosY++;
					continue;
				}
				else
				{
					return FALSE;
				}
			}

		}
		// 반직선이 오른쪽 아래로 향한다면
		else if (l_DeltaX > 0 && l_DeltaY > 0)
		{
			// 오른쪽 선분 체크, 교차한다면 오른쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_RightSegmentS, l_RightSegmentE, l_Line) == TRUE)
			{
				l_SideA = TRUE;
			}
			// 아래쪽 선분 체크, 교차한다면 아래쪽으로 진행
			if (CollisionCheck_2D_Segment_Line(l_BottomSegmentS, l_BottomSegmentE, l_Line) == TRUE)
			{
				l_SideB = TRUE;
			}

			// 어느 한 쪽만 교차하는 경우는 그쪽으로 이동
			if (l_SideA == TRUE && l_SideB == FALSE)
			{
				l_NowPosX++;
				continue;
			}
			else if (l_SideA == FALSE && l_SideB == TRUE)
			{
				l_NowPosY++;
				continue;
			}
			// 양쪽으로 교차하는 경우는(대각) 둘 다 이동가능하면 이동, 아니면 포기
			else if (l_SideA == TRUE && l_SideB == TRUE)
			{
				l_TempNodeA = GetNode(l_NowPosX+1, l_NowPosY);
				l_TempNodeB = GetNode(l_NowPosX, l_NowPosY+1);
				if (l_TempNodeA && l_TempNodeB &&
					!l_TempNodeA->IsCollisonTile() && !l_TempNodeB->IsCollisonTile() )
				{
					l_NowPosX++;
					l_NowPosY++;
					continue;
				}
				else
				{
					return FALSE;
				}
			}


		}

		HTRACE_FUNC();
		OutputDebugString(
			"여기까지 왔다면 아무것도 아니라는건데, 그런 상황은 원하지 않는 결과다\n");

		return FALSE;
	}

	HTRACE_FUNC();
	OutputDebugString(
		"루프를 다 돌았지만 직선체크가 끝나지 않았습니다!\n");

	return FALSE;

}

BOOL CTileManager::CollisionCheck_OneLine_New( VECTOR3* p_pStart,VECTOR3* p_pEnd )
{
	// 현 위치 검사
	if( p_pStart->x >= 51200 || p_pStart->x < 0 ||
		p_pStart->z >= 51200 || p_pStart->z < 0 )
	{
		OutputDebugString(
			"현재 위치하고 있는 지점이 타일의 범위를 벗어났습니다.");
		return FALSE;
	}

	// 타일상의 포지션으로 호출
	return CollisionCheck_OneLine_New( (DWORD)(p_pStart->x / eDefaultTileRatio),
		(DWORD)(p_pStart->z / eDefaultTileRatio),
		(DWORD)(p_pEnd->x / eDefaultTileRatio),
		(DWORD)(p_pEnd->z / eDefaultTileRatio) );
}

BOOL CTileManager::FindClosest1xTile(VECTOR3& p_SafePosition, DWORD p_NowPosX, DWORD p_NowPosZ )
{
	// 영역 제한
	DWORD l_NowPosXmin = max(0, p_NowPosX - 3);
	DWORD l_NowPosXmax = min(m_dwWidth1x, p_NowPosX + 3);
	
	DWORD l_NowPosZmin = max(0, p_NowPosZ - 3);
	DWORD l_NowPosZmax = min(m_dwHeight1x, p_NowPosZ + 3);

	for (DWORD j = l_NowPosZmin; j<l_NowPosZmax; j++)
	{
		for (DWORD i = l_NowPosXmin; i<l_NowPosXmax; i++)
		{
			if ( GetNode1x(i,j)->IsCollisonTile() == FALSE )
			{
				VECTOR3 l_SafePos;
				l_SafePos.x = (float)(i * eDefaultTileRatio);
				l_SafePos.y = 0;
				l_SafePos.z = (float)(j * eDefaultTileRatio);

				p_SafePosition = l_SafePos;

				return TRUE;
			}
		}
	}

	return FALSE;
}

// 091208 ShinJS --- 3배속 타일정보 갱신
void CTileManager::UpdateNodeInfo( VECTOR3* pPos )
{
	if( !pPos )
		return;

	// 3배 타일정보 갱신
	DWORD t_X, t_Y;
	t_X = (DWORD)(pPos->x / 150);
	t_Y = (DWORD)(pPos->z / 150);

	// 9개의 하부 타일이 모두 갈 수 있는 곳일 경우만 Ok
	CTile* t_Tile_1 = GetNode(t_X*3, t_Y*3);
	CTile* t_Tile_2 = GetNode(t_X*3+1, t_Y*3);
	CTile* t_Tile_3 = GetNode(t_X*3+2, t_Y*3);
	CTile* t_Tile_4 = GetNode(t_X*3, t_Y*3+1);
	CTile* t_Tile_5 = GetNode(t_X*3+1, t_Y*3+1);
	CTile* t_Tile_6 = GetNode(t_X*3+2, t_Y*3+1);
	CTile* t_Tile_7 = GetNode(t_X*3, t_Y*3+2);
	CTile* t_Tile_8 = GetNode(t_X*3+1, t_Y*3+2);
	CTile* t_Tile_9 = GetNode(t_X*3+2, t_Y*3+2);
	if ( t_Tile_1 && t_Tile_2 && t_Tile_3 && t_Tile_4 &&
		t_Tile_5 && t_Tile_6 && t_Tile_7 && t_Tile_8 && t_Tile_9 )
	{
		if ( !t_Tile_1->IsCollisonTile() &&
			!t_Tile_2->IsCollisonTile() &&
			!t_Tile_3->IsCollisonTile() &&
			!t_Tile_4->IsCollisonTile() &&
			!t_Tile_5->IsCollisonTile() &&
			!t_Tile_6->IsCollisonTile() &&
			!t_Tile_7->IsCollisonTile() &&
			!t_Tile_8->IsCollisonTile() &&
			!t_Tile_9->IsCollisonTile() )
		{
			TILE_ATTR l_Attrib;
			l_Attrib.uAttr = 0;
			l_Attrib.uSkillObjectBlock = 0;
			m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetTileAttrib(l_Attrib);
			m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosX(t_X);
			m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosY(t_Y);

		}
		else
		{
			TILE_ATTR l_Attrib;
			l_Attrib.uAttr = 1;
			l_Attrib.uSkillObjectBlock = 0;
			m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetTileAttrib(l_Attrib);
			m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosX(t_X);
			m_pAllNode3x[ m_dwWidth3x*t_Y + t_X ].SetPosY(t_Y);
		}
	}	
}