#pragma once


class CPunishUnit
{
protected:
	DWORD	m_dwUserIdx;
	int		m_nPunishKind;
	DWORD	m_dwEndTime;

public:
	CPunishUnit(void);
	~CPunishUnit(void);

	void Init( DWORD dwUserIdx, int nPunishKind, DWORD dwEndTime );
	BOOL IsTimeEnd();

	DWORD GetRemainTime();
	DWORD GetUserIdx() { return m_dwUserIdx; }
};


#define PUNISHMGR CPunishManager::GetInstance()

class CPunishManager
{
protected:
	CYHHashTable< CPunishUnit >			m_htPunishUnit[ePunish_Max];
	// 090923 ONS 메모리풀 교체
	CPool< CPunishUnit >*				m_pmpPunishUnit;
public:

	MAKESINGLETON( CPunishManager );

	CPunishManager(void);
	~CPunishManager(void);

	void Init();
	void Release();
	void Process();

	void AddPunishUnit( DWORD dwUserIdx, int nPunishKind, DWORD dwPunishTime );
	void RemovePunishUnit( DWORD dwUserIdx, int nPunishKind );
	void RemovePunishUnitAll( DWORD dwUserIdx );
	CPunishUnit* GetPunishUnit( DWORD dwUserIdx, int nPunishKind );
};
