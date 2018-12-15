#include "stdafx.h"
#include "SHFarmManageDlg.h"
#include "ObjectManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "ObjectGuagen.h"
#include "cPushupButton.h"
#include "cScriptManager.h"
#include "SHFarmManager.h"
#include "cResourceManager.h"
#include "..\hseos\Family\SHFamily.h"

CYHHashTable< int > imageHashTable;

CSHFarmManageGardenDlg::CSHFarmManageGardenDlg()
{
	ZeroMemory( m_pcsCrop, sizeof( m_pcsCrop ) );
	ZeroMemory(m_bMine, sizeof(m_bMine));
}

CSHFarmManageGardenDlg::~CSHFarmManageGardenDlg()
{
}

void CSHFarmManageGardenDlg::SetCropImgInfo( ITEM_KIND itemKind, int imageIndex )
{
	static BOOL isUninitialize = TRUE;

	if( isUninitialize )
	{
		imageHashTable.Initialize( 5 );
		isUninitialize = ! isUninitialize;
	}

	if( imageHashTable.GetData( itemKind ) )
	{
		imageHashTable.Remove( itemKind );
	}

	imageHashTable.Add( LPINT( imageIndex ), itemKind );
}

int CSHFarmManageGardenDlg::GetImgNum( int nCropKind )
{
	return int( imageHashTable.GetData( nCropKind ) );
}

void CSHFarmManageGardenDlg::SetCropImg(int nCropIndex, int nCropKind, DWORD nCropOwner)
{
  	if (nCropKind == 0)
	{
		m_pcsCrop[nCropIndex]->SetActive(FALSE);
		m_bMine[nCropIndex] = FALSE;
		return;
	}

	cImage img;
	SCRIPTMGR->GetImage( GetImgNum( nCropKind ), &img, PFT_ITEMPATH);
	m_pcsCrop[nCropIndex]->SetBasicImage(&img);
	m_pcsCrop[nCropIndex]->SetImageRGB(RGB(190,190,190));
	m_pcsCrop[nCropIndex]->SetActive(TRUE);

	if (nCropOwner == gHeroID)
	{
		m_bMine[nCropIndex] = TRUE;
	}
}

void CSHFarmManageGardenDlg::Linking()
{
 	for(int i=0; i<MAX_CROP_NUM; i++)
	{
  		m_pcsCrop[i] = (cPushupButton*)GetWindowForID(FARM_MANAGE_GARDEN_CROP01+i);
		m_pcsCrop[i]->SetActive(FALSE);
	}

	SCRIPTMGR->GetImage( 122, &m_csMineSign, PFT_HARDPATH);
}

void CSHFarmManageGardenDlg::Render()
{
 	if( !IsActive() ) return;
	cDialog::Render();

	for(int i=0; i<MAX_CROP_NUM; i++)
	{
		VECTOR2 stPos = m_pcsCrop[i]->m_absPos;
		stPos.x -= (m_csMineSign.GetImageRect()->right - m_csMineSign.GetImageRect()->left)/2;
		stPos.y -= (m_csMineSign.GetImageRect()->bottom - m_csMineSign.GetImageRect()->top)/2;
		if (m_bMine[i])
		{
			m_csMineSign.RenderSprite(NULL, NULL, 0.0f, &stPos, 0xffffffff);
		}
	}
}

CSHFarmManageDlg::CSHFarmManageDlg()
{
	m_pcsMapName					= NULL;
	m_pcsFarmName					= NULL;
	m_pcsCropNum					= NULL;
	m_pcsCropName					= NULL;
	m_pcsCropOwner 					= NULL;
	m_pcsCropGrowTime				= NULL;
	m_pcsCropGrowRemainTime1		= NULL;
	m_pcsCropGrowRemainTime2		= NULL;
	m_pcsCropGrowStep1				= NULL;
	m_pcsCropGrowStep2				= NULL;
	m_pcsCropLife 					= NULL;
	m_pcsCropImg					= NULL;
	m_pcsGardenDlg					= NULL;
	m_pcsCropGrowRemainTimeGauge	= NULL;
	m_pcsCropGrowStepGauge			= NULL;
	m_pcsCropLifeGauge				= NULL;
	
	m_pcsGardenTabBtn				= NULL;
	m_pcsAnimalCageTabBtn			= NULL;

	m_pcsGardenDlg					= NULL;

	m_pcsSelCrop					= NULL;

	m_nFarmZone						= 0;
	m_wFarmGrade					= 0;
	m_csFarm.CreateGarden( 1, CSHFarmManageGardenDlg::MAX_CROP_NUM );
}

