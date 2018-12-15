#include "stdafx.h"
#include "DistributeDBMsgParser.h"
#include "DataBase.h"
#include "CommonDBMsgParser.h"
#include "UserTable.h"
#include "ServerTable.h"
#include "Network.h"
#include "UserManager.h"
#include "BillConnector.h"

extern int g_nServerSetNum;
extern DWORD g_dwMaxUser;

DBMsgFunc g_DBMsgFunc[] =
{
	NULL,
	RLoginCheckQuery,
	RIpCheckQuery,			/// eIpCheckQuery 
	RLoginCheckQueryTest,
	RCheckDynamicPW,
	RLoginCheckQueryForJP,
	RLoginCheckQuery,
};

void LoginCheckInit()
{
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	g_DB.FreeQuery(0, 0, "EXEC %s", "MP_CHARACTER_LOGININIT");
}

BOOL IPCheckQuery(char* ip, DWORD AuthKey, DWORD dwConnectionIndex)
{
	// 090325 ONS 필터링문자체크
	char szBufIp[MAX_IPADDRESS_SIZE];
	SafeStrCpy(szBufIp, ip, MAX_IPADDRESS_SIZE);
	if(IsCharInString(szBufIp, "'"))	return FALSE;

	char query[128];
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(query, "EXEC %s \'%s\', %d", "UP_IP_CHECKIN", szBufIp, AuthKey);

	if(g_DB.LoginQuery(eQueryType_FreeQuery, eIpCheckQuery, dwConnectionIndex, query) == FALSE)
	{
		g_Console.Log(eLogDisplay,4,"DB is Busy(IP_CHECK)  IP:%s  ConnectionIndex:%d", szBufIp, dwConnectionIndex);
		return FALSE;
	}

//	g_Console.Log(eLogDisplay,4,"IP_QUERY   IP: %s,  ConnectionIndex: %d", ip, dwConnectionIndex);
	return TRUE;
}

/// 060911 PKH 홍콩IP체크
/* --------------------------------------------------------------------------
* 함수이름 : IPCheckQuery
* 목    적 : 홍콩IP체크
* 주의사항 :
*---------------------------------------------------------------------------*/
BOOL IPCheckQueryHK(char* ip, DWORD AuthKey, DWORD dwConnectionIndex)
{
	// 090325 ONS 필터링문자체크
	char szBufIp[MAX_IPADDRESS_SIZE];
	SafeStrCpy(szBufIp, ip, MAX_IPADDRESS_SIZE);
	if(IsCharInString(szBufIp, "'"))	return FALSE;
	
	char query[128];
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(query, "EXEC %s \'%s\', %d", "UP_IP_CHECKINHK", szBufIp, AuthKey);

	if(g_DB.LoginQuery(eQueryType_FreeQuery, eIpCheckQuery, dwConnectionIndex, query) == FALSE)
	{
		g_Console.Log(eLogDisplay,4,"DB is Busy(IP_CHECK)  IP:%s  ConnectionIndex:%d", szBufIp, dwConnectionIndex);
		return FALSE;
	}

	//	g_Console.Log(eLogDisplay,4,"IP_QUERY   IP: %s,  ConnectionIndex: %d", ip, dwConnectionIndex);
	return TRUE;
}

void LoginError(DWORD dwConnectionIdx, DWORD AuthKey,DWORD ErrorCode,DWORD dwParam)
{	
	MSG_DWORD2 msguser;
	msguser.Category = MP_USERCONN;
	msguser.Protocol = MP_USERCONN_LOGIN_NACK;
	msguser.dwData1 = ErrorCode;
	msguser.dwData2 = dwParam;
	
	g_pUserTable->SendToUser(dwConnectionIdx, AuthKey, &msguser, sizeof(msguser) );

	
//	g_Console.Log(eLogDisplay,4,"  LOGINERROR   Errorcode: %d,  ConnectionIndex: %d",ErrorCode,dwConnectionIdx);
}

