#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeMap.h
//	DESC		: The class to contain a map info for siege war.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "./Siege_AddObj.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define MAX_MAP_NUMBER	300												// Use this to check limit map number. Current max number is 300.
#define MAX_STEP_INDEX	250												// Use this to check limit index of step. Current max index is 250.





//-------------------------------------------------------------------------------------------------
//		The class CSiegeMap.
//-------------------------------------------------------------------------------------------------
class CSiegeMap
{
	WORD m_wThemeIdx ;

	// The index variable for this map class.
	BYTE m_byIdx ;

	// The index of step this map part.
	//BYTE m_byCurStep ;

	// The number variable for map.
	MAPTYPE m_byMapNum ;

	//// The Map containter to contain a add object info.
	//typedef std::map< BYTE, CSiege_AddObj >	M_ADDOBJ ;
	//M_ADDOBJ	m_mAddObj ;

	// 
	typedef std::map< BYTE, CSiegeStep >	M_STEP ;
	M_STEP	m_mStep ;

public:
	CSiegeMap();
	virtual ~CSiegeMap();
	void Set_ThemeIdx(WORD wIndex) { m_wThemeIdx = wIndex ; }			// The function to setting theme index.
	WORD Get_ThemeIdx() { return m_wThemeIdx ; }						// The function to return theme index.


	// Setting & return base info.
	void Set_Idx(BYTE byIdx) ;											// The function to seting index of this class.
	BYTE Get_Idx() { return m_byIdx ; }									// The function to return index of this class.

	//void Set_CurStepIdx(BYTE byStep) ;									// The function to setting current step index.
	//BYTE Get_CurStepIdx() { return m_byCurStep ; }						// The function to return current step index.

	void Set_MapNum(MAPTYPE mapNum) ;									// The function to setting map number.
	MAPTYPE Get_MapNum() { return m_byMapNum ; }						// The function to return map number.

	BYTE Get_StepCount() { return (BYTE)m_mStep.size() ; }					// The function to return step count.

	void Add_Step(BYTE byIndex, CSiegeStep* pStepInfo) ;				// The function to add step info.
	CSiegeStep* Get_StepInfo(BYTE byIndex) ;							// The function to return step info.

	void Execute_Step(MAPTYPE mapNum, BYTE byStepIdx) ;					// The function to execute step.


	//// Step part.
	//BYTE Get_AllStepCount() ;											// The function to return all step count.
	//CSiegeStep* Get_SiegeStepByIdx( BYTE byIdx ) ;						// The function to return siege step info.

	//void Increase_Step(DWORD dwIndex, DWORD dwObjID) ;					// The function to increase step.
	//void Decrease_Step(DWORD dwIndex, DWORD dwObjID) ;					// The function to decrease step.

	//void Execute_Step(DWORD dwIndex, DWORD dwObjID, BYTE byStep) ;		// The function to execute step.


	//// Add object aprt.
	//BYTE Get_AddObjCount() { return m_mAddObj.size() ; }				// The function to return add object count.
	//CSiege_AddObj* Get_AddObjInfo( BYTE byIdx ) ;						// The function to return add object info.

	//void Insert_AddObj(CSiege_AddObj* pAddObj) ;						// The function to insert add obj info.


	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.
};











