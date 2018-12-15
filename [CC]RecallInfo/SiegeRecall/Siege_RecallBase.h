#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiege_RecallBase.h
//	DESC		: The class to contain object info.
//	PROGRAMMER	: Yongs Lee
//	DATE		: July 30, 2008
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include <map>

#include "./SiegeCommand.h"
#include "./SiegeTerm.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define MAX_BASE_INDEX	250





//-------------------------------------------------------------------------------------------------
//		The class CSiege_RecallBase.
//-------------------------------------------------------------------------------------------------

class CSiege_RecallBase
{
	BYTE m_byBaseKind ;

	//
	typedef std::map< BYTE, CSiegeTerm >		M_STERM ;
	M_STERM		m_mSTerm ;

	// 
	typedef std::map< BYTE, CSiegeTerm >		M_ETERM ;
	M_ETERM		m_mETerm ;

	// 
	typedef std::map< BYTE, CSiegeCommand >		M_COMMAND ;
	M_COMMAND	m_mCommand ;

public:
	CSiege_RecallBase(void);
	virtual ~CSiege_RecallBase(void);
	void Set_BaseKind(BYTE byKind);
	BYTE Get_BaseKind() const { return m_byBaseKind; }
	BYTE Get_ChildCount();
	BYTE Get_StartTermCount() { return (BYTE)m_mSTerm.size() ; }			// The function to return start term count.
	BYTE Get_EndTermCount() { return (BYTE)m_mETerm.size() ; }			// The function to return end term count.
	BYTE Get_CommandCount() { return (BYTE)m_mCommand.size() ; }			// The function to return command count.
	BYTE Get_ChildKind(BYTE byIdx) ;								// The function to return child kind.

	void Insert_Sterm(BYTE byIdx, CSiegeTerm* pTerm) ;				// The function to insert start term.
	void Insert_ETerm(BYTE byIdx, CSiegeTerm* pTerm) ;				// The function to insert end term.
	void Insert_Command(BYTE byIdx, CSiegeCommand* pCommand) ;		// The function to insert command.

	CSiegeTerm* Get_StartTermInfo(BYTE byIdx) ;						// The function to return start term info.
	CSiegeTerm* Get_EndTermInfo(BYTE byIdx) ;						// The function to return end term info.

	CSiegeCommand* Get_CommandInfo(BYTE byIdx) ;					// The function to return commmand ifno.


	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.
};