void RIpCheckQuery(LPQUERY pData, LPDBMESSAGE pMessage)
{
	if(pMessage->dwResult == 0)
	{
		// DB Error(DB is busy)
		return;
	}

	WORD Ecode =(WORD)atoi((char*)pData->Data[0]);
	DWORD AuthKey = atoi((char*)pData->Data[1]);

	DWORD dwConnectionIdx	= pMessage->dwID;
	USERINFO* pUserInfo		= g_pUserTable->FindUser(dwConnectionIdx);

	if( pUserInfo == NULL )
		return;

	if( pUserInfo->dwUniqueConnectIdx != AuthKey )
		return;

	if( Ecode == 0 )
	{
		LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_INVALID_IP );
        DisconnectUser( dwConnectionIdx );
		return;
	}


	SERVERINFO* FastInfo = g_pServerTable->GetFastServer( AGENT_SERVER );
	
	if(FastInfo == NULL)
	{
		LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_NOAGENTSERVER );
        DisconnectUser( dwConnectionIdx );
		return;
	}

	MSG_DWORD3BYTE2 msg;
	msg.Category	= MP_USERCONN;
	msg.Protocol	= MP_USERCONN_NOTIFY_USERLOGIN_SYN;
	msg.dwObjectID	= pUserInfo->dwUserID;
	msg.dwData1		= AuthKey;
	msg.dwData2		= dwConnectionIdx;
	msg.dwData3		= 0;
	msg.bData1		= pUserInfo->UserLevel;
	msg.bData2		= 0;
				
	g_Network.Send2Server(FastInfo->dwConnectionIndex,(char*)&msg,sizeof(msg));
}

DWORD testTime[1024];
BOOL LoginCheckQueryTest(char* id, char* pw, DWORD AuthKey, DWORD dwConnectionIndex)
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(id, "'") || IsCharInString(pw, "'"))	return FALSE;
	
	char guery[128];
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(guery, "EXEC %s \'%s\', \'%s\', %d, %d", "UP_MEMBER_CHECKIN", id, pw, AuthKey, 1);

	if(g_DB.LoginQuery(101, eLoginCheckQueryTest, dwConnectionIndex, guery) == FALSE)
	{
		g_Console.Log(eLogDisplay,4,"DB is Busy  ID:%d  ConnectionIndex:%d",id,dwConnectionIndex);
		return FALSE;
	}
	
	g_Console.Log(eLogDisplay,4,"IDPW_QUERY   ID: %s,  ConnectionIndex: %d",id,dwConnectionIndex);
	return TRUE;
}

void	RLoginCheckQueryTest(LPQUERY pData, LPDBMESSAGE pMessage)
{
	DWORD AuthKey =atoi((char*)pData->Data[2]);
	
	testTime[AuthKey-1] = GetTickCount() - pMessage->dwID;

	g_Console.Log(eLogDisplay,4,"IDPW TestReceived    ID: %d  Time: %d",AuthKey,testTime[AuthKey-1]);

	if(AuthKey == 1024)
	{
		FILE* fp = fopen("test.txt","w");
		for(int n=0;n<1024;++n)
			fprintf(fp,"%d  ElapsedTime: %d\n",n+1,testTime[n]);
		fclose(fp);
	}
}

//* MemberDB 짹쨀횄쩌 ?횤쩐첨 2004.03.22
void LoginCheckDelete(DWORD UserID)
{
	char txt[ MAX_PATH ] = { 0 };
	// 090710 LUJ, 로그아웃 시 서버셋 번호를 전달함
	sprintf(txt, "EXEC dbo.UP_GAMELOGOUT %d, %d", UserID, g_nServerSetNum );
	g_DB.LoginQuery(eQueryType_FreeQuery, eNull, 0, txt);
}
//*/

