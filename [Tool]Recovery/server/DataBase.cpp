#include "stdafx.h"
#include "DataBase.h"
#include "..\..\[CC]ServerModule\CommonDBMsgParser.h"
// 080402 LUJ, 로그 표시용
#include "UserManager.h"

BOOL g_bWriteQuery = FALSE;

void ReadDBResultMessage1(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(0,ret,pMessage);	}
void ReadDBResultMessage2(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(1,ret,pMessage);	}
void ReadDBResultMessage3(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(2,ret,pMessage);	}
void ReadDBResultMessage4(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(3,ret,pMessage);	}
void ReadDBResultMessage5(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(4,ret,pMessage);	}
void ReadDBResultMessage6(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(5,ret,pMessage);	}
void ReadDBResultMessage7(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(6,ret,pMessage);	}
void ReadDBResultMessage8(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(7,ret,pMessage);	}
void ReadDBResultMessage9(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(8,ret,pMessage);	}
void ReadDBResultMessage10(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(9,ret,pMessage);	}

void ReadLoginDBResultMessage(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(100,ret,pMessage);	}
void ReadLogDBResultMessage(DWORD ret, LPDBMESSAGE pMessage){	ReadDBResultMessage(200,ret,pMessage);	}

void castingdelete(LPVOID pTarget1, LPVOID pTarget2, int nNum);
void WriteMessage(char* pmsg);

#define	WM_DB_MESSAGE  WM_USER+231

char	LOGINDBNAME[256]		= "LUNA_MEMBERDB";
char	LOGINDBID[256]			= "GameSrv";
char	LOGINDBPASSWORD[256]	= "luna";
char	LOGDBNAME[256]			= "LUNA_LOGDB";
char	LOGDBID[256]			= "GameSrv";
char	LOGDBPASSWORD[256]		= "luna";
char	GAMEDBNAME[256]			= "LUNA_GAMEDB";
char	GAMEDBID[256]			= "GameSrv";
char	GAMEDBPASSWORD[256]		= "luna";

CDataBase g_DB;

extern HWND g_hWnd;

BOOL DBTH::Init(DESC_DBTHREAD* pDesc,BOOL bUsingLoginDB)
{
	HRESULT hr = CoCreateInstance(CLSID_DBTHREAD_DLL, NULL, CLSCTX_INPROC_SERVER, IID_DBTHREAD_DLL, (void**)&m_pIDB);
	if (FAILED(hr))
	{
		MessageBox(NULL,"Cannot create IID_DBTHREAD_DLL  : unregistered Component","Error",NULL);
		return FALSE;
	}
	
	BOOL ret = m_pIDB->CreateDBThread(pDesc);
	if (!ret)
	{
		LPSTR pmsg = "can not init db class";
		WriteMessage(pmsg);
		return FALSE;
	}

	return TRUE;
}
void DBTH::Release()
{
	if(m_pIDB)
	{
		m_pIDB->Release();
		m_pIDB = NULL;
	}
}

BOOL DBTH::Query(DBQUERY* pQuery)
{
	LPDBFreeQuery pData = (LPDBFreeQuery) m_pIDB->GetDBMessageStackMemory();
	if(!pData)
	{
		return FALSE;
	}

	pData->pCon = m_pIDB->GetAdMinCon();

	sprintf(pData->szBuffer, "%s", 	pQuery->strQuery);
	
	LPDBMESSAGE pTemp = (LPDBMESSAGE) m_pIDB->GetDBMessageMemory();
	if(!pTemp)
	{
		m_pIDB->FreeThisMemory( NULL, pData );
		return FALSE;
	}
	
	pTemp->bFNNo = pQuery->QueryType;									// 엔진상의 쿼리의 종류
	pTemp->bNotifyOnlyError = 0;
	pTemp->dwMessageID = pQuery->dwMessageID;							// 어떤 쿼리가 호출되었는가 구별
	pTemp->dwID = pQuery->dwID;											// 특별히 정의되지 않는 것
	pTemp->pData = (LPVOID) pData;
	
	
	m_pIDB->SendMessageDB(pTemp);	//프로그램 종료시에 여기서 에러남

	return TRUE;
}

