#include "stdafx.h"
#include "stdio.h"
#include "DataBase.h"
#include "CMSDBMsgParser.h"
#include "MonitorNetwork.h"
#include "UserIPCheckMgr.h"
#include "../[CC]ServerModule/ServerListManager.h"
#include "MASManager.h"

DBMsgFunc g_DBMsgFunc[MaxQuery] =
{
	NULL,
};

char txt[512];

void OperatorLoginCheck(DWORD dwConnectIdx, char* strId, char* strPwd, char* strIp)
{
	g_DB.LoginMiddleQuery(
		ROperatorLoginCheck,
		dwConnectIdx,
		"EXEC dbo.RP_OPERATORLOGINCHECK \'%s\', \'%s\', \'%s\'",
		strId,
		strPwd,
		strIp );
}

void ROperatorLoginCheck(LPMIDDLEQUERY query, LPDBMESSAGE dbMessage )
{
	const DWORD connectIdx = dbMessage->dwID;

	if( 1 != dbMessage->dwResult )
	{
		TMSG_WORD msg;
		ZeroMemory( &msg, sizeof( msg ) );
		msg.Category = MP_RMTOOL_CONNECT;
		msg.Protocol = MP_RMTOOL_CONNECT_NACK;
		MNETWORK->SendToMC( connectIdx, (char*)&msg, sizeof(msg));
		return;
	}

	const MIDDLEQUERYST& record = query[ 0 ];
	const DWORD loginResult = atoi( LPCTSTR( record.Data[ 0 ] ) );

	if( loginResult )
	{
		TMSG_WORD msg;
		ZeroMemory( &msg, sizeof( msg ) );
		msg.Category = MP_RMTOOL_CONNECT;
		msg.Protocol = MP_RMTOOL_CONNECT_NACK;
		msg.Code = char(loginResult);
		MNETWORK->SendToMC( connectIdx, (char*)&msg, sizeof(msg));

		// 090629 ShinJS --- 접속 Error시 Log를 남긴다
		FILE* fpLog = NULL;
		fpLog = fopen( "./MonitoringServerLog.txt", "a+" );
		if( fpLog )
		{
			char strIP[20] = {0,};
			WORD wPort = 0;
			MNETWORK->GetUserAddress(connectIdx, strIP, &wPort);

			SYSTEMTIME sysTime;
			GetLocalTime( &sysTime );

			fprintf( fpLog, "OperatorLoginCheck Failed !	ErrorCode : %d, IP : %s, Port : %d, Date : %04d-%02d-%02d %02d:%02d:%02d\n", 
				loginResult,
				strIP,
				wPort,
				sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

			fclose( fpLog );
		}
		return;
	}

	TMSG_OPERATOR_LOGIN Msg;
	ZeroMemory( &Msg, sizeof( Msg ) );
	Msg.Category = MP_RMTOOL_CONNECT;
	Msg.Protocol = MP_RMTOOL_CONNECT_ACK;
	OPERATOR& operatorData = Msg.sOper;
	StringCopySafe(
		operatorData.strOperID,
		LPCTSTR( record.Data[ 1 ] ),
		sizeof( operatorData.strOperID ) / sizeof( *operatorData.strOperID ) );
	StringCopySafe(
		operatorData.strOperName,
		LPCTSTR( record.Data[ 2 ] ),
		sizeof( operatorData.strOperName ) / sizeof( *operatorData.strOperName ) );
	StringCopySafe(
		operatorData.strRegDate, 
		LPCTSTR( record.Data[ 4 ] ),
		sizeof( operatorData.strRegDate ) / sizeof( *operatorData.strRegDate ) );
	operatorData.mPower = eOPERPOWER( atoi( LPCTSTR( record.Data[ 5 ] ) ) );
	// 100316 ONS DB로부터 OperIdx값을 가져오도록 수정.
	operatorData.dwOperIdx = atoi( LPCTSTR( record.Data[ 8 ] ) );
	StringCopySafe(
		Msg.sIP.strIP,
		LPCTSTR( record.Data[ 3 ] ),
		sizeof( Msg.sIP.strIP ) / sizeof( *Msg.sIP.strIP ) );
	
	IPCHECKMGR->AddUser( connectIdx, operatorData, Msg.sIP );
	MNETWORK->SendToMC(connectIdx, ( char* )&Msg, sizeof( Msg ) );
}

// 090519 ShinJS --- Server Off 시 Insert SMS Data
void InsertSMSData( DWORD wConnectIdx, WORD wServerKind, WORD wServerNum )
{
	stSMSReceiver* pReceiver = NULL;

	// SMS Receiver File 변경 확인
	MASMGR->CheckSMSReceiverFile();

	MASMGR->SetSMSTableHeadPos();
	while ( pReceiver = MASMGR->GetSMSTableData() )
	{
		char Msg[ MAX_PATH ] = {0,};

		switch( wServerKind )
		{
		case DISTRIBUTE_SERVER:
			sprintf( Msg, "%s [Set %d][DISTRIBUTE %d]", pReceiver->Msg, SERVERLISTMGR->GetServerSetNum(), wServerNum );
			break;
		case AGENT_SERVER:
			sprintf( Msg, "%s [Set %d][AGENT %d]", pReceiver->Msg, SERVERLISTMGR->GetServerSetNum(), wServerNum );
			break;
		case MAP_SERVER:
			sprintf( Msg, "%s [Set %d][MAP %d]", pReceiver->Msg, SERVERLISTMGR->GetServerSetNum(), wServerNum );
			break;
		default:
			continue;
		}
		sprintf( txt, "EXEC LUNA_TONG.DBO.TP_SMS_INSERT \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\'",
			Msg,
			pReceiver->Name,
			pReceiver->Phone1,
			pReceiver->Phone2,
			pReceiver->Phone3,
			pReceiver->SenderName,
			pReceiver->SenderPhone1,
			pReceiver->SenderPhone2,
			pReceiver->SenderPhone3 );
		g_DB.LoginQuery(
			eQueryType_FreeQuery,
			0,
			wConnectIdx,
			txt);
	}
}