//=========================================================================
//	NAME : LoginCheckQuery
//	DESC : The function to run procedure to check login infor.
//=========================================================================
BOOL LoginCheckQuery(char* id, char* pw, char* ip, WORD ServerNo, DWORD AuthKey, 
	DWORD dwConnectionIndex, DWORD dwMaxUser, WORD ServerNum, char* pSecurityPW)
{
	if( !id ) return FALSE ;														// 인자로 넘어온 포인터 무결성 체크.

	if( !pw ) return FALSE ;

	if( !ip ) return FALSE ;

	if( !pSecurityPW ) return FALSE ;


	char tempSecurityPW[17] = {0, } ;

	if( strlen(pSecurityPW) <= 0 )
	{
		// 080821 LYW --- DistributeDBMsgParser : Stack around 에러 수정.
		//strcpy(tempSecurityPW, "''") ;
		SafeStrCpy(tempSecurityPW, "''", 17) ;
	}
	else
	{
		// 080821 LYW --- DistributeDBMsgParser : Stack around 에러 수정.
		//strcpy(tempSecurityPW, pSecurityPW) ;
		SafeStrCpy(tempSecurityPW, pSecurityPW, 17) ;
	}

	// 090325 ONS 필터링문자체크
	if(IsCharInString(id, "'") || IsCharInString(pw, "'") || IsCharInString(ip, "'") )	return FALSE;
	if(strlen(tempSecurityPW) > strlen("''") && IsCharInString(tempSecurityPW, "'")) return FALSE;

	char guery[128] = {0, } ;														// 임시 버퍼를 선언하고 초기화 한다.
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(		guery,															// 프로시저 실행 명령어를 완성한다.
					"EXEC UP_GAMELOGINADULT_FREE \'%s\', \'%s\', \'%s\', %d, %d, %d, %d, %d, \'%s\'", 
                    id,																// 아이디.
					pw,																// 비밀번호.
					ip,																// ip.
					ServerNo,														// 서버 셋 번호.									
					AuthKey,														// 고유 키.
					dwMaxUser,														// 최대 유저 수.
					gUserMGR.GetLimitAge(),											// 제한 연령.
					ServerNum,														// 서버 번호.
					tempSecurityPW													// 보안 비밀 번호.
			) ;	

	BOOL bResult = g_DB.LoginQuery(
		eQueryType_FreeQuery,
		eLoginCheckQuery,
		dwConnectionIndex, 
		guery);

	if(!bResult)																	// 실패 했다면,
	{
		g_Console.Log(	eLogDisplay,												// 서버 콘솔 창에 에러 메시지를 출력한다.
						4,
						"DB is Busy  ID:%d  ConnectionIndex:%d",
						id,
						dwConnectionIndex) ;

		LoginError( dwConnectionIndex, AuthKey, LOGIN_ERROR_DISTSERVERISBUSY ) ;	// 유저에게 로그인 에러를 전송한다.

		return FALSE ;
	}

	return TRUE;
}

