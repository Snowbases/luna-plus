#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeCommand.h
//	DESC		: The class to contain object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 25, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define MAX_COMMAND_IDX		250





//-------------------------------------------------------------------------------------------------
//		The class CSiegeCommand.
//-------------------------------------------------------------------------------------------------
class CSiegeCommand
{
	BYTE m_byIndex ;
	// 
	typedef std::map< BYTE, st_DIE_RECALL_OBJ >		M_RECALLOBJ ;
	M_RECALLOBJ		m_mRecallObj ;

public:
	CSiegeCommand(void);
	virtual ~CSiegeCommand(void);
	void Set_Index(BYTE byIdx) ;									// The function to setting index.
	BYTE Get_Index() { return m_byIndex ; }							// The function to return index.

	BYTE Get_ChildCount() ;											// The function to return child count.
	BYTE Get_ChildKind(BYTE byIdx) ;								// The function to return child kind.

	st_DIE_RECALL_OBJ* Get_DieRecallObjInfo(BYTE byIdx) ;			// The function to return die recall object info.

	void Add_Die_Recall_Obj(st_DIE_RECALL_OBJ* pInfo) ;				// The function to add recall info when die.


	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.
};
