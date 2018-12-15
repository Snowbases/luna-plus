// UserIPCheckMgr.h: interface for the CUserIPCheckMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERIPCHECKMGR_H__BC37305F_9CD0_4C52_9F1D_948D50952626__INCLUDED_)
#define AFX_USERIPCHECKMGR_H__BC37305F_9CD0_4C52_9F1D_948D50952626__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IPCHECKMGR USINGTON(CUserIPCheckMgr)
#define	MAX_IPARRAY	100

class CUserIPCheckMgr  
{
public:
	CUserIPCheckMgr();
	virtual ~CUserIPCheckMgr();
	BOOL CheckIP( DWORD connectionIndex );
	// 090511 LUJ, 사용자를 추가함
	void AddUser( DWORD connectionIndex, const OPERATOR&, const IPINFO& );
	void RemoveUser( DWORD connectionIndex );
	// 090511 LUJ, 해당 접속자의 권한을 반환하도록 함
	BOOL IsInvalidProtocol( DWORD connectionIndex, MP_PROTOCOL_MORNITORTOOL );
	void Filter( DWORD connectionIndex, MSGMONITORTOOLALLSTATE& );

private:
	DWORD mUserSize;
	inline DWORD GetUserSize() const { return mUserSize; }
	LPCTSTR GetIpAddress( DWORD connectionIndex ) const;
	struct User
	{
		DWORD dwIdx;
		char sID[ MAX_NAME_LENGTH + 1 ];
		eOPERPOWER mPower;
		char mIpAddress[ 32 ];

		User() :
		dwIdx( 0 ),
		mPower( eOperPower_Max )
		{
			ZeroMemory( sID, sizeof( sID ) );
			ZeroMemory( mIpAddress, sizeof( mIpAddress ) );
		}
	}
	mUserArray[ MAX_IPARRAY ];
	User& GetUser( DWORD connectionIndex );
};

EXTERNGLOBALTON(CUserIPCheckMgr)

#endif // !defined(AFX_USERIPCHECKMGR_H__BC37305F_9CD0_4C52_9F1D_948D50952626__INCLUDED_)