void DBTH::DeletePointer(LPDBMESSAGE pMessage,DWORD ret)
{			
	m_pIDB->DeleteThisPointer(pMessage->bFNNo,(LPVOID)ret);
	m_pIDB->FreeThisMemory(pMessage,pMessage->pData);
}

//////////////////////////////////////////////////////////////////////////
			
CDataBase::CDataBase()
{
	m_pDB		= NULL;
	/// 071222 DB Thread 분리
	m_pLoginDB	= NULL;
	m_pLogDB	= NULL;
	/////////////////////////
	m_curDBNum	= 0;

	m_bInit		= FALSE;
}

CDataBase::~CDataBase()
{
	Release();
}
BOOL CDataBase::AllCleared()
{
	if(m_QueryQueue.GetCount() != 0)
		return FALSE;

	for(DWORD n=0;n<m_MaxDBThreadNum;++n)
	{
		if(m_pDB[n].m_pIDB->GetUnusedDBMemoryNum() != m_SameTimeQueryPerThread)
			return FALSE;
		if(m_pDB[n].m_pIDB->GetUnusedStackMemoryNum() != m_SameTimeQueryPerThread)
			return FALSE;
	}

	/// 071222 DB Thread 분리
	if(m_pLoginDB->m_pIDB->GetUnusedDBMemoryNum() != m_SameTimeQueryPerThread)
		return FALSE;
	if(m_pLoginDB->m_pIDB->GetUnusedStackMemoryNum() != m_SameTimeQueryPerThread)
		return FALSE;

	if(m_pLogDB->m_pIDB->GetUnusedDBMemoryNum() != m_SameTimeQueryPerThread)
		return FALSE;
	if(m_pLogDB->m_pIDB->GetUnusedStackMemoryNum() != m_SameTimeQueryPerThread)
		return FALSE;
	/////////////////////////
	return TRUE;
}
void CDataBase::Release()
{
	if(m_pDB)
	{
		while(AllCleared() == FALSE)
		{
			ProcessingQueue();
			ProcessingDBMessage();
			Sleep(50);
		}

		for(DWORD n=0;n<m_MaxDBThreadNum;++n)
			m_pDB[n].Release();
		m_MaxDBThreadNum = 0;
		delete [] m_pDB;
		m_pDB = NULL;
	}
}

DBTH* CDataBase::GetCurDB(DWORD Index)
{
	//---KES DB Process Fix 071114
	//round로
	if( m_MaxDBThreadNum )
	{
		if(Index = 0)
            m_curDBNum = (++m_curDBNum)%m_MaxDBThreadNum;
		else
			m_curDBNum = Index%m_MaxDBThreadNum;
	}
	else
		return NULL;


	return &m_pDB[m_curDBNum];
}

