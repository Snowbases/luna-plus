// PartyDialog.h: interface for the CPartyDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTYDIALOG_H__B103D700_3CF5_4F20_8828_16E4212A76C2__INCLUDED_)
#define AFX_PARTYDIALOG_H__B103D700_3CF5_4F20_8828_16E4212A76C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatusIconDlg.h"

class CPartyMemberDlg;
class CPartyBtnDlg;


class CPartyDialog 
{
	CPartyMemberDlg * m_MemberDlg[MAX_PARTY_LISTNUM-1];
	CPartyBtnDlg * m_PartyBtnDlg;
	BOOL m_bActive;
	
	DWORD m_ClickedMemberID;
	
	BOOL	m_bShowMember;
	BOOL	m_bOption;


	// 090121 NYJ - 파티원버프스킬 리스트 저장구조체
	typedef std::list< StatusInfo >			SkillList;
	typedef std::map< DWORD, SkillList >	SkillMap;
	SkillMap								m_skillMap;

public:
	CPartyDialog();
	virtual ~CPartyDialog();

	void RegistMemberDlg(int i, CPartyMemberDlg* pDlg);
	void RegistBtnDlg(CPartyBtnDlg* pDlg);

	BOOL IsActive();
	void SetActive(BOOL bState);
	void RefreshDlg();
		
	void MemberBtnPushUp(DWORD id);

	void SetClickedMemberID(DWORD id);
	DWORD GetClickedMemberID();
	const PARTY_MEMBER& GetClickedMember();
	CPartyMemberDlg* GetMemberDlg( BYTE index ) ; 

	// 080318 LUJ, 파티원 버프 스킬 표시 추가/삭제
	void AddSkill( const SKILLOBJECT_INFO& );
	void RemoveSkill( const SKILLOBJECT_INFO& );
	void RemoveAllSkill( DWORD playerIndex );
	// 080613 LUJ, 파티 버프 표시 위치를 다시 설정한다. 버프 아이콘이 많을 수록 처리 시간이 지연되므로, 프로세스 단에서는 호출하지 않도록 하자.
	void RelocateSkill();

	// 080318 LUJ, 파티원이 현재 클라이언트에 가진 버프를 초기화한다
	void InitializeSkill( const PARTY_MEMBER& );

	// 080318 LUJ, 파티원의 버프 스킬 표시를 토글한다
	virtual void OnActionEvent( LONG lId, void* p, DWORD we );

	// 090121 NYJ - 파티원버프스킬 리스트 백업&갱신
	void BackupMemberSkillList();
	void UpdateMemberSkillList();
	void RemoveMemberSkillList(DWORD dwPlayerIdx);
	void ClearMemberSkillList();
};

#endif // !defined(AFX_PARTYDIALOG_H__B103D700_3CF5_4F20_8828_16E4212A76C2__INCLUDED_)
