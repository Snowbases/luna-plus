#pragma once
#include "dungeonprocessorbase.h"

class CDungeonProcessorTempleofGreed :
	public CDungeonProcessorEmpty
{
public:
	CDungeonProcessorTempleofGreed(void);
	virtual ~CDungeonProcessorTempleofGreed(void);

	static CDungeonProcessorBase* Clone()
	{
		return new CDungeonProcessorTempleofGreed;
	}

	virtual void SetBossMonster(stDungeon*);
};
