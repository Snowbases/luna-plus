// TileManager.h: interface for the CTileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_)
#define AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tile.h"
#include "IPathFinder.h"
#define fTILE_WIDTH 50
#define fTILE_HEIGHT 50
#define TILECOLLISON_DETAIL		50.f

// 090907 ShinJS --- 5팀 길찾기 Source 적용, PathManager 같은 이름 사용으로 PathFinder로 변경. TileManager를 PathManager로 변경 하였으나, 변경하지 않고 추가적인 부분 적용.

enum ePathFindLayer
{
	ePathFindLayer_1x,
	ePathFindLayer_3x,

	eDefaultTileRatio = 50,
};

/// 길찾기용 수학함수.
enum eH2DLineSide
{
	eH2DLineSide_Up_Left,		// 위쪽이나 왼쪽
	eH2DLineSide_Down_Right,	// 아래쪽이나 오른쪽
	eH2DLineSide_Equal,			// 선 위에 있을 때
};

// 2D상의 y = ax + b의 직선
class H2DLine
{
public:
	// 두 개의 점으로부터 직선을 만든다
	H2DLine( VECTOR2 p_P1, VECTOR2 p_P2 );

	// 한 점이 이 직선의 어느쪽 방향에 있나를 리턴해준다
	// 직선보다 위쪽이면 FALSE 같거나 아래쪽이면 TRUE
	// 단지 기울기가 없을 경우( x = 3 식의 경우) 왼쪽이면 FALSE 같거나 오른쪽이면 TRUE
	eH2DLineSide CheckSide( VECTOR2 p_Point );

	float GetA() const { return m_a; }
	BOOL GetFlag_x_equal_b() const { return m_Flag_x_equal_b; }

private:
	// 절대로 막지은 이름이 아니다 수학에서 이렇게 쓰니까..
	float	m_a;				// 기울기
	float	m_b;				// y절편
	BOOL	m_Flag_x_equal_b;	// x = b형태일때 플래그

};


// 2d상태에서 선분과 직선의 충돌검출
BOOL CollisionCheck_2D_Segment_Line(VECTOR2 p_SegA, VECTOR2 p_SegB, VECTOR2 p_LineA, VECTOR2 p_LineB);
BOOL CollisionCheck_2D_Segment_Line(VECTOR2 p_SegA, VECTOR2 p_SegB, H2DLine& p_Line);

class CTileManager : public IPathFinder
{
protected:
	DWORD m_dwTileWidth;
	DWORD m_dwTileHeight;
	
	CTile* m_pTile;
	
	CTile* GetTile(DWORD dwX, DWORD dwZ);
	CTile* GetTile(float fx, float fz);

public:
	CTileManager();
	~CTileManager();

	DWORD GetTileWidth() { return m_dwTileWidth; }
	BOOL IsInTile(int x, int y, MAPTYPE MapNum,CObject* pObject);
	BOOL CollisionTile(int x, int y, MAPTYPE MapNum,CObject* pObject);
	BOOL CollisionLine(VECTOR3* pStart,VECTOR3* pEnd, VECTOR3 * Target, MAPTYPE MapNum,CObject* pObject);
	BOOL NonCollisionLine(VECTOR3* pStart, VECTOR3* pEnd, VECTOR3* pRtNonCollisonPos, MAPTYPE MapNum, CObject* pObj);	// 충돌아니면 TRUE반환하고 pRtNonCollisonPos에 비충돌 타일 세팅

	BOOL LoadTileInfo(char* TileFile);
	BOOL CollisonCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos,CObject* pObj);		// 충돌하면 TRUE반환하고 pRtCollisonPos에 새로운 타겟 설정

	void AddTileAttrByAreaData(CObject* Object,VECTOR3* pPos,AREATILE areatile);
	void RemoveTileAttrByAreaData(CObject* Object,VECTOR3* pPos,AREATILE areatile);

	void			SetPathFindMode( ePathFindLayer p_PathFindMode );
	ePathFindLayer	GetPathFindMode();

	// 개량된 (정확한) 가시성 판단 함수
	// 특정 직선과, 타일 하나
	BOOL CollisionCheck_OneLine_New(DWORD p_StartX, DWORD p_StartY, DWORD p_EndX, DWORD p_EndY);
	BOOL CollisionCheck_OneLine_New(VECTOR3* p_pStart,VECTOR3* p_pEnd);

	

