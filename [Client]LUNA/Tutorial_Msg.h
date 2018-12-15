#pragma once





//==========================================================================//
//																			//
//==========================================================================//





// INCLUDE PART
#include "stdafx.h"															// 표준 헤더파일을 포함한다.





struct MSG_LINE
{
	int nLine ;

	char msg[1024] ;
} ;


class cTutorial_Msg
{
	int				m_nIndex ;

	cPtrList		m_Msg_List ;
public:
	cTutorial_Msg(void);
	virtual ~cTutorial_Msg(void);

	void Release() ;

	void Set_MsgIndex(int nIndex) { m_nIndex = nIndex ; }
	int  Get_MsgIndex() { return m_nIndex ; }


	int	 Get_LineCount() { return m_Msg_List.GetCount() ; }

    void Add_MsgLine(char* pMsg) ;

	MSG_LINE* Get_MsgLine(int nIndex) ;
};