void RLoginCheckQuery(LPQUERY pData, LPDBMESSAGE pMessage)
{
	int		stateNo = atoi((char*)pData->Data[0]);
	WORD	ServerNo = (WORD)atoi((char*)pData->Data[1]);
	DWORD	UserIdx = atoi((char*)pData->Data[2]);
	WORD	wType = (WORD)atoi((char*)pData->Data[3]);
	int		nTime = atoi((char*)pData->Data[4]);
//	WORD	bTen = atoi((char*)pData->Data[5]);
	DWORD	AuthKey = atoi((char*)pData->Data[6]);
	BYTE	UserLevel = (BYTE)atoi((char*)pData->Data[7]);
	WORD	AgentNo = (WORD)atoi((char*)pData->Data[8]);
	DWORD	dwConnectionIdx = pMessage->dwID;


	USERINFO* pUserInfo = g_pUserTable->FindUser(dwConnectionIdx);

	if(pUserInfo == NULL)					//유저가 나갔다.
	{
		//접속 성공했을 때만 로그인 delete를 해준다.
		if( stateNo == 1 ) LoginCheckDelete(UserIdx);
		return;
	}

	if(pUserInfo->dwUniqueConnectIdx != AuthKey)	//쿼리보낸 유저 끊기고, 다른 유저가 들어왔다.
	{												//***pUserInfo는 내것이 아니다. 지우면 안된다.***
		//접속 성공했을 때만 로그인 delete를 해준다.		
		if( stateNo == 1 ) LoginCheckDelete(UserIdx);
		return;
	}

	// 100226 pdy 스크립트채크시 서버의 키값은 한번만 읽도록 변경
	const ScriptCheckValue* check = g_pServerSystem->GetScriptCheckValue( );
 
 	if( check->mValue != pUserInfo->check.mValue && UserLevel > eUSERLEVEL_GM )
 	{
 		LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_FILE_CRC );
 		return;
 	}

	pUserInfo->dwUserID		= UserIdx;
	pUserInfo->UserLevel	= UserLevel;
	pUserInfo->State		= stateNo;

	// 100706 ShinJS 로그인 에러 파일 로그
	if( stateNo != 1 )
	{
		FILE* fpLog = NULL;
		fpLog = fopen( "./Log/LoginError.txt", "a+" );
		if( fpLog )
		{
			SYSTEMTIME sysTime;
			GetLocalTime( &sysTime );

			fprintf( fpLog, "[%04d-%02d-%02d %02d:%02d:%02d]\t [state : %d , UserIdx : %d]\n", 
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
				stateNo,
				UserIdx );

			fclose( fpLog );
		}
	}

	switch(stateNo)
	{
	case -11:	// 블럭 적용일 경우
	case -10:	// DB 에러 발생시
	case -9:	// DB 에러 발생시
	case -8:	// 로그인 세션 체크 4회 이상 조회일 경우 (외부에서 로그인 세션 호출할 경우)
	case -6:	// 회원의 보안번호가 존재하지 않을 경우
	case -5:	// 로그인 세션 회원 고유번호가 다를 경우
	case -4:	// 로그인 회원의 고유번호가 존재하지 않을 경우
	case -3:	// 로그인 세션 IP가 다를 경우
	case -2:	// 로그인 세션 KEY가 다를 경우
	case -1:	// 로그인 세션 KEY가 존재하지 않을 경우
		{
			LoginError( dwConnectionIdx, pUserInfo->dwUniqueConnectIdx, LOGIN_ERROR_LOGINSESSION_INVALID );
		}
		break;
	case 1:	//로그인 성공
		{
			pUserInfo->dwDisconOption |= eDISCONOPT_DELETELOGINCHECK;	//AAA
			//이제부터 끊기면 DeleteLoginCheck을 해주게 된다. 
			//여러부분에서 호출하던 것을 OndisconnectUser에서 호출하도록 통일한다.

			g_LoginUser.Remove( UserIdx );

			if(gUserMGR.IsValidUserLevel(UserLevel) == FALSE)
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_INVALIDUSERLEVEL);
				DisconnectUser(dwConnectionIdx);
				return;
			}

			char strIP[16] = {0,};
			WORD wPort = 0;
			g_Network.GetUserAddress( dwConnectionIdx, strIP, &wPort );

			if( UserLevel == eUSERLEVEL_GM )
			{
				IPCheckQuery( strIP, AuthKey, dwConnectionIdx );
				return;
			}

			SERVERINFO* pAgentServer = g_pServerTable->GetServer(AGENT_SERVER, AgentNo);
			if(pAgentServer == NULL)
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_NOAGENTSERVER);
				DisconnectUser( dwConnectionIdx );
				return;
			}

			LPCTSTR userId = LPCTSTR(pData->Data[9]);

			MSG_NAME loginidMessage;
			ZeroMemory( &loginidMessage, sizeof(loginidMessage) );
			loginidMessage.Category = MP_USERCONN;
			loginidMessage.Protocol = MP_USERCONN_GETLOGINID_ACK;
			loginidMessage.dwObjectID = UserIdx;
			SafeStrCpy( loginidMessage.Name, userId, sizeof(loginidMessage.Name) );
			g_Network.Send2User(dwConnectionIdx, (char*)&loginidMessage, sizeof(loginidMessage));

			MSG_DWORD3BYTE2 msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_NOTIFY_USERLOGIN_SYN;
			msg.dwObjectID	= UserIdx;
			msg.dwData1		= AuthKey;
			msg.dwData2		= dwConnectionIdx;
			msg.dwData3		= (DWORD)nTime;
			msg.bData1		= (BYTE)UserLevel;
			msg.bData2		= (BYTE)wType;
			
			g_Network.Send2Server(pAgentServer->dwConnectionIndex,(char*)&msg,sizeof(msg));
				
			g_pServerSystem->GetBilling().GameStart(
				UserIdx,
				strIP);
		}
		break;
	case 2: // 계정비번 에러
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_WRONGIDPW);
		}
		break;
	case 3: // 계정비번 에러
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_WRONGIDPW);			
		}
		break;
	case 4: // 중복로그인
		{
			if(g_nServerSetNum == ServerNo)
			{
				MSG_DWORD msg;
				msg.Category	= MP_USERCONN;
				msg.Protocol	= MP_USERCONN_NOTIFY_OVERLAPPEDLOGIN;
				msg.dwData		= UserIdx;
				g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));
			
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_OVERLAPPEDLOGIN,0);
			}
			else
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_OVERLAPPEDLOGININOTHERSERVER,ServerNo);
			}
		}
		break;

	case 5: //블럭
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_BLOCKUSERLEVEL);
			DisconnectUser(dwConnectionIdx);	//AAA 접속을 끊는 것이 좋다.
		}
		break;


	case 7: // DB Connection Error
		{
			ASSERTMSG(0, "login check error ret 0");
			DisconnectUser(dwConnectionIdx);	//AAA 접속을 끊는 것이 좋다.
		}
		break;

	case 8: // ? 맥스유저?
		{			
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_MAXUSER );
			DisconnectUser(dwConnectionIdx);	//AAA 접속을 끊는 것이 좋다.
		}
		break;

	// 080111 LYW --- DistributeDBMsgParser : 보안 비밀 에러 처리 추가.
	case 9 :
	case -7:// 로그인 세션 보안번호가 다를 경우
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_INVALID_SEC_PASS) ;
		}
		break ;

	// 080111 LYW --- DistributeDBMsgParser : 비밀번호 정보가 유효하지 않음, 비밀번호를 재 등록 하세요.
	case 10 :
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_CHANGE_SEC_PASS) ;
		}
		break ;

