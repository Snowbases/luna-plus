#include ".\tutorial_msg.h"

cTutorial_Msg::cTutorial_Msg(void)
{
}

cTutorial_Msg::~cTutorial_Msg(void)
{
	Release();
}





void cTutorial_Msg::Release()
{
	MSG_LINE* pMsgLine ;

	PTRLISTPOS pDeletePos = NULL ;
	pDeletePos = m_Msg_List.GetHeadPosition() ;

	while(pDeletePos)
	{
		pMsgLine = NULL ;
		pMsgLine = (MSG_LINE*)m_Msg_List.GetNext(pDeletePos) ;

		if( !pMsgLine ) continue ;

		delete pMsgLine ;
	}

	m_Msg_List.RemoveAll() ;
}





void cTutorial_Msg::Add_MsgLine(char* pMsg)
{
	if( !pMsg )
	{
		return ;
	}

	MSG_LINE* pMsgLine = NULL ;
	pMsgLine = new MSG_LINE ;

	if( !pMsgLine )
	{
		return ;
	}

	int nMsgCount = m_Msg_List.GetCount() ;

	pMsgLine->nLine = nMsgCount ;
	strcpy(pMsgLine->msg, pMsg) ;

	m_Msg_List.AddTail( pMsgLine ) ;
}





MSG_LINE* cTutorial_Msg::Get_MsgLine(int nIndex)
{
	int nMsg_Count = 0 ;													// 메시지 라인 수를 받을 변수를 선언하고 0으로 초기화 한다.
	nMsg_Count = m_Msg_List.GetCount() ;									// 메시지 라인 개수를 받는다.

	if( nIndex < 0 || nIndex >= nMsg_Count ) return NULL ;					// 인자로 넘어온 인덱스가, 0보다 작거나, 현재 메시지 라인 수보다 크면, null return 처리를 한다.

	PTRLISTPOS returnPos = NULL ;											// 인덱스에 해당하는 메시지 라인 위치를 담을 포인터를 선언하고 null 처리를 한다.
	returnPos = m_Msg_List.FindIndex(nIndex) ;								// 인덱스에 해당하는 메시지 라인의 위치정보를 받는다.

	if( !returnPos ) return NULL ;											// 위치 정보가 유효하지 않으면, null return 처리를 한다.

	return (MSG_LINE*)m_Msg_List.GetAt(returnPos) ;							// 위치에 해당하는, 메시지 라인 정보를 return 처리를 한다.
}
