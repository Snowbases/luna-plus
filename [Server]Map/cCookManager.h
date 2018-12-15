#pragma once
#include "stdafx.h"
#include "../[cc]Header/CommonGameDefine.h"
#include "../[cc]Header/CommonStruct.h"
#include "Player.h"

#define COOKMGR cCookManager::GetInstance()

class cCookManager
{
protected:
	CYHHashTable<DWORD> m_CookerList;		// 요리중인 플레이어 리스트
	CYHHashTable<DWORD> m_FireNpcList;		// 모닥불 리스트

	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv1;
	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv2;
	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv3;
	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv4;

	WORD m_wMaxCookCount[MAX_COOKING_LEVEL];
	WORD m_wMaxEatCount[MAX_COOKING_LEVEL];
	WORD m_wMaxFireCount[MAX_COOKING_LEVEL];
	DWORD m_dwLevelUpReward[MAX_COOKING_LEVEL];

public:
	cCookManager(void);
	virtual ~cCookManager(void);
	static cCookManager* GetInstance();
	void NetworkMsgParse(BYTE Protocol, void* pMsg, DWORD dwLength);
	void Process();

	bool CanLevelUp(CPlayer* pPlayer, WORD wNextLevel);
	void SetLevel(CPlayer* pPlayer, WORD wLevel);
	bool IsFireNpc(DWORD dwNpcIndex);

	int GetCookLevelFromRecipe(DWORD dwRecipeIdx) {return dwRecipeIdx/10000000;}
	stRecipeInfo* GetRecipeInfo(DWORD dwRecipeIdx);
	WORD GetMaxEatCount(WORD wLevel) {if(eCOOKLEVEL4<=wLevel) return 0;	return m_wMaxEatCount[wLevel];}
	WORD GetMaxFireCount(WORD wLevel) {if(eCOOKLEVEL4<=wLevel) return 0; return m_wMaxFireCount[wLevel];}
	DWORD GetLevelUpReward(WORD wLevel)	{if(eCOOKLEVEL4<wLevel) return 0; return m_dwLevelUpReward[wLevel-1];}

private:
	void Init();
	void Cook_Syn(void* pMsg);
	WORD CheckCookUtil(CPlayer*);
	bool CheckIngredients(CPlayer*, stRecipeInfo*, WORD wMakeNum);
	WORD GetMaxCookCount(WORD wLevel) {if(eCOOKLEVEL4<=wLevel) return 0;	return m_wMaxCookCount[wLevel];}
};