//---KES PUNISH
	case 11:
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_BLOCKUSERLEVEL,nTime) ;
		}
		break;
//-------------

	case 14:
		{
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_NOT_CLOSEBETAWINNER );
			DisconnectUser(dwConnectionIdx);	//AAA 접속을 끊는 것이 좋다.
		}
		break;
	case 15: //미성년자
		{
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_MINOR_INADULTSERVER );
			DisconnectUser(dwConnectionIdx);	//AAA 접속을 끊는 것이 좋다.
		}
		break;
	case 50: // 탈퇴유저
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_SECEDEDACCOUNT);
			DisconnectUser(dwConnectionIdx);	//AAA 접속을 끊는 것이 좋다.
		}
		break;
	case 99:		
		{
			LoginGetDynamicPWQuery( (char*)pData->Data[8], dwConnectionIdx );		
		}
		break;
	case 999:
		{
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_NOIPREGEN );
			DisconnectUser(dwConnectionIdx);	//AAA 접속을 끊는 것이 좋다.
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}
}
//*/

BOOL LoginGetDynamicPWQuery( char* id, DWORD dwConnectionIndex )
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(id, "'"))	return FALSE;

	char guery[128];
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( guery, "EXEC %s \'%s\', '', 1", "LP_MAT_OUT", id );

	if(g_DB.LoginMiddleQuery( RLoginGetDynamicPWQuery, dwConnectionIndex, guery) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

/// 060920 PKH 다이나믹IP체크
BOOL LoginGetDynamicPWQueryHK( char* strIP, char* id, DWORD dwConnectionIndex )
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(strIP, "'") || IsCharInString(id, "'"))	return FALSE;

	char guery[128];
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf( guery, "EXEC %s \'%s\', \'%s\', '', 1", "LP_MAT_OUT_HK", strIP, id );

	if(g_DB.LoginMiddleQuery( RLoginGetDynamicPWQuery, dwConnectionIndex, guery) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

void RLoginGetDynamicPWQuery(LPMIDDLEQUERY pData, LPDBMESSAGE pMessage)
{
	DWORD dwConnectionIdx = pMessage->dwID;
	int rt = atoi((char*)pData->Data[0]);

	if( rt == 1 )
	{
		//send mat msg
		MSG_USE_DYNAMIC_ACK msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_USE_DYNAMIC_ACK;
		SafeStrCpy( msg.mat, (char*)pData->Data[1], 32 );

		g_Network.Send2User( dwConnectionIdx, (char*)&msg, sizeof(msg) );			
	}
	else
	{
		//error msg
		MSGBASE msg;
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_USE_DYNAMIC_NACK;
		g_Network.Send2User( dwConnectionIdx, (char*)&msg, sizeof(msg) );
	}
}

BOOL CheckDynamicPW( char* id, char* pw, char* ip, WORD ServerNo, DWORD AuthKey, DWORD dwConnectionIndex, DWORD dwMaxUser, WORD ServerNum )
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(id, "'") || IsCharInString(pw, "'") || IsCharInString(ip, "'"))	return FALSE;

	char guery[128];
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(guery, "EXEC UP_GAMELOGINMAT_FREE \'%s\', \'%s\', \'%s\', %d, \'%d\', %d, %d, %d",
		id, pw, ip, ServerNo, AuthKey, dwMaxUser, gUserMGR.GetLimitAge(), ServerNum );

	if(g_DB.LoginQuery(eQueryType_FreeQuery, eCheckDynamicPW, dwConnectionIndex, guery) == FALSE)
		return FALSE;

	return TRUE;
}

