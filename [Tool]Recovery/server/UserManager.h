#pragma once


class CUserManager  
{
public:	
	CUserManager();
	virtual ~CUserManager();

	static CUserManager& GetInstance();

	bool IsConnect( DWORD connectionIndex ) const;
	bool IsConnectedUser( const char* id ) const;
	bool IsConnectedIp( const char* ip ) const;


public:
	struct User
	{
		eGM_POWER	mPower;
		std::string	mId;
		std::string	mName;
		std::string mIp;
		std::string	mRegistedDate;
	};

	bool		AddUser( DWORD connectionIndex, const char* id, const char* name, const char* ip, const char* registedDate, eGM_POWER );
	void		RemoveUser( DWORD connectionIndex );
	const User& GetUser( DWORD connectionIndex )	const;
	const User& GetUser( const char* id )			const;

    
protected:
	// 키: 접속 인덱스
	typedef stdext::hash_map< DWORD, User >	UserMap;
	UserMap									mUserMap;


	// 사용 권한 관리
public:
	typedef std::list< MSG_RM_AUTHORITY::Authority > AuthorityList;
	void SetAuthority( eGM_POWER, const AuthorityList& );
	void SetAuthority( eGM_POWER, const MSG_RM_AUTHORITY& );
	bool IsAuthorize( RecoveryCategory, BYTE protocol, DWORD connectionIndex ) const;	
private:	
	typedef	LONGLONG	MessageKey;
	// 프로토콜에 따라 사용가능한 권한이 정해져있음. 귀찮지만 모든 프로토콜이 정해져야함.
	typedef stdext::hash_set< MessageKey >					MessageKeySet;
	typedef stdext::hash_map< eGM_POWER, MessageKeySet >	AuthorityMap;
	AuthorityMap mAuthorityMap;
protected:
	// 두 값을 받아 고유한 키를 생성한다
	MessageKey GetMessageKey( RecoveryCategory, BYTE protocol ) const;
};