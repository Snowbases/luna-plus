#pragma once
#include "dungeonprocessorbase.h"

#define Switch_for_FallingRock	0
#define Switch_for_FallenRock	1

#define EffectDescNum_FallingRock	9379
#define EffectDescNum_FallenRock	9380

#define Range_FallenRock			700
#define Range_X_FallenRock			43500
#define Range_Z_FallenRock			16300

class CDungeonProcessorBrokenShipwrecked :
	public CDungeonProcessorBase
{
public:
	CDungeonProcessorBrokenShipwrecked(void);
	virtual ~CDungeonProcessorBrokenShipwrecked(void);

	static CDungeonProcessorBase* Clone()
	{
		return new CDungeonProcessorBrokenShipwrecked;
	}

	virtual void	Create(stDungeon* pDungeon);
	virtual void	Process();

	virtual void	SetSwitch(DWORD dwChannelID, WORD num, BOOL val);
	virtual void	Info_Syn(CObject* pObject);
	virtual void	SetBossMonster(stDungeon* pDungeon);
};