CSHFarmManageDlg::~CSHFarmManageDlg()
{
}
void CSHFarmManageDlg::SetActive( BOOL val )
{
 	m_pcsGardenDlg->SetActive(val);
	cDialog::SetActive( val );
}

void CSHFarmManageDlg::Linking()
{
 	m_pcsMapName				= (cStatic*)GetWindowForID(FARM_MANAGE_MAPNAME);
 	m_pcsFarmName				= (cStatic*)GetWindowForID(FARM_MANAGE_FARM_NAME);
 	m_pcsCropNum				= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_NUM);
 	m_pcsCropName				= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_NAME);
 	m_pcsCropOwner 				= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_OWNER);
 	m_pcsCropGrowTime			= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_GROW_TIME);
 	m_pcsCropGrowRemainTime1 	= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_GROW_REMAIN_TIME1);
 	m_pcsCropGrowRemainTime2	= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_GROW_REMAIN_TIME2);
 	m_pcsCropGrowStep1			= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_GROW_STEP1);
 	m_pcsCropGrowStep2			= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_GROW_STEP2);
 	m_pcsCropLife 				= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_LIFE);
 	m_pcsCropImg				= (cStatic*)GetWindowForID(FARM_MANAGE_CROP_IMAGE);

	m_pcsCropGrowRemainTimeGauge	=	(CObjectGuagen*)GetWindowForID(FARM_MANAGE_CROP_GROW_REMAIN_TIME_GAUGE);
	m_pcsCropGrowStepGauge			=	(CObjectGuagen*)GetWindowForID(FARM_MANAGE_CROP_GROW_STEP_GAUGE);
	m_pcsCropLifeGauge				=	(CObjectGuagen*)GetWindowForID(FARM_MANAGE_CROP_LIFE_GAUGE);

	m_pcsGardenTabBtn			 = (cPushupButton*)GetWindowForID(FARM_MANAGE_CROP_TAB);
	m_pcsAnimalCageTabBtn		 = (cPushupButton*)GetWindowForID(FARM_MANAGE_ANIMAL_TAB);

	m_pcsGardenDlg				 = (CSHFarmManageGardenDlg*)GetWindowForID(FARM_MANAGE_GARDEN_DLG);
	m_pcsGardenDlg->Linking();
}

void CSHFarmManageDlg::SetFarm(int nFarmZone, int nFarmID, WORD wGrade)
{ 
	char szText[256];

	m_nFarmZone = nFarmZone; 
	m_csFarm.SetID(
		WORD(nFarmID));

	// 091204 ONS ÆÐ¹Ð¸® ´ÙÀÌ¾ó·Î±×¿¡ ³óÀå¼¼±Ý ³³ºÎ ±â´É Ãß°¡
	// ÅÔ¹ç ·¹º§ ¼³Á¤.
	m_wFarmGrade = wGrade;

	// ¸Ê ÀÌ¸§
	if (GetFarmMapName(m_nFarmZone)) sprintf(szText, "%s", GetFarmMapName(m_nFarmZone));
	else sprintf(szText, "%s", "?");

	m_pcsMapName->SetStaticText(szText);	

	// ³óÀå ÀÌ¸§
  	int nFarmLoc = m_csFarm.GetID()/15;
	switch(nFarmLoc)
	{
	case 0:	sprintf(szText, "%s %d", RESRCMGR->GetMsg(800), m_csFarm.GetID()%15+1); break;
	case 1: sprintf(szText, "%s %d", RESRCMGR->GetMsg(801), m_csFarm.GetID()%15+1); break;
	case 2: sprintf(szText, "%s %d", RESRCMGR->GetMsg(803), m_csFarm.GetID()%15+1); break;
	case 3: sprintf(szText, "%s %d", RESRCMGR->GetMsg(802), m_csFarm.GetID()%15+1); break;
	}
	
	m_pcsFarmName->SetStaticText(szText);
}

void CSHFarmManageDlg::Clear()
{
	char szText[256]="";

	m_pcsMapName->SetStaticText(szText);	
	m_pcsFarmName->SetStaticText(szText);

	for(int i=0; i<15; i++)
	{
		CSHCrop* pCrop = GetFarm()->GetGarden(0)->GetCrop(i);

		pCrop->SetCrop(CSHCrop::CROP_RESULT_DIE);
		SetCropImg(pCrop->GetID(), pCrop->GetKind(), pCrop->GetOwner());
		SetCropViewInfo(pCrop->GetID());
	}
}

