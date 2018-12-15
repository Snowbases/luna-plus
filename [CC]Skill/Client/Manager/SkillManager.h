#pragma once

class CHero;
class CHeroPet;
class CActionTarget;
class cSkillInfo;
class cSkillObject;
class cActiveSkillInfo;

#define SKILLMGR	cSkillManager::GetInstance()

class cSkillManager
{
	CYHHashTable<cSkillInfo>	m_SkillInfoTable;
	CYHHashTable<cSkillObject>	m_SkillObjectTable;
	CYHHashTable<SKILL_TREE_INFO>	m_SkillTreeTable;

	cSkillObject* DoCreateSkillObject(const SKILLOBJECT_INFO&, const ITEMBASE&);
	cSkillObject* DoCreateSkillObject(const SKILLOBJECT_INFO&);
	void DoCreateTempSkillObject(cActiveSkillInfo&, CActionTarget&, const ITEMBASE&);
	BOOL DoChangeTempSkillObject(SKILLOBJECT_INFO*);
	BOOL IsInvalidOnVehicle(CHero&, const ACTIVE_SKILL_INFO*);
	void ReleaseSkillObject(cSkillObject*);
	void RecallGuild(const ACTIVE_SKILL_INFO&);
	void RecallParty(const ACTIVE_SKILL_INFO&);
	cSkillInfo*	mpSkillInfo;

	// 081021 LYW --- SkillManager : 펫 용 스킬 정보를 추가한다.
	// 펫이 스킬을 사용할 경우 캐릭터에게도 같은 이펙트가 처리되며
	// 공격 및 이동이 불가능한 버그 수정.
	cSkillInfo*	mpPetSkillInfo ;

public:
	cSkillManager(void);
	virtual ~cSkillManager(void);

	//GETINSTANCE(cSkillManager);
	static cSkillManager* GetInstance();

	void Init();
	void Release();

	void ReleaseAllSkillObject();

	void LoadSkillInfoList();

	cSkillObject* GetSkillObject(DWORD SkillObjectID);
	cSkillInfo*	GetSkillInfo(DWORD SkillInfoIdx);
	SKILL_TREE_INFO* GetSkillTreeInfo( WORD ClassIndex );

	DWORD GetSkillTooltipInfo(DWORD SkillInfoIdx);
	DWORD GetComboSkillIdx(CHero* pHero);
	void SetSkill(cSkillInfo* pSkillInfo) { mpSkillInfo = pSkillInfo; }

	// 스킬을 사용
	BOOL ExcuteSkillSYN(CHero* pHero,CActionTarget* pTarget,BOOL bSkill);
	BOOL ExcuteSkillSYN(CHero* pHero,CActionTarget* pTarget,cSkillInfo* pInfo);
	BOOL RealExcuteSkillSYN(CHero* pHero,CActionTarget* pTarget,cSkillInfo* pInfo);

	BOOL ExcutePetSkillSYN(CHeroPet* pHeroPet,CActionTarget* pTarget,BOOL bSkill);
	BOOL ExcutePetSkillSYN(CHeroPet* pHeroPet,CActionTarget* pTarget,cSkillInfo* pInfo);
	BOOL RealExcutePetSkillSYN(CHeroPet* pHeroPet,CActionTarget* pTarget,cSkillInfo* pInfo);

	BOOL OnSkillCommand(CHero* pHero,CActionTarget* pTarget,BOOL bSkill);
	BOOL OnSkillCommand( DWORD idx );
	void OnComboTurningPoint(CHero* pHero);
	void OnPetComboTurningPoint(CHeroPet* pHeroPet);
	BOOL SetNextComboSkill(CHero* pHero,CActionTarget* pTarget,BOOL bSkill);

	BOOL OnPetSkillCommand(CHeroPet* pHeroPet,CActionTarget* pTarget,BOOL bSkill);
	BOOL OnPetSkillCommand( DWORD idx );
	DWORD GetPetComboSkillIdx(CHeroPet* pHeroPet);
	void SetNextPetComboSkill(CHeroPet* pHeroOet,CActionTarget* pTarget,BOOL bSkill);

