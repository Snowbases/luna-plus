// ServerListManager.h: interface for the CServerListManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERLISTMANAGER_H__8C451C80_DDFB_49B3_85ED_7C45458E85B6__INCLUDED_)
#define AFX_SERVERLISTMANAGER_H__8C451C80_DDFB_49B3_85ED_7C45458E85B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../[Lib]YHLibrary/PtrList.h"

#define SERVERLISTMGR USINGTON(CServerListManager)

class CSimpleCodec
{
	FILE* m_fp;
	int m_CurCodecKeyPos;
	char* m_CodecKeybuf;
	int m_CodecKeyLen;
public:
	CSimpleCodec(char* filename,BOOL bEncoding,char* CodecKey);
	~CSimpleCodec();
	BOOL Encode(void* pBufIn,int length);
	BOOL Decode(void* pBufOut,int length);
};

class CServerListManager  
{
	int m_ServerSetNum;
	SERVERINFO mBillingInfo;
	cPtrList m_ServerInfoList;
	PTRLISTPOS m_Pos;

	void ConvertText(TCHAR*, const char*, size_t) const;

public:
	CServerListManager();
	virtual ~CServerListManager();

	BOOL Add(SERVERINFO* pInfo);
	BOOL Save(char* pFileName);
	BOOL Load(char* pFileName);
	BOOL LoadFromText(const char* pFileName);
	BOOL CheckIp(LPCTSTR pIp);
	void Clear();

	SERVERINFO* GetServerInfoByNum(DWORD ServerKind,WORD ServerNum);
	SERVERINFO* GetServerInfoByPort(DWORD ServerKind,WORD Port);
	SERVERINFO* GetServerInfoByIp(DWORD ServerKind,LPCTSTR ip);
	const SERVERINFO& GetBillingInfo() const { return mBillingInfo; }
	void SetServerSetNum(int setnum) { m_ServerSetNum = setnum;	}
	int GetServerSetNum() const { return m_ServerSetNum; }
	void SetPositionHead();
	SERVERINFO* GetNextServerInfo();
};

EXTERNGLOBALTON(CServerListManager)

#endif // !defined(AFX_SERVERLISTMANAGER_H__8C451C80_DDFB_49B3_85ED_7C45458E85B6__INCLUDED_)
