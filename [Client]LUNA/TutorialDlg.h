#pragma once

#include "./Interface/cDialog.h"											// 다이얼로그 인터페이스를 포함한다.

class cWindow;
class cImageSelf;
class cListDialog;
class cTutorial;
struct SUB_TUTORIAL;

class cTutorialDlg : public cDialog											// 튜토리얼 다이얼로그 클래스.
{
	cListDialog* m_pSpeechList;
	cWindow* m_pOkBtn;
	cWindow* m_pPrevBtn;
	cWindow* m_pNextBtn;
	cWindow* m_pPassBtn;
	cWindow* m_pSpeechBack;
	cTutorial* m_pTutorial;
	SUB_TUTORIAL* m_pCurSubTutorial;
	int	m_nCurSubIndex;
	int	m_nTotalSubCount;
	cImageSelf*	m_NpcImage;
	int	m_nDispWidth;
	int	m_nDispHeight;
	int	m_nBtnStartXpos;
	int	m_nBtnStartYpos;
	VECTOR2	m_vImagePos;
	// 100111 LUJ, 라인 당 표시되는 글자 수
	size_t mCharacterSizePerLine;
	// 100111 LUJ, 창이 없어지는 틱카운트
	DWORD mInvisibleTick;
	DWORD mWishedAlpha;

public:
	cTutorialDlg(void);
	virtual ~cTutorialDlg(void);
	void Linking();
	virtual void Render();
	virtual void SetActive(BOOL);
	virtual void OnActionEvent(LONG lId, void* p, DWORD we);
	void Set_TutorialInfo(cTutorial*);
	void Start_Tutorial();
	void LoadNpcImage();
	void Prepare_Tutorial();
	void DeActiveAllBtn();
	void ActiveBtns();
	BOOL IsSameEndNpc(DWORD dwNpcIdx);
	BOOL IsHaveNextSubTutorial();
	void StartNextSubTutorial();
	void UpdateTutorial();
	SUB_TUTORIAL* GetCurSubTutorial();
	void SetNpcImage(LPCTSTR);
	void SetText(LPCTSTR);
	void SetInvisibleSecond(DWORD second);
};