void RCheckDynamicPW( LPQUERY pData, LPDBMESSAGE pMessage )
{
	DWORD stateNo = atoi((char*)pData->Data[0]);
	WORD ServerNo = (WORD)atoi((char*)pData->Data[1]);
	DWORD UserIdx = atoi((char*)pData->Data[2]);
	WORD	wType = (WORD)atoi((char*)pData->Data[3]);
	int		nTime = atoi((char*)pData->Data[4]);
//	WORD bTen = atoi((char*)pData->Data[5]);
	DWORD AuthKey = atoi((char*)pData->Data[6]);
	BYTE UserLevel = (BYTE)atoi((char*)pData->Data[7]);
	WORD AgentNo = (WORD)atoi((char*)pData->Data[8]);
	DWORD dwConnectionIdx = pMessage->dwID;

	USERINFO* pUserInfo = g_pUserTable->FindUser(dwConnectionIdx);

	if(pUserInfo == NULL)					//유저가 나갔다.
	{	
		//접속 성공했을 때만 로그인 delete를 해준다.
		if( stateNo == 1 ) LoginCheckDelete(UserIdx);
		return;
	}

	if(pUserInfo->dwUniqueConnectIdx != AuthKey)
	{
		//접속 성공했을 때만 로그인 delete를 해준다.		
		if( stateNo == 1 ) LoginCheckDelete(UserIdx);
		return;
	}

	pUserInfo->dwUserID		= UserIdx;
	pUserInfo->UserLevel	= UserLevel;
	pUserInfo->State		= stateNo;

	switch(stateNo)
	{
	case 1:
		{
			pUserInfo->dwDisconOption |= eDISCONOPT_DELETELOGINCHECK;	//AAA
			//이제부터 끊기면 DeleteLoginCheck을 해주게 된다. 
			//여러부분에서 호출하던 것을 OndisconnectUser에서 호출하도록 통일한다.

			g_LoginUser.Remove( UserIdx );

			if(gUserMGR.IsValidUserLevel(UserLevel) == FALSE)
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_INVALIDUSERLEVEL);
				DisconnectUser(dwConnectionIdx);
				return;
			}
			
			if( UserLevel == eUSERLEVEL_GM )
			{
				char strIP[16] = {0,};
				WORD wPort;
				g_Network.GetUserAddress( dwConnectionIdx, strIP, &wPort );

				IPCheckQuery( strIP, AuthKey, dwConnectionIdx );
				return;
			}

			SERVERINFO* pAgentServer = g_pServerTable->GetServer(AGENT_SERVER, AgentNo);
			if(pAgentServer == NULL)
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_NOAGENTSERVER);
				DisconnectUser(dwConnectionIdx);
				return;
			}

			MSG_DWORD3BYTE2 msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_NOTIFY_USERLOGIN_SYN;
			msg.dwObjectID	= UserIdx;
			msg.dwData1		= AuthKey;
			msg.dwData2		= dwConnectionIdx;
			msg.dwData3		= (DWORD)nTime;
			msg.bData1		= (BYTE)UserLevel;
			msg.bData2		= (BYTE)wType;
				
			g_Network.Send2Server(pAgentServer->dwConnectionIndex,(char*)&msg,sizeof(msg));
				
			//g_pLoginUser.Remove( pUserInfo->dwUserID );
		}
		break;
	case 2: // ID쩐첩?쩍.
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_WRONGIDPW);
		}
		break;
	case 3: // 쨘챰쨔횖쨔첩횊짙쩔?쨌첫.
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_WRONGIDPW);			
		}
		break;
	case 4: // 째횚?횙쩔징 횁짖쩌횙횁횩.
		{
			if(g_nServerSetNum == ServerNo)
			{
				MSG_DWORD msg;
				msg.Category = MP_USERCONN;
				msg.Protocol = MP_USERCONN_NOTIFY_OVERLAPPEDLOGIN;
				msg.dwData = UserIdx;
				g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));
			
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_OVERLAPPEDLOGIN,0);
			}
			else
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_OVERLAPPEDLOGININOTHERSERVER,ServerNo);
			}
		}
		break;

	case 5: // 쨘챠쨌째쨉횊 째챔횁짚. @state_num = 6?횑쨍챕 째챔횁짚쨘챠쨌째 (?짜 째체쨍짰쨍챨쨉책쩔징쩌짯 쨩챌쩔챘)
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_BLOCKUSERLEVEL);
		}
		break;

	case 7: // DB Connection Error
		{
			ASSERTMSG(0, "login check error ret 0");
		}
		break;

	case 8: // ?횓쩔첩횄횎째첬
		{			
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_MAXUSER );
		}
		break;
	case 14:
		{
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_NOT_CLOSEBETAWINNER );
		}
		break;
	case 15: //쩌쨘?횓쩌쨌쩔징 쨔횑쩌쨘쨀창?횣째징 쨉챕쩐챤쩔?쨌횁째챠 횉횩쨈횢.
		{
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_MINOR_INADULTSERVER );
		}
		break;
	case 50: // 횊쨍쩔첩횇쨩횇챨횉횗 째챔횁짚?횙.
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_SECEDEDACCOUNT);
		}
		break;
	case 999:
		{
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_NOIPREGEN );
		}
		break;
	default:
		{
			ASSERT(0);
		}
	}
}

