/*********************************************************************

	 파일		: SHGroup.h
	 작성자		: hseos
	 작성일		: 2007/07/10

	 파일설명	: CSHGroup 클래스의 헤더

 *********************************************************************/

#pragma once

#include "../Group/SHGroup.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHFamilyMember
//
class CSHFamilyMember : public CSHGroupMember 
{
public:
	//----------------------------------------------------------------------------------------------------------------
	struct stINFO_EX																			// 멤버 구조
	{
		char			szGuild[MAX_NAME_LENGTH+1];												// ..길드
	};

private:
	//----------------------------------------------------------------------------------------------------------------
	stINFO_EX			m_stInfoEx;																// 추가 정보

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFamilyMember();
	virtual ~CSHFamilyMember();
	void			Set(stINFO* pstInfo)							{ CSHGroupMember::Set(pstInfo); }
	void			Set(stINFO* pstInfo, stINFO_EX* pstInfoEx)		{ CSHGroupMember::Set(pstInfo); m_stInfoEx = *pstInfoEx; }
	void			SetEx(stINFO_EX* pstInfoEx)						{ m_stInfoEx = *pstInfoEx; }
	//				추가 정보 얻기
	stINFO_EX*		GetEx()											{ return &m_stInfoEx; }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHFamily
//
class CSHFamily : public CSHGroup
{
public:
	//----------------------------------------------------------------------------------------------------------------
	static const int	MAX_MEMBER_NUM	=	5;													// 최대 맴버 수

	struct stINFO_EX																			// 패밀리 구조
	{
		DWORD			nHonorPoint;															// ..명예포인트
		DWORD			nHonorPointTimeTick;													// ..명예포인트 시간 틱
		int				nNicknameON;															// ..호칭ON
	};

private:
	//----------------------------------------------------------------------------------------------------------------
	stINFO_EX			m_stInfoEx;																// 추가 정보
	CSHFamilyMember		m_pcsMember[MAX_MEMBER_NUM];
	DWORD				m_nEmblemChangedNum;													// 문장 변경 회수

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFamily();
	CSHFamily(DWORD nMasterID, char* pszName);
	// 090915 LUJ, 파생 클래스의 소멸자가 호출되도록 수정
	virtual ~CSHFamily();
	VOID				Set(CSHFamily* pcsFamily)
						{
							Set(pcsFamily->Get(), pcsFamily->GetEx());
							for(UINT i=0; i<pcsFamily->Get()->nMemberNum; i++)
							{
								m_pcsMember[i].Set(pcsFamily->GetMember(i)->Get(), pcsFamily->GetMember(i)->GetEx());
							}
							m_nEmblemChangedNum = pcsFamily->GetEmblemChangedNum();
						}

	VOID				Set(stINFO* pstInfo)										{ CSHGroup::Set(pstInfo); }
	VOID				Set(stINFO* pstInfo, stINFO_EX* pstInfoEx)					{ CSHGroup::Set(pstInfo); m_stInfoEx = *pstInfoEx; }
	VOID				SetEx(stINFO_EX* pstInfoEx)									{ m_stInfoEx = *pstInfoEx; }
	stINFO_EX*			GetEx()														{ return &m_stInfoEx; }

	//					멤버 추가
	VOID				AddMember(CSHFamilyMember* pcsMember)
						{
							m_pcsMember[m_stInfo.nMemberNum].Set(pcsMember->Get(), pcsMember->GetEx()); 
							m_stInfo.nMemberNum++; 
						}
	//					멤버 삭제
	VOID				DelMember(DWORD nMemberID);
	//					멤버 설정
	VOID				SetMember(CSHFamilyMember* pcsMember, int nIndex)			{ m_pcsMember[nIndex].Set(pcsMember->Get(), pcsMember->GetEx()); }
	//					멤버 얻기
	CSHFamilyMember*	GetMember(int nIndex)										{ return &m_pcsMember[nIndex]; }
	CSHFamilyMember*	GetMemberFromID(DWORD nID)
						{
							for(UINT i=0; i<Get()->nMemberNum; i++)
							{
								if (m_pcsMember[i].Get()->nID == nID)
								{
									return &m_pcsMember[i];
								}
							}
							return NULL;
						}

	//					문장 변경 회수 얻기
	VOID				IncreaseEmblemChangedNum()									{ m_nEmblemChangedNum++; }
	//					문장 변경 회수 얻기
	DWORD				GetEmblemChangedNum()										{ return m_nEmblemChangedNum; }

	//----------------------------------------------------------------------------------------------------------------
	//					테이블에서의 인덱스 얻기
	int					GetIndexAtTbl()												{ return m_nIndexAtTbl; }
};