BOOL CDataBase::Init(DWORD MaxDBThread,DWORD MaxSameTimeQuery,BOOL bUsingLoginDB)
{
	// desc_hseos_DB접속01
	// S DB접속 추가 added by hseos 2007.06.15
	// ..SQL 접속 정보를 파일에서 읽는다. 파일이 없으면 원래 소스에서 읽던대로..
	CMHFile file;
	file.Init("chord.tmp", "rb");
	if(file.IsInited())
	{
		file.GetString(LOGINDBNAME);
		file.GetString(LOGINDBID);
		file.GetString(LOGINDBPASSWORD);

		file.GetString(LOGDBNAME);
		file.GetString(LOGDBID);
		file.GetString(LOGDBPASSWORD);

		file.GetString(GAMEDBNAME);
		file.GetString(GAMEDBID);
		file.GetString(GAMEDBPASSWORD);
	}
	// E DB접속 추가 added by hseos 2007.06.15


	m_QueryPool.Init( MaxSameTimeQuery, MaxSameTimeQuery/4, "QueryPool" );

	DESC_DBTHREAD desc;
	desc.bUsingDLL = TRUE;							// 네트웍 부문을 DLL 모듈을 쓰는가
	desc.bUsingThread = TRUE;						// 쓰레드를 사용하는가?
	
	//---KES DB Process Fix 071114
	desc.bUsingEventObject = TRUE;					// 이벤트 오브젝트를 통해 결과를 받을 건가..
	//desc.bUsingEventObject = FALSE;
	//----------------------------

	strcpy(desc.AdminCursor,"Cursor");
	strcpy(desc.AdminID,LOGINDBID);
	
	strcpy(desc.ClientCursor ,"Cursor");
	strcpy(desc.ClientID ,LOGINDBID);	

	strcpy(desc.DBName,GAMEDBNAME);
	strcpy(desc.AdminID,GAMEDBID);
	strcpy(desc.ClientID ,GAMEDBID);	
	strcpy(desc.AdminPW ,GAMEDBPASSWORD);
	strcpy(desc.ClientPW,GAMEDBPASSWORD);

	strcpy(desc.Default_User_account,"Test");
	strcpy(desc.Default_User_guid,"Test");
	strcpy(desc.Default_User_pw,"Test");
	strcpy(desc.Default_User_table,"Test");

	desc.dwDBMessageID =	WM_DB_MESSAGE;			// 메세지를 통해 결과를 받을 경우 쓸 메세지 입력

#ifdef _RMTOOL_
	desc.hWndToPostMessage = AfxGetMainWnd()->m_hWnd;
#else
	desc.hWndToPostMessage = GetActiveWindow();		// 메세지를 통해 결과를 받을 경우 메세지를 포스트할 윈도 핸들
#endif
//	desc.ReadDBResult = ReadDBResultMessage;		// 이벤트 방식을 이용할 경우 결과도착시에 call할 함수포인터
	desc.WriteMessage = WriteMessage;				// 각종 메세지 출력시에 이용할 함수포인터
	
	desc.dwMessageMaxSize	= 30;		// DBMESSAGE 구조체의 최대 크기 

	m_SameTimeQueryPerThread = 1024;//MaxSameTimeQuery/MaxDBThread;

	desc.dwNumofMessage		= m_SameTimeQueryPerThread;				// 최대 쿼리 갯수 (동시 )
	desc.dwNumofResult		= 100;//m_SameTimeQueryPerThread;		// select한 결과의 최대 개수 
	desc.dwResultMaxSize	= sizeof(QUERYST)*MAX_ROW_NUM;	// select 문 의 최대 결과 크기 
	desc.dwSizeofStack		= sizeof(DBFreeQuery);		// 함수인자를 담는 스택의 크기

	ASSERT(MaxDBThread <= 10);

	m_MaxDBThreadNum = MaxDBThread;
	m_pDB = new DBTH[m_MaxDBThreadNum];
	
	for(DWORD n=0;n<m_MaxDBThreadNum;++n)
	{
		if(n==0)	desc.ReadDBResult = ReadDBResultMessage1;
		if(n==1)	desc.ReadDBResult = ReadDBResultMessage2;
		if(n==2)	desc.ReadDBResult = ReadDBResultMessage3;
		if(n==3)	desc.ReadDBResult = ReadDBResultMessage4;
		if(n==4)	desc.ReadDBResult = ReadDBResultMessage5;
		if(n==5)	desc.ReadDBResult = ReadDBResultMessage6;
		if(n==6)	desc.ReadDBResult = ReadDBResultMessage7;
		if(n==7)	desc.ReadDBResult = ReadDBResultMessage8;
		if(n==8)	desc.ReadDBResult = ReadDBResultMessage9;
		if(n==9)	desc.ReadDBResult = ReadDBResultMessage10;

		BOOL rt = m_pDB[n].Init(&desc,bUsingLoginDB);
		ASSERT(rt);
	}

	/// 071222 DB Thread 분리
	m_pLoginDB = new DBTH;
	strcpy(desc.DBName,LOGINDBNAME);
	strcpy(desc.AdminID,LOGINDBID);
	strcpy(desc.ClientID ,LOGINDBID);	
	strcpy(desc.AdminPW ,LOGINDBPASSWORD);
	strcpy(desc.ClientPW,LOGINDBPASSWORD);

	strcpy(desc.DBName,LOGINDBNAME);
	desc.ReadDBResult = ReadLoginDBResultMessage;
	m_pLoginDB->Init(&desc,bUsingLoginDB);

	m_pLogDB = new DBTH;
	strcpy(desc.DBName,LOGDBNAME);
	strcpy(desc.AdminID,LOGDBID);
	strcpy(desc.ClientID ,LOGDBID);	
	strcpy(desc.AdminPW ,LOGDBPASSWORD);
	strcpy(desc.ClientPW,LOGDBPASSWORD);

	strcpy(desc.DBName,LOGDBNAME);
	desc.ReadDBResult = ReadLogDBResultMessage;
	m_pLogDB->Init(&desc,bUsingLoginDB);
	/////////////////////////

	m_bInit = TRUE;

	return TRUE;
}

