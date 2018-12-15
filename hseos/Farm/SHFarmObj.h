/*********************************************************************

	 파일		: SHFarmObj.h
	 작성자		: hseos
	 작성일		: 2007/04/18

	 파일설명	: 농장 기본 물체 클래스의 헤더

 *********************************************************************/

#pragma once

#include "SHFarmRenderObj.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class CSHFarmObj
{
public:
	//----------------------------------------------------------------------------------------------------------------
	static const int	MAX_CHILD_NUM					= 256;		// 최대 자식 개수

	//----------------------------------------------------------------------------------------------------------------
	enum OWN_STATE													// 소유 상태						
	{
		OWN_STATE_EMPTY = 0,										// ..비어 있는 상태										
		OWN_STATE_OWNED,											// ..누군가가 소유하고 있는 상태
		OWN_STATE_MAX = 100000,										// ..끝
	};

	//----------------------------------------------------------------------------------------------------------------
	enum GRADE														// 등급
	{
		GRADE_1 = 1,												 
		GRADE_2,
		GRADE_3,
		GRADE_4,
		GRADE_5,
		GRADE_6,
		GRADE_7,
		GRADE_8,
		MAX_GRADE = GRADE_8,
	};

	//----------------------------------------------------------------------------------------------------------------
	struct stEVENT													// 발생한 이벤트
	{
		DWORD nOwnerID;												// ..소유자
		WORD nEvent[256];											// ..이벤트
		WORD nEventNum;												// ..이벤트 개수
	};

protected:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarmObj*						m_pcsParent;					// 부모
	CSHFarmObj*						m_pcsChild[MAX_CHILD_NUM];		// 자식
	WORD							m_nChildNum;					// 자식 개수

	//----------------------------------------------------------------------------------------------------------------
	WORD							m_nID;							// ID

	DWORD							m_nOwnerID;						// 소유자
	OWN_STATE						m_eOwnState;					// 소유상태
	WORD							m_nLife;						// 수명
	WORD							m_nGrade;						// 등급

	VECTOR3							m_stPos;						// 좌표
	float							m_nDir;							// 방향

	//----------------------------------------------------------------------------------------------------------------
	stEVENT							m_stEvent;						// 이벤트
	WORD							m_nEventKind;					// 이벤트 종류

	//----------------------------------------------------------------------------------------------------------------
	CSHFarmRenderObj*				m_pcsRenderObj;					// 화면출력 오브젝트.
	CSHFarmRenderObj*				m_ppcsRenderObjEx[30];			// 화면출력 확장 오브젝트.

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarmObj();
	// 090629 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CSHFarmObj();
	//								부모 설정
	VOID							SetParent(CSHFarmObj* pcsParent)	{ m_pcsParent = pcsParent; }
	//								부모 얻기
	CSHFarmObj*						GetParent()							{ return m_pcsParent; }

	//----------------------------------------------------------------------------------------------------------------
	//								자식 설정
	VOID							AddChild(CSHFarmObj* pcsChild, int nParentEvent = NULL)	
									{ 
										m_pcsChild[m_nChildNum] = pcsChild;
										m_pcsChild[m_nChildNum]->SetEventKind((WORD)nParentEvent);
										m_nChildNum++;
										if (m_nChildNum >= MAX_CHILD_NUM)
										{
											m_nChildNum = MAX_CHILD_NUM-1;
											#if defined(_DEBUG)
												MessageBox(NULL, "Overflow : CSHFarmObj::MAX_CHILD_NUM", NULL, NULL);
											#endif
										}
									}

	//----------------------------------------------------------------------------------------------------------------
	//								ID 설정
	VOID							SetID(WORD nID)						{ m_nID = nID; }
	//								ID 얻기
	WORD							GetID()								{ return m_nID; }

	//----------------------------------------------------------------------------------------------------------------
	//								소유자 설정
	VOID							SetOwner(DWORD nOwnerID)			{ m_nOwnerID = nOwnerID; }
	//								소유자 얻기
	DWORD							GetOwner()							{ return m_nOwnerID; }

	//----------------------------------------------------------------------------------------------------------------
	//								소유 상태 설정
	VOID							SetOwnState(OWN_STATE eOwnState)	{ m_eOwnState = eOwnState; }
	//								소유 상태 얻기
	OWN_STATE						GetOwnState()						{ return m_eOwnState; }

	//----------------------------------------------------------------------------------------------------------------
	//								수명 설정
	VOID							SetLife(WORD nLife)					{ m_nLife = nLife; }
	//								수명 얻기
	WORD							GetLife()							{ return m_nLife; }

	//----------------------------------------------------------------------------------------------------------------
	//								등급 설정
	VOID							SetGrade(WORD nGrade)				{ m_nGrade = nGrade; }
	//								등급 얻기
	WORD							GetGrade()							{ return m_nGrade; }

	//----------------------------------------------------------------------------------------------------------------
	//								좌표 설정
	VOID							SetPos(VECTOR3* pPos)				{ m_stPos = *pPos; }
	//								좌표 얻기
	VECTOR3*						GetPos()							{ return &m_stPos; }

	//----------------------------------------------------------------------------------------------------------------
	//								방향 설정
	VOID							SetDir(float nDir)					{ m_nDir = nDir; }
	//								방향 얻기
	float							GetDir()							{ return m_nDir; }

	//----------------------------------------------------------------------------------------------------------------
	//								이벤트 초기화
	VOID							InitEvent()
									{
										ZeroMemory(&m_stEvent, sizeof(m_stEvent));
									}
	//								이벤트 추가
	VOID							AddEvent(DWORD nOwnerID, WORD nEvent)
									{
										m_stEvent.nOwnerID = m_nOwnerID;
										m_stEvent.nEvent[m_stEvent.nEventNum] = nEvent;
										m_stEvent.nEventNum++;
									}
	//								이벤트 얻기
	stEVENT*						GetEvent()							{ return &m_stEvent; }
	//								이벤트 종류 설정
	VOID							SetEventKind(WORD nEventKind)		{ m_nEventKind = nEventKind; }
	//								이벤트 종류 얻기
	WORD							GetEventKind()						{ return m_nEventKind; }

	//----------------------------------------------------------------------------------------------------------------
	//								화면 출력 오브젝트 설정
	VOID							SetRenderObj(CSHFarmRenderObj* pObj)				{ m_pcsRenderObj = pObj; }
	VOID							SetRenderObjEx(CSHFarmRenderObj* pObj, int nID)		{ m_ppcsRenderObjEx[nID] = pObj; }
	//								화면 출력 오브젝트 얻기
	CSHFarmRenderObj*				GetRenderObj()										{ return m_pcsRenderObj; }
	CSHFarmRenderObj*				GetRenderObjEx(int nID)								{ return m_ppcsRenderObjEx[nID]; }

	//----------------------------------------------------------------------------------------------------------------
	//								메인 루프
	virtual VOID					MainLoop();
};