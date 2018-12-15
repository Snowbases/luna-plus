// Tile.cpp: implementation of the CTile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTile::CTile()
: m_PosX(0)
, m_PosY(0)
{
}

CTile::~CTile()
{
}

void CTile::AddTileAttrib(AREATILE areatile)
{
	if(areatile == SKILLAREA_ATTR_BLOCK)
	{
		++m_Attr.uSkillObjectBlock;
	}
}

void CTile::RemoveTileAttrib(AREATILE areatile)
{
	if(areatile == SKILLAREA_ATTR_BLOCK)
	{
		ASSERT(m_Attr.uSkillObjectBlock > 0);
		if(m_Attr.uSkillObjectBlock != 0)
			--m_Attr.uSkillObjectBlock;
	}
}


void CTile::Calc_Cost(IPathNode* p_DestinationNode)
{
	CTile* t_DestinationNode = static_cast<CTile*>(p_DestinationNode);

	/// G Cost
	// 부모가 없다면 내가 시작 지점이므로, GCost는 0이다
	if (m_AstarParentNode == NULL)
	{
		m_GCost = 0.f;
	}
	else
	{
		// 아닌경우 부모와의 거리를 계산한다
		// G Cost는 부모 Cost와의 누적이다!
		int t_DistInt = abs( (int)m_PosX - (int)m_AstarParentNode->GetPosX()) + abs( (int)m_PosY - (int)m_AstarParentNode->GetPosY());
		float t_Dist = (t_DistInt == 1) ? 1.0f : 1.4142f;
		m_GCost =  m_AstarParentNode->GetGCost() + t_Dist;
	}

	/// H Cost
	// 목표점까지의 기대값 (보통 거리)
	//m_HCost = 0;
	/// 현 노드의 위치와 목적 노드의 위치를 기반으로 거리를 계산한다
	m_HCost = sqrt( pow( (float)((int)m_PosX - (int)t_DestinationNode->GetPosX()), 2) + 
		pow( (float)((int)m_PosY - (int)t_DestinationNode->GetPosY()), 2) );

	/// F Cost
	m_FCost = m_GCost + m_HCost;
}

// 현 인접 노드가 양 목록상에 등록돼있지 않다면 이것저것 셋팅을 해 준다
void CTile::Astar_MakeOpened( IPathNode* p_NowSelectedNode, IPathNode* p_DestinationNode )
{
	// 추가되는 노드들은 현재 선택 노드를 부모로 해 준다 (이걸 먼저해야 cost를 잴 수 있다)
	// 추가되는 노드의 F = G+H를 계산/저장해준다
	// 오픈 노드로 만들어준다
	m_AstarParentNode = p_NowSelectedNode;
	Calc_Cost(p_DestinationNode);
	m_AStarStatus = eOpened;	// 함수 나간다음 실제로 열린 노드에 추가도 될 것이다
}

BOOL CTile::Astar_CompareParentCost( IPathNode* p_NowSelectedNode, int p_Index )
{
	// 상하좌우로만 움직이므로 무조건 노드간의 거리는 1
	// 대각선이 추가되면 그 노드간 거리는 루트 2
	float t_Dist = 0;
	if (p_Index < 4) t_Dist = 1;
	if (p_Index >= 4) t_Dist = 1.4142f;
	if ( p_NowSelectedNode->GetGCost() + t_Dist < m_GCost )
	{
		m_AstarParentNode = p_NowSelectedNode;
		return TRUE;
	}

	return FALSE;
}