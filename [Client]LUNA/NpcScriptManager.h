#pragma once

#define NPCSCRIPTMGR CNpcScriptManager::GetInstance()

class cPage;
class cDialogueList;
class cHyperTextList;
class CNpc;

class CNpcScriptManager
{
protected:
	typedef DWORD PageIndex;
	typedef stdext::hash_map< PageIndex, cPage* > PageMap;
	typedef DWORD NpcIndex;
	typedef stdext::hash_map< NpcIndex, PageMap > NpcMap;
	NpcMap mNpcMap;
	typedef stdext::hash_map< NpcIndex, PageIndex > MainPageMap;
	MainPageMap mMainPageMap;
	cDialogueList* m_pDialogue;
	cHyperTextList*	m_pHyperText;
	CNpc* m_pCurSelectedNpc;

public:
	CNpcScriptManager();
	virtual ~CNpcScriptManager();
	static CNpcScriptManager* GetInstance();
	cPage* GetMainPage(DWORD npcIndex);
	cPage* GetPage(DWORD npcIndex, DWORD pageIndex);
	cDialogueList* GetDialogueList() const { return m_pDialogue; }
	cHyperTextList* GetHyperTextList() const { return m_pHyperText; }
	void SetSelectedNpc( CNpc* pNpc ) { m_pCurSelectedNpc = pNpc; }
	CNpc* GetSelectedNpc() const { return m_pCurSelectedNpc; }
	void StartNpcScript( CNpc* pNpc );
	void StartNpcBusiness(DWORD dwData, DWORD dwParam=0);
	void Business_Dealer();
	void Business_Changgo(DWORD dwData, DWORD dwParam);
	void Business_Guild(DWORD dwData);
	void Business_MapChange(DWORD dwData);
	void Business_Family(DWORD dwData);
	void Business_Identifycation();
	void Business_Housing(DWORD dwData);

protected:
	void AddScript(DWORD npcIndex, DWORD pageIndex, BYTE emotion);
	void LoadScript();
};