//--------------------------------------------------
//	CWayPointManager.h
//	[7/19/2009 LeHide]
//
//	웨이포인트들을 관리
//
//	Version		:	1.0.2009719
//--------------------------------------------------

#pragma once

enum eWMStatus
{
	eWMStatus_Inactive,
	eWMStatus_Active,
	eWMStatus_Success,
	eWMStatus_Fail,
};


class CWayPointManager
{
public:
	CWayPointManager();
	virtual ~CWayPointManager();


private:
	std::deque<VECTOR3>		m_deq_WayPoint;			// 웨이포인트
	VECTOR3					m_FlagDestination;		// 최종 목적지(마우스 클릭된 곳)

	eWMStatus				m_WMStatus;				// 매니저의 상태


public:
	// 웨이포인트 넣기
	void AddWayPoint(const VECTOR3& p_WayPoint);

	// 웨이포인트 꺼내기
	BOOL PopWayPoint(VECTOR3* p_WayPoint);

	// 모든 웨이포인트 삭제(무효화)
	void ClearAllWayPoint();

	// 웨이포인트가 비었나?
	BOOL IsWayPointEmpty();

	// 특정 지점인데 웨이포인트들을 분석해서, 적당한 다음 웨이포인트를 리턴


	/// 웨이포인트 매니저의 상태
	// 현 웨이포인트 매니저의 상태
	// 1. 활성화(데이터가 있고, 꺼내어서 쓰고 하는 단계)
	// 2. 비활성화(데이터가 없고, 꺼내지도 못함. 이동중 아님)
	// 3. 연산중(길찾기가 내부적으로 쓰레드 연산이라서,
	//			 웨이포인트가 임시로 있기는 하지만, 대기중인 상태)
	eWMStatus GetWMStatus() const { return m_WMStatus; }
	void SetWMStatus(eWMStatus val) { m_WMStatus = val; }


	/// 최종 목표지 - 깃발꼽기
	VECTOR3 GetFlagDestination() const { return m_FlagDestination; }
	void SetFlagDestination(VECTOR3 val) { m_FlagDestination = val; }

	// 벡터 자체를 핸들링
	std::deque<VECTOR3>& GetVec_WayPoint() { return m_deq_WayPoint; }
	void SetVec_WayPoint(std::deque<VECTOR3>& p_vec_WayPoint) { m_deq_WayPoint = p_vec_WayPoint; }
	
	// 첫 번째 포인트를 체인지한다(마지막 목표점을 바꾼다)
	void ChangeDestination( const VECTOR3& p_Dest );


	/// 디버그용
	// 웨이포인트를 TRACE
	void Debug_TRACEWayPoint();

};

