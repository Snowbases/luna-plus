//=================================================================================================
//	FILE		: CChatRoomMgr.cpp
//	DESC		: 채팅방 매니져 구현부.
//	DATE		: APRIL 2, 2008 LYW
//				:
//	DESC		: FEBRUARY 25, 2009 - 기존 유저관리 등을 STL MAP으로 관리 하던 것을 CYHHashTable
//				: 관리 방식으로 수정 함.
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		헤더파일 포함.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "../[CC]ServerModule/Network.h"

#include "./ChatRoomMgr.h"





//-------------------------------------------------------------------------------------------------
//	NAME : CChatRoomMgr
//	DESC : 생성자 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::CChatRoomMgr( void) 
{
	// 유저 관리를 하는 해쉬 테이블을 초기화한다.
	m_htUser.Initialize( MAX_USER_BUCKETCNT ) ;



	// 검색용 이름을 관리하는 해쉬 테이블을 초기화한다.
	m_htSearchName.Initialize( MAX_USER_BUCKETCNT ) ;

    

	// 채팅방 인덱스 관리 배열 초기화.
	memset( m_byRoomIdx, 0, sizeof(BYTE)*MAX_ROOM_COUNT ) ;

	m_byRoomIdx[0] = 1 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomMgr
//	DESC : 소멸자 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::~CChatRoomMgr( void )
{
	// 채팅방/ 검색용 채팅방 해제.
	ReleaseRoom() ;



	// 검색용 이름 정보 해쉬 테이블을 해제한다.
	ST_SEARCH_NAME* pNameInfo = NULL ;
	m_htSearchName.SetPositionHead() ;
	while( (pNameInfo = m_htSearchName.GetData()) != NULL )
	{
		delete pNameInfo ;
		pNameInfo = NULL ;
	}
	m_htSearchName.RemoveAll() ;



	// 유저 관리용 해쉬 테이블을 해제한다.
	ST_CR_USER* pUserInfo = NULL ;
	m_htUser.SetPositionHead() ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		delete pUserInfo ;
		pUserInfo = NULL;
	}
	m_htUser.RemoveAll() ;
}




//-------------------------------------------------------------------------------------------------
//	NAME		: GetInstance()
//	DESC		: The function that returns this class's instance to static type.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 25, 2009
//-------------------------------------------------------------------------------------------------
CChatRoomMgr* CChatRoomMgr::GetInstance()
{
	// 참조: http://www.codeproject.com/useritems/VC2003MeyersSingletonBug.asp
	//
	// 정적 인라인 함수는 참조시마다 생성된다(인라인 함수는 직접 대치될 뿐 링크되지 않는다). 
	// 이로 인해 싱글턴 변수까지 참조시마다 생성된다!
	// 이를 막으려면 단일한 링크 지점을 만들어줘야한다. C 파일에 직접 구현하면 해결된다.



	static CChatRoomMgr instance ;

	return &instance ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ReleaseRoom
//	DESC : 채팅방 리소스 해제 함수.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ReleaseRoom()
{
	// 방 삭제.
	ST_CR_ROOM_SRV* pInfo = NULL ;
	PTRLISTPOS pos = NULL ;

	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		m_pRoomList.Remove( pInfo ) ;

		delete pInfo ;
		pInfo = NULL ;
	}

	m_pRoomList.RemoveAll() ;



	// 검색용 방 삭제.
	ST_CR_ROOM_CLT* pTitle = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pTitle = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pTitle ) continue ;

		m_pTitleList.Remove( pTitle ) ;

		delete pTitle ;
		pTitle = NULL ;
	}

	m_pTitleList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : IsInRoomList
//	DESC : 방 리스트에, 방 정보가 있는지 체크하는 함수.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::IsInRoomList( BYTE byRoomIdx ) 
{
	ST_CR_ROOM_SRV* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byIdx == byRoomIdx ) return TRUE ;
	}

	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : AddRoom
//	DESC : 채팅방 정보를 추가하는 함수.
//		 : 채팅방 정보 뿐 아니라, 검색용 채팅방 정보도 같이 추가를 한다.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::AddRoom( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return FALSE ;
	}



	// 방추가.
	m_pRoomList.AddTail( pInfo ) ;



	// 방 인덱스 세팅.
	m_byRoomIdx[ pInfo->byIdx ] = pInfo->byIdx ;



	// 검색 용 방 추가.
	ST_CR_ROOM_CLT* pTitle = NULL ;
	pTitle = new ST_CR_ROOM_CLT ;

	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( "Failed to create title room!!", __FUNCTION__ ) ;
		return FALSE ;
	}

	pTitle->byIdx				= pInfo->byIdx ;
	pTitle->bySecretMode		= pInfo->bySecretMode ;
	pTitle->byRoomType			= pInfo->byRoomType ;
	pTitle->byCurGuestCount		= pInfo->byCurGuestCount ;
	pTitle->byTotalGuestCount	= pInfo->byTotalGuestCount ;

	SafeStrCpy( pTitle->title, pInfo->title, TITLE_SIZE ) ;

	m_pTitleList.AddTail( pTitle ) ;

	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : DeleteRoom
//	DESC : 채팅방 정보를 삭제하는 함수.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::DeleteRoom( BYTE byRoomIdx )
{
	// 방 삭제 처리.
	ST_CR_ROOM_SRV* pItem = NULL ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byIdx != byRoomIdx ) continue ;

		m_pRoomList.Remove( pItem ) ;

		delete pItem ;
		pItem = NULL ;

		m_byRoomIdx[ byRoomIdx ] = 0 ;

		break ;
	}



	// 검색용 방 삭제 처리.
	ST_CR_ROOM_CLT* pTitle ;
	pos = m_pTitleList.GetHeadPosition() ;

	while( pos )
	{
		pTitle = NULL ;
		pTitle = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pTitle ) continue ;

		if( pTitle->byIdx != byRoomIdx ) continue ;

		m_pTitleList.Remove( pTitle ) ;

		delete pTitle ;
		pTitle = NULL ;

		break ;
	}

	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetRoomInfo
//	DESC : 채팅방 정보를 반환하는 함수.
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
ST_CR_ROOM_SRV* CChatRoomMgr::GetRoomInfo( BYTE byRoomIdx ) 
{
	ST_CR_ROOM_SRV* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byIdx != byRoomIdx ) continue ;

		return pItem ;
	}

	return NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetRoomCountByType
//
//	DESC : 채팅방 분류에 따라서, 리스트에 몇개나 담겨 있는지 반환하는 함수.
//		 : 
//		 : * 방 개수는 최대 250개 이므로 BYTE형을 사용한다.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::GetRoomCountByType( BYTE byType )
{
	BYTE byRoomCount = 0 ;

	ST_CR_ROOM_SRV* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		pItem = NULL ;
		pItem = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pItem ) continue ;

		if( pItem->byRoomType != byType ) continue ;

		++byRoomCount ;
	}

	return byRoomCount ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SendErrors
