/*********************************************************************

	 파일		: SHDebug.cpp
	 작성자		: hseos
	 작성일		: 2007/04/13

	 파일설명	: Debug 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../Common/SHMath.h"
#include "SHDebug.h"
#include <io.h>

#if defined(_MAPSERVER_)
	#include "../Farm/SHFarmManager.h"
#elif defined(_AGENTSERVER)
#elif defined(_DISTRIBUTESERVER_)
#else
	#include "cFont.h"
	#include "GameIn.h"
	#include "DateMatchingDlg.h"
	#include "ObjectManager.h"
	#include "../Farm/SHFarmManager.h"
	#include "../Farm/SHFarmUpgradeDlg.h"
	#include "../Farm/SHFarmManageDlg.h"
	#include "../hseos/ResidentRegist/SHResidentRegistManager.h"
	#include ".\facialmanager.h"
	#include "../hseos/Family/SHFamilyManager.h"
	#include "FamilyDialog.h"
	#include "../interface/cWindowManager.h"
	#include "../WindowIDEnum.h"
	#include "../ChatManager.h"
	#include "../cMsgBox.h"
	#include "../Date/SHDateZoneListDlg.h"
	#include "../hseos/Date/SHDateManager.h"
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHDebug
//

BOOL CSHDebug::m_bShowDebugInfo[CSHDebug::DEBUG_INFO_KIND_MAX_NUM];
int	 CSHDebug::m_nValue[5][CSHDebug::DEBUG_INFO_KIND_MAX_NUM];
int	 CSHDebug::m_nExceptionProcGrade;
UINT CSHDebug::m_nErrorCode;

CSHDebug csUselessDebugForStaticInit;
// -------------------------------------------------------------------------------------------------------------------------------------
// CSHDebug Method																												  생성자
//
CSHDebug::CSHDebug()
{
	ZeroMemory(m_bShowDebugInfo, sizeof(m_bShowDebugInfo));
	m_nExceptionProcGrade = 0;
	if (_access("USE_TRYCATCH", 0) != -1)
	{
		m_nExceptionProcGrade = 1;
	}

	#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER) && !defined(_DISTRIBUTESERVER_) && !defined(_GMTOOL_)
		m_nExceptionProcGrade = 1;
	#endif

	m_nErrorCode = 0;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHDebug Method																												  파괴자
//
CSHDebug::~CSHDebug()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------
// MainLoop Method																													메인
//
VOID CSHDebug::MainLoop()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------
// Render Method																												  그리기
//
VOID CSHDebug::Render()
{
	RenderDebugInfoFarm();
}

// -------------------------------------------------------------------------------------------------------------------------------------
// LogExceptionError Method																									   예외 처리
//
void CSHDebug::LogExceptionError(char* pszTxt)
{
	SYSTEMTIME t;
	GetLocalTime(&t);

	TCHAR szTime[_MAX_PATH];
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	FILE *fp = fopen("Log/TryCatch_Map.log", "a+");
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pszTxt,  szTime);
		fclose(fp);
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ParseTextInput Method																						   텍스트 입력 분석/처리
//
VOID CSHDebug::ParseTextInput(char* pszText, char* pCmd)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER) && !defined(_DISTRIBUTESERVER_) && defined(_GMTOOL_)
 	if (stricmp(pCmd, "DATE_MATCHING") == 0)
	{
		GAMEIN->GetDateMatchingDlg()->SetActive( TRUE );
	}

	if (stricmp(pCmd, "FU_1") == 0)
	{
		GAMEIN->GetFarmUpgradeDlg()->SetUpgradeTarget(0, 25, CSHFarm::FARM_EVENT_GARDEN);
		GAMEIN->GetFarmUpgradeDlg()->SetActive( TRUE );
	}
	if (stricmp(pCmd, "FU_2") == 0)
	{
		GAMEIN->GetFarmUpgradeDlg()->SetUpgradeTarget(0, 25, CSHFarm::FARM_EVENT_FENCE);
		GAMEIN->GetFarmUpgradeDlg()->SetActive( TRUE );
	}
	if (stricmp(pCmd, "FU_3") == 0)
	{
		GAMEIN->GetFarmUpgradeDlg()->SetUpgradeTarget(0, 25, CSHFarm::FARM_EVENT_HOUSE);
		GAMEIN->GetFarmUpgradeDlg()->SetActive( TRUE );
	}
	if (stricmp(pCmd, "FU_4") == 0)
	{
		GAMEIN->GetFarmUpgradeDlg()->SetUpgradeTarget(0, 25, CSHFarm::FARM_EVENT_WAREHOUSE);
		GAMEIN->GetFarmUpgradeDlg()->SetActive( TRUE );
	}
	if (stricmp(pCmd, "FU_5") == 0)
	{
		GAMEIN->GetFarmUpgradeDlg()->SetUpgradeTarget(0, 25, CSHFarm::FARM_EVENT_ANIMALCAGE);
		GAMEIN->GetFarmUpgradeDlg()->SetActive( TRUE );
	}

	if (stricmp(pCmd, "FARM_BUY") == 0)
	{
		g_csFarmManager.CLI_RequestBuyFarmToSrv();
	}

	if (stricmp(pCmd, "FARM_BUY_") == 0)
	{
		int nFarmZone = 0, nFarmIndex = 0;
		sscanf(pszText, "%s %d %d", pCmd, &nFarmZone, &nFarmIndex);
		g_csFarmManager.CLI_RequestBuyFarmToSrv(
			CSHFarmZone::FARM_ZONE(nFarmZone),
			WORD(nFarmIndex));
	}

	if (stricmp(pCmd, "FARM_UPGRADE") == 0)
	{
  		int nFarmZone = 0, nFarmIndex = 0, nObjKind = 0;
		sscanf(pszText, "%s %d %d %d", pCmd, &nFarmZone, &nFarmIndex, &nObjKind);
		g_csFarmManager.CLI_RequestUpgradeFarm(
			CSHFarmZone::FARM_ZONE(nFarmZone),
			WORD(nFarmIndex),
			nObjKind);
	}

	if (stricmp(pCmd, "FARM_TAX") == 0)
	{
		g_csFarmManager.CLI_RequestPayTax();
	}

	if (stricmp(pCmd, "SHOW_FARM") == 0)
	{
		ToggleShowDebugInfo(DEBUG_INFO_KIND_FARM_OWN);
	}

	if (stricmp(pCmd, "SHOW_FARM_CROP") == 0)
	{
		int nFarmZone = 0, nFarmIndex = 0, nGardenIndex = 0;
		sscanf(pszText, "%s %d %d %d", pCmd, &nFarmZone, &nFarmIndex, &nGardenIndex);
		ToggleShowDebugInfo(DEBUG_INFO_KIND_FARM_GARDEN, nFarmZone, nFarmIndex, nGardenIndex);
	}

	if (stricmp(pCmd, "RESIDENT_REGIST") == 0)
	{
		CSHResidentRegistManager::stRegistInfo stInfo;

		strcpy(stInfo.szNickName, HERO->GetObjectName());
		stInfo.nSex					= HERO->GetCharacterTotalInfo()->Gender;
		stInfo.nAge					= 30;
		stInfo.nLocation			= 1;
		stInfo.nFavor				= 99;
		stInfo.nPropensityLike01	= 1;
		stInfo.nPropensityLike02	= 2;
		stInfo.nPropensityLike03	= 3;
		stInfo.nPropensityDisLike01	= 4;
		stInfo.nPropensityDisLike02	= 5;

		g_csResidentRegistManager.CLI_RequestRegist(&stInfo);
	}

	if (stricmp(pCmd, "RR_UPDATE") == 0)
	{
		char szIntroduction[MAX_INTRODUCE_LEN] = {0};
		sscanf(pszText, "%s %s", pCmd, szIntroduction);

		g_csResidentRegistManager.CLI_RequestUpdateIntroduction(szIntroduction);
	}

	if (stricmp(pCmd, "RR_SERCH") == 0)
	{
		char szIntroduction[MAX_INTRODUCE_LEN] = {0};
		sscanf(pszText, "%s %s", pCmd, szIntroduction);

		g_csResidentRegistManager.CLI_RequestSerchDateMatching(0, 0, 0);
	}

	if (stricmp(pCmd, "RRS_N") == 0)
	{
		char szIntroduction[MAX_INTRODUCE_LEN] = {0};
		sscanf(pszText, "%s %s", pCmd, szIntroduction);

		g_csResidentRegistManager.CLI_RequestSerchDateMatching(0, 0, 0, 1);
	}

	if (stricmp(pCmd, "RRS_B") == 0)
	{
		char szIntroduction[MAX_INTRODUCE_LEN] = {0};
		sscanf(pszText, "%s %s", pCmd, szIntroduction);

		g_csResidentRegistManager.CLI_RequestSerchDateMatching(0, 0, 0, -1);
	}

	if (stricmp(pCmd, "SHOW_RR") == 0)
	{
		ToggleShowDebugInfo(DEBUG_INFO_KIND_RESIDENT_REGIST);
	}

	if (stricmp(pCmd, "RELOAD_FACETIME") == 0)
	{
		FACIALMGR->LoadScriptFileData();
	}

	if (stricmp(pCmd, "CHAT_REQUEST") == 0)
	{
		int nID = 0;
		sscanf(pszText, "%s %d", pCmd, &nID);

//		g_csResidentRegistManager.CLI_RequestDateMatchingChat(nID);
	}

	if (stricmp(pCmd, "CHAT_CANCEL") == 0)
	{
		g_csResidentRegistManager.CLI_CancelDateMatchingChat();
	}

	if (stricmp(pCmd, "CHAT_REFUSE") == 0)
	{
		g_csResidentRegistManager.CLI_RefuseDateMatchingChat();
	}

  	if (stricmp(pCmd, "CHAT_START") == 0)
	{
		g_csResidentRegistManager.CLI_StartDateMatchingChat();
	}

	if (stricmp(pCmd, "CHAT") == 0)
	{
		char szText[256] = {0};
		sscanf(pszText, "%s %s", pCmd, szText);

		g_csResidentRegistManager.CLI_SendDateMatchingChatText(szText);
	}

	if (stricmp(pCmd, "CHAT_END") == 0)
	{
		g_csResidentRegistManager.CLI_EndDateMatchingChat();
	}

	if (stricmp(pCmd, "FAMILY_CREATE") == 0)
	{
		char szName[256] = {0};
		sscanf(pszText, "%s %s", pCmd, szName);

		g_csFamilyManager.CLI_RequestCreateFamily(szName);
	}

	if (stricmp(pCmd, "FAMILY_INVITE") == 0)
	{
		CObject* pPlayer = OBJECTMGR->GetSelectedObject();
		if (pPlayer)
		{
			if (pPlayer->GetObjectKind() == eObjectKind_Player)
			{
				g_csFamilyManager.CLI_RequestInvite((CPlayer*)pPlayer);
			}
			else
			{
				MessageBox(NULL, "플레이어가 아니여~", NULL, NULL);
			}
		}
		else
		{
			MessageBox(NULL, "플레이어를 선택해주.", NULL, NULL);
		}
	}

	if (stricmp(pCmd, "FAMILY_INVITE_Y") == 0)
	{
		g_csFamilyManager.CLI_RequestAnswerToInvite(CSHFamilyManager::FIR_ACCEPT_PLAYER);
	}

	if (stricmp(pCmd, "FAMILY_INVITE_N") == 0)
	{
		g_csFamilyManager.CLI_RequestAnswerToInvite(CSHFamilyManager::FIR_DENY_PLAYER);
	}

	if (stricmp(pCmd, "FAMILY_NICKNAME") == 0)
	{
		char szName[256] = {0};
		int nTargetPlayerID;

		sscanf(pszText, "%s %d %s", pCmd, &nTargetPlayerID, szName);

		g_csFamilyManager.CLI_RequestChangeNickname(nTargetPlayerID ,szName);
	}

	if (stricmp(pCmd, "FAMILY_EMBLEM") == 0)
	{
		char szName[256] = {0};

		sscanf(pszText, "%s %s", pCmd, szName);

		g_csFamilyManager.CLI_RequestRegistEmblem(szName);
	}

	if (stricmp(pCmd, "FAMILY_LEAVE") == 0)
	{
 		WINDOWMGR->MsgBox( MBI_FAMILY_LEAVE, MBT_YESNO, CHATMGR->GetChatMsg( 1142 ), HERO->GetFamily()->Get()->szName, int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_LEAVE]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_LEAVE] );
	}

	if (stricmp(pCmd, "FAMILY_EXPEL") == 0)
	{
   		WINDOWMGR->MsgBox( MBI_FAMILY_EXPEL, MBT_YESNO, CHATMGR->GetChatMsg( 1143 ), HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->szName, int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_EXPEL]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_EXPEL] );
	}

	if (stricmp(pCmd, "FAMILY_BREAKUP") == 0)
	{
   		WINDOWMGR->MsgBox( MBI_FAMILY_BREAKUP, MBT_YESNO, CHATMGR->GetChatMsg( 1144 ), int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_BREAKUP]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_BREAKUP] );
	}

	if (stricmp(pCmd, "FAMILY_MANAGE") == 0)
	{
		GAMEIN->GetFarmManageDlg()->SetActive( TRUE );
	}

	if (stricmp(pCmd, "SHOW_FAMILY") == 0)
	{
		ToggleShowDebugInfo(DEBUG_INFO_KIND_FAMILY);
	}
	if (stricmp(pCmd, "SHOW_FAMILY") == 0)
	{
		ToggleShowDebugInfo(DEBUG_INFO_KIND_FAMILY);
	}

	if (stricmp(pCmd, "DATEZONE1") == 0)
	{
		g_csDateManager.CLI_RequestDateMatchingEnterDateZone(0);;
	}

	if (stricmp(pCmd, "DATEZONE2") == 0)
	{
		g_csDateManager.CLI_RequestDateMatchingEnterDateZone(1);;
	}

 	if (stricmp(pCmd, "DATEZONE3") == 0)
	{
		g_csDateManager.CLI_RequestDateMatchingEnterDateZone(2);;
	}

	if (stricmp(pCmd, "DATEZONELIST") == 0)
	{
		GAMEIN->GetDateZoneListDlg()->SetActive( TRUE );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ShowDebugInfoFarm Method																						   농장 디버그 정보 표시
//
VOID CSHDebug::RenderDebugInfoFarm()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER) && !defined(_DISTRIBUTESERVER_) && defined(_GMTOOL_)
	static char szText[2048];
	static RECT rect;
	int nFarmZoneNum = CSHFarmZone::FARM_ZONE_MAX;

	if (HERO == NULL) return;

	// 농장 소유 상태 표시
	if (m_bShowDebugInfo[DEBUG_INFO_KIND_FARM_OWN])
	{
		for(WORD i=0; i<nFarmZoneNum; ++i)
		{
			CSHFarmZone* pFarmZone = g_csFarmManager.GetFarmZone(i);
			for(int j=0; j<pFarmZone->GetFarmNum(); j++)
			{
				CSHFarm* pFarm = pFarmZone->GetFarm(j);

				sprintf( szText, "Zone:%d Farm:%d OwnState:%d OwnerID:%d", pFarmZone->GetID(), pFarm->GetID(), pFarm->GetOwnState(), pFarm->GetOwner());
 				SetRect( &rect, 900, 30+15*j, 1, 1 );
 				CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
 			}
		}
	}

	// 텃밭 상태 표시
	if (m_bShowDebugInfo[DEBUG_INFO_KIND_FARM_GARDEN])
	{
		for(WORD i=0; i<nFarmZoneNum; ++i)
		{
			if (i == m_nValue[DEBUG_INFO_KIND_FARM_GARDEN][0])
			{
				CSHFarmZone* pFarmZone = g_csFarmManager.GetFarmZone(i);
				if (pFarmZone == NULL) continue;

				for(int j=0; j<pFarmZone->GetFarmNum(); j++)
				{
					if (j == m_nValue[DEBUG_INFO_KIND_FARM_GARDEN][1])
					{
						CSHFarm* pFarm = pFarmZone->GetFarm(j);
						for(int k=0; k<pFarm->GetGardenNum(); k++)
						{
							CSHGarden* pGarden = pFarm->GetGarden(k);
							for(int p=0; p<pGarden->GetCropNum(); p++)
							{
								CSHCrop* pCrop = pGarden->GetCrop(p);
								sprintf(szText, "Zone:%d Farm:%d Garden:%d, Crop:%d Owner:%d Kind:%d Step:%d LIfe:%d/%d NextStepTime:%d/%d SeedGrade:%d", 
  									pFarmZone->GetID(), pFarm->GetID(), pGarden->GetID(), pCrop->GetID(), pCrop->GetOwner(), pCrop->GetKind(), pCrop->GetStep(), pCrop->GetLife(), pCrop->GetMaxLife(), pCrop->GetNextStepTime(), pCrop->GetNextStepTimeTickFromTbl(pCrop->GetSeedGrade(), pCrop->GetStep()), pCrop->GetSeedGrade());
 								SetRect( &rect, 10+250, 70+15*p, 1, 1 );
 								CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
							}
						}
					}
 				}
			}
		}
	}

	// 주민 등록 상태 표시
	if (m_bShowDebugInfo[DEBUG_INFO_KIND_RESIDENT_REGIST])
	{
		int nCnt = 0;
		sprintf(szText, "Registered : %d", HERO->GetDateMatchingInfo().bIsValid);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

		sprintf(szText, "Age : %d", HERO->GetDateMatchingInfo().dwAge);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

		sprintf(szText, "Location : %d", HERO->GetDateMatchingInfo().dwRegion);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
		
		sprintf(szText, "Favor : %d", HERO->GetDateMatchingInfo().dwGoodFeelingDegree);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

		int nSize = sizeof(HERO->GetDateMatchingInfo().pdwGoodFeeling)/sizeof(HERO->GetDateMatchingInfo().pdwGoodFeeling[0]);
		for(int i=0; i<nSize; i++)
		{
			sprintf(szText, "Like : %d", HERO->GetDateMatchingInfo().pdwGoodFeeling[i]);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
		}

		nSize = sizeof(HERO->GetDateMatchingInfo().pdwBadFeeling)/sizeof(HERO->GetDateMatchingInfo().pdwBadFeeling[0]);
		for(int i=0; i<nSize; i++)
		{
			sprintf(szText, "DisLike : %d", HERO->GetDateMatchingInfo().pdwBadFeeling[i]);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
		}

		sprintf(szText, "Introduction : %s", HERO->GetDateMatchingInfo().szIntroduce);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
	}

	if (m_bShowDebugInfo[DEBUG_INFO_KIND_FAMILY])
	{
		int nCnt = 0;
		sprintf(szText, "Name : %s", HERO->GetFamily()->Get()->szName);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

		sprintf(szText, "Master : %s", HERO->GetFamily()->Get()->szMasterName);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

		sprintf(szText, "MemberNum : %d", HERO->GetFamily()->Get()->nMemberNum);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
		
		sprintf(szText, "HonorPoint : %d", HERO->GetFamily()->GetEx()->nHonorPoint);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

		sprintf(szText, "NicknameON : %d", HERO->GetFamily()->GetEx()->nNicknameON);
 		SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
		nCnt++;
 		CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );
		nCnt++;
		nCnt++;

		for(UINT i=0; i<HERO->GetFamily()->Get()->nMemberNum; i++)
		{
			sprintf(szText, "Name : %s(%d)", HERO->GetFamily()->GetMember(i)->Get()->szName, HERO->GetFamily()->GetMember(i)->Get()->nID);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "Race : %d", HERO->GetFamily()->GetMember(i)->Get()->nRace);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "Sex : %d", HERO->GetFamily()->GetMember(i)->Get()->nSex);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "JobFirst : %d", HERO->GetFamily()->GetMember(i)->Get()->nJobFirst);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "JobCur : %d", HERO->GetFamily()->GetMember(i)->Get()->nJobCur);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "JobGrade : %d", HERO->GetFamily()->GetMember(i)->Get()->nJobGrade);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "Level : %d", HERO->GetFamily()->GetMember(i)->Get()->nLV);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "Nickname : %s", HERO->GetFamily()->GetMember(i)->Get()->szNickname);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "Guild : %s", HERO->GetFamily()->GetMember(i)->GetEx()->szGuild);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			sprintf(szText, "ConState : %d", HERO->GetFamily()->GetMember(i)->Get()->eConState);
 			SetRect( &rect, 10+250, 70+15*nCnt, 1, 1 );
			nCnt++;
 			CFONT_OBJ->RenderFont( 0, szText, strlen(szText), &rect, 0xffffffff );

			nCnt++;
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
// RenderErrorCode Method																												 
//
VOID CSHDebug::RenderErrorCode()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER) && !defined(_DISTRIBUTESERVER_)
	static char szText[256];
	static RECT rect;

	if (m_nErrorCode)
	{
		sprintf(szText, "Error:%d", m_nErrorCode);
 		SetRect( &rect, 10, 75, 1, 1 );
  		CFONT_OBJ->RenderFont( 1, szText, strlen(szText), &rect, 0xffffffff );

		m_nErrorCode = 0;
	}
#endif
}