// for Japan
void LoginCheckQueryForJP( char* id, char* pw, char* ip, WORD ServerNo, DWORD AuthKey, DWORD dwConnectionIndex, DWORD dwMaxUser, WORD ServerNum )
{
	// 090325 ONS 필터링문자체크
	if(IsCharInString(id, "'") || IsCharInString(pw, "'") || IsCharInString(ip, "'"))	return;

	char guery[128];
	// 081012 LUJ, 일부 로컬은 프로시저 호출 시 대소문자까지 일치해야 한다. 단, dbo는 소문자로 해야함. 이에 모든 프로시저 이름을 수정함
	sprintf(guery, "EXEC UP_GAMELOGINADULT_FREE_JP \'%s\', \'%s\', \'%s\', %d, %d, %d, %d, %d", 
				   id, pw, ip, ServerNo, AuthKey, dwMaxUser, gUserMGR.GetLimitAge(), ServerNum);

	if(g_DB.LoginQuery(eQueryType_FreeQuery, eLoginCheckQueryForJP, dwConnectionIndex, guery) == FALSE)
	{
		g_Console.Log(eLogDisplay,4,"DB is Busy  ID:%d  ConnectionIndex:%d",id,dwConnectionIndex);
		LoginError( dwConnectionIndex, AuthKey, LOGIN_ERROR_DISTSERVERISBUSY );
		return;
	}
}

void RLoginCheckQueryForJP( LPQUERY pData, LPDBMESSAGE pMessage )
{
	DWORD dwConnectionIdx	= pMessage->dwID;
	DWORD stateNo			= atoi((char*)pData->Data[0]);
	WORD ServerNo			= (WORD)atoi((char*)pData->Data[1]);
	DWORD UserIdx			= atoi((char*)pData->Data[2]);
	WORD wType				= (WORD)atoi((char*)pData->Data[3]);
	int nTime				= atoi((char*)pData->Data[4]);
//	WORD bTen				= atoi((char*)pData->Data[5]);
	DWORD AuthKey			= atoi((char*)pData->Data[6]);
	BYTE UserLevel			= (BYTE)atoi((char*)pData->Data[7]);
	WORD AgentNo			= (WORD)atoi((char*)pData->Data[8]);

	USERINFO* pUserInfo = g_pUserTable->FindUser(dwConnectionIdx);
	if(pUserInfo == NULL)
	{
		//접속 성공했을 때만 로그인 delete를 해준다.
		if( stateNo == 1 ) LoginCheckDelete(UserIdx);
		return;
	}

	if(pUserInfo->dwUniqueConnectIdx != AuthKey)		// ?횑쨔횑 쨀짧째징째챠 쨈횢쨍짜 쨀횗?횑 쨉챕쩐챤쩔횂 째챈쩔챙
	{
		//접속 성공했을 때만 로그인 delete를 해준다.		
		if( stateNo == 1 ) LoginCheckDelete(UserIdx);
		return;
	}

	pUserInfo->dwUserID = UserIdx;
	pUserInfo->UserLevel = UserLevel;
	pUserInfo->State = stateNo;
	
	switch(stateNo)
	{
	case 1: // success
		{
			pUserInfo->dwDisconOption |= eDISCONOPT_DELETELOGINCHECK;	//AAA
			//이제부터 끊기면 DeleteLoginCheck을 해주게 된다. 
			//여러부분에서 호출하던 것을 OndisconnectUser에서 호출하도록 통일한다.

			g_LoginUser.Remove( UserIdx );

			// check userlevel
			if(gUserMGR.IsValidUserLevel(UserLevel) == FALSE)
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_INVALIDUSERLEVEL);

				DisconnectUser(dwConnectionIdx);
				return;
			}

			if( UserLevel == eUSERLEVEL_GM )
			{
				char strIP[16] = {0,};
				WORD wPort;
				g_Network.GetUserAddress( dwConnectionIdx, strIP, &wPort );

				IPCheckQuery( strIP, AuthKey, dwConnectionIdx );
				return;
			}

			SERVERINFO* pAgentServer = g_pServerTable->GetServer(AGENT_SERVER, AgentNo);
			if(pAgentServer == NULL)
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_NOAGENTSERVER);
				DisconnectUser(dwConnectionIdx);
				return;
			}

			MSG_DWORD3BYTE2 msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_NOTIFY_USERLOGIN_SYN;
			msg.dwObjectID	= UserIdx;
			msg.dwData1		= AuthKey;
			msg.dwData2		= dwConnectionIdx;
			msg.dwData3		= (DWORD)nTime;
			msg.bData1		= UserLevel;
			msg.bData2		= (BYTE)wType;
				
			g_Network.Send2Server(pAgentServer->dwConnectionIndex,(char*)&msg,sizeof(msg));
				
			//g_pLoginUser.Remove( pUserInfo->dwUserID );
		}
		break;
	case 2: // id error
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_WRONGIDPW);
		}
		break;
	case 3: // password error
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_WRONGIDPW);			
		}
		break;
	case 4: // already in other server
		{
			if(g_nServerSetNum == ServerNo)
			{
				MSG_DWORD msg;
				msg.Category = MP_USERCONN;
				msg.Protocol = MP_USERCONN_NOTIFY_OVERLAPPEDLOGIN;
				msg.dwData = UserIdx;
				g_Network.Broadcast2AgentServer((char*)&msg,sizeof(msg));
			
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_OVERLAPPEDLOGIN,0);
			}
			else
			{
				LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_OVERLAPPEDLOGININOTHERSERVER,ServerNo);
			}
		}
		break;
	case 5: // block user
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_BLOCKUSERLEVEL);
		}
		break;
	case 7: // DB Connection Error
		{
			ASSERTMSG(0, "login check error ret 0");
		}
		break;
	case 8: // check maxuser
		{			
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_MAXUSER );
		}
		break;
	case 50: // no account
		{
			LoginError(dwConnectionIdx,AuthKey,LOGIN_ERROR_SECEDEDACCOUNT);
		}
		break;
	case 999:
		{
			LoginError( dwConnectionIdx, AuthKey, LOGIN_ERROR_NOIPREGEN );
		}
		break;
	default:
		{
			ASSERT(0);
		}
	}
}