//	DESC : 에러 메시지를 전송하는 함수.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SendErrors( DWORD dwIndex, DWORD dwPlayerID, BYTE byProtocol, BYTE byErr ) 
{
	MSG_BYTE msg ;
	memset( &msg, 0, sizeof(MSG_BYTE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= byProtocol ;
	msg.dwObjectID	= dwPlayerID ;

	msg.bData		= byErr ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_BYTE) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : NetworkMsgParser
//	DESC : 채팅방과 관련한, Dist 서버로 넘어오는 메시지를 처리하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::NetworkMsgParser( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 기본 메시지 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTB, __FUNCTION__ ) ;
		return ;
	}



	// 프로토콜 체크.
	switch( pmsg->Protocol )
	{
	case MP_CHATROOM_ADD_USER_SYN :			Add_User_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_FORCE_ADD_USER_SYN :	Force_Add_User_Syn( dwIndex, pMsg, dwLength ) ;	break ;
	case MP_CHATROOM_DEL_USER_SYN :			Del_User_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_ROOM_SYN :				Room_Syn( dwIndex, pMsg, dwLength ) ;			break ;
	case MP_CHATROOM_CREATE_ROOM_SYN :		Create_Room_Syn( dwIndex, pMsg, dwLength ) ;	break ;
	case MP_CHATROOM_JOIN_ROOM_SYN :		Join_Room_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_OUT_ROOM_SYN :			Out_Room_Syn( dwIndex, pMsg, dwLength ) ;		break ;
	case MP_CHATROOM_CHANGE_OPTION_SYN :	Option_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_CHANGE_OWNER_SYN :		Owner_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_KICK_GUEST_SYN :		Kick_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_REQUEST_FRIEND_SYN :	Friend_Syn( dwIndex, pMsg, dwLength ) ;			break ;	
	case MP_CHATROOM_UPDATE_USERINFO_SYN :	UpdateUserInfo( dwIndex, pMsg, dwLength ) ;		break ;	
	case MP_CHATROOM_SEARCH_FOR_NAME_SYN :	SearchName_Syn( dwIndex, pMsg, dwLength ) ;		break ;	
	case MP_CHATROOM_SEARCH_FOR_TITLE_SYN :	SearchTitle_Syn( dwIndex, pMsg, dwLength ) ;	break ;	
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Add_GuestInfo_Syn
//		 : 
//	DESC : 유저 정보를 등록하는 함수.
//		 :
//		 : 1. 채팅중인 유저인지 체크.
//		 : 2. 유저 등록.
//		 : 3. 검색용 이름 등록.
//		 : 4. 결과 전송.
//		 :
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_USER* pmsg = NULL ;
	pmsg = (MSG_CR_USER*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 채팅중인지 체크.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->user.dwPlayerID ) ;
	if( pUserInfo ) return ;



	// 유저 정보 등록.
	ST_CR_USER* pNewUser = new ST_CR_USER ;
	ASSERT( pNewUser ) ;
	if( !pNewUser ) return ;

	memcpy( pNewUser, &pmsg->user, sizeof(ST_CR_USER) ) ;
	pNewUser->dwConnectionIdx = dwIndex ;

	m_htUser.Add( pNewUser, pNewUser->dwPlayerID ) ;



	// 결과 전송.
	MSGBASE msg ;
	memset(&msg, 0, sizeof(MSGBASE)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_ADD_USER_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Force_Add_User_Syn
//		 : 
//	DESC : 이미 등록되어 있는 유저 정보가 있던말던, 강제로 유저를 등록하고, 채팅방 리스트를 전송한다.
//		 : 
//		 : 1. 등록 된 유저인지 체크.
//		 :    1-1. 채팅중인지 체크.
//		 :	  1-2. 채팅방 받기.
//		 :    1-3. 유저 아웃 처리.
//		 :    1-4. 검색용 이름 삭제.
//		 :    1-5. 유저 정보 삭제.
//		 : 2. 유저 등록.
//		 : 3. 검색용 이름 등록.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Force_Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_USER* pmsg = NULL ;
	pmsg = (MSG_CR_USER*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 유저 받기.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->user.dwPlayerID ) ;
	if( pUserInfo )
	{
		// 방 받기.
		ST_CR_ROOM_SRV* pRoom = NULL ;
		pRoom = GetRoomInfo( pUserInfo->byIdx ) ;

		// 방 있으면,
		if( pRoom )
		{
			// 아웃 처리.
			MSGBASE msg ;
			memset( &msg, 0, sizeof(MSGBASE) ) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_SYN ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			Out_Room_Syn( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
		}

		// 유저 정보 삭제.
		m_htUser.Remove( pUserInfo->dwPlayerID ) ;
	}



	// 유저 등록.
	ST_CR_USER* pNewUser = new ST_CR_USER ;
	ASSERT( pNewUser ) ;
	if( !pNewUser ) return ;

	memcpy( pNewUser, &pmsg->user, sizeof(ST_CR_USER) ) ;
	pNewUser->dwConnectionIdx = dwIndex ;

	m_htUser.Add( pNewUser, pNewUser->dwPlayerID ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Del_GuestInfo_SynDWORD
//		 : 
//	DESC : 캐릭터가 로그 아웃을 했을 때, 유저 정보를 완전히 삭제 시키는 함수.
//		 : 
//		 : 1. 등록 된 유저인지 체크.
//		 :    1-1. 채팅중인지 체크.
//		 :	  1-2. 채팅방 받기.
//		 :    1-3. 유저 아웃 처리.
//		 : 2. 유저 삭제.
//		 : 3. 검색용 이름 삭제.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Del_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 유저 받기.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( pUserInfo )
	{
		// 방 받기.
		ST_CR_ROOM_SRV* pRoom = NULL ;
		pRoom = GetRoomInfo( pUserInfo->byIdx ) ;

		// 방 있으면,
		if( pRoom )
		{
			// 아웃 처리.
			MSGBASE msg ;
			memset( &msg, 0, sizeof(MSGBASE) ) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_SYN ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			Out_Room_Syn( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
		}

		// 검색용 유저 이름 삭제.
		EraseUserName( pUserInfo->name ) ;

		// 유저 정보 삭제.
		m_htUser.Remove( pUserInfo->dwPlayerID ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : RoomList_Syn
//		 : 
//	DESC : 요청이 들어온 페이지에 해당하는 방 정보를 담아서 Agent로 전송한다.
//		 : 
//		 : 1. 리스트 헤더 세팅.
//		 : 2. 채팅방 분류에 따라 방정보를 담는다.
//		 : 3. 페이지 세팅.
//		 : 4. 전송하는 리스트 분류 세팅.
//		 : 5. 결과 전송.
//		 : 
//  DATE : APRIL 22, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 에러 메시지 정의.
	// 0 = err_Empty_Room,
	// 1 = err_Delay_Time,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_CR_ROOMLIST msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_ROOMLIST_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;



	// 방정보를 담을 범위 인덱스 세팅.
	BYTE byIndex = 0 ;
	BYTE byStartIdx = 0 ;
	byStartIdx = pmsg->bData2 * ROOM_COUNT_PER_PAGE ;



	// 리스트 헤더 세팅.
	ST_CR_ROOM_SRV* pInfo = NULL ;
	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		// 방정보 받기.
		pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;
		if( !pInfo ) continue ;

		// 인덱스 유효 체크.
		if( byIndex < byStartIdx )
		{
			++byIndex ;
			continue ;
		}

		// 전송 할 개수 체크.
		if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

		if( pmsg->bData1 != e_RTM_AllLooK )
		{
			if( pInfo->byRoomType != pmsg->bData1-1 ) continue ;
		}

		// 방 정보 담기.
		msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
		msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
		msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
		msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
		msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

		++msg.byCount ;
	}



	// 페이지 세팅.
	msg.byCurPage	= pmsg->bData2 ;
	if( pmsg->bData1 != e_RTM_AllLooK )
	{
		msg.byTotalPage	= GetRoomCountByType( pmsg->bData1-1 ) / ROOM_COUNT_PER_PAGE ;
	}
	else
	{
		msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE);
	}



	// 담은 채팅방 분류 세팅.
	msg.byRoomType	= pmsg->bData1 ;



	// 결과 전송.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Create_Room_Syn
//		 : 
//	DESC : 채팅방 생성 요청이 왔을 때 처리하는 함수.
//		 : 
//		 : 1. 채팅방 개수 체크.
//		 : 2. 등록 된 유저인지 체크.
//		 : 3. 채팅중인지 체크.
//		 : 4. 채팅방 생성.
//		 : 5. 관리 리스트에 방 추가.
//		 : 6. 유저의 채팅방 번호 세팅.
//		 : 7. 결과 전송.
//		 :
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Create_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 에러 메시지 정의.
	// 0 = err_Already_Chatting,
	// 1 = err_RoomCount_Over,
	// 2 = err_User_Is_Not_In_Lobby,
	// 3 = err_Registed_But_Not_Found_Room,
	// 4 = err_Registed_But_Not_Found_User,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_ELEMENT* pmsg = NULL ;
	pmsg = (MSG_CR_ELEMENT*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 채팅방 개수 체크.
	if( m_pRoomList.GetCount() >= MAX_ROOM_COUNT )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CREATE_ROOM_NACK, 1 ) ;
		return ;
	}



	// 등록 된 유저인지 체크.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CREATE_ROOM_NACK, 2 ) ;
		return ;
	}



	// 채팅중인지 체크.
	if( pUserInfo->byIdx != 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CREATE_ROOM_NACK, 0 ) ;
		return ;
	}



	// 채팅방 생성.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = new ST_CR_ROOM_SRV ;

	ASSERT( pRoom ) ;

	if( !pRoom )
	{
		Throw_Error( "Failed to create chatroom !!", __FUNCTION__ ) ;
		return ;
	}



	// 기본 방 정보 세팅.
	pRoom->byIdx			= GetEmptyRoomIdx() ;
	pRoom->dwOwnerIdx		= pmsg->dwObjectID ;

	pRoom->bySecretMode		= pmsg->bySecretMode ;
	pRoom->byRoomType		= pmsg->byRoomType ;
	pRoom->byCurGuestCount	= 1 ;

	SafeStrCpy( pRoom->code, pmsg->code, SECRET_CODE_SIZE ) ;



	// 최대 참여 인원 세팅.
	BYTE byTotalGuest = e_GC_20 ;

	switch( pmsg->byTotalGuestCount )
	{
	case e_GC_20 :	byTotalGuest = e_GC_20 ;	break ;
	case e_GC_15 :	byTotalGuest = e_GC_15 ;	break ;
	case e_GC_10 :	byTotalGuest = e_GC_10 ;	break ;
	case e_GC_5 :	byTotalGuest = e_GC_5 ;		break ;
	default :									break ;
	}

	pRoom->byTotalGuestCount	= byTotalGuest ;



	// 방 제목 세팅.
	SafeStrCpy( pRoom->title, pmsg->title, TITLE_SIZE ) ;



	// 참여자 아이디 세팅.
	pRoom->dwUser[ 0 ] = pUserInfo->dwPlayerID ;



	// 관리 리스트에 방 추가.
	AddRoom( pRoom ) ;



	// 유저의 채팅방 번호 세팅.
	pUserInfo->byIdx = pRoom->byIdx ;



	// 검색용 이름 등록.
	InsertUserName( pUserInfo->name ) ;



	// 검색용 참여 정보 세팅.
	UpdateNameRoomIdx( pUserInfo->name, pRoom->byIdx ) ;



	// 결과 전송.
	MSG_CR_ROOMINFO msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMINFO) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CREATE_ROOM_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.room		= *pRoom ;
	msg.user[ 0 ]	= *pUserInfo ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMINFO) ) ;



	// 다른 유저들에게 방이 변경되었음을 공지한다.
	UpdateInfo_Created_Room( pRoom ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Join_Room_Syn
//		 : 
//	DESC : 채팅방 참여 요청이 들어왔을 때 처리하는 함수.
//		 : 
//		 : 1. 등록 된 유저인지 체크.
//		 : 2. 채팅중인지 체크.
//		 : 3. 채팅방 받기.
//		 : 4. 비밀 번호 체크.
//		 : 5. 채팅방의 수용 인원 체크.
//		 : 6. 현재 참여자 수 체크.
//		 : 7. 채팅방의 참여자 수 증가.
//		 : 8. 유저의 채팅방 인덱스 세팅.
//		 : 9. 참여 성공 메시지 처리.
//		 :10. 참여 공지 메시지 처리.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Join_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 에러 메시지 정의
    // 0 = err_Already_Chatting,
	// 1 = err_Invalid_Room_Info,
	// 2 = err_Invalid_Secret_Code,
	// 3 = err_Guest_Count_Is_Over,
	// 4 = err_Invalid_User_Info,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_JOIN_SYN* pmsg = NULL ;
	pmsg = (MSG_CR_JOIN_SYN*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 등록 된 유저인지 체크.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 4 ) ;
		return ;
	}



	// 채팅중인지 체크.
	if( pUserInfo->byIdx != 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 0 ) ;
		return ;
	}



	// 채팅방 받기.
	if( pmsg->byRoomIdx == 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 1 ) ;
		return ;
	}

	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pmsg->byRoomIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 1 ) ;
		return ;
	}



	// 비밀 번호 체크.
	if( pRoom->bySecretMode == e_RM_Close )
	{
		if( strcmp( pRoom->code, pmsg->code) != 0 )
		{
			SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 2 ) ;
			return ;
		}
	}



	// 채팅방의 수용 인원 체크.
	BYTE byTotalCount = 0 ;
	switch( pRoom->byTotalGuestCount )
	{
	case e_GC_20 :	byTotalCount = 20 ; break ;
	case e_GC_15 :	byTotalCount = 15 ; break ;
	case e_GC_10 :	byTotalCount = 10 ; break ;
	case e_GC_5 :	byTotalCount =  5 ; break ;
	default :							break ;
	}



	// 현재 참여자 수 체크.
	if( pRoom->byCurGuestCount >= byTotalCount )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_JOIN_ROOM_NACK, 3 ) ;
		return ;
	}



	// 참여자 인덱스 세팅.
	pRoom->dwUser[ pRoom->byCurGuestCount ] = pUserInfo->dwPlayerID ;



	// 채팅방의 참여자 수 증가.
	++pRoom->byCurGuestCount ;



	// 유저의 채팅방 인덱스 세팅.
	pUserInfo->byIdx = pRoom->byIdx ;



	// 검색용 이름 등록.
	InsertUserName( pUserInfo->name ) ;



	// 유저가 참여하는 방 인덱스 업데이트.
	UpdateNameRoomIdx( pUserInfo->name, pRoom->byIdx ) ;



	// 참여 성공 메시지 처리.
	MSG_CR_ROOMINFO msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMINFO) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_JOIN_ROOM_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.room		= *pRoom ;



	// 참여자 정보 담기.
	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		msg.user[ msg.byCount ] = *pMemberInfo ;

		++msg.byCount ;
	}



	// 결과 전송.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMINFO) ) ;



	// 참여 공지 메시지 처리.
	MSG_CR_JOIN_NOTICE msg2 ;
	memset( &msg2, 0, sizeof(MSG_CR_JOIN_NOTICE) ) ;

	msg2.Category	= MP_CHATROOM ;
	msg2.Protocol	= MP_CHATROOM_JOIN_ROOM_NOTICE ;
	msg2.dwObjectID	= pmsg->dwObjectID ;

	msg2.user		= *pUserInfo ;



	// 참여자 인덱스 담기.
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		if( pMemberInfo->dwPlayerID == pUserInfo->dwPlayerID ) continue ;

		msg2.dwUser[ msg2.byCount ] = pMemberInfo->dwPlayerID ;

		++msg2.byCount ;
	}

	// 결과 전송.
	g_Network.Send2Server( dwIndex, (char*)&msg2, sizeof(MSG_CR_JOIN_NOTICE) ) ;



	// 다른 유저들에게 방이 변경되었음을 공지한다.
	UpdateInfo_Changed_Current_GuestCount( pRoom ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Syn
//		 : 
//	DESC : 유저가 채팅방을 나가는 처리를 하는 함수.
//		 : 
//		 : 1. 등록 된 유저인지 체크.
//		 : 2. 방정보 받기.
//		 : 3. 방에 남아있는 유저 수 체크.
//		 :	  3-1. 방 삭제.
//		 :	  3-2. 유저의 방 인덱스 삭제.
//		 :	  3-3. 결과 전송.
//		 : 4. 마지막이 아니면,
//		 :    4-1. 채팅방의 참여중인 유저수를 감소시킨다.
//		 :	  4-2. 나가는 유저의 방 인덱스를 초기화.
//		 :	  4-3. 나가기 성공 메시지 처리.
//		 :    4-4. 결과를 공지한다.
//		 :	  4-5. 방장이 나갔다면, 방장 바뀜 공지.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 에러 메시지 정의.
	// 0 = err_CanNot_Found_Room,
	// 1 = err_CanNot_Found_Outer,



	// 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 유저 정보 받기.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_OUT_ROOM_NACK, 1 ) ;
		return ;
	}



	// 방정보 받기.
	if( pUserInfo->byIdx == 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_OUT_ROOM_NACK, 0 ) ;
		return ;
	}

	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pUserInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_OUT_ROOM_NACK, 0 ) ;
		return ;
	}



	// 유저의 방 인덱스 삭제.
	pUserInfo->byIdx = 0 ;



	// 검색용 유저 이름 삭제.
	EraseUserName( pUserInfo->name ) ;



	// 방에 남아있는 유저 수 체크.
	if( pRoom->byCurGuestCount == 1 )
	{
		// 다른 유저들에게 방이 삭제되었음을 공지한다.
		UpdateInfo_Deleted_Room( pRoom ) ;

		// 방 삭제.
		DeleteRoom( pRoom->byIdx ) ;

		// 방이 비었다고, 그냥 에러 메시지 보내지 않고, 클라이언트의 채팅방 리스트를 모두 비우도록 하자.
		if( m_pRoomList.GetCount() == 0 )
		{
			MSGBASE msg ;
			memset(&msg, 0, sizeof(MSGBASE)) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_EMPTYROOM ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSGBASE) ) ;
			return ;
		}
		else
		{
			// 전송 메시지 세팅.
			MSG_CR_ROOMLIST msg ;
			memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_OUT_ROOM_LAST_MAN ;
			msg.dwObjectID	= pmsg->dwObjectID ;

			// 리스트 헤더 세팅.
			ST_CR_ROOM_SRV* pInfo ;
			PTRLISTPOS pos = NULL ;

			pos = m_pRoomList.GetHeadPosition() ;

			// 방정보를 담을 범위 인덱스 세팅.
			BYTE byStartIdx = 0 ;
			byStartIdx = byStartIdx * ROOM_COUNT_PER_PAGE ;

			BYTE byIndex = 0 ;

			// 리스트 검색.
			while( pos )
			{
				// 방정보 받기.
				pInfo = NULL ;
				pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

				if( !pInfo ) continue ;

				// 인덱스 유효 체크.
				if( byIndex < byStartIdx ) continue ;

				// 전송 할 개수 체크.
				if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

				// 방 정보 담기.
				msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
				msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
				msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
				msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
				msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

				SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

				++msg.byCount ;
			}

			// 페이지 세팅.
			msg.byCurPage	= 0 ;
			msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE) ;

			// 담은 채팅방 분류 세팅.
			msg.byRoomType	= e_RTM_AllLooK ;

			// 결과 전송.
			g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;
		}
	}
	else
	{
		// 방장이 후임 방장 위임없이 튕기거나, 채팅을 종료했는지 체크.
		BYTE byChangedOwner = FALSE ;
		if( pRoom->dwOwnerIdx == pUserInfo->dwPlayerID ) byChangedOwner = TRUE ;

		// 유저 인덱스를 임시 보관한다.
		BYTE byCheckCount = 0 ;
		DWORD dwPlayerID[ MAX_USER_PER_ROOM ] = {0, } ;
		for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
		{
			if( pRoom->dwUser[ count ] == pUserInfo->dwPlayerID ) continue ;

			dwPlayerID[ byCheckCount ] = pRoom->dwUser[ count ] ;
			++byCheckCount ;
		}

		// 채팅방의 참여자 수 감소.
		--pRoom->byCurGuestCount ;

		// 유저 인덱스 복원
		for( BYTE count = 0 ; count  < pRoom->byCurGuestCount ; ++count )
		{
			pRoom->dwUser[ count ] = dwPlayerID[ count ] ;
		}

		// 후임 방장 결정.
		if( byChangedOwner )
		{
			pRoom->dwOwnerIdx = dwPlayerID[ 0 ] ;
		}

		// 다른 유저들에게 방이 변경되었음을 공지한다.
		UpdateInfo_Changed_Current_GuestCount( pRoom ) ;


		// 전송 메시지 세팅.
		MSG_CR_ROOMLIST msg ;
		memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_OUT_ROOM_ACK ;
		msg.dwObjectID	= pmsg->dwObjectID ;

		// 리스트 헤더 세팅.
		ST_CR_ROOM_SRV* pInfo ;
		PTRLISTPOS pos = NULL ;

		pos = m_pRoomList.GetHeadPosition() ;

		// 방정보를 담을 범위 인덱스 세팅.
		BYTE byStartIdx = 0 ;
		byStartIdx = byStartIdx * ROOM_COUNT_PER_PAGE ;

		BYTE byIndex = 0 ;

		// 리스트 검색.
		while( pos )
		{
			// 방정보 받기.
			pInfo = NULL ;
			pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

			if( !pInfo ) continue ;

			// 인덱스 유효 체크.
			if( byIndex < byStartIdx ) continue ;

			// 전송 할 개수 체크.
			if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

			// 방 정보 담기.
			msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
			msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
			msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
			msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
			msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

			SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

			++msg.byCount ;
		}

		// 페이지 세팅.
		msg.byCurPage	= 0 ;
		msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE) ;

		// 담은 채팅방 분류 세팅.
		msg.byRoomType	= e_RTM_AllLooK ;

		// 결과 전송.
		g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;



		// 참여자 나감 공지 처리.
		MSG_CR_IDNAME msg2 ;
		memset( &msg2, 0, sizeof(MSG_CR_IDNAME) ) ;

		msg2.Category	= MP_CHATROOM ;
		msg2.Protocol	= MP_CHATROOM_OUT_ROOM_NOTICE ;
		msg2.dwObjectID	= pmsg->dwObjectID ;

		msg2.dwID		= pUserInfo->dwPlayerID ;

		SafeStrCpy( msg2.name, pUserInfo->name, MAX_NAME_LENGTH + 1 ) ;

		ST_CR_USER* pMemberInfo = NULL ;
		for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
		{
			pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
			if( !pMemberInfo ) continue ;

			msg2.dwUser[ msg2.byCount ] = pMemberInfo->dwPlayerID ;

			++msg2.byCount ;
		}

		g_Network.Send2Server( dwIndex, (char*)&msg2, sizeof(MSG_CR_IDNAME) ) ;



		// 방장 바뀜 공지 처리.
		if( !byChangedOwner ) return ;

		MSG_CR_IDNAME msg3 ;
		memset( &msg3, 0, sizeof(MSG_CR_IDNAME) ) ;

		msg3.Category	= MP_CHATROOM ;
		msg3.Protocol	= MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE ;
		msg3.dwObjectID	= pmsg->dwObjectID ;

		pMemberInfo = NULL ;
		for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
		{
			pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
			if( !pMemberInfo ) continue ;

			if( pMemberInfo->dwPlayerID == pRoom->dwOwnerIdx )
			{
				msg3.dwID = pMemberInfo->dwPlayerID ;
				SafeStrCpy( msg3.name, pMemberInfo->name, MAX_NAME_LENGTH + 1 ) ;
			}

			msg3.dwUser[ msg3.byCount ] = pMemberInfo->dwPlayerID ;

			++msg3.byCount ;
		}

		g_Network.Send2Server( dwIndex, (char*)&msg3, sizeof(MSG_CR_IDNAME) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Option_Syn
//		 : 
//	DESC : The function to change option of selected room.
//		 : 
//		 : 1. 등록 된 유저인지 체크.
//		 : 2. 방 정보 받기.
//		 : 3. 방장인지 검사.
//		 : 4. 방 옵션 변경.
//		 : 5. 결과 공지.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Option_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) 
{
	// 에러 메시지 정의.
	// 0 = err_You_Are_Not_In_ChatRoom,
	// 1 = err_CanNot_Found_Room,
	// 2 = err_You_Are_Not_Owner,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_ELEMENT* pmsg = NULL ;
	pmsg = (MSG_CR_ELEMENT*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 등록 된 유저인지 체크.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OPTION_NACK, 0 ) ;
		return ;
	}



	// 방정보 받기.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pUserInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OPTION_NACK, 1 ) ;
		return ;
	}



    // 방장인지 검사.
	if( pUserInfo->dwPlayerID != pRoom->dwOwnerIdx )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OPTION_NACK, 2 ) ;
		return ;
	}


	
	// 채팅방 변경 내용 공지 처리.
	// 다른 유저들에게 방이 변경되었음을 공지한다.
	BYTE byChangedSecretMode = FALSE ;
	if( pRoom->bySecretMode != pmsg->bySecretMode )
	{
		byChangedSecretMode = TRUE ;
	}

	BYTE byChangedRoomType = FALSE ;
	if( pRoom->byRoomType != pmsg->byRoomType )
	{
		byChangedRoomType = TRUE ;
	}

	BYTE byChangedGuestCount = FALSE ;
	if( pRoom->byTotalGuestCount != pmsg->byTotalGuestCount )
	{
		byChangedGuestCount = TRUE ;
	}

	BYTE byChangedTitle = FALSE ;
	if( strcmp( pRoom->title, pmsg->title ) != 0 )
	{
		byChangedTitle = TRUE ;
	}



	// 방 옵션 변경.
	pRoom->bySecretMode			= pmsg->bySecretMode ;
	pRoom->byRoomType			= pmsg->byRoomType ;
	pRoom->byTotalGuestCount	= pmsg->byTotalGuestCount ;

	SafeStrCpy( pRoom->code, pmsg->code, SECRET_CODE_SIZE ) ;

	SafeStrCpy( pRoom->title, pmsg->title, TITLE_SIZE ) ;



	// 검색용 방 정보 업데이트.
	UpdateSearchRoomInfo( pRoom ) ;



	// 결과 공지.
	MSG_CR_ROOM_NOTICE msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOM_NOTICE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CHANGE_OPTION_NOTICE ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.room		= *pRoom ;

	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		msg.dwUser[ msg.byCount ] = pMemberInfo->dwPlayerID ;

		++msg.byCount ;
	}

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOM_NOTICE) ) ;



	// 변경 된 정보를 공지 처리.
	if( byChangedSecretMode )
	{
		UpdateInfo_Changed_SecretMode( pRoom ) ;
	}
	
	if( byChangedRoomType )
	{
		UpdateInfo_Changed_RoomType( pRoom ) ;
	}
	
	if( byChangedGuestCount )
	{
		UpdateInfo_Changed_Total_GuestCount( pRoom ) ;
	}
	
	if( byChangedTitle )
	{
		UpdateInfo_Changed_Title( pRoom ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Owner_Syn
//		 : 
//	DESC : The function to change owner of chatroom.
//		 : 
//		 : 1. 현재 방장 받기.
//		 : 2. 후임 방장 받기.
//		 : 3. 방정보 받기.
//		 : 4. 현재 방장이 방장인지 체크.
//		 : 5. 현재 참여중인 유저 수 체크.
//		 : 6. 채팅방의 방장인덱스 변경.
//		 : 7. 결과 공지.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Owner_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// 에러 메시지 정의.
	// 0 = err_Invalid_Current_Owner,
	// 1 = err_Invalid_Next_Owner,
	// 2 = err_CanNot_Found_RoomInfo,
	// 3 = err_You_Are_Not_Owner,
	// 4 = err_You_Alone,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 현재 방장 받기.
	ST_CR_USER* pOwnerInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pOwnerInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 0 ) ;
		return ;
	}



	// 후임 방장 받기.
	ST_CR_USER* pNewOwnerInfo = m_htUser.GetData( pmsg->dwData ) ;
	if( !pNewOwnerInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 1 ) ;
		return ;
	}



	// 방정보 받기.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pOwnerInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 2 ) ;
		return ;
	}



	// 현재 방장이 방장인지 체크.
	if( pRoom->dwOwnerIdx != pOwnerInfo->dwPlayerID )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 3 ) ;
		return ;
	}



	// 현재 참여중인 유저 수 체크.
	if( pRoom->byCurGuestCount <= 1 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_CHANGE_OWNER_NACK, 4 ) ;
		return ;
	}



	// 채팅방의 방장 아이디 변경.
	pRoom->dwOwnerIdx = pNewOwnerInfo->dwPlayerID ;



	// 결과 공지.
	MSG_CR_IDNAME msg ;
	memset( &msg, 0, sizeof(MSG_CR_IDNAME) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CHANGE_OWNER_NOTICE ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.dwID		= pNewOwnerInfo->dwPlayerID ;

	SafeStrCpy( msg.name, pNewOwnerInfo->name, MAX_NAME_LENGTH + 1 ) ;

	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[count] ) ;
		if( !pMemberInfo ) continue ;

		msg.dwUser[ msg.byCount ] = pMemberInfo->dwPlayerID ;

		++msg.byCount ;
	}



	// 결과 전송.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_IDNAME) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Kick_Syn