BOOL CDataBase::AddQueryQueue(DBQUERY* pQuery, BOOL bAddQueueWhenFull, DWORD index)
{
	if(m_pDB == NULL)	return FALSE; // 이미 Release된 경우
#ifndef _MONITORINGSERVER
	if(g_bWriteQuery)
	{
		FILE* fp = fopen("DB_Query.txt","a+");
		if(fp)
		{
			fprintf(fp,"%s    %s",pQuery->strQuery,GetCurTimeToString());
			fclose(fp);
		}
	}
#endif
	/// 071222 DB Thread 분리
	//DBTH* pDB = GetCurDB();	//젤 한가한 DB

	DBTH* pDB = NULL;

	if(pQuery->dbcontype == DBTH::eDBCON_ADMIN)
	{
		pDB = GetCurDB(index);	//젤 한가한 DB
	}
	else if(pQuery->dbcontype == DBTH::eDBCON_LOGIN)
	{
		pDB = m_pLoginDB;
	}
	else if(pQuery->dbcontype == DBTH::eDBCON_LOG)
	{
		pDB = m_pLogDB;
	}
	//////////////////////////

	if(pDB == NULL)
	{
		m_QueryQueue.AddTail(pQuery);
		return TRUE;
	}

	if(pDB->Query(pQuery) == FALSE)		// 큐가 비어있다면 쿼리를 요청해보고
	{
		if(bAddQueueWhenFull)
			m_QueryQueue.AddTail(pQuery);		// 처리가 안되면 큐에 집어넣는다.
		else
			return FALSE;
	}
	else
	{
		m_QueryPool.Free(pQuery);	// 쿼리가 처리 되었다면 Free시킨다.
	}

	return TRUE;
}

BOOL CDataBase::LoginQuery(DBMsgFunc callBackFunction, DWORD ID, LPTSTR strQuery, ...)
{
	if(m_pDB == NULL)	return FALSE; // 이미 Release된 경우

	static char temp[4096];
	va_list vl;
	va_start(vl, strQuery);
	vsprintf(temp, strQuery, vl);
	va_end(vl);

	DBQUERY* pQuery = m_QueryPool.Alloc();	//
	ASSERT(pQuery);
	pQuery->SetQuery(
		DBTH::eDBCON_LOGIN,
		eQueryType_FreeQuery,
		DWORD(callBackFunction),
		temp,
		ID);

#ifdef _MONITORINGSERVER
	// 080402 LUJ, 로그
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( ID );

	if( ! user.mId.empty() )
	{
		PutLog(
			"%s(%s): %s",
			user.mId.c_str(),
			user.mName.c_str(),
			strQuery );
	}	
#endif

	return AddQueryQueue(pQuery);
}

