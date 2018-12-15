#pragma once

#define CHEATMGR USINGTON(CCheatMsgParser)
class CCheatMsgParser  
{
	BOOL m_bCheatEnable;
	CObject* m_pTargetObj;

public:
	enum{ eKindOption_None=0, eKindOption_Reinforce=1 /*, eKindOption_Rare=2,*/ };
	WORD	m_wItemOptionKind;
	DWORD	m_wTargetItemIdx;
	POSTYPE	m_ItemAbsPos;
	WORD	m_OptionCount;
	//ITEM_OPTION	m_OptionInfo;
	BOOL	m_bReadyToOptionCheat;

//	//MAKESINGLETON(CCheatMsgParser);

	CCheatMsgParser();
	virtual ~CCheatMsgParser();

	void NetworkMsgParse(BYTE Category,BYTE Protocol,MSGBASE* pMsg);

	BOOL IsCheatEnable() { return m_bCheatEnable; }
	void SetCheatEnable( BOOL bCheat ) { m_bCheatEnable = bCheat; }
	void SetCheatTargetObject(CObject* pObj) { m_pTargetObj = pObj; }
	CObject* GetCheatTargetObject()	{ return m_pTargetObj; }

	// 071128 LYW --- CheatMsgParser : 네트워크 메시지 파서 부분 처리 함수화 작업.
#ifdef _CHEATENABLE_
	void Cheat_ChangeMap_Ack( void* pMsg ) ;
	//void Cheat_ChangeMap_Nack( void* pMsg ) ;
	void Cheat_WhereIs_Ack( void* pMsg ) ;
	void Cheat_WhereIs_MapServer_Ack( void* pMsg ) ;
	void Cheat_WhereIs_Nack( void* pMsg ) ;
	void Cheat_BlockCharacter_Ack( void* pMsg ) ;
	void Cheat_BlockCharacter_Nack( void* pMsg ) ;
	void Cheat_BanCharacter_Ack( void* pMsg ) ;
	//void Cheat_BanCharacter_Nack( void* pMsg ) ;
	void Cheat_Item_Ack( void* pMsg ) ;
	void Cheat_AgentCheck_Ack( void* pMsg ) ;
#endif	//_CHEATENABLE_
	void Cheat_Move_Ack(LPVOID);
	void Cheat_Hide_Ack(LPVOID);
	void Cheat_EventNotify_On(LPVOID);
	void Cheat_EventNotify_Off(LPVOID);
	void Cheat_GM_Login_Ack(LPVOID);
	void Cheat_GM_Login_Nack(LPVOID);
	void Cheat_PartyInfo_Ack(LPVOID);
	void Cheat_NpcHide_Ack(LPVOID);
	void Cheat_PvP_Damage_Rate(LPVOID);
	void Cheat_Clear_Inventory(LPVOID);
	void Cheat_Dungeon_Observer(LPVOID);
	void Cheat_Monster_Script_Ack(TESTMSG*);
	void Cheat_Monster_Script_Nack(TESTMSG*);
};

#ifdef _CHEATENABLE_
BOOL CheatFunc(char* cheat);
#endif

EXTERNGLOBALTON(CCheatMsgParser)