
#ifndef __MSDBMSGPARSER_H__
#define __MSDBMSGPARSER_H__

#include "db.h"


typedef	void (*DBMsgFunc)(LPQUERY pData, LPDBMESSAGE pMessage);
typedef	void (*DBMiddleMsgFunc)(LPMIDDLEQUERY pData, LPDBMESSAGE pMessage);
typedef	void (*DBLargeMsgFunc)(LPLARGEQUERY pData, LPDBMESSAGE pMessage);
extern DBMsgFunc g_DBMsgFunc[];

enum   DBMESSAGEIDFUNC 
{
	eOperInsertSMSData,
	MaxQuery,
};

void DeleteIpAdress( WORD wConnectIdx, WORD wTempIdx, DWORD ipIdx );
void DeleteOperator( WORD wConnectIdx, WORD wTempIdx, DWORD operIdx );
void OperatorLoginCheck( DWORD dwConnectIdx, char* strId, char* strPwd, char* strIp );
void ROperatorLoginCheck( LPMIDDLEQUERY, LPDBMESSAGE );
void DeleteGMIpAdress( WORD wConnectIdx, WORD wTempIdx, DWORD ipIdx );
void InsertSMSData( DWORD wConnectIdx, WORD wServerKind, WORD wServerNum );


#endif