#pragma once
#include "stdafx.h"
#include "../[cc]Header/CommonGameDefine.h"
#include "../[cc]Header/CommonStruct.h"
#include "item.h"

#define COOKMGR USINGTON(cCookMgr)

class cCookMgr
{
public:
	cCookMgr(void);
	virtual ~cCookMgr(void);

	void Init();
	void NetworkMsgParse(BYTE Protocol,void* pMsg);

	int GetCookLevelFromRecipe(DWORD dwRecipeIdx) {return dwRecipeIdx/10000000;}
	
	WORD GetCookLevel()					{return m_wLevel;}
	WORD GetCookCount()					{return m_wCookCnt;}
	WORD GetEatCount()					{return m_wEatCnt;}
	WORD GetFireCount()					{return m_wFireCnt;}
	WORD GetMaxCookCount()				{if(eCOOKLEVEL4<=m_wLevel) return 0;	return m_wMaxCookCount[m_wLevel];}
	WORD GetMaxEatCount()				{if(eCOOKLEVEL4<=m_wLevel) return 0;	return m_wMaxEatCount[m_wLevel];}
	WORD GetMaxFireCount()				{if(eCOOKLEVEL4<=m_wLevel) return 0;	return m_wMaxFireCount[m_wLevel];}

	stRecipeInfo* GetRecipeInfo(WORD wLevel, DWORD dwIndex);

protected:
	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv1;
	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv2;
	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv3;
	std::map<DWORD, stRecipeInfo*> m_mapRecipeLv4;

	WORD m_wLevel;		// ·¹º§
	WORD m_wCookCnt;	// ¿ä¸®È½¼ö
	WORD m_wEatCnt;		// ¸Ô±âÈ½¼ö
	WORD m_wFireCnt;	// ¸ð´ÚºÒÈ½¼ö

	stRecipeLv4Info	m_MasterRecipe[MAX_RECIPE_LV4_LIST];

	WORD m_wMaxCookCount[MAX_COOKING_LEVEL];
	WORD m_wMaxEatCount[MAX_COOKING_LEVEL];
	WORD m_wMaxFireCount[MAX_COOKING_LEVEL];
	DWORD m_dwLevelUpReward[MAX_COOKING_LEVEL];
};

EXTERNGLOBALTON(cCookMgr)