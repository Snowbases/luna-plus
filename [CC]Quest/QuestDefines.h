#ifndef __QUESTDEFINES_H__
#define __QUESTDEFINES_H__

#pragma pack(push,1)

#ifdef _MAPSERVER_																	// 맵서버가 선언이 되어있으면,
#define	PLAYERTYPE	CPlayer															// PLAYERTYPE을 CPlayer 클래스로 정의하고,
class CPlayer;																		// 플레이어 클래스를 선언한다.
#else																				// 맵서버가 선언되어 있지 않으면,
#define PLAYERTYPE	CHero															// PLAYERTYPE을 CHero 클래스로 정의하고,
class CHero;																		// 히어로 클래스를 선언한다.
#endif																				// 분기 처리를 종료한다.


#define MAX_QUEST_PROBABILITY		10000											// 최대 퀘스트 변수.

#define	MAX_QUEST					200												// 최대 퀘스트 수.
#define	MAX_QUESTEVENT_MGR			1000											// 최대 퀘스트 이벤트 매니져?
#define MAX_QUESTEVENT_PLYER		100												// 최대 퀘스트 이벤트 플레이어수.
#define	MAX_SUBQUEST				32												// 최대 서브 퀘스트 수.

#define	MAX_ITEMDESC_LENGTH			255												// 최대 아이템 설명 길이.
#define	MAX_ITEMDESC_LINE			20												// 최대 아이템 설명 라인.
#define MAX_PROCESSQUEST			100												// 최대 퀘스트 프로세스 수.
#define VIEW_QUESTITEM_PERPAGE		12												// 한 페이지 당 보여지는 최대 퀘스트 아이템 수.
#define MAX_QUESTITEM				100												// 최대 퀘스트 아이템 수.
#define QUEST_DESC_COLOR			RGB(0, 0, 64)									// 퀘스트 설명 색상.
#define QUEST_DESC_HIGHLIGHT		RGB(128, 0, 0)									// 퀘스트 설명 하이라이트 색상.
#define COMBINEKEY(a, b, c)				\
	if(b<100)			c=a*100+b;		\
	else if(b<1000)		c=a*1000+b;		\
	else if(b<10000)	c=a*10000+b;	


enum eQuestEvent																	// 퀘스트 이벤트를 구분하는 이넘 코드.
{
	eQuestEvent_NpcTalk = 1,														// npc 대화 이벤트.
	eQuestEvent_Hunt,																// 사냥 이벤트.
	eQuestEvent_EndSub,																// 서브 퀘스트 종료 이벤트.
	eQuestEvent_Count,																// 카운트 이벤트.
	eQuestEvent_GameEnter,															// 게임 엔터 이벤트.
	eQuestEvent_Level,																// 레벨 이벤트.
	eQuestEvent_UseItem,															// 아이템 사용 이벤트.
	eQuestEvent_MapChange,															// 맵 변경 이벤트.
	eQuestEvent_Die,																// 다이 이벤트.
	eQuestEvent_Time,																// 시간 이벤트.
	eQuestEvent_HuntAll,															// 모두 사냥 이벤트.
};

enum eQuestExecute																	// 퀘스트 실행을 구분하기 위한 이넘 코드.
{
	eQuestExecute_EndQuest,															// 퀘스트 종료.
	eQuestExecute_StartQuest,														// 퀘스트 시작.

	eQuestExecute_EndSub,															// 서브 퀘스트 종료.
	eQuestExecute_EndOtherSub,														// 다른 서브 퀘스트 종료.
	eQuestExecute_StartSub,															// 서브 퀘스트 시작.
	
	eQuestExecute_AddCount,															// 카운트 추가.
	eQuestExecute_MinusCount,														// 카운트 감소.

	eQuestExecute_GiveQuestItem,													// 퀘스트 아이템을 주는것. quest item 창에 들어가는 거
	eQuestExecute_TakeQuestItem,													// 퀘스트 아이템을 획득하는 것.
	
	eQuestExecute_GiveItem,															// 퀘스트 아이템을 주는 것.
	eQuestExecute_GiveMoney,														// 퀘스트 머니를 주는 것.
	eQuestExecute_TakeItem,															// 퀘스트 아이템을 획득하는 것.
	eQuestExecute_TakeMoney,														// 퀘스트 머니를 획득하는 것.
	eQuestExecute_TakeExp,															// 퀘스트 경험치를 획득하는 것.
	eQuestExecute_TakeSExp,															// 퀘슽 sp 포인트를 획득하는 것.

	eQuestExecute_RandomTakeItem,													// 랜덤하게 아이템을 획득하는 것.

