//--------------------------------------------------
//	IPathFinder
//	[2009/6/18 LeHide]
//
//	길찾기
//
//	Version		:	1.0.2009618
//--------------------------------------------------

#pragma once

class IPathNode;


///--------------------------------------------------
///	길찾기 엔진
///--------------------------------------------------
class IPathFinder
{
public:
	/// 리셋 - 길찾기용 중간 데이터
	virtual void AStarDataReset() PURE;

	/// 시작 노드와 끝 노드를 입력
	virtual BOOL InputPoint( IPathNode* p_StartNode, IPathNode* p_DestNode ) PURE;

	/// 길찾기!
	virtual BOOL PathFind_AStar( UINT p_LoopCount ) PURE;

	/// 길찾기 완료되었고, Path를 역추적, 만들어서 리턴
	virtual BOOL MakeAStarPath() PURE;

	/// A-Star를 통해서 나온 위치값들을 가시성 판단, 직선화 실시
	virtual BOOL Optimize() PURE;


};


///--------------------------------------------------
///	Description
///--------------------------------------------------
/*

2009.06.18

A*의 알고리즘만을 분리해서 담아볼까 한다

열린 목록이니, 닫힌 목록이니 하는 것은 이 부분에서 추상화하고,
실제 구현은 이것을 상속한 클래스들이 하도록.




*/
