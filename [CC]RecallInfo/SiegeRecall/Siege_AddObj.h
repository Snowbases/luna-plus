#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiege_AddObj.h
//	DESC		: The class to contain object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "./SiegeStep.h"

#include "./SiegeTerm.h"
#include "./SiegeCommand.h"

#include "./Siege_RecallBase.h"

//class CSiege_RecallBase ;
//class CSiegeTerm ;
//class CSiegeCommand ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define MAX_ADDOBJ_IDX		250





//-------------------------------------------------------------------------------------------------
//		The class CSiege_AddObj.
//-------------------------------------------------------------------------------------------------
//class CSiege_AddObj : public CSiegeStep
class CSiege_AddObj : public CSiege_RecallBase
{
	// parent step index.
	BYTE m_byParentStepIdx ;

	// index.
	BYTE m_byIdx ;

	st_SIEGEOBJ		m_ObjInfo ;

	//090812 pdy 공성전 Agent와 Map서버간의 소환Obj 인덱스관리가 잘못되있는 버그 수정 
	DWORD	m_dwMonsterID;

public:
	CSiege_AddObj();
	virtual ~CSiege_AddObj();
	void Set_ParentStepIdx(BYTE byIdx);
	BYTE Get_ParentStepIdx() const { return m_byParentStepIdx; }
	void Set_Index(BYTE byIndex);
	BYTE Get_Index() const { return m_byIdx; }
	void Init_ObjInfo(st_SIEGEOBJ*);
	st_SIEGEOBJ* Get_ObjInfo() { return &m_ObjInfo; }
	void Throw_Error(char* szErr, char* szCaption);
	void WriteLog(char* pMsg);
	void Set_MosterID(DWORD dwID) { m_dwMonsterID = dwID; }
	DWORD Get_MonsterID() const {return m_dwMonsterID; }
};
