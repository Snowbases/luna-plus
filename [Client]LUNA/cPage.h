#pragma once

enum LINKTYPE {
	emLink_Null,
	emLink_Page,
	emLink_End,
	emLink_Open,
	emLink_Quest,
	emLink_QuestLink,
	emLink_QuestStart,
	emLink_QuestContinue,
	emLink_QuestEnd,
	emLink_MapChange,
	emLink_EventQuestStart,
	emLink_Image,
	emLink_ChangeStage,
	emLink_ChangeMapPage,
	emLink_FishPointChange,
	emLink_RecallNpc,
	emLink_SpellSpecificBuff,
	emLink_SpellAllBuff,
	emLink_ChangeMapRecallNpc,
	emLink_MapMoveList_Rushen,
	emLink_MapMoveList_Zevyn,
	emLink_CastleFlag,
	emLink_MoveToDungeon,
};

struct HYPERLINK
{
	WORD wLinkId;
	LINKTYPE wLinkType;
	DWORD dwData;
	float fXpos;
	float fZpos;
};

class cPage
{
protected:
	typedef DWORD OrderIndex;
	typedef DWORD DialogIndex;
	typedef stdext::hash_map< OrderIndex, DialogIndex > DialogMap;
	DialogMap mDialogMap;
	typedef stdext::hash_map< OrderIndex, HYPERLINK > HyperLinkMap;
	HyperLinkMap mHyperLinkMap;
	DWORD m_dwPageId;
	BYTE mEmotion;

public:
	cPage();
	virtual ~cPage();
	void Init(BYTE emotion);
	void AddDialogue(DWORD dialogIndex);
	DWORD GetDialogue() const;
	DWORD GetDialogue(DWORD orderIndex) const;
	DWORD GetRandomDialogue() const;
	BYTE GetEmotion() const { return mEmotion; }
	void AddHyperLink(const HYPERLINK&);
	int GetHyperLinkCount() const { return mHyperLinkMap.size(); }
	HYPERLINK* GetHyperText(DWORD orderIndex);
};