	eQuestExecute_TakeQuestItemFQW,													// from 특정 무기로
	eQuestExecute_AddCountFQW,														// from 특정 무기로 카운트를 추가하는 것.
	eQuestExecute_TakeQuestItemFW,													// from 무기류로 퀘스트 아이템을 획득하는 것.
	eQuestExecute_AddCountFW,														// 퀘스트 아이템 카운트를 추가하는 것.
	
	eQuestExecute_TakeMoneyPerCount,												// 카운트 별, 퀘스트 머니를 획득하는 것.
	
	eQuestExecute_RegenMonster,														// 몬스터를 리젠하는 것.
	eQuestExecute_MapChange,														// 맵을 교체하는 것.

	eQuestExecute_ChangeStage,														// 스테이지를 수정하는 것.
	
	eQuestExecute_ChangeSubAttr,													// 서브 Attr를 수정하는 것.

	eQuestExecute_RegistTime,														// 시간을 등록하는 것.

	eQuestExecute_LevelGap,															// 레벨 차이를 적용하는 것.
	eQuestExecute_MonLevel,															// 몬스터 레벨을 적용하는 것

	eQuestExecute_TakeSelectItem,													// 100414 ONS 퀘스트보상 선택아이템을 획득 추가	
};

enum eQuestLimitKind																// 퀘스트 제한 설정을 위한 이넘 코드.
{
	eQuestLimitKind_Level,															// 레벨 제한.
	eQuestLimitKind_Money,															// 머니 제한.
	eQuestLimitKind_Quest,															// 퀘스트 제한.
	eQuestLimitKind_SubQuest,	
	// 070415 LYW --- QuestDefine : Active quest limit kine enum stage.
	eQuestLimitKind_Stage,															// 퀘슽 상태 제한.
	eQuestLimitKind_Attr,															// 퀘스트 속성 제한다.
	// 071011 LYW --- QuestDefine : Add quest limit to check running quest.			// 진행 중인 퀘스트를 제한한다.
	eQuestLimitKind_RunningQuest,
};

enum eQuestValue																	// 퀘스트 값에 따른 이넘 코드.
{
	eQuestValue_Add,																// 퀘스트 추가.
	eQuestValue_Minus,																// 퀘스트 삭제.
	eQuestValue_Reset,
};

enum																				// 퀘스트 트리에 따른 이넘 코드.
{
	eQTree_NotUse,																	// 트리 사용 안하기, 
	eQTree_Close,																	// 닫기.
	eQTree_Open,																	// 열기.
};

enum																				// 퀘스트 아이템 카운트 관련 이넘 코드.
{
	eQItem_AddCount,																// 퀘스트 추가 카운트.
	eQItem_SetCount,																// 퀘스트 세팅 카운트.
	eQItem_GetCount,																// 퀘스트 반환 카운트.
};

enum
{
	eQuest_NpcScript_Normal,														// 일반상태의 퀘스트 NPC스크립트 
	eQuest_NpcScript_Lack,															// 조건 불충족시 상태의 퀘스트 NPC스크립트
};

struct SUBQUEST																		// 서브 퀘스트 구조체.
{
	DWORD	dwMaxCount;																// 최대 카운트.
	DWORD	dwData;																	// 데이터 값.
	DWORD	dwTime;																	// 시간 값.
	SUBQUEST() : dwMaxCount(0), dwData(0), dwTime(0)	{}							// 기본 함수.
};

struct QUESTITEM																	// 퀘스트 아이템 구조체.
{
	DWORD	dwQuestIdx;																// 퀘스트 인덱스.
	DWORD	dwItemIdx;																// 아이템 인덱스.
	DWORD	dwItemNum;																// 아이템 개수.
	QUESTITEM() : dwQuestIdx(0), dwItemIdx(0), dwItemNum(0)	{}						// 기본 함수.
};

struct QUEST_ITEM_INFO																// 퀘스트 아이템 정보 구조체.
{
	DWORD		ItemIdx;															// 아이템 인덱스.
	DWORD		QuestKey;															// 퀘스트 키 값.
	char		ItemName[MAX_ITEMNAME_LENGTH+1];									// 아이템 명.
	WORD		Image2DNum;															// 2d 이미지.
	WORD		SellPrice;															// 판매 가격.
	char		ItemDesc[MAX_ITEMDESC_LENGTH+1];									// 아이템 설명.
};

enum eQuestState
{
	eQuestState_ImPossible,		// 수행불가
	eQuestState_Possible,		// 수행가능
	eQuestState_Executed,		// 수행중
};

#pragma pack(pop)
#endif
