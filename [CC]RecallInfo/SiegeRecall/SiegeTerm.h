#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CSiegeTerm.h
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
#define MAX_TERM_INDEX	250





//-------------------------------------------------------------------------------------------------
//		The class CSiegeTerm.
//-------------------------------------------------------------------------------------------------
class CSiegeTerm
{
	BYTE m_byParentIdx ;

	BYTE m_byIndex ;

	typedef std::map< BYTE, st_CHECK_OBJ_DIE >	M_CHKOBJDIE ;
	M_CHKOBJDIE		m_mCheckObjDie ;

	typedef std::map< BYTE, st_CHECK_USE_ITEM > M_CHKUSEITEM ;
	M_CHKUSEITEM	m_mCheckUseItem ;

public:
	CSiegeTerm();
	virtual ~CSiegeTerm();
	void Set_ParentIdx(BYTE byIndex) ;								// The function to setting parent index.
	BYTE Get_ParentIdx() { return m_byParentIdx ; }					// The function to return parent index.

	void Set_Index(BYTE byIndex) ;									// The function to setting index.
	BYTE Get_Index() { return m_byIndex ; }							// The function to return index.

	BYTE Get_ChildCount() ;											// The function to return child count.
	BYTE Get_ChildKind(BYTE byIdx) ;								// The function to return child kind.

	void Insert_CheckObjDie(st_CHECK_OBJ_DIE* pInfo) ;				// The function to insert check object die.
	void Remove_CheckObjDie(st_CHECK_OBJ_DIE* pInfo) ;				// The function to remove check object die.

	BYTE Get_CheckObjDieCount() { return (BYTE)m_mCheckObjDie.size() ; }	// The function to return check object die count.

	void Insert_CheckUseItem(st_CHECK_USE_ITEM* pInfo) ;			// The function to insert chec use item.
	void Decrease_UseItemCount(st_CHECK_USE_ITEM* pInfo) ;			// The function to decrease use item count.

	BYTE Get_CheckUseItemCount() { return (BYTE)m_mCheckUseItem.size() ; }	// The function to return che use item count.

	st_CHECK_OBJ_DIE* Get_ChkObjDieInfo(BYTE byIdx) ;				// The function to return check object die info.
	st_CHECK_USE_ITEM* Get_ChkUseItemInfo(BYTE byIdx) ;				// The function to return check use item info.


	////////////////////////////////
	// [ 에러 메시지 처리 파트. ] //
	////////////////////////////////

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.
};
