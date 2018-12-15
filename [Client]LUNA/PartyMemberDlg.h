// PartyMemberDlg.h: interface for the CPartyMemberDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTYMEMBERDLG_H__7426A94E_644C_44BE_80FE_877AE4A137B7__INCLUDED_)
#define AFX_PARTYMEMBERDLG_H__7426A94E_644C_44BE_80FE_877AE4A137B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"

class CObjectGuagen;
class cPushupButton;
class cStatic;

// 080318 LUJ, 파티원 버프 스킬 표시위한 클래스 참조
class CStatusIconDlg;
class cCheckBox;

class CPartyMemberDlg  : public cDialog
{
	cPushupButton * m_pName;
	CObjectGuagen * m_pLife;
	CObjectGuagen * m_pMana;
	cStatic		  * m_pLevel;
	// 091127 LUJ, 파티 마스터 아이콘을 표시한다
	cImage			m_MasterMark;

public:

	DWORD m_MemberID;
	
	CPartyMemberDlg();
	virtual ~CPartyMemberDlg();
	
	void SetActive(BOOL val);
	void Linking(int i);
	void SetMemberData(PARTY_MEMBER*);
	void SetNameBtnPushUp(BOOL);
	virtual DWORD ActionEvent(CMouse*);
	virtual void Render();
	// 080318 LUJ, 파티창이 표시하고 있는 플레이어 번호를 반환한다
	inline DWORD GetPlayerIndex() const { return m_MemberID; }

	// 080318 LUJ,	버프 아이콘 표시 위치 갱신 위해 오버라이딩.
	//				주의: SetAbsX(), SetAbsY() 등의 함수가 있는데 사용하지 않아 오버라이딩하지 않았다
	virtual void SetAbsXY( LONG x, LONG y);

	// 080318 LUJ, 버프 스킬을 표시 관리
	void SetVisibleSkill( BOOL );
	BOOL IsVisibleSkill() const;
	void SetLife(float);
	void SetMana(float);
	void SetLevel(LONG);

private:
	void SetTip(const PARTY_MEMBER&);

	// 080318 LUJ, 파티원 버프 스킬 표시 관리
private:	
	CStatusIconDlg* mStatusIconDialog;
public:
	inline CStatusIconDlg* GetStatusIconDialog() const { return mStatusIconDialog; }

	// 080318 LUJ, 버프 스킬 토글 위한 컨트롤
private:	
	cCheckBox* mCheckBox;
	VECTOR2 mMasterMarkPosition;
};

#endif // !defined(AFX_PARTYMEMBERDLG_H__7426A94E_644C_44BE_80FE_877AE4A137B7__INCLUDED_)
