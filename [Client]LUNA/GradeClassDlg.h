#pragma once
#include "./Interface/ctabdialog.h"

#define MAX_FIGHT_CLASS_BTN	19
#define MAX_ROGUE_CLASS_BTN	18
#define MAX_MAGE_CLASS_BTN	18
#define MAX_DEVIL_CLASS_BTN 5


// 전직 가이드 튜토리얼 번호
#define GLADECLASS_TUTORIAL_NO	20

class cPushupButton;
class cStatic;
class cButton;

// 전직 트리 가이드 클래스
// 100302 ONS 전직 트리 가이드 다이얼로그를 텝으로 변경, 마족 정보 추가
class CGradeClassDlg : public cTabDialog
{
	// 전직 레벨버튼
	cButton*		m_pStaticGradeBtn[6];

	// 직업별 버튼그룹
    cPushupButton*		m_pFighterClassBtn[MAX_FIGHT_CLASS_BTN];
	cPushupButton*		m_pRogueClassBtn[MAX_ROGUE_CLASS_BTN];
	cPushupButton*		m_pMageClassBtn[MAX_MAGE_CLASS_BTN];
	cPushupButton*		m_pDevilClassBtn[MAX_DEVIL_CLASS_BTN];

public:
	CGradeClassDlg();
	virtual ~CGradeClassDlg();

	void Linking();
	void Init();

	virtual void Add( cWindow* );
	virtual void Render();
	virtual void SetActive( BOOL val );
private:
	void SetClassTree(cPushupButton** pPushBtn, ENUM_CLASS eClass);
};