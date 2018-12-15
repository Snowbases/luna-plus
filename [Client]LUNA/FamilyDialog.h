#pragma once

#define MAX_MEMBER_COUNT 5

#include "INTERFACE\cDialog.h"																		// 다이얼로그 해더 파일을 불러온다.

class cButton;
class cListDialog;
class cStatic;
class cImage;
class CSHFamily;
class CSHFamilyMember;
class CFamily;

class CFamilyDialog : public cDialog  
{
	cStatic*	m_pFamilyName;																		// 패밀리 이름.
	cStatic*	m_pMasterName;																		// 마스터 이름.
	cStatic*	m_pHonorPoint ;																		// 명예 포인트.

	cListDialog*	m_pLvListDlg;																	// 멤버 Lv 정보 리스트 다이얼로그.
	cListDialog*	m_pIDListDlg;																	// 멤버 ID 정보 리스트 다이얼로그.

	cStatic*	m_pMemberID ;																		// 멤버 아이디.
	cStatic*	m_pMemberLevel ;																	// 멤버 레벨.
	cStatic*	m_pMemberClass ;																	// 멤버 클래스.
	cStatic*	m_pMemberLogin ;																	// 멤버 로그인.
	cStatic*	m_pMemberNickName ;																	// 멤버 호칭.
	cStatic*	m_pMemberGuildName ;																// 멤버 길드명.

	cButton*	m_pChangeNickName ;																	// 호칭변경 버튼.
	cButton*	m_pInvite ;																			// 초대신청 버튼.
	cButton*	m_pRegistMark ;																		// 문장등록 버튼.

	// desc_hseos_패밀리01
	// S 패밀리 추가 added by hseos 2007.10.22
	cButton*	m_pLeaveBtn;
	cButton*	m_pExpelBtn;
	cButton*	m_pFarmManageBtn;
	// 091204 ONS 패밀리 다이얼로그에 패밀리장 이양 및 농장세금 납부 기능 버튼 추가
	cButton*	m_pPayTaxBtn;
	cButton*	m_pTransferBtn;
	cImage		m_pClassImg[MAX_MEMBER_COUNT];
	cImage		m_pLongInImg[MAX_MEMBER_COUNT];
	cImage		m_pLogOutImg[MAX_MEMBER_COUNT];
	int			m_nCurSelectIdx;

public:
	CFamilyDialog();
	virtual ~CFamilyDialog();
	void Linking();
	void UpdateAllInfo();
	void SetMemberList(CSHFamily*);
	void SetMemberInfo(CSHFamilyMember*);
	virtual void Render();
	virtual DWORD ActionEvent(CMouse* mouseInfo);
	void OnActionEvent( LONG lId, void* p, DWORD we );
	int GetSelectedMemberIdx() const { return m_nCurSelectIdx; }
	void SetHonorPoint(DWORD dwPoint);
	int GetClassIconNum(int nClass);
	void SelectMemberOfList(int nIndex);
};