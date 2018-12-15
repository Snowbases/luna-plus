#pragma once

class CParty;
class CPlayer;

class CDistributer
{
	struct DAMAGEOBJ
	{
		DWORD dwID;
		DWORD dwData;
	};

	CYHHashTable<DAMAGEOBJ>	m_DamageObjectTableSolo;	// 개인의 데미지를 담은 테이블.
	CYHHashTable<DAMAGEOBJ>	m_DamageObjectTableParty;	// 파티의 데미지를 담은 테이블.
	DWORD m_1stPlayerID;								// 처음 데미지를 준 플레이어의 아이디.
	DWORD m_1stPartyID;									// 처음 데미지를 준 파티의 아이디.
	DWORD m_TotalDamage;								// 데미지 합계.
	DWORD  m_FirstDamage ;								// 첫 데미지.
	int m_nTotalKillerCount;
	LEVELTYPE m_HighLevel;
	DWORD mKillerPlayerIndex;
	CPlayer* m_pKiller;
	CMonster* m_pMob;
	VECTOR3	m_pKilledPosition;
	DWORD m_dwDropItemID;
	DWORD m_dwDropItemRatio;
	typedef DWORD ObjectIndex;
	typedef std::list< ObjectIndex > IndexContainer;
	IndexContainer mPlayerIndexContainer;

public:
	CDistributer();
	~CDistributer();
	void Release();
	void AddDamageObject(CPlayer*, DWORD damage, DWORD plusdamage);
	void DistributeItemPerDamage(CMonster&) ;
	void DistributePerDamage(CMonster&);
	void DamageInit();
	void DeleteDamagedPlayer(DWORD CharacterID);
	void SetInfoToDistribute(DWORD dwKillerID, DWORD DropItemID, DWORD DropItemRatio, CMonster&);
	void Distribute();

private:
	BOOL Chk(CPlayer&, DWORD GridID);
	void ChooseOne(const DAMAGEOBJ&, DWORD& pBigDamage, DWORD& pBigID);
	void DoAddDamageObj(CYHHashTable<DAMAGEOBJ>&, DWORD dwID, DWORD damage);
	// 킬러가 여러명일 때 처리하는 함수 파트																												 //
	void DistributeToKillers() ;										// 킬러가 다수일 때 처리하는 함수.
	void DistributerToSolo(MONEYTYPE money) ;							// 킬러가가 파티에 소속되지 않았을때 처리하는 함수.
	void DistributeToFirstKiller(CPlayer&, DWORD dwMyDamage) ;	// 파티가 없고, 첫 데미지를 준 킬러를 처리하는 함수.
	void DistributeToOtherKiller(CPlayer&, DWORD dwMyDamage) ;	// 파티가 없고, 나중에 데미지를 준 킬러를 처리하는 함수.
	void DistributeToPartys(MONEYTYPE money) ;							// 킬러가 파티에 소속되었을 때 처리하는 함수.
	void DistributeToFirstParty(CParty&, DWORD dwPartyDamage);			// 첫 데미지를 준 파티를 처리하는 함수.
	void DistributeToOtherParty(CParty&, DWORD dwPartyDamage) ;			// 나중에 데미지를 준 파티를 처리하는 함수.
	void SendMoneyToPartys(float ChangeValue) ;							// 킬러의 파티로 머니를 전송하는 함수.
	void DistributeItemToKillers() ;									// 킬러가 다수일 때 아이템 분배를 처리하는 함수.

	// 킬러가 한명일 때 처리하는 함수 파트
	void DistributeToKiller() ;											// 킬러가 한명일 때 처리하는 함수.
	void DistributeToPerson(CPlayer&);
	void DistributeToParty(CPlayer&);									// 킬러가 한명이고, 파티에 소속되었을 때 처리하는 함수. 
	void SendMoneyToPerson(CPlayer* pPlayer, MONEYTYPE ChangeValue) ;	// 킬러에게 머니를 전송하는 함수.
	void SendItemToPerson(CPlayer* pPlayer) ;							// 킬러에게 아이템을 전송하는 함수.
	DWORD CalcObtainExp(LEVELTYPE KillerLevel, int nMemberCount) ;		// 주어진 레벨에 따른 몬스터의 경험치를 반환하는 함수.
	BOOL GetFirstDamange();
	
	void GetTotalKillerCount() ;										// 파티는 1로하고 총 킬러수를 구하는 함수.
	void GetHighLevelOfKillers() ;										// 킬러 들 중 최고 레벨을 뽑는다.
	int  GetLevelPenaltyToMonster(LEVELTYPE);
	void GetAllPartyDamage();
	void SaveCandidacy(CParty&);										// 파티원 중 후보를 담는 함수.
	void DistribuSequence(CParty&);
	void DistributeDamage();
	void SendToPersonalExp(CPlayer*, EXPTYPE);

	enum eGET_LVTYPE { eGET_CURLEVEL, eGET_MAXLEVEL };
	void GetPartyLevels(CParty&, LEVELTYPE& pHighLevel, LEVELTYPE& pTotalLevel , eGET_LVTYPE eGetLvType = eGET_CURLEVEL) ;	// 파티의 최고레벨과, 총 레벨 합을 구하는 함수.
	void AddMoney(CPlayer*, MONEYTYPE, WORD MonsterKind);
};