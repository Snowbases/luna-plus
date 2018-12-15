#pragma once

class CDungeonMgr;

/*
	주 : 인던 맵별로 던전처리자 클래스를 생성해서 사용해야 합니다.
	던전처리자 클래스는 CDungeonProcessorBase를 상속받아서 사용하시면 됩니다.

	DungeonMgr::Init()에서 던전처리자를 생성하며,
	DungeonMgr::Process()는 모든 던전의 공통 프로세스를 처리합니다.
				던전별로 다른 처리는 던전처리자의 Process()에서 처리하여야 합니다.
				
				모든 던전에 공통으로 처리는 DungeonMgr에서 처리하며,
				던전별로 다른부분은 인터페이스를 추가해서 별도로 처리하여야 합니다.
*/

class CDungeonProcessorBase
{
public:
	CDungeonProcessorBase(void);
	virtual ~CDungeonProcessorBase(void);

	virtual void	Init(CDungeonMgr* pParent);
	virtual void	Create(stDungeon* pDungeon) = 0;
	virtual void	Process() = 0;

	virtual void	SetSwitch(DWORD dwChannelID, WORD num, BOOL val) = 0;	// 스위치의 상태가 변경되었을 때 처리
	virtual void	Info_Syn(CObject* pObject) = 0;							// 클라이언트에서 MP_DUNGEON_INFO_SYN 이 왔을 때 처리
	virtual void	SetBossMonster(stDungeon* pDungeon) = 0;

private:
	CDungeonMgr*		m_pParent;
};

class CDungeonProcessorEmpty : public CDungeonProcessorBase
{
public:
	CDungeonProcessorEmpty(void) {}
	virtual ~CDungeonProcessorEmpty(void) {}

	static CDungeonProcessorBase* Clone()
	{
		return new CDungeonProcessorEmpty;
	}

	virtual void	Create(stDungeon* pDungeon) {}
	virtual void	Process() {}
	virtual void	SetSwitch(DWORD dwChannelID, WORD num, BOOL val) {}
	virtual void	Info_Syn(CObject*) {}
	virtual void	SetBossMonster(stDungeon* pDungeon) {}
};