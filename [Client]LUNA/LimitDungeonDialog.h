#include "interface/cDialog.h"

class cStatic;
class CMouse;

class CLimitDungeonDialog : public cDialog
{
private:
	cStatic* mMapNameStatic;
	cStatic* mDifficultyStatic;
	cStatic* mMonsterStatic;
	cStatic* mTimeStatic;
	DWORD mGoalTick;

public:
	CLimitDungeonDialog();
	virtual ~CLimitDungeonDialog();
	void SetPlayTime(__time64_t);
	__time64_t GetPlayTime() const;
	virtual void Render();
	virtual void OnActionEvent(LONG, LPVOID, DWORD);
	virtual void SetActive(BOOL);
	void SetMonsterCount(DWORD monsterCount);	

private:
	void UpdateTime();
	void SetMapName(cStatic*);
	void SetDifficulty(cStatic*);
};