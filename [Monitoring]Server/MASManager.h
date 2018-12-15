// MASManager.h: interface for the CMASManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MASMANAGER_H__7D213F53_3D21_4CE7_AA50_F5B5A366D352__INCLUDED_)
#define AFX_MASMANAGER_H__7D213F53_3D21_4CE7_AA50_F5B5A366D352__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MASMGR USINGTON(CMASManager)

#include "Hashtable.h"
#include <sys/stat.h>

#define SMSRECEIVER_FILE	"SMSReceiver.ini"

struct stSMSReceiver
{
	char Name[33];					// 수신자 이름
	char Phone1[5];					// 수신자 수신번호 000-XXXX-XXXX
	char Phone2[5];					// 수신자 수신번호 XXX-0000-XXXX
	char Phone3[5];					// 수신자 수신번호 XXX-XXXX-0000
	char SenderName[33];			// 송신자 이름
	char SenderPhone1[5];			// 송신자 회신번호 000-XXXX-XXXX
	char SenderPhone2[5];			// 송신자 회신번호 XXX-0000-XXXX
	char SenderPhone3[5];			// 송신자 회신번호 XXX-XXXX-0000
	char Msg[256];					// 송신 메세지
};

struct stServerInfo
{
	WORD ServerType;
	WORD ServerNum;
	WORD ServerPort;
	char ServerIP[MAX_IPADDRESS_SIZE];
};

class CMASManager  
{
	CYHHashTable<SERVERSTATE>	m_ServerStateTable;

	// 090623 ShinJS --- SMS Receiver 정보
	CYHHashTable<stSMSReceiver>	m_SMSReceiverTable;
	struct stat					m_SMSFileStat;

public:
	CMASManager();
	virtual ~CMASManager();

	void LoadMapServerInfo();
	void AddServerState(SERVERSTATE * NewState, DWORD key);
	SERVERSTATE * GetServerState(DWORD key);

	void SetStateInitForConn(DWORD dwConnectionIdx);

	void Release();
	void SendServerStateToMCs(DWORD key);
	void SendServerStateToMC(DWORD dwConnectionIdx, DWORD key);
	void SendServerStateAllToMC(DWORD dwConnectionIdx);
	void SendServerStateBootListToMS(DWORD dwConnectionIdx, WORD ServerTypeOrig, WORD ServerPortOrig);
	void SendServerStateAllToMCAll();
	void SendMSServerStateAllToMCAll();

	void	SendToAllMC( char* pMsg, DWORD dwLength );

	// 090623 ShinJS --- SMS Receiver 정보
    void LoadSMSReceiverInfo();
	void CheckSMSReceiverFile();
	void SetSMSTableHeadPos()										{ m_SMSReceiverTable.SetPositionHead(); }
	stSMSReceiver* GetSMSTableData()								{ return (stSMSReceiver*)m_SMSReceiverTable.GetData(); }
	// 090706 ShinJS --- SMSReceiver 정보 제거 추가
	void RemoveReceiverInfo();
};
EXTERNGLOBALTON(CMASManager)
#endif // !defined(AFX_MASMANAGER_H__7D213F53_3D21_4CE7_AA50_F5B5A366D352__INCLUDED_)
