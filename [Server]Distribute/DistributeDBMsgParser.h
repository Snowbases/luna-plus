
#ifndef __DISTRIBUTEDBMSGPARSER_H__
#define __DISTRIBUTEDBMSGPARSER_H__

#include "db.h"
#include "CommonDBMsgParser.h"

//-----------------------------------------------------------------------
// DBÄõ¸® ¸®ÅÏ ÇÔ¼ö°¡ ´Ã¾î³¯¶§¸¶´Ù Ãß°¡
// °è¼Ó Ãß°¡µÇ´Â ºÎºÐ
enum 
{
	eNull = 0,
	eLoginCheckQuery,				/// ID/PWÅ×½ºÆ® Äõ¸®
	eIpCheckQuery,
	eLoginCheckQueryTest,
	eCheckDynamicPW,
	eLoginCheckQueryForJP,
	eWebLauncherLoginWithLoginKey,
};

void LoginCheckInit();

/*
BOOL LoginCheckQuery(char* id, char* pw, DWORD AuthKey, DWORD dwConnectionIndex);
void LoginCheckDelete(DWORD UserID);
*/

BOOL IPCheckQuery(char* ip, DWORD AuthKey, DWORD dwConnectionIndex);
void RIpCheckQuery(LPQUERY pData, LPDBMESSAGE pMessage);
/// 060911 PKH 홍콩IP체크
BOOL IPCheckQueryHK(char* ip, DWORD AuthKey, DWORD dwConnectionIndex);

BOOL LoginCheckQueryTest(char* id, char* pw, DWORD AuthKey, DWORD dwConnectionIndex);
void RLoginCheckQueryTest(LPQUERY pData, LPDBMESSAGE pMessage);

BOOL LoginGetDynamicPWQuery(char* id, DWORD dwConnectionIndex );
void RLoginGetDynamicPWQuery(LPMIDDLEQUERY pData, LPDBMESSAGE pMessage);
/// 060920 PKH 다이나믹IP체크
BOOL LoginGetDynamicPWQueryHK( char* strIP, char* id, DWORD dwConnectionIndex );

BOOL CheckDynamicPW( char* id, char* pw, char* ip, WORD ServerNo, DWORD AuthKey, DWORD dwConnectionIndex, DWORD dwMaxUser, WORD ServerNum );
void RCheckDynamicPW( LPQUERY pData, LPDBMESSAGE pMessage );

void LoginError(DWORD dwConnectionIdx, DWORD AuthKey,DWORD ErrorCode,DWORD dwParam = 0);

// 080111 LYW --- DistributeDBMsgParser : 로그인 처리 쿼리문을 수정한다. (보안 비밀번호 추가)
BOOL LoginCheckQuery(char* id, char* pw, char* ip, WORD ServerNo, DWORD AuthKey, DWORD dwConnectionIndex, DWORD dwMaxUser, WORD ServerNum, char* pSecurityPW);

//* MemberDB ±³Ã¼ ÀÛ¾÷ 2004.03.22
//BOOL LoginCheckQuery(char* id, char* pw, char* ip, WORD ServerNo, DWORD AuthKey, DWORD dwConnectionIndex, DWORD dwMaxUser, WORD ServerNum);
void LoginCheckDelete(DWORD UserID);
void RLoginCheckQuery(LPQUERY pData, LPDBMESSAGE pMessage);
//*/

// for Japan
void LoginCheckQueryForJP( char* id, char* pw, char* ip, WORD ServerNo, DWORD AuthKey, DWORD dwConnectionIndex, DWORD dwMaxUser, WORD ServerNum );
void RLoginCheckQueryForJP( LPQUERY pData, LPDBMESSAGE pMessage );

// 100429 ShinJS --- 웹런처 LoginKey를 이용한 Login처리
void WebLauncherLoginWithLoginKey( DWORD dwConnectionIndex, char* loginKey, int nGameKey, char* ip, char* securityPw );
void RGameRoomData(LPMIDDLEQUERY, LPDBMESSAGE);

#endif //__DISTRIBUTEDBMSGPARSER_H__
