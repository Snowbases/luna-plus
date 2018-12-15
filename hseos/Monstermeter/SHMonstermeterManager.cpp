/*********************************************************************

	 파일		: SHMonstermeterManager.cpp
	 작성자		: hseos
	 작성일		: 2007/07/08

	 파일설명	: 몬스터미터 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "SHMonstermeterManager.h"
#include "MHFile.h"
#include "ItemManager.h"
#include "Player.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHMonstermeterManager
//

CSHMonstermeterManager g_csMonstermeterManager;

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHMonstermeterManager Method																								  생성자
//
CSHMonstermeterManager::CSHMonstermeterManager()
{
	ZeroMemory(m_stReward, sizeof(m_stReward));
	LoadScriptFileData();
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHMonstermeterManager Method																								  파괴자
//
CSHMonstermeterManager::~CSHMonstermeterManager()
{
	for(int i=0; i<MAX_REWARD_BEHAVE_TYPE; i++)
	{
		SAFE_DELETE_ARRAY(m_stReward[i].stRewardBase);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  LoadScriptData Method																							  스크립트 파일 로드
//
BOOL CSHMonstermeterManager::LoadScriptFileData()
{
 	char szFile[MAX_PATH];
	char szLine[MAX_PATH], szTxt[MAX_PATH];
	int	 nKind = 0;
	CMHFile fp;

	// 최대개수를 얻기 위해 처음 읽는 부분
	sprintf(szFile, "./System/Resource/MonstermeterReward.bin");
	fp.Init(szFile, "rb");
	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s 파일이 존재하지 않습니다.", szFile);
		ASSERTMSG(0, szTmp);
		return FALSE;
	}

	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;			
		else if (strstr(szLine, "END_KIND")) 
		{
			nKind++;
			continue;
		}

		m_stReward[nKind].nNum++;
	}

	fp.Release();

	for(int i=0; i<MAX_REWARD_BEHAVE_TYPE; i++)
	{
		m_stReward[i].stRewardBase	= new stREWARD_BASE[m_stReward[i].nNum];
	}

	// 데이터를 읽기 위해 두 번째..
	int nBehaveNum = 0, nValue[MAX_VALUE_NUM] = {0}, nRewardNum = 0;
	nKind = 0;
	fp.Init(szFile, "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}
	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;			
		else if (strstr(szLine, "END_KIND"))
		{
			nKind++;
			nRewardNum = 0;
			continue;
		}

		// 데이터 읽기
		sscanf(szLine, "%d %s %u %u", &nBehaveNum, szTxt, &nValue[0], &nValue[1]);
		if (strcmp(szTxt, "ITEM") == 0)	m_stReward[nKind].stRewardBase[nRewardNum].eRewardType = REWARD_TYPE_ITEM;
		else 
		{
			char szTmp[256];
			sprintf(szTmp, "존재하지 않는 키워드입니다.(키워드:%s, 파일:%s)", szTxt, szFile);
			ASSERTMSG(0, szTmp);
		}

		m_stReward[nKind].stRewardBase[nRewardNum].nBehaveNum = nBehaveNum;
		m_stReward[nKind].stRewardBase[nRewardNum].nValue[0] = nValue[0];
		m_stReward[nKind].stRewardBase[nRewardNum].nValue[1] = nValue[1];
		nRewardNum++;
	}
	
	fp.Release();

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ProcessReward Method																									   보상 처리
//
VOID CSHMonstermeterManager::ProcessReward(CPlayer* pPlayer, REWARD_BEHAVE_TYPE eType, int nBehaveNum)
{
	// stRewardBase[].nBehaveNum 에 공배수가 존재할 경우 아이템이 처음 것만 지급되거나, 중복 지급된다.(return 여하에 따라)
	// 그래서 내림차순으로 처리하여 스크립트에서 제일 아래 항목 1개만 지급되게 한다.
	// 따라서, 스크립트에서 nBehaveNum 을 순서(오름차순)대로 작성해야 한다.
	for(int i=m_stReward[eType].nNum-1; i>= 0; i--)
	{
		if ((nBehaveNum%m_stReward[eType].stRewardBase[i].nBehaveNum) == 0)
		{
			switch(m_stReward[eType].stRewardBase[i].eRewardType)
			{
				case REWARD_TYPE_ITEM:
					{
						CItemSlot* pSlot = pPlayer->GetSlot(eItemTable_Inventory);
						if(NULL == pSlot) ASSERTMSG(0,"올바른 아이템 슬롯을 얻을 수 없습니다.");

						// 071211 KTH -- pPlayer 추가
						DWORD nNum = ITEMMGR->GetTotalEmptySlotNum(pSlot, pPlayer);
						if(nNum == 0)
						{
							return;
						}

						int nItemID = m_stReward[eType].stRewardBase[i].nValue[0];
						int nItemNum = m_stReward[eType].stRewardBase[i].nValue[1];

						// 유효 아이템인가?
						// ..스크립트 데이터 파일이나 기타등등의 오류로 유효하지 않은 아이템이라면 처리하지 않는다.
						ITEM_INFO* pstItem = ITEMMGR->GetItemInfo(nItemID);
						if (pstItem == NULL)
						{
							continue;
						}

						ITEMMGR->ObtainMonstermeterItem(pPlayer, nItemID, (WORD)nItemNum);
					}
					break;
			}
			return;
		}
	}
}

