// MSSystem.h: interface for the CMSSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSSYSTEM_H__88388F7A_0177_4524_AAE2_9C7E947CDC2C__INCLUDED_)
#define AFX_MSSYSTEM_H__88388F7A_0177_4524_AAE2_9C7E947CDC2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMSSystem  
{
	struct KillServerInfo
	{
		WORD		wServerType;
		WORD		wServerNum;
		__time64_t	KillTime;
	};

public:
	WORD m_wServerType;
	BOOL m_bInit;

	// 100513 ONS 서버가 다운되었을 경우, 1분후 프로세스를 종료시키고, 다시 1분후 해당서버 ON
	typedef std::map<std::pair<WORD, WORD>, KillServerInfo>		KillServerMap;

public:
	CMSSystem();
	virtual ~CMSSystem();
	
	// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
	static void __stdcall OnAcceptServer(DWORD dwConnectionIndex);
	static void __stdcall OnDisconnectServer(DWORD dwConnectionIndex);
	static void __stdcall ReceivedMsgFromServer(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
	static void __stdcall ReceivedMsgFromMS(DWORD dwConnectionIndex,char* pmsg,DWORD dwLength);
	static void __stdcall ReceivedMsgFrom2Server(DWORD dwConnectionIndex,char* pmsg,DWORD dwLength);
	static void __stdcall OnConnectMASSuccess(DWORD dwConnectionIndex, void* pVoid);
	static void __stdcall OnConnectMASFail(void* pVoid);

	void Start(WORD wType);
	void End();

	// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
	static void __stdcall Process( DWORD eventIndex );

	//mas only
	// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
	static void __stdcall OnAcceptUser(DWORD dwConnectionIndex);
	static void __stdcall ReceivedMsgFromUser(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
	static void __stdcall OnDisconnectUser(DWORD dwConnectionIndex);

	// 복구 TOOL only
	// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
	static void __stdcall ReceivedMsgFromRMTool(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);
	static void __stdcall ConnentMsgParse(DWORD dwConnectionIndex,char* pMsg,DWORD dwLength);

	// autopatch
	static void AutoPatchProcess();

	// 080813 LUJ, 수정된 inetwork.dll에 맞춰 호출 형식 변경
	static void __stdcall OnConnectServerSuccess(DWORD dwConnectionIndex, void* pVoid);
	static void __stdcall OnConnectServerFail(void* pVoid);

};
extern CMSSystem * g_pMSSystem;


#endif // !defined(AFX_MSSYSTEM_H__88388F7A_0177_4524_AAE2_9C7E947CDC2C__INCLUDED_)
