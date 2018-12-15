#include "StdAfx.h"
#include "LimitDungeonDialog.h"
#include "LimitDungeonMgr.h"
#include "ChatManager.h"
#include "WindowIDEnum.h"
#include "interface\cWindowManager.h"
#include "MHMap.h"
#include "GameResourceManager.h"
#include "..\interface\cStatic.h"

CLimitDungeonDialog::CLimitDungeonDialog() :
mGoalTick(0),
mMapNameStatic(0),
mDifficultyStatic(0),
mMonsterStatic(0),
mTimeStatic(0)
{}

CLimitDungeonDialog::~CLimitDungeonDialog()
{}

void CLimitDungeonDialog::SetPlayTime(__time64_t time)
{
	mGoalTick = gCurTime + DWORD(time) * 1000;
}

__time64_t CLimitDungeonDialog::GetPlayTime() const
{
	if(FALSE == IsActive())
	{
		return 0;
	}

	return __time64_t(max(0, mGoalTick - gCurTime)) / 1000;
}

void CLimitDungeonDialog::Render()
{
	cDialog::Render();
	UpdateTime();
}

void CLimitDungeonDialog::UpdateTime()
{
	if(0 == mTimeStatic)
	{
		return;
	}
	else if(mGoalTick < gCurTime)
	{
		mTimeStatic->SetStaticText(
			"00:00:00.000");		
	}
	else
	{
		const DWORD oneSecondTick = 1000;
		const DWORD oneMinuteTick = 60 * oneSecondTick;
		const DWORD oneHourTick = oneMinuteTick * 60;
		DWORD tick = mGoalTick - gCurTime;

		const DWORD hour = tick / oneHourTick;
		tick = tick - hour * oneHourTick;
		const DWORD minute = tick / oneMinuteTick;
		tick = tick - minute * oneMinuteTick;
		const DWORD second = tick / oneSecondTick;
		tick = tick - second * oneSecondTick;

		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"%02d:%02d:%02d.%03d",
			hour,
			minute,
			second,
			tick);
		mTimeStatic->SetStaticText(text);
	}	
}

void CLimitDungeonDialog::OnActionEvent(LONG id, LPVOID, DWORD event)
{
	switch(id)
	{
	case 0:
		{
			class
			{
			public:
				virtual cWindow* operator()(cDialog& dialog, WINDOW_TYPE type, int value)
				{
					cWindow* const window = dialog.GetWindowForID(value);

					if(0 == window)
					{
						return 0;
					}

					return type == window->GetType() ? window : 0;
				}
			}
			GetStatic;

			mMapNameStatic = (cStatic*)GetStatic(
				*this,
				WT_STATIC,
				LD_MAPNAME);
			mDifficultyStatic = (cStatic*)GetStatic(
				*this,
				WT_STATIC,
				LD_DIFFICULTY);
			mMonsterStatic = (cStatic*)GetStatic(
				*this,
				WT_STATIC,
				LD_LIMIT_MONSTER);
			mTimeStatic = (cStatic*)GetStatic(
				*this,
				WT_STATIC,
				LD_LIMIT_TIME);
			break;
		}		
	}
}

void CLimitDungeonDialog::SetMapName(cStatic* staticWindow)
{
	if(0 == staticWindow)
	{
		return;
	}

	staticWindow->SetStaticText(
		GetMapName(MAP->GetMapNum()));
}

void CLimitDungeonDialog::SetDifficulty(cStatic* staticWindow)
{
	if(0 == staticWindow)
	{
		return;
	}

	const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
		MAP->GetMapNum(),
		gChannelNum + 1);
	staticWindow->SetStaticText(
		CHATMGR->GetChatMsg(script.mNameIndex));
}

void CLimitDungeonDialog::SetActive(BOOL isActive)
{
	if(isActive)
	{
		SetMapName(mMapNameStatic);
		SetDifficulty(mDifficultyStatic);
	}

	cDialog::SetActive(isActive);
}

void CLimitDungeonDialog::SetMonsterCount(DWORD monsterCount)
{
	if(mMonsterStatic)
	{
		mMonsterStatic->SetStaticValue(monsterCount);
	}
}