void WebLauncherLoginWithLoginKey( DWORD dwConnectionIndex, char* loginKey, int nGameKey, char* ip, char* securityPw )
{
	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionIndex );
	if( pUserInfo == NULL )
	{
		return;
	}

	SERVERINFO* FastInfo = g_pServerTable->GetFastServer( AGENT_SERVER );
	if(FastInfo == NULL)
	{
		LoginError( dwConnectionIndex, pUserInfo->dwUniqueConnectIdx, LOGIN_ERROR_NOAGENTSERVER );
		DisconnectUser( dwConnectionIndex );
		g_Console.Log(eLogDisplay,4, "Can't Find Agent Server" );
		return;
	}

	char txt[ MAX_PATH ]={0,};
	sprintf( txt, "EXEC UP_WEBLAUNCHER_LOGIN_WITH_LOGINKEY \'%s\', %d, \'%s\', \'%s\', %d, %d, %d, %d, %d", 
		loginKey,
		nGameKey,
		ip,
		securityPw,
		g_nServerSetNum,
		pUserInfo->dwUniqueConnectIdx,
		g_dwMaxUser,
		gUserMGR.GetLimitAge(),
		FastInfo->wServerNum );

	g_DB.LoginQuery( eQueryType_FreeQuery, eWebLauncherLoginWithLoginKey, dwConnectionIndex, txt );
}

void RGameRoomData(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage)
{
	const DWORD userIndex = dbMessage->dwID;
	const MIDDLEQUERYST& record = query[0];
	LPCTSTR gameRoomUpperIndex = LPCTSTR(record.Data[0]);
	LPCTSTR gameRoomLowerIndex = LPCTSTR(record.Data[1]);

	if(const DWORD gameRoomIndex = _ttoi(gameRoomLowerIndex) + _ttoi(gameRoomUpperIndex))
	{
		MSG_PACKET_GAME message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_BILLING_START_ACK;
		message.dwObjectID = userIndex;
		message.mPacketGame.User_Age = _ttoi(gameRoomUpperIndex) + _ttoi(gameRoomLowerIndex);
		message.mPacketGame.Packet_Result = 1;

		g_Network.Broadcast2AgentServer(
			(char*)&message,
			sizeof(message));
		return;
	}

	MSGBASE message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_BILLING_START_NACK;
	message.dwObjectID = userIndex;

	g_Network.Broadcast2AgentServer(
		(char*)&message,
		sizeof(message));
}