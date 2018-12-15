#include "stdafx.h"
#include "SHFarmUpgradeDlg.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "../hseos/Farm/SHFarmManager.h"

CSHFarmUpgradeDlg::CSHFarmUpgradeDlg()
{
	m_CurGrade = NULL;
	m_NextGrade = NULL;
	m_NeedGold = NULL;
	m_NeedFamilyHonor = NULL;

	m_nFarmZone = 0;
	m_nFarmID = 0;
	m_nUpgradeKind = 0;
}

CSHFarmUpgradeDlg::~CSHFarmUpgradeDlg()
{
}
void CSHFarmUpgradeDlg::SetActive( BOOL val )
{
	cDialog::SetActive( val );
}

void CSHFarmUpgradeDlg::Linking()
{
 	m_CurGrade = (cStatic*)GetWindowForID(FARM_UPGRADE_CUR_GRADE);
	m_NextGrade = (cStatic*)GetWindowForID(FARM_UPGRADE_NEXT_GRADE);
 	m_NeedGold = (cStatic*)GetWindowForID(FARM_UPGRADE_NEED_GOLD);
	m_NeedFamilyHonor = (cStatic*)GetWindowForID(FARM_UPGRADE_NEED_HONOR);
}

void CSHFarmUpgradeDlg::Render()
{
 	if( !IsActive() ) return;

	char szTxt[256];
	int nGrade = 0;

	switch(m_nUpgradeKind)
	{
	case CSHFarm::FARM_EVENT_GARDEN:
		nGrade = g_csFarmManager.GetFarmZone((WORD)m_nFarmZone)->GetFarm((WORD)m_nFarmID)->GetGarden(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_FENCE:
		nGrade = g_csFarmManager.GetFarmZone((WORD)m_nFarmZone)->GetFarm((WORD)m_nFarmID)->GetFence(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_HOUSE:
		nGrade = g_csFarmManager.GetFarmZone((WORD)m_nFarmZone)->GetFarm((WORD)m_nFarmID)->GetHouse(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_WAREHOUSE:
		nGrade = g_csFarmManager.GetFarmZone((WORD)m_nFarmZone)->GetFarm((WORD)m_nFarmID)->GetWarehouse(0)->GetGrade();
		break;
	case CSHFarm::FARM_EVENT_ANIMALCAGE:
		nGrade = g_csFarmManager.GetFarmZone((WORD)m_nFarmZone)->GetFarm((WORD)m_nFarmID)->GetAnimalCage(0)->GetGrade();
		break;
	}

	sprintf(szTxt, "%d", nGrade);
	AddComma(szTxt);
	m_CurGrade->SetStaticText(szTxt);

	sprintf(szTxt, "%d", nGrade+1);
	AddComma(szTxt);
	m_NextGrade->SetStaticText(szTxt);

	sprintf(szTxt, "%d", g_csFarmManager.GetUpgradeCost((CSHFarm::FARM_EVENT)m_nUpgradeKind, nGrade)->nGold);
	AddComma(szTxt);
	m_NeedGold->SetStaticText(szTxt);

	sprintf(szTxt, "%d", g_csFarmManager.GetUpgradeCost((CSHFarm::FARM_EVENT)m_nUpgradeKind, nGrade)->nFamilyHonorPoint);
	AddComma(szTxt);
	m_NeedFamilyHonor->SetStaticText(szTxt);

	cDialog::Render();
}