private:
	// 레이어별 타일의 가로세로값
	DWORD	m_dwWidth1x;		// 1배 원본
	DWORD	m_dwHeight1x;

	DWORD	m_dwWidth3x;		// 3배 (원본으로부터 생성)
	DWORD	m_dwHeight3x;
	
	DWORD	m_dwWidth4x;		// 4배 (원본으로부터 생성)
	DWORD	m_dwHeight4x;

	float	m_NodeRatio;


	///--------------------------------------------------
	///	길찾기 엔진 - IPathFinder 구현
	///--------------------------------------------------
public:

	/// 리셋 - 길찾기용 중간 데이터
	virtual void AStarDataReset();

	/// 시작 노드와 끝 노드를 입력
	virtual BOOL InputPoint( IPathNode* p_StartNode, IPathNode* p_DestNode );
			BOOL InputPoint( const VECTOR3& p_StartPosition, const VECTOR3& p_DestPosition );

	/// 길찾기!
	virtual BOOL PathFind_AStar( UINT p_LoopCount );

	/// 길찾기 완료되었고, Path를 역추적, 만들어서 리턴
	virtual BOOL MakeAStarPath();

	/// A-Star를 통해서 나온 위치값들을 가시성 판단, 직선화 실시
	virtual BOOL Optimize();
		private:
			BOOL Optimize(std::deque<IPathNode*>& p_deq_AStarPath, std::deque<IPathNode*>& p_deq_AStarPath_Optimize);


public:
	DWORD GetWidth1x() const { return m_dwWidth1x; }
	DWORD GetHeight1x() const { return m_dwHeight1x; }

	// 노드를 2차원 배열의 인자로 얻어온다
	IPathNode*	GetMovableNode(DWORD p_PosX, DWORD p_PosY);
	IPathNode*	GetMovableNode(float p_PosX, float p_PosY);
	CTile*	GetNode(DWORD p_PosX, DWORD p_PosY);
	CTile*	GetNode(float p_PosX, float p_PosY);
	
	CTile*	GetNode1x(DWORD p_PosX, DWORD p_PosY);
	CTile*	GetNode1x(float p_PosX, float p_PosY);

	std::vector<VECTOR3> GetPathResult(float p_Ratio);
	std::deque<VECTOR3> GetPathResult_Optimize(float p_Ratio);
	std::deque<VECTOR3> GetPathResult_Optimize_Debug(float p_Ratio);


	/// 맵 상에서의 빠른 길찾기를 위한 추가구현 부분
	
	// 특정 위치에서 가장 가까운 Nx타일을 찾는다 (3배속)
	BOOL FindClosestTile(const VECTOR3& p_Position, IPathNode*& p_ClosestNode);
	// 특정 타일 주위의 열려 있는 타일을 찾는다
	BOOL FindClosest1xTile(VECTOR3& p_SafePosition, DWORD p_NowPosX, DWORD p_NowPosY);

	// Node의 Ratio를 반환한다.
	float GetNodeRatio() const { return m_NodeRatio; }

	// 현재 위치의 3배 타일정보를 갱신한다.
	void UpdateNodeInfo( VECTOR3* pPos );


private:
	/// A-Star
	// 기준이 되는 전체 노드. 원본
	CTile*								m_pAllNode1x;		// 1배 원본
	CTile*								m_pAllNode3x;		// 3배 (원본으로부터 생성)

	// 중간변수. 포인터의 복사본일뿐이다
	std::multimap<float, IPathNode*>	m_map_OpenList;
	std::vector<IPathNode*>				m_vec_ClosedList;
	IPathNode*							m_StartNode;
	IPathNode*							m_DestNode;
	IPathNode*							m_NowNode;

	// 결과로 찾은 길. 복사본
	std::deque<IPathNode*>				m_AStarPath;
	std::deque<IPathNode*>				m_AStarPath_Optimize;

	// 실제 위치 데이터 & 디버그용
	std::vector<VECTOR3>				m_PathResult;
	std::deque<VECTOR3>					m_PathResult_Optimize;
	std::deque<VECTOR3>					m_PathResult_Optimize_Debug;
};

#endif // !defined(AFX_TILEMANAGER_H__5FD3CE62_5211_46E8_9C7B_4B5CB5D006CA__INCLUDED_)
