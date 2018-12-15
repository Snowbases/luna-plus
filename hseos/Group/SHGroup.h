/*********************************************************************

	 파일		: SHGroup.h
	 작성자		: hseos
	 작성일		: 2007/07/10

	 파일설명	: CSHGroup 클래스의 헤더

 *********************************************************************/

#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHGroupMember
//
class CSHGroupMember
{
public:
	//----------------------------------------------------------------------------------------------------------------
	enum MEMBER_CONSTATE																		// 멤버 접속 상태
	{
		MEMBER_CONSTATE_LOGOFF = 0,																// ..접속 안 함
		MEMBER_CONSTATE_LOGIN,																	// ..접속 중
	};

	struct stINFO																				// 멤버 구조
	{
		DWORD			nID;																	// ..ID
		int				nRace;																	// ..종족
		int				nSex;																	// ..성별
		int				nJobFirst;																// ..직업(처음)
		int				nJobCur;																// ..직업(현재)
		int				nJobGrade;																// ..직업등급
		int				nLV;																	// ..레벨
		MEMBER_CONSTATE	eConState;																// ..접속 상태
		char			szNickname[MAX_NAME_LENGTH+1];											// ..호칭
		char			szName[MAX_NAME_LENGTH+1];												// ..이름
	};

protected:
	//----------------------------------------------------------------------------------------------------------------
	stINFO				m_stInfo;																// 정보

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHGroupMember();
	virtual ~CSHGroupMember();
	void			Set(stINFO* pstInfo)							{ m_stInfo = *pstInfo; }
	//				정보 얻기
	stINFO*			Get()											{ return &m_stInfo; }
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHGroup
//
class CSHGroup
{
public:
	//----------------------------------------------------------------------------------------------------------------
	struct stINFO																				// 그룹 구조
	{
		DWORD					nID;															// ..ID
		char					szName[MAX_NAME_LENGTH+1];										// ..이름
		DWORD					nMasterID;														// ..마스터ID
		char					szMasterName[MAX_NAME_LENGTH+1];								// ..마스터 이름
		UINT					nMemberNum;														// ..멤버 수
	};

protected:
	//----------------------------------------------------------------------------------------------------------------
	stINFO						m_stInfo;														// 정보
	CSHGroupMember*				m_pcsMember;													// 멤버
	UINT						m_nMemberNumMax;												// 최대 멤버 수

	//----------------------------------------------------------------------------------------------------------------
	UINT						m_nIndexAtTbl;													// 테이블에서의 인덱스

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHGroup();
	CSHGroup(DWORD nMasterID, char* pszName);
	// 090915 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CSHGroup();
	VOID				Set(stINFO* pstInfo)	{ m_stInfo = *pstInfo; }
	//					정보 얻기
	stINFO*				Get()					{ return &m_stInfo; }

	//					멤버 추가
	VOID				AddMember(CSHGroupMember* pcsMember)		
						{
							m_pcsMember[m_stInfo.nMemberNum].Set(pcsMember->Get()); 
							m_stInfo.nMemberNum++; 
						}
	//					멤버 설정
	VOID				SetMember(CSHGroupMember* pcsMember, int nIndex)	{ m_pcsMember[nIndex].Set(pcsMember->Get()); }
	//					멤버 얻기
	CSHGroupMember*		GetMember(int nIndex)								{ return &m_pcsMember[nIndex]; }
	UINT				GetMemberNumMax()									{ return m_nMemberNumMax; }

	//----------------------------------------------------------------------------------------------------------------
	//					테이블에서의 인덱스 설정
	VOID				SetIndexAtTbl(int nIndex)							{ m_nIndexAtTbl = nIndex; }
	//					테이블에서의 인덱스 얻기
	UINT				GetIndexAtTbl()										{ return m_nIndexAtTbl; }
};