char* CSHFarmManageDlg::GetFarmMapName(int nFarmZone)
{
	int nMapNum = 0;
	if (g_csFarmManager.GetFarmMapNum(nFarmZone, &nMapNum))
	{
		return GetMapName(
			MAPTYPE(nMapNum));
	}

	return "";
}

void CSHFarmManageDlg::SetCropViewInfo(int nCropindex, BOOL bUpdate)
{
 	if (bUpdate)
	{
		if (m_pcsSelCrop != m_csFarm.GetGarden(0)->GetCrop(nCropindex))
		{
			return;
		}
	}

	char szText[256];

 	if (m_pcsSelCrop)
	{
		m_pcsGardenDlg->GetCrop(m_pcsSelCrop->GetID())->SetImageRGB(RGB(190,190,190));
	}

 	m_pcsGardenDlg->GetCrop(nCropindex)->SetBasicImage(m_pcsGardenDlg->GetCrop(nCropindex)->GetBasicImage());
	m_pcsGardenDlg->GetCrop(nCropindex)->SetImageRGB(RGB(255,255,255));
	m_pcsSelCrop = m_csFarm.GetGarden(0)->GetCrop(nCropindex);

	if (m_pcsSelCrop->GetKind())
	{
	 	cImage img;
		SCRIPTMGR->GetImage( m_pcsGardenDlg->GetImgNum(m_pcsSelCrop->GetKind()), &img, PFT_ITEMPATH);
		m_pcsCropImg->SetBasicImage(&img);
		m_pcsCropImg->SetActive(TRUE);
	}

	sprintf(szText, "%d", m_pcsSelCrop->GetID()+1);
 	m_pcsCropNum->SetStaticText(szText);
	m_pcsCropName->SetStaticText(g_csFarmManager.GetRenderObjNameFromTbl(m_pcsSelCrop->GetKind(), m_pcsSelCrop->GetStep()));
	CSHFamilyMember* pOwner = HERO->GetFamily()->GetMemberFromID(m_pcsSelCrop->GetOwner());
	if (pOwner)
	{
		sprintf(szText, "%s", pOwner->Get()->szName);
	}
	else
	{
		sprintf(szText, "%s", "?");
	}
	m_pcsCropOwner->SetStaticText(szText);

	int nGrowTime = 0;
	for(int i=CSHCrop::CROP_STEP_1; i<CSHCrop::CROP_STEP_COMPLETE; i++)
	{
		nGrowTime += m_pcsSelCrop->GetNextStepTimeTickFromTbl(m_pcsSelCrop->GetSeedGrade(), (CSHCrop::CROP_STEP)i);
	}
	sprintf(szText, "%d", nGrowTime);
	m_pcsCropGrowTime->SetStaticText(szText);

	sprintf(szText, "%d/%d", m_pcsSelCrop->GetNextStepTime(), m_pcsSelCrop->GetNextStepTimeTickFromTbl(m_pcsSelCrop->GetSeedGrade(), m_pcsSelCrop->GetStep()));
	m_pcsCropGrowRemainTime2->SetStaticText(szText);

 	sprintf(szText, "%d/%d", m_pcsSelCrop->GetStep()-CSHCrop::CROP_STEP_1+1, CSHCrop::CROP_STEP_COMPLETE-CSHCrop::CROP_STEP_1+1);
 	m_pcsCropGrowStep2->SetStaticText(szText);

	sprintf(szText, "%d/%d", m_pcsSelCrop->GetLife(), m_pcsSelCrop->GetMaxLife());
	m_pcsCropLife->SetStaticText(szText);

	float nRemainTime = (float)m_pcsSelCrop->GetNextStepTime()/m_pcsSelCrop->GetNextStepTimeTickFromTbl(m_pcsSelCrop->GetSeedGrade(), m_pcsSelCrop->GetStep());
	m_pcsCropGrowRemainTimeGauge->SetValue(nRemainTime, 0);
	float nStep = (float)(m_pcsSelCrop->GetStep()-CSHCrop::CROP_STEP_1+1)/(CSHCrop::CROP_STEP_COMPLETE-CSHCrop::CROP_STEP_1+1);
	m_pcsCropGrowStepGauge->SetValue(nStep, 0);
	float nLife = (float)m_pcsSelCrop->GetLife()/m_pcsSelCrop->GetMaxLife();
	m_pcsCropLifeGauge->SetValue(nLife, 0);

	if (m_pcsSelCrop->GetStep() == CSHCrop::CROP_STEP_EMPTY)
	{
 		m_pcsCropNum->SetStaticText("");
 		m_pcsCropName->SetStaticText("");
		m_pcsCropOwner->SetStaticText("");
		m_pcsCropGrowTime->SetStaticText("");
		m_pcsCropGrowRemainTime2->SetStaticText("");
 		m_pcsCropGrowStep2->SetStaticText("");
		m_pcsCropLife->SetStaticText("");
		m_pcsCropGrowRemainTimeGauge->SetValue(0, 0);
		m_pcsCropGrowStepGauge->SetValue(0, 0);
		m_pcsCropLifeGauge->SetValue(0, 0);
		m_pcsCropImg->SetActive(FALSE);
		return;
	}
  	else if (m_pcsSelCrop->GetStep() == CSHCrop::CROP_STEP_COMPLETE)
	{
		sprintf(szText, "%d/%d", m_pcsSelCrop->GetNextStepTimeTickFromTbl(m_pcsSelCrop->GetSeedGrade(), m_pcsSelCrop->GetStep()), m_pcsSelCrop->GetNextStepTimeTickFromTbl(m_pcsSelCrop->GetSeedGrade(), m_pcsSelCrop->GetStep()));
		m_pcsCropGrowRemainTime2->SetStaticText(szText);
		m_pcsCropGrowRemainTimeGauge->SetValue(1, 0);
	}
}