	void OnReceiveSkillObjectAdd(MSG_SKILLOBJECT_ADD2* pmsg);
	void OnReceiveSkillObjectRemove(MSG_DWORD* pmsg);
	void OnReceiveSkillResult(MSG_SKILL_RESULT* pmsg);
	void OnReceiveSkillCancel(MSG_DWORD* pmsg);
	void OnReceiveSkillDelayReset(MSG_DWORD2* pmsg);
	void OnReceiveSkillCount(MSG_DWORD2* pmsg);

	void NetworkMsgParse(BYTE Protocol,void* pMsg);

	void AddPassive( DWORD SkillIdx );
	void RemovePassive( DWORD SkillIdx );
	DWORD GetSkillSize( DWORD skillIndex ) const;
	void Process();

	void SkillCancel( cSkillObject* pSkill );

	// 071228 LYW --- SkillManager : 파치 중 사용할 수 있는 버프 스킬인지 체크하는 함수 추가.
	BOOL IsUseableBuff_InParty(DWORD dwSkillIdx) ;

	// 080318 LUJ, 스킬 오브젝트가 담긴 컨테이너를 반환한다
	inline CYHHashTable< cSkillObject >& GetSkillObjectHashTable() { return m_SkillObjectTable; }
#ifdef _TESTCLIENT_
	inline CYHHashTable< cSkillInfo >& GetSkillInfoHashTable() { return m_SkillInfoTable; }
#endif

private:
	typedef std::map< DWORD, DWORD >	SkillSize;
	SkillSize							mSkillSize;

	// 081203 LUJ, 스킬 스크립트
public:
	// 081203 LUJ, 스킬 인덱스로 스킬 스크립트를 반환한다
	const SkillScript& GetSkillScript( DWORD skillIndex ) const;

	// 100315 ShinJS --- 사용가능 장비타입에 대한 Msg 반환
	const char* GetSkillEquipTypeText( const ACTIVE_SKILL_INFO* const pActiveSkillInfo, char* msg, size_t msgSize );
	const char* GetSkillWeaponTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  );
	const char* GetSkillArmorTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  );

	// 100316 ShinJS --- 소모포인트에 대한 Msg 반환
	const char* GetConsumePointTypeText( const ACTIVE_SKILL_INFO* const pActiveSkillInfo, char* msg, size_t msgSize  );

	// 100315 ShinJS --- 버프 조건에 대한 Msg 반환
	const char* GetBuffConditionTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  );
	const char* GetBuffCountTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize , const int nCurrentCount=0 );

	
private:
	// 100312 ShinJS --- ToolTipMsg 예약어 정보
	std::map< DWORD, int > m_mapToolTipMsgKeyWord;
public:
	// 100312 ShinJS --- ToolTipMsg 예약어 변환 추가
	// 툴팁문자열테이블 정보를 초기화한다.
	void InitToolTipMsgKeyWordTable();
	// 문자열에서 툴팁예약어를 찾는다.
	BOOL GetToolTipMsgKeyWordFromText( std::string& strMsg, std::string& keyword, std::string::size_type& keywordStart, std::string::size_type& keywordSize );
	// 툴팁예약어Type으로부터 변환된 문자열을 반환한다.
	const char* GetTextFromToolTipMsgKeyWordType( DWORD& dwToolTipIndex, int keywordType, char* txt, size_t txtSize, int prevKeyWordType = 0 );
	// Script문자열에서 툴팁예약어를 변환한다.
	void ConvertToolTipMsgKeyWordFormText( DWORD dwToolTipIndex, std::string& strMsg, std::string::size_type startPos = 0, int prevKeyWordType = 0 );

    BOOL IsUsedPhysicAttackSpeed( const ACTIVE_SKILL_INFO& ) const;
};