//		 : 
//	DESC : The function to kick selected user from chatroom.
//		 : 
//		 : 1. 방장 받기.
//		 : 2. 방정보 받기.
//		 : 3. 방장인지 체크.
//		 : 4. 강제 퇴장 대상 받기.
//		 : 5. 대상의 방 인덱스 초기화.
//		 : 6. 채팅방의 접속 유저수 감소.
//		 : 7. 강퇴 처리.
//		 : 8. 강퇴 공지 처리.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Kick_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// 에러 메시지 정의.
	// 0 = err_CanNot_Found_RoomInfo,
	// 1 = err_Invalid_OwnerInfo,
	// 2 = err_You_Are_Not_Owner,
	// 3 = err_Invalid_Target,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 방장 받기.
	ST_CR_USER* pOwnerInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pOwnerInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 1 ) ;
		return ;
	}



	// 방정보 받기.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pOwnerInfo->byIdx ) ;
	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 0 ) ;
		return ;
	}



	// 방장인지 체크.
	if( pOwnerInfo->dwPlayerID != pRoom->dwOwnerIdx )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 2 ) ;
		return ;
	}



	// 강제 퇴장 대상 받기.
	ST_CR_USER* pPurgeeInfo = m_htUser.GetData( pmsg->dwData ) ;
	if( !pPurgeeInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_KICK_GUEST_NACK, 3 ) ;
		return ;
	}



	// 대상의 방인덱스 초기화.
	pPurgeeInfo->byIdx = 0 ;



	// 검색용 유저 이름 삭제.
	EraseUserName( pPurgeeInfo->name ) ;



	// 유저 인덱스를 임시 보관한다.
	BYTE byCheckCount = 0 ;
	DWORD dwPlayerID[ MAX_USER_PER_ROOM ] = {0, } ;
	for( BYTE count = 0 ; count < pRoom->byCurGuestCount ; ++count )
	{
		if( pRoom->dwUser[ count ] == pPurgeeInfo->dwPlayerID ) continue ;

		dwPlayerID[ byCheckCount ] = pRoom->dwUser[ count ] ;
		++byCheckCount ;
	}



	// 채팅방의 참여자 수 감소.
	--pRoom->byCurGuestCount ;



	// 유저 인덱스 복원
	for( BYTE count = 0 ; count  < pRoom->byCurGuestCount ; ++count )
	{
		pRoom->dwUser[ count ] = dwPlayerID[ count ] ;
	}



	// 전송 메시지 세팅.
	MSG_CR_KICK_ACK msg ;
	memset( &msg, 0, sizeof(MSG_CR_KICK_ACK) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_KICK_GUEST_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.dwKickPlayer = pmsg->dwData ;



	// 방정보를 담을 범위 인덱스 세팅.
	BYTE byIndex = 0 ;
	BYTE byStartIdx = 0 ;
	byStartIdx = byStartIdx * ROOM_COUNT_PER_PAGE ;



	// 리스트 헤더 세팅.
	ST_CR_ROOM_SRV* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_pRoomList.GetHeadPosition() ;

	while( pos )
	{
		// 방정보 받기.
		pInfo = NULL ;
		pInfo = (ST_CR_ROOM_SRV*)m_pRoomList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		// 인덱스 유효 체크.
		if( byIndex < byStartIdx ) continue ;

		// 전송 할 개수 체크.
		if( msg.byCount >= ROOM_COUNT_PER_PAGE ) break ;

		// 방 정보 담기.
		msg.room[ msg.byCount ].byIdx				= pInfo->byIdx ;
		msg.room[ msg.byCount ].bySecretMode		= pInfo->bySecretMode ;
		msg.room[ msg.byCount ].byRoomType			= pInfo->byRoomType ;
		msg.room[ msg.byCount ].byCurGuestCount		= pInfo->byCurGuestCount ;
		msg.room[ msg.byCount ].byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy( msg.room[ msg.byCount ].title, pInfo->title, TITLE_SIZE ) ;

		++msg.byCount ;
	}



	// 페이지 세팅.
	msg.byCurPage	= 0 ;
	msg.byTotalPage	= (BYTE)(m_pRoomList.GetCount() / ROOM_COUNT_PER_PAGE) ;



	// 담은 채팅방 분류 세팅.
	msg.byRoomType	= e_RTM_AllLooK ;



	// 결과 전송.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_KICK_ACK) ) ;



	// 강퇴 공지 처리.
	MSG_CR_IDNAME msg2 ;
	memset( &msg2, 0, sizeof(MSG_CR_IDNAME) ) ;

	msg2.Category	= MP_CHATROOM ;
	msg2.Protocol	= MP_CHATROOM_KICK_GUEST_NOTICE ;
	msg2.dwObjectID	= pmsg->dwObjectID ;



	// 강퇴 대상자 정보 세팅.
	msg2.dwID		= pPurgeeInfo->dwPlayerID ;
	SafeStrCpy( msg2.name, pPurgeeInfo->name, MAX_NAME_LENGTH + 1 ) ;



	// 공지 대상자 담기.
	ST_CR_USER* pMemberInfo = NULL ;
	for( BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count )				
	{
		pMemberInfo = m_htUser.GetData( pRoom->dwUser[ count ] ) ;
		if( !pMemberInfo ) continue ;

		// 공지 대상에서 강퇴자는 뺀다.
		if( pMemberInfo->dwPlayerID == pmsg->dwData ) continue ;	

		msg2.dwUser[ msg2.byCount ] = pMemberInfo->dwPlayerID ;

		++msg2.byCount ;
	}



	// 결과 전송.
	g_Network.Send2Server( dwIndex, (char*)&msg2, sizeof(MSG_CR_IDNAME) ) ;



	// 다른 유저들에게 방이 변경되었음을 공지한다.
	UpdateInfo_Changed_Current_GuestCount( pRoom ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Friend_Syn
//		 : 
//	DESC : The function to check user info for add friend.
//		 : 
//		 : 1. 요청자 받기.
//		 : 2. 대상 받기.
//		 : 3. 같은 채팅방인지 체크.
//		 : 4. 결과 전송.
//		 : 
//  DATE : APRIL 4, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Friend_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength )
{
	// 에러 메시지 정의.
	// 0 = err_Invalid_Requester_Info,
	// 1 = err_Invalid_Target_Info.
	// 2 = err_You_Are_Not_In_SameRoom,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_DWORD* pmsg = NULL ;
	pmsg = (MSG_DWORD*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 요청자 받기.
	ST_CR_USER* pApplicantInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pApplicantInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_REQUEST_FRIEND_NACK, 0 ) ;
		return ;
	}



	// 대상 받기.
	ST_CR_USER* pAcceptantInfo = m_htUser.GetData( pmsg->dwData ) ;
	if( !pAcceptantInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_REQUEST_FRIEND_NACK, 1 ) ;
		return ;
	}



	// 같은 채팅방인지 체크.
	if( pApplicantInfo->byIdx != pAcceptantInfo->byIdx )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_REQUEST_FRIEND_NACK, 2 ) ;
		return ;
	}



	// 결과 메시지 전송.
	MSG_NAME msg ;
	memset( &msg, 0, sizeof(MSG_NAME) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_REQUEST_FRIEND_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	SafeStrCpy( msg.Name, pAcceptantInfo->name, MAX_NAME_LENGTH+1 ) ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_NAME) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateUserInfo
