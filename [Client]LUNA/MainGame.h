// MainGame.h: interface for the CMainGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINGAME_H__7AEA4DB0_7847_40E1_8CDC_15C321ED0CB3__INCLUDED_)
#define AFX_MAINGAME_H__7AEA4DB0_7847_40E1_8CDC_15C321ED0CB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define MAINGAME USINGTON(CMainGame)

class CGameState;
class CEngine;

enum eGAMESTATE
{
	eGAMESTATE_NONE,
	eGAMESTATE_END,
	eGAMESTATE_CONNECT,
	eGAMESTATE_TITLE,
	eGAMESTATE_CHARSELECT,
	eGAMESTATE_CHARMAKE,
	eGAMESTATE_GAMEIN,
	eGAMESTATE_MAPCHANGE,
};

class CMainGame
{
	BOOL m_bEndGame;
	CEngine* m_pEngine;
	BOOL m_bChangeState;
	CGameState* m_pCurrentGameState;
	char* m_pStateInitParam;
	eGAMESTATE m_ChangeState;
	eGAMESTATE m_nCurStateNum;
	int m_nUserLevel;
	CGameState* GetGameState(int StateNum);

public:
	CMainGame();
	virtual ~CMainGame();
	void Init(HWND);
	void Release();
	void SetGameState(eGAMESTATE, LPVOID pStateInitParam = 0,int ParamLen = 0);
	int Process();
	void BeforeRender();
	void AfterRender();
	eGAMESTATE GetCurStateNum() const { return m_nCurStateNum; }
	int GetUserLevel() const { return m_nUserLevel; }
	void SetUserLevel(int nUserLevel) { m_nUserLevel = nUserLevel; }
	void PauseRender(BOOL bPause);
	CEngine* GetEngine() { return m_pEngine; }
	void LoadPack(LPCTSTR);
};

EXTERNGLOBALTON(CMainGame);

DWORD _stdcall BeforeRenderFunc();
DWORD _stdcall AfterRenderFunc();

#endif // !defined(AFX_MAINGAME_H__7AEA4DB0_7847_40E1_8CDC_15C321ED0CB3__INCLUDED_)