BOOL CDataBase::LoginMiddleQuery(DBMiddleMsgFunc pReturnFunc, DWORD ID, LPCTSTR strQuery,...)
{
	static char temp[4096];
	va_list vl;
	va_start(vl, strQuery);
	vsprintf(temp, strQuery, vl);
	va_end(vl);
	
	DBQUERY* pQuery = m_QueryPool.Alloc();
	ASSERT(pQuery);
	pQuery->SetQuery(DBTH::eDBCON_LOGIN,eQueryType_FreeMiddleQuery,(DWORD)pReturnFunc,temp,ID);

	AddQueryQueue(pQuery);

#ifdef _MONITORINGSERVER
	// 080402 LUJ, 로그
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( ID );

	if( ! user.mId.empty() )
	{
		PutLog(
			"%s(%s): %s",
			user.mId.c_str(),
			user.mName.c_str(),
			temp );
	}	
#endif

	return TRUE;
}

BOOL CDataBase::LogQuery(DBMsgFunc callBackFunction, DWORD index, LPCTSTR strQuery, ...)
{
	if(m_pDB == NULL)	return FALSE; // 이미 Release된 경우

	static char temp[4096];
	va_list vl;
	va_start(vl, strQuery);
	vsprintf(temp, strQuery, vl);
	va_end(vl);

	DBQUERY* pQuery = m_QueryPool.Alloc();
	ASSERT(pQuery);
	pQuery->SetQuery(
		DBTH::eDBCON_LOG,
		eQueryType_FreeQuery,
		DWORD(callBackFunction),
		temp,
		index);

	AddQueryQueue(pQuery);

	// 080402 LUJ, 로그
	// 080929 LUJ, 로그를 표시되도록 인자 수정
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( index );

	if( ! user.mId.empty() )
	{
		PutLog(
			"%s(%s): %s",
			user.mId.c_str(),
			user.mName.c_str(),
			temp );
	}

	return TRUE;
}

BOOL CDataBase::LogMiddleQuery(DBMiddleMsgFunc pReturnFunc, DWORD ID, LPCTSTR strQuery, ... )
{
	static char temp[4096];
	va_list vl;
	va_start(vl, strQuery);
	vsprintf(temp, strQuery, vl);
	va_end(vl);

	DBQUERY* pQuery = m_QueryPool.Alloc();
	ASSERT(pQuery);
	pQuery->SetQuery(DBTH::eDBCON_LOG,eQueryType_FreeMiddleQuery,(DWORD)pReturnFunc,temp,ID);

	AddQueryQueue(pQuery);

#ifdef _MONITORINGSERVER
	// 080402 LUJ, 로그
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( ID );

	if( ! user.mId.empty() )
	{
		PutLog(
			"%s(%s): %s",
			user.mId.c_str(),
			user.mName.c_str(),
			temp );
	}	
#endif

	return TRUE;
}

BOOL CDataBase::FreeLargeQuery(DBLargeMsgFunc pReturnFunc,DWORD ID, LPCTSTR strQuery,...)
{
	if(m_pDB == NULL)	return FALSE; // 이미 Release된 경우

	static char temp[4096];
	va_list vl;
	va_start(vl, strQuery);
	vsprintf(temp, strQuery, vl);
	va_end(vl);
	
	DBQUERY* pQuery = m_QueryPool.Alloc();
	ASSERT(pQuery);
	pQuery->SetQuery(DBTH::eDBCON_ADMIN,eQueryType_FreeLargeQuery,(DWORD)pReturnFunc,temp,ID);

	AddQueryQueue(pQuery);

#ifdef _MONITORINGSERVER
	// 080402 LUJ, 로그
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( ID );

	if( ! user.mId.empty() )
	{
		PutLog(
			"%s(%s): %s",
			user.mId.c_str(),
			user.mName.c_str(),
			temp );
	}	
#endif

	return TRUE;
}

