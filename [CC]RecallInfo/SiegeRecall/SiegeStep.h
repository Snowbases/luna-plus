#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeStep.h
//	DESC		: The class to contain step info for siege war.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 23, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

class CSiege_AddObj ;
//#include "./Siege_AddObj.h"

//#include "./SiegeTerm.h"
//#include "./SiegeCommand.h"
//class CSiegeTerm ;
//class CSiegeCommand ;





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define MAX_STEP_IDX		250

//#define MAX_COMMAND_COUNT	250
//
//#define MAX_GROUP_IDX		250
//#define MAX_STEP_IDX		250
//#define MAX_TERM_IDX		250





//-------------------------------------------------------------------------------------------------
//		The class CSiegeStep.
//-------------------------------------------------------------------------------------------------
class CSiegeStep
{
	WORD m_wThemeIdx ;

	// The group index.
	//BYTE m_byGroupIdx ;

	MAPTYPE m_ParentMapNum ;

	// The Step Index.
	BYTE m_byStepIdx ;

	//
	typedef std::map< BYTE, CSiege_AddObj >		M_MADDOBJ ;
	M_MADDOBJ	m_mAddObj ;

	// The variable to contain type of step.
	//BYTE m_byStepKind ;

	////
	//typedef std::map< BYTE, CSiegeTerm >		M_STERM ;
	//M_STERM		m_mSTerm ;

	//// 
	//typedef std::map< BYTE, CSiegeTerm >		M_ETERM ;
	//M_ETERM		m_mETerm ;

	//// 
	//typedef std::map< BYTE, CSiegeCommand >		M_COMMAND ;
	//M_COMMAND	m_mCommand ;

public:
	CSiegeStep();
	virtual ~CSiegeStep();
	void Set_ThemeIdx(WORD wIndex) { m_wThemeIdx = wIndex ; }	// The function to setting theme index.
	WORD Get_ThemeIdx() { return m_wThemeIdx ; }				// The function to return theme index.

	void Set_ParentMapNum(MAPTYPE mapNum) ;						// The function to setting parent map number of this step.
	MAPTYPE Get_ParentMapNum() { return m_ParentMapNum ; }		// The function to return parent map number of this step.

	//void Set_GroupIndex(BYTE byGroupIdx) ;						// The function to setting group index of this step.
	//BYTE Get_GroupIndex() { return m_byGroupIdx ; }				// The function to return group index.

	void Set_StepIndex(BYTE byIdx) ;							// The function to setting index of this step.
	BYTE Get_StepIndex() { return m_byStepIdx ; }				// The function to return step index.

	BYTE Get_ChildCount() ;										// The function to return child controls count.
	BYTE Get_ChildKind(BYTE byIdx) ;							// The function to return child kind.

	void Insert_AddObj(BYTE byIndex, CSiege_AddObj* pAddObj) ;	// The function to insert add object info.
	CSiege_AddObj* Get_AddObjInfo(BYTE byIndex) ;				// The function to return add object info.

	//void Set_StepKind(BYTE byStepKind) ;						// The function to setting step kind of this step.
	//BYTE Get_StepKind() { return m_byStepKind ; }				// The function to return step kind.

	//BYTE Get_StartTermCount() { return m_mSTerm.size() ; }		// The function to return start term count.
	//CSiegeTerm* Get_StartTerm(BYTE byIdx ) ;					// The function to return start term info.

	//BYTE Get_EndTermCount() { return m_mETerm.size() ; }		// The function to return end term count.
	//CSiegeTerm* Get_EndTerm(BYTE byIdx) ;						// The function to return end term info.

	//BYTE Get_CommandCount() { return m_mCommand.size() ; }		// The function to return command count.
	//CSiegeCommand* Get_Command( BYTE byIdx ) ;					// The function to return command info.

	//void Insert_Command(BYTE byIdx, CSiegeCommand* pCommand) ;	// The function to insert command to this step.


	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.
};









