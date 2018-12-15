#pragma once

class cDialogueList
{
protected:
	typedef DWORD LineNumber;
	typedef stdext::hash_map< LineNumber, DIALOGUE > DialogMap;
	typedef DWORD MessageIndex;
	typedef stdext::hash_map< MessageIndex, DialogMap > MessageMap;
	MessageMap mMessageMap;
	typedef stdext::hash_map< MessageIndex, std::string > FullMessageMap;
	FullMessageMap mFullMessageMap;
	DWORD m_dwDefaultColor;
	DWORD m_dwStressColor;

	// 091216 ShinJS --- Npc_Msg.bin의 Data를 저장(인덱스,라인,메세지)
	typedef std::pair<DWORD, DWORD> NpcMsgDataKey;
	typedef std::map< NpcMsgDataKey, std::string > NpcMsgData;
	NpcMsgData m_NpcMsgData;

public:
	cDialogueList();
	virtual ~cDialogueList();
	DIALOGUE* GetDialogue(DWORD dwMsgId, WORD wLine);
	LPCTSTR GetDialogue(DWORD dwMsgId);

protected:
	void LoadScript();
	void AddLine( DWORD dwId, LPCTSTR, DWORD color, WORD Line, WORD type);
	void ParsingLine(DWORD dwId, LPCTSTR);
};