void CDataBase::FreeQuery(DBMsgFunc callBackFunction, DWORD index, LPCTSTR text, ...)
{
	if(0 == m_pDB)
	{
		return;
	}

	TCHAR query[4096] = {0};
	va_list variableIList = 0;
	va_start(
		variableIList,
		text);
	vsprintf(
		query,
		text,
		variableIList);
	va_end(
		variableIList);

	if(DBQUERY* const databaseQuery = m_QueryPool.Alloc())
	{
		databaseQuery->SetQuery(
			DBTH::eDBCON_ADMIN,
			eQueryType_FreeQuery,
			DWORD(callBackFunction),
			query,
			index);
		AddQueryQueue(
			databaseQuery);

		const CUserManager::User& user = CUserManager::GetInstance().GetUser(
			index);

		if( ! user.mId.empty() )
		{
			PutLog(
				"%s(%s): %s",
				user.mId.c_str(),
				user.mName.c_str(),
				query);
		}
	}
}

BOOL CDataBase::FreeMiddleQuery(DBMiddleMsgFunc pReturnFunc,DWORD ID, LPCTSTR strQuery, ...)
{
	if(m_pDB == NULL)	return FALSE; // 이미 Release된 경우
	
	static char temp[4096];
	va_list vl;
	va_start(vl, strQuery);
	vsprintf(temp, strQuery, vl);
	va_end(vl);
	
	DBQUERY* pQuery = m_QueryPool.Alloc();
	ASSERT(pQuery);
	pQuery->SetQuery(DBTH::eDBCON_ADMIN,eQueryType_FreeMiddleQuery,(DWORD)pReturnFunc,temp,ID);

	AddQueryQueue(pQuery);

#ifdef _MONITORINGSERVER
	// 080402 LUJ, 로그
	const CUserManager::User& user = CUserManager::GetInstance().GetUser( ID );

	if( ! user.mId.empty() )
	{
		PutLog(
			"%s(%s): %s",
			user.mId.c_str(),
			user.mName.c_str(),
			temp );
	}	
#endif

	return TRUE;
}

// 항상 호출되어야 하는 것
void CDataBase::ProcessingDBMessage()
{
	if( !m_bInit )
		return;

	for(DWORD n=0;n<m_MaxDBThreadNum;++n)
	{
		m_pDB[n].m_pIDB->ProcessingDBMessage();
	}

	if( m_MaxDBThreadNum > 0 )
	{
		m_pLoginDB->m_pIDB->ProcessingDBMessage();
		m_pLogDB->m_pIDB->ProcessingDBMessage();
	}
	/////////////////////////

	//---KES DB Process Fix 071114
	//밀린것 처리를 항상 시도해 본다.
	ProcessingQueue();
}

void CDataBase::ProcessingQueue()
{
	while(m_QueryQueue.GetCount() != 0)
	{		
		DBQUERY* pQuery = (DBQUERY*)m_QueryQueue.GetHead();
		
		/// 071222 DB Thread 분리
		//DBTH* pDB = GetCurDB();	//젤 한가한 DB

		DBTH* pDB = NULL;

		if(pQuery->dbcontype == DBTH::eDBCON_ADMIN)
		{
			pDB = GetCurDB();	//젤 한가한 DB
		}
		else if(pQuery->dbcontype == DBTH::eDBCON_LOGIN)
		{
			pDB = m_pLoginDB;
		}
		else if(pQuery->dbcontype == DBTH::eDBCON_LOG)
		{
			pDB = m_pLogDB;
		}
		///////////////////////////

		if( pDB == NULL ) break;

		if( pDB->Query(pQuery) )
		{
			m_QueryQueue.RemoveHead();
			m_QueryPool.Free(pQuery);
		}
		else
		{
			break;
		}
	}
}