void CSHFarmManageDlg::OnActionEvent( LONG lId, void* p, DWORD we )																	
{
	if( we & WE_PUSHDOWN )																											
	{
 		switch( lId )																												
		{
		case FARM_MANAGE_CROP_TAB:																									
			{
				if (m_pcsAnimalCageTabBtn->IsPushed())
				{
					m_pcsAnimalCageTabBtn->SetPush(FALSE);
					m_pcsGardenDlg->SetActive(TRUE);
				}
			}
			break;
		case FARM_MANAGE_ANIMAL_TAB:																								
			{
				if (m_pcsGardenTabBtn->IsPushed())
				{
					m_pcsGardenTabBtn->SetPush(FALSE);
					m_pcsGardenDlg->SetActive(FALSE);
				}
			}
			break;
		case FARM_MANAGE_GARDEN_CROP01:
		case FARM_MANAGE_GARDEN_CROP02:
		case FARM_MANAGE_GARDEN_CROP03:
		case FARM_MANAGE_GARDEN_CROP04:
		case FARM_MANAGE_GARDEN_CROP05:
		case FARM_MANAGE_GARDEN_CROP06:
		case FARM_MANAGE_GARDEN_CROP07:
		case FARM_MANAGE_GARDEN_CROP08:
		case FARM_MANAGE_GARDEN_CROP09:
		case FARM_MANAGE_GARDEN_CROP10:
		case FARM_MANAGE_GARDEN_CROP11:
		case FARM_MANAGE_GARDEN_CROP12:
		case FARM_MANAGE_GARDEN_CROP13:
		case FARM_MANAGE_GARDEN_CROP14:
		case FARM_MANAGE_GARDEN_CROP15:
			{
				SetCropViewInfo(lId - FARM_MANAGE_GARDEN_CROP01);
			}
			break;		
		}
	}
	else if (we & WE_PUSHUP)
	{
		switch( lId )																												
		{
		case FARM_MANAGE_CROP_TAB:																									
			{
				m_pcsGardenTabBtn->SetPush(TRUE);
			}
			break;
		case FARM_MANAGE_ANIMAL_TAB:																								
			{
				m_pcsAnimalCageTabBtn->SetPush(TRUE);
			}
			break;
		case FARM_MANAGE_GARDEN_CROP01:
		case FARM_MANAGE_GARDEN_CROP02:
		case FARM_MANAGE_GARDEN_CROP03:
		case FARM_MANAGE_GARDEN_CROP04:
		case FARM_MANAGE_GARDEN_CROP05:
		case FARM_MANAGE_GARDEN_CROP06:
		case FARM_MANAGE_GARDEN_CROP07:
		case FARM_MANAGE_GARDEN_CROP08:
		case FARM_MANAGE_GARDEN_CROP09:
		case FARM_MANAGE_GARDEN_CROP10:
		case FARM_MANAGE_GARDEN_CROP11:
		case FARM_MANAGE_GARDEN_CROP12:
		case FARM_MANAGE_GARDEN_CROP13:
		case FARM_MANAGE_GARDEN_CROP14:
		case FARM_MANAGE_GARDEN_CROP15:
			{
				SetCropViewInfo(lId - FARM_MANAGE_GARDEN_CROP01);
			}
			break;		
		}
	}
}