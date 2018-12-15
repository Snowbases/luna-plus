// TrafficLog.h: interface for the CTrafficLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAFFICLOG_H__3B802B9B_36F2_496B_9A01_B2D1386084B5__INCLUDED_)
#define AFX_TRAFFICLOG_H__3B802B9B_36F2_496B_9A01_B2D1386084B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TRAFFIC	USINGTON(CTrafficLog)
#define MAX_LOG_BUFFER_SIZE		102400		// 100KB
class CTrafficLog  
{
public:
	CTrafficLog();
	virtual ~CTrafficLog();

//KES
protected:

	DWORD	m_dwStartTime;			

	DWORD	m_dwReceive[2][MP_MAX][256];
	DWORD	m_dwSend[2][MP_MAX][256];


public:
	void StartRecord();
	void EndRecord();
	void AddReceivePacket( DWORD dwCategory, DWORD dwLength );
	void AddSendPacket( DWORD dwCategory, DWORD dwLength );

	void AddReceivePacketAll( WORD category, WORD Protocol, DWORD dwLength );
	void AddSendPacketAll( WORD category, WORD Protocol, DWORD dwLength );

	void Process();
	void WriteTrafficLogFile();
	void Clear();

// 06. 01. 유저별 트래픽 로그 기록 - 이영준
private:
	DWORD m_dwCheckTime;
	DWORD m_dwUnValuedCount;
	DWORD m_dwValuedCount;

	struct USERTRAFFIC{
		DWORD dwConnectionIndex;
		DWORD dwUserID;
		
		DWORD dwLoginTime;
		DWORD dwTotalPacketCount;
		
		DWORD dwValuedCount;
		DWORD dwUnValuedCount;
		
		bool  bLogin;
	};

	CYHHashTable<USERTRAFFIC>		m_UserTrafficTable;
	// 090923 ONS 메모리풀 교체
	CPool<USERTRAFFIC>*				m_pUserTrafficPool;

	DWORD m_dwLastPacketCheckTime;
	DWORD m_dwLastPacketWriteTime;

	void ResetUserPacket(DWORD dwTime);
	void WriteUserTrafficLogFile();

public:
	void Init();
	void End();
	void AddUser(DWORD dwUserID, DWORD dwConnectionIndex);
	void RemoveUser(DWORD dwUserID);
	void AddUserPacket(DWORD dwUserID, BYTE Category);
};
EXTERNGLOBALTON(CTrafficLog);

#endif // !defined(AFX_TRAFFICLOG_H__3B802B9B_36F2_496B_9A01_B2D1386084B5__INCLUDED_)