void ReadDBResultMessage(int ThreadNum,DWORD ret, LPDBMESSAGE pMessage)
{
	switch(pMessage->bFNNo)
	{
	case eQueryType_FreeQuery:
		{
			if( ret && pMessage->dwMessageID)
				((DBMsgFunc)pMessage->dwMessageID)((LPQUERY)ret, pMessage);
		}
		break;
	case eQueryType_FreeMiddleQuery:
		{
			if( ret && pMessage->dwMessageID)
				((DBMiddleMsgFunc)pMessage->dwMessageID)((LPMIDDLEQUERY)ret, pMessage);
			/// 071222 DB Thread 분리
			//g_DB.m_pDB[ThreadNum].DeletePointer(pMessage,ret);
			/////////////////////////
		}
		break;
	case eQueryType_FreeLargeQuery:
		{
			if( ret && pMessage->dwMessageID)
				((DBLargeMsgFunc)pMessage->dwMessageID)((LPLARGEQUERY)ret, pMessage);
			/// 071222 DB Thread 분리
			//g_DB.m_pDB[ThreadNum].DeletePointer(pMessage,ret);
			/////////////////////////
		}
		break;
	default:
		{
			ASSERT(0);
		}
	}

	/// 071222 DB Thread 분리
	if( ThreadNum == 100 )
	{
		g_DB.m_pLoginDB->DeletePointer(pMessage,ret);
	}
	else if( ThreadNum == 200 )
	{
		g_DB.m_pLogDB->DeletePointer(pMessage,ret);
	}
	else
	{
		g_DB.m_pDB[ThreadNum].DeletePointer(pMessage,ret);
	}
	/////////////////////////
}

#ifdef _MONITORINGSERVER
char* GetCurTimeToString()
{
	static char t[2] = {0,};
	return t;
}
#endif

void  WriteMessage(char* pmsg)
{
	SYSTEMTIME time;
	char szFile[256] = {0,};
	GetLocalTime( &time );

	// 080311 LUJ, 복구툴에 로그 표시
#ifdef _MONITORINGSERVER
	PutLog( pmsg );
#endif

	// 080225 LUJ, 복구 툴 서버의 릴리즈 버전 컴파일을 위해 매크로 비교문 추가
	// _MONITORINGSERVER를 복구툴 서버의 매크로로 쓰고 있음
#ifndef _MONITORINGSERVER

#ifdef _MAPSERVER_	
	sprintf( szFile, "./Log/DB_ERROR_LOG_%02d_%4d%02d%02d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay );
#else
	sprintf( szFile, "./Log/DB_ERROR_LOG_%4d%02d%02d.txt", time.wYear, time.wMonth, time.wDay );
#endif

	g_Console.LOG(2, pmsg);

	FILE* fp = fopen(szFile,"a+");
	fprintf(fp,"%s, time:%s\n",pmsg, GetCurTimeToString());
	fclose(fp);	
#endif

	char cmp[64] = "SqlState: 08S01";
	if( strstr( pmsg, cmp ) )
	{
#ifndef _MONITORINGSERVER
		SendMessage( g_hWnd, WM_CLOSE, 0, 0 );
//		g_bCloseWindow = TRUE;
#endif
		return;
	}
}



void castingdelete(LPVOID pTarget1, LPVOID pTarget2, int nNum)
{
	switch(nNum)
	{
	case 1:
		delete (DBDATA1*) pTarget1;
		delete (DBDATA1*) pTarget2;
		return;
	case 2:
		delete (DBDATA2*) pTarget1;
		delete (DBDATA2*) pTarget2;
		return;
	case 3:
		delete (DBDATA3*) pTarget1;
		delete (DBDATA3*) pTarget2;
		return;
	case 4:
		delete (DBDATA4*) pTarget1;
		delete (DBDATA4*) pTarget2;
		return;
	case 5:
		delete (DBDATA5*) pTarget1;
		delete (DBDATA5*) pTarget2;
		return;
	case 6:
		delete (DBDATA6*) pTarget1;
		delete (DBDATA6*) pTarget2;
		return;
	case 7:
		delete (DBDATA7*) pTarget1;
		delete (DBDATA7*) pTarget2;
		return;
	case 8:
		delete (DBDATA8*) pTarget1;
		delete (DBDATA8*) pTarget2;
		return;
	case 9:
		delete (DBDATA9*) pTarget1;
		delete (DBDATA9*) pTarget2;
		return;
	case 10:
		delete (DBDATA10*) pTarget1;
		delete (DBDATA10*) pTarget2;
		return;
		
	}
}
