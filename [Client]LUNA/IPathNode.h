//--------------------------------------------------
//	IPathNode
//	[2009/6/22 LeHide]
//
//	길찾기 노드 추상화
//
//	Version		:	1.0.2009622
//--------------------------------------------------

#pragma once

enum eAStarStatus
{
	eUnidentified, eOpened, eClosed,
};


// 데이터도 추상화해서 다룬다
class IPathNode
{
public:
	IPathNode()
		: m_GCost(0)
		, m_HCost(0)
		, m_FCost(0)
		, m_AstarParentNode(NULL)
		, m_AstarNextNode(NULL)
		, m_AStarStatus(eUnidentified)
	{
	}

	void Reset()
	{
		m_GCost = 0;
		m_HCost = 0;
		m_FCost = 0;
		m_AstarParentNode = NULL;
		m_AstarNextNode = NULL;
		m_AStarStatus = eUnidentified;
	}

	// Cost 관련
	float	GetGCost() const	{ return m_GCost; }
	void	SetGCost(float val) { m_GCost = val; }
	float	GetHCost() const	{ return m_HCost; }
	void	SetHCost(float val) { m_HCost = val; }
	float	GetFCost() const	{ return m_FCost; }
	void	SetFCost(float val) { m_FCost = val; }

	virtual void Calc_Cost(IPathNode* p_DestinationNode) PURE;


	// AStar Status
	virtual void Astar_MakeOpened( IPathNode* p_NowSelectedNode, IPathNode* p_DestinationNode ) PURE;	// 오픈 노드로 셋팅해준다
	virtual BOOL Astar_CompareParentCost( IPathNode* p_NowSelectedNode, int p_Index ) PURE;				// G비용을 봐서 부모를 갈아치운다

	eAStarStatus GetAStarStatus() const { return m_AStarStatus; }
	void SetAStarStatus(eAStarStatus val) { m_AStarStatus = val; }


public:
	virtual DWORD	GetPosX() const PURE;
	virtual void	SetPosX(DWORD val) PURE;
	virtual DWORD	GetPosY() const PURE;
	virtual void	SetPosY(DWORD val) PURE;

	IPathNode* GetAstarParentNode() const { return m_AstarParentNode; }
	void SetAstarNextNode(IPathNode* val) { m_AstarNextNode = val; }

protected:
	float			m_GCost;			// 노드간
	float			m_HCost;			// 휴리스틱
	float			m_FCost;			// 합

	IPathNode*		m_AstarParentNode;	// 부모 노드
	IPathNode*		m_AstarNextNode;	// 다음 노드

	eAStarStatus	m_AStarStatus;		// 노드의 상태. 재계산시 리셋된다

};