//	DESC : 유저의 정보가 업데이트 되었을 때, 정보를 업데이트 하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateUserInfo( DWORD dwIndex, char*pMsg, DWORD dwLength )
{
	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_USER* pmsg = NULL ;
	pmsg = (MSG_CR_USER*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 유저 정보 업데이트.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( pUserInfo )
	{
		// 검색용 이름 체크.
		if( strcmp( pUserInfo->name, pmsg->user.name ) != 0 )
		{
			EraseUserName( pUserInfo->name ) ;

			pUserInfo->byLevel	= pmsg->user.byLevel ;
			pUserInfo->byMapNum	= pmsg->user.byMapNum ;

			SafeStrCpy( pUserInfo->name, pmsg->user.name, MAX_NAME_LENGTH + 1 ) ;

			InsertUserName( pUserInfo->name ) ;
			UpdateNameRoomIdx( pUserInfo->name, pUserInfo->byIdx ) ;
		}
		else
		{
			pUserInfo->byLevel  = pmsg->user.byLevel ;
			pUserInfo->byMapNum	= pmsg->user.byMapNum ;

			SafeStrCpy( pUserInfo->name, pmsg->user.name, MAX_NAME_LENGTH + 1 ) ;

			UpdateNameRoomIdx( pUserInfo->name, pUserInfo->byIdx ) ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchName_Syn
//	DESC : 이름으로 채팅방 정보를 검색하는 함수.
//  DATE : APRIL 23, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchName_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength ) 
{
	// 에러 메시지 정의.
	// 0 = err_CanNot_Found_User,
	// 1 = err_CanNot_Found_Name,
	// 2 = err_ChatRoom_Destroyed,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_SEARCH_NAME* pmsg = NULL ;
	pmsg = (MSG_CR_SEARCH_NAME*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 유저 검색.
	ST_CR_USER* pUserInfo = m_htUser.GetData( pmsg->dwObjectID ) ;
	if( !pUserInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_NAME_NACK, 0 ) ;
		return ;
	}



	// 이름 검색.
	const char* name = pmsg->name ;
	ST_SEARCH_NAME* pNameInfo = m_htSearchName.GetData( GetHashCode( name ) ) ;
	if( !pNameInfo )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_NAME_NACK, 1 ) ;
		return ;
	}



	// 방 검색.
	ST_CR_ROOM_SRV* pRoom = NULL ;
	pRoom = GetRoomInfo( pNameInfo->byRoomIdx ) ;

	if( !pRoom )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_NAME_NACK, 2 ) ;
		return ;
	}



	// 결과 전송.
	MSG_CR_SEARCH_NAME msg ;
	memset( &msg, 0, sizeof(MSG_CR_SEARCH_NAME) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_SEARCH_FOR_NAME_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;

	msg.byRoomIdx	= pRoom->byIdx ;

	SafeStrCpy( msg.name, pmsg->name, MAX_NAME_LENGTH + 1 ) ;

	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_SEARCH_NAME) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchTitle_Syn
//	DESC : 제목으로 채팅방 정보를 검색하는 함수.
//  DATE : APRIL 23, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchTitle_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength )
{
	// 에러 메시지 정의.
	// 0 = err_CanNot_Found_Room,



	// 함수 파라메터 체크.
	ASSERT( pMsg ) ;

	if( !pMsg )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 원본 메시지 변환.
	MSG_CR_SEARCH_TITLE* pmsg = NULL ;
	pmsg = (MSG_CR_SEARCH_TITLE*)pMsg ;

	ASSERT( pmsg ) ;

	if( !pmsg )
	{
		Throw_Error( err_FCMTO, __FUNCTION__ ) ;
		return ;
	}



	// 결과 메시지 세팅.
	MSG_CR_ROOMLIST msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMLIST) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_SEARCH_FOR_TITLE_ACK ;
	msg.dwObjectID	= pmsg->dwObjectID ;



	// 제목으로 채팅방 검색.
	ST_CR_ROOM_CLT* pInfo ;

	PTRLISTPOS pos = NULL ;
	pos = m_pTitleList.GetHeadPosition() ;

	BYTE byCount = 0 ;

	while( pos )
	{
		pInfo = NULL ;
		pInfo = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		if( strstr( pInfo->title, pmsg->title ) == NULL ) continue ;

		++byCount ;

		if( msg.byCount >= MAX_USER_PER_ROOM ) continue ;

		msg.room[ msg.byCount ] = *pInfo ;

		++msg.byCount ;
	}



	// 검색 된 개수 체크.
	if( msg.byCount == 0 )
	{
		SendErrors( dwIndex, pmsg->dwObjectID, MP_CHATROOM_SEARCH_FOR_TITLE_NACK, 0 ) ;
		return ;
	}



	// 검색모드로 세팅.
	msg.byRoomType = e_RTM_Searched ;



	// 페이지 세팅.
	msg.byCurPage	= 0 ;
	msg.byTotalPage = byCount / ROOM_COUNT_PER_PAGE ;



	// 결과 전송.
	g_Network.Send2Server( dwIndex, (char*)&msg, sizeof(MSG_CR_ROOMLIST) ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetEmptyRoomIdx
//	DESC : 채팅방을 관리하는 맵에서, 앞에서 부터 비어있는 방 인덱스를 반환하는 함수. 0번은 제외.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::GetEmptyRoomIdx()
{
	for( BYTE count = 1 ; count < MAX_ROOM_COUNT ; ++count )
	{
		if( m_byRoomIdx[ count ] == 0 ) return count ;
	}

	ASSERT( TRUE ) ;

	Throw_Error( "Cant found empty room index!!", __FUNCTION__ ) ;

	return 0 ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetHashCode
//	DESC : 인자로 주어진 스트링을 해쉬맵에 사용할 코드 값으로 변환하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
DWORD CChatRoomMgr::GetHashCode( const char* pName )
{
	// 함수 파라메터 체크.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return 5381 ;
	}

	DWORD dwCount	= 0 ;
	DWORD dwLength	= 0 ;
	DWORD dwCh		= 0 ;
	DWORD dwResult	= 0 ;

	dwLength = strlen( pName ) ;
	dwResult = 5381 ;

	for( dwCount = 0 ; dwCount < dwLength ; dwCount++ )
	{
		dwCh = (DWORD)pName[ dwCount ] ;
		dwResult = ( (dwResult << 5) + dwResult ) + dwCh ; // hash * 33 + ch
	}	  

	return dwResult ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : InsertUserName
//	DESC : 검색용 해쉬맵에 유저 이름을 추가하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::InsertUserName( char* pName ) 
{
	// 함수 파라메터 체크.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 정보를 추가한다.
	const char* name = pName ;
	DWORD dwNameCode = GetHashCode( name ) ;

	ST_SEARCH_NAME* pNameInfo = new ST_SEARCH_NAME ;
	ASSERT( pNameInfo ) ;

	if( !pNameInfo ) return ;

	ZeroMemory( pNameInfo, sizeof(pNameInfo) );
	SafeStrCpy( pNameInfo->name, name, MAX_NAME_LENGTH + 1 ) ;

	m_htSearchName.Add( pNameInfo, dwNameCode ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : EraseUserName
//	DESC : 검색용 해쉬맵의 유저 이름을 삭제하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::EraseUserName( char* pName )
{
	// 함수 파라메터 체크.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 검색을 한다.
	const char* name = pName ;
	DWORD dwNameCode = GetHashCode( name ) ;

	ST_SEARCH_NAME* pNameInfo = m_htSearchName.GetData( dwNameCode ) ;
	if( pNameInfo )
	{
		delete pNameInfo ;
		pNameInfo = NULL ;
	}

	m_htSearchName.Remove( dwNameCode ) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateNameRoomIdx
//	DESC : 검색용 해쉬맵의 유저가 참여한 방 번호를 업데이트 하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateNameRoomIdx( char* pName, BYTE byRoomIdx )
{
	// 함수 파라메터 체크.
	ASSERT( pName ) ;

	if( !pName || strlen( pName ) == 0 )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 방 인덱스를 업데이트 한다.
	const char* name = pName ;
	DWORD dwNameCode = GetHashCode( name ) ;

	ST_SEARCH_NAME* pNameInfo = m_htSearchName.GetData( dwNameCode ) ;
	if( pNameInfo )
	{
		pNameInfo->byRoomIdx = byRoomIdx ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateSearchRoomInfo
//	DESC : 검색용 방의 정보를 업데이트 하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateSearchRoomInfo( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 검색 용 방 업데이트.
	ST_CR_ROOM_CLT* pTitle = NULL ;
	PTRLISTPOS pos = NULL ;

	pos = m_pTitleList.GetHeadPosition() ;

	while( pos )
	{
		pTitle = NULL ;
		pTitle = (ST_CR_ROOM_CLT*)m_pTitleList.GetNext( pos ) ;

		if( !pInfo ) continue ;

		if( pInfo->byIdx != pTitle->byIdx ) continue ;

		pTitle->byIdx				= pInfo->byIdx ;
		pTitle->bySecretMode		= pInfo->bySecretMode ;
		pTitle->byRoomType			= pInfo->byRoomType ;
		pTitle->byCurGuestCount		= pInfo->byCurGuestCount ;
		pTitle->byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy( pTitle->title, pInfo->title, TITLE_SIZE ) ;

		return ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Created_Room
//	DESC : 서버 내, 채팅방이 추가되었음을 클라이언트에 알리는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Created_Room( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 채팅 참여중인 유저가 있는지 확인한다.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_CR_ROOMINFO_TO_NOTICE msg ;
	memset( &msg, 0, sizeof(MSG_CR_ROOMINFO_TO_NOTICE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_CREATED_ROOM ;

	msg.room		= *pInfo ;	



	// 유저에게 메시지 전송.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_CR_ROOMINFO_TO_NOTICE) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Deleted_Room
//	DESC : 서버 내, 채팅방이 삭제되었음을 클라이언트에 알리는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Deleted_Room( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 채팅 참여중인 유저가 있는지 확인한다.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_BYTE msg ;
	memset( &msg, 0, sizeof(MSG_BYTE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_DELETED_ROOM ;

	msg.bData		= pInfo->byIdx ;



	// 유저에게 메시지 전송.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_SecretMode
//	DESC : 서버 내, 채팅방의 공개/비공개가 변경되었음을 공지하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_SecretMode( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 채팅 참여중인 유저가 있는지 확인한다.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_SECRETMODE ;

	msg.bData1		= pInfo->byIdx ;
	msg.bData2		= pInfo->bySecretMode ;	

	

	// 유저에게 메시지 전송.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_RoomType
//	DESC : 서버 내, 채팅방분류가 변경되었음을 공지하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_RoomType( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 채팅 참여중인 유저가 있는지 확인한다.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_ROOMTYPE ;

	msg.bData1		= pInfo->byIdx ;
	msg.bData2		= pInfo->byRoomType ;	

	

	// 유저에게 메시지 전송.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_Title
//	DESC : 서버 내, 채팅방의 제목이 변경되었음을 공지하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_Title( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 채팅 참여중인 유저가 있는지 확인한다.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_CR_SEARCH_TITLE msg ;
	memset( &msg, 0, sizeof(MSG_CR_SEARCH_TITLE) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_TITLE ;

	msg.byRoomIdx	= pInfo->byIdx ;
	SafeStrCpy( msg.title, pInfo->title, TITLE_SIZE ) ;	

	

	// 유저에게 메시지 전송.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_Current_GuestCount
//	DESC : 채팅방의 참여자 인원이 변경되었음을 공지하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_Current_GuestCount( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 채팅 참여중인 유저가 있는지 확인한다.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_CUR_GUESTCOUNT ;



	// 유저에게 메시지 전송.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.bData1		= pInfo->byIdx ;
		msg.bData2		= pInfo->byCurGuestCount ;	

		msg.dwObjectID = pUserInfo->dwPlayerID ;

		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Changed_Total_GuestCount
//	DESC : 채팅방의 최대 참여자 인원이 변경되었음을 공지하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Changed_Total_GuestCount( ST_CR_ROOM_SRV* pInfo )
{
	// 함수 파라메터 체크.
	ASSERT( pInfo ) ;

	if( !pInfo )
	{
		Throw_Error( err_IMP, __FUNCTION__ ) ;
		return ;
	}



	// 채팅 참여중인 유저가 있는지 확인한다.
	if( m_htUser.GetDataNum() == 0 )
	{
		Throw_Error( "Failed to notice add room!!", __FUNCTION__ ) ;
		return ;
	}



	// 전송 메시지 세팅.
	MSG_BYTE2 msg ;
	memset( &msg, 0, sizeof(MSG_BYTE2) ) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_UPDATEINFO_TOTAL_GUESTCOUNT ;

	msg.bData1		= pInfo->byIdx ;
	msg.bData2		= pInfo->byTotalGuestCount ;	

	

	// 유저에게 메시지 전송.
	ST_CR_USER* pUserInfo = NULL ;
	while( (pUserInfo = m_htUser.GetData()) != NULL )
	{
		msg.dwObjectID = pUserInfo->dwPlayerID ;
		g_Network.Send2Server( pUserInfo->dwConnectionIdx, (char*)&msg, sizeof(MSG_BYTE2) ) ;
	}
}

void CChatRoomMgr::Throw_Error( BYTE errType, char* szCaption )
{
	// 함수 파라메터 체크.
	ASSERT( szCaption ) ;

	if( !szCaption || strlen( szCaption ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid Caption!!" ) ;
		WriteLog(tempStr) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	char tempErr[ 257 ] = {0, } ;

	switch(errType)
	{
	case err_IMP :		SafeStrCpy( tempErr, "Invalid a message parameter!!", 256 ) ;					break ;
	case err_FCMTB :	SafeStrCpy( tempErr, "Failed to convert a message to base!!", 256 ) ;			break ;
	case err_FCMTO :	SafeStrCpy( tempErr, "Failed to convert a message to original!!", 256 ) ;		break ;
	default : break ;
	}

	// 에러 출력/로그.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, tempErr, szCaption, MB_OK ) ;
#else
	char tempStr[ 257 ] = {0, } ;

	SafeStrCpy( tempStr, szCaption, 256 ) ;
	strcat( tempStr, " - " ) ;
	strcat( tempStr, tempErr ) ;
	WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : Throw_Error
//	DESC : 채팅방 내, 에러 메시지 관련 로그를 남기는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error( char* szErr, char* szCaption )
{
	// 함수 파라메터 체크.
	ASSERT( szCaption ) ;

	if( !szErr || strlen( szErr ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid err message!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid err message!!" ) ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	if( !szCaption || strlen( szCaption ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat (tempStr, " - " ) ;
		strcat( tempStr, "Invalid Caption!!")  ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	// 에러 출력/로그.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK ) ;
#else
	char tempStr[ 257 ] = {0, } ;

	SafeStrCpy( tempStr, szCaption, 256 ) ;
	strcat( tempStr, " - " ) ;
	strcat( tempStr, szErr ) ;
	WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : Throw_Error
//	DESC : 채팅방 내, 에러 메시지 관련 로그를 남기는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error( char* szCommonErr, char* szErr, char* szCaption )
{
	// 함수 파라메터 체크.
	ASSERT( szCaption ) ;

	if( !szErr || strlen( szErr ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid err message!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid err message!!" ) ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	if( !szCaption || strlen( szCaption ) <= 1 )
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK ) ;
#else
		char tempStr[ 257 ] = {0, } ;

		SafeStrCpy( tempStr, __FUNCTION__, 256 ) ;
		strcat( tempStr, " - " ) ;
		strcat( tempStr, "Invalid Caption!!" ) ;
		WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	// 에러 출력/로그.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK ) ;
#else
	char tempStr[ 257 ] = {0, } ;

	SafeStrCpy( tempStr, szCaption, 256) ;
	strcat( tempStr, " - " ) ;
	strcat( tempStr, szErr ) ;
	WriteLog( tempStr ) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : WriteLog
//	DESC : 채팅방 내, 에러 메시지 관련 로그를 남기는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::WriteLog( char* pMsg )
{
	SYSTEMTIME time ;
	GetLocalTime( &time ) ;

	TCHAR szTime[_MAX_PATH ] = {0, } ;
	sprintf( szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond ) ;

	FILE *fp = fopen( "Log/Dist-ChatRoomErr.log", "a+" ) ;
	if ( fp )
	{
		fprintf( fp, "%s [%s]\n", pMsg,  szTime ) ;
		fclose( fp ) ;
	}
}












