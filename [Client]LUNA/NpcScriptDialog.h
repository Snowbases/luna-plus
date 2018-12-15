#ifndef __NPCSCRIPT_DIALOG__
#define __NPCSCRIPT_DIALOG__

#include "cDialog.h"

struct ITEM_INFO;
struct HYPERLINK;
class cButton;
class cStatic;
class cDialogueList;
class cHyperTextList;
class cIconGridDialog;
class cPage;
class cListDialogEx;

// 090924 ShinJS --- 퀘스트 수락/완료 버튼의 종류
enum eQuestBtnType
{
	eQuestBtnType_Allow,		// 수락
	eQuestBtnType_Complete,		// 완료(1개)
	eQuestBtnType_CompleteAll,	// 모두 완료(완료 가능한 퀘스트 모두)
	eQuestBtnType_Progress,		// 진행
};

class cNpcScriptDialog  : public cDialog										// 다이얼로그 클래스를 상속받은 NPC 스크립트 다이얼로그 클래스.
{
protected:
	DWORD			m_dwCurNpc;													// 현재 NPC 아이디.
	WORD			m_wNpcChxNum ;												// 현재 NPC CHX 번호.
	DWORD			m_dwCurPageId;												// 현재 페이지 아이디.
	DWORD			m_dwMainLinkPageId ;										// 메인 링크 페이지 아이디.
	cListDialogEx*	m_pListDlg;													// NPC 대사 리스트 다이얼로그.
	cListDialogEx*	m_pLinkListDlg ;											// NPC와 관련 된 링크 리스트 다이얼로그.
	cButton*		m_pQuestBtn;
	eQuestBtnType	m_QuestBtnType;
	cStatic*		m_pTitleText;
	// 100414 ONS 퀘스트보상관련 변수추가
	cDialog*			m_pRequitalInfoDlg;
	cIconGridDialog*	m_pFixRequitalGrid;									
	cIconGridDialog*	m_pSelectRequitalGrid;
	cStatic*			m_pRequitalQuestName;
	cStatic*			m_pRequitalGold;
	cStatic*			m_pRequitalExp;

	cPtrList			m_QuestFixRequitalList ;							
	cPtrList			m_QuestSelectRequitalList;
	CIndexGenerator		m_IndexGenerator;

	// 선택한 보상의 인덱스와 갯수
	DWORD				m_RequitalIndex;
	DWORD				m_RequitalCount;
	int					m_RequitalItemTooltipIdx ;
	DWORD m_dwQuestIdx;

public:
	cNpcScriptDialog();
	virtual ~cNpcScriptDialog();
	virtual DWORD ActionKeyboardEvent(CKeyboard*);
	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we);
	virtual void SetActive(BOOL val);
	void DisActiveEx();
	BOOL OpenDialog( DWORD dwNpcId, WORD npcChxNum );
	void RefreshQuestLinkPage();
	void EndDialog();
	void HyperLinkParser(LPCTSTR,DWORD);
	DWORD GetCurNpcIdx() const { return m_dwCurNpc; }
	WORD GetConversationListFontIdx() const;
	LONG GetConversationListTextWidth() const;
	virtual DWORD ActionEvent(CMouse*);
	virtual void Render();

private:
	void MoveCursorToLink();
	const HYPERLINK& GetHyperLink(LPCTSTR,DWORD);
	void ResetConversationList(cDialogueList* pList, DWORD dwMsg);
	void ResetMainLinkList(cPage* pMainPage, cHyperTextList* pHyper, cListDialogEx*);
	void ResetConversationLinkList(cPage* pMainPage, cHyperTextList* pHyper) ;
	void SettingNpcImage(cPage* pMainPage, WORD npcChxNum);
	void OpenLastPageOfQuest();
	void AddRequitalItem(ITEM_INFO*, DWORD Count, BYTE bType);
	void ShowRequitalItemToolTip(cPtrList& RequitalList, DWORD Index);
	void RefreshRequitalGrid( BYTE bRequitalType );
	void OpenRequitalPage( DWORD dwQuestIdx );
	BOOL CanTakeSelectRequital( DWORD dwQuestIdx );
	BOOL IsRequitalSelected( DWORD dwQuestIdx );
	void OpenQuestLinkPage( DWORD dwPageId );
	void OpenChangeMapLinkPage( DWORD dwPageId );
	void RefreshChangeMapLinkPage();
	BOOL OpenLinkPage(DWORD dwPageId);
};

#endif