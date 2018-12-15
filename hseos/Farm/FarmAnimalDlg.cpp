#include "stdafx.h"
#include "FarmAnimalDlg.h"
#include "WindowIDEnum.h"
#include "ObjectGuagen.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "cScriptManager.h"
#include "../hseos/Farm/SHFarmManager.h"

CYHHashTable< int > imageTable;

CAnimalDialog::CAnimalDialog(void)
{
	m_pAnimal = NULL;
	m_bLockGetItemBtn = FALSE;
}

CAnimalDialog::~CAnimalDialog(void)
{
}

void CAnimalDialog::Linking()
{
	m_pSlotNum				=	(cStatic*)GetWindowForID(FARM_ANIMAL_SLOT_NUM);
	m_pName					=	(cStatic*)GetWindowForID(FARM_ANIMAL_NAME);
	m_pContentmentGuage		=	(CObjectGuagen*)GetWindowForID(FARM_ANIMAL_CONTENTMENT_GUAGE);
	m_pContentment			=	(cStatic*)GetWindowForID(FARM_ANIMAL_CONTENTMENT);
	m_pHPGuage				=	(CObjectGuagen*)GetWindowForID(FARM_ANIMAL_HP_GUAGE);
	m_pHP					=	(cStatic*)GetWindowForID(FARM_ANIMAL_HP);
	m_pInterestGuage		=	(CObjectGuagen*)GetWindowForID(FARM_ANIMAL_INTEREST_GUAGE);
	m_pInterest				=	(cStatic*)GetWindowForID(FARM_ANIMAL_INTEREST);
	m_pImage				=	(cStatic*)GetWindowForID(FARM_ANIMAL_IMAGE);
	m_pGetItemBtn			=	(cPushupButton*)GetWindowForID(FARM_ANIMAL_GET_ITEM_BTN);
	m_pGetItemBtn->SetActive( FALSE );
}

void CAnimalDialog::SetActive(BOOL val)
{
	if( val )
	{
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);
	}
	else
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);

	cDialog::SetActive(val);
}

VOID CAnimalDialog::SetSlotNum(char* pText)
{
	if( pText )
	{
		m_pSlotNum->SetStaticText(pText);
	}
}

VOID CAnimalDialog::SetName(char* pText)
{
	if( pText )
	{
		m_pName->SetStaticText(pText);
	}
}

VOID CAnimalDialog::SetContentment(char* pText)
{
	if( pText )
	{
		m_pContentment->SetStaticText(pText);
	}
}

VOID CAnimalDialog::SetInterest(char* pText)
{
	if( pText )
	{
		m_pInterest->SetStaticText(pText);
	}
}

VOID CAnimalDialog::SetContentmentValue(GUAGEVAL val, DWORD estTime)
{
	if( m_pContentmentGuage )
	{
		m_pContentmentGuage->SetValue(val, estTime);
	}
}

VOID CAnimalDialog::SetInterestValue(GUAGEVAL val, DWORD estTime)
{
	if( m_pInterestGuage )
	{
		m_pInterestGuage->SetValue(val, estTime);
	}
}

VOID CAnimalDialog::SetAnimal(CAnimal* pAnimal)
{
	if( pAnimal )
	{
		m_pAnimal = pAnimal;

		RefreshAnimalInfo();
	}
}

VOID CAnimalDialog::SetHP(char* pText)
{
	if( m_pHP )
	{
		m_pHP->SetStaticText(pText);
	}
}

VOID CAnimalDialog::RefreshAnimalInfo()
{
	if( IsActive() )
	{
		if( m_pAnimal )
		{
			char text[256];

			memset(text, NULL, sizeof(text));
			sprintf( text, "%d/%d", m_pAnimal->GetContentment(), CAnimal::ANIMAL_STEP_MAX_CONTENTMENT);
			SetContentment( text );
			SetContentmentValue((GUAGEVAL)m_pAnimal->GetContentment()/(GUAGEVAL)CAnimal::ANIMAL_STEP_MAX_CONTENTMENT, 0);

			memset(text, NULL, sizeof(text));
			sprintf( text, "%d/%d", m_pAnimal->GetInterest(), CAnimal::ANIMAL_STEP_MAX_INTEREST);
			SetInterest( text );
			SetInterestValue((GUAGEVAL)m_pAnimal->GetInterest()/(GUAGEVAL)CAnimal::ANIMAL_STEP_MAX_INTEREST, 0);

			memset(text, NULL, sizeof(text));
			sprintf( text, "%d/%d", m_pAnimal->GetLife(), CAnimal::ANIMAL_STEP_MAX_LIFE );
			SetHP( text );
			SetHPValue((GUAGEVAL)m_pAnimal->GetLife()/(GUAGEVAL)CAnimal::ANIMAL_STEP_MAX_LIFE, 0);

			memset(text, NULL, sizeof(text));
			sprintf( text, "%d", m_pAnimal->GetID()+1 );
			SetSlotNum( text );
			SetName( m_pAnimal->GetRenderObj()->GetObjectName() );

			if(m_pAnimal->GetLife() == 0)
			{
				GetItemBtn()->SetActive( TRUE );
			}
			else
			{
				GetItemBtn()->SetActive( FALSE );
			}

			cImage img;
			SCRIPTMGR->GetImage( GetImgNum(m_pAnimal->GetKind()), &img, PFT_ITEMPATH);
			m_pImage->SetBasicImage( &img );
			m_pImage->SetActive( TRUE );
		}
	}
}

VOID CAnimalDialog::SetHPValue(GUAGEVAL val, DWORD estTime)
{
	if( m_pHPGuage )
	{
		m_pHPGuage->SetValue(val, estTime);
	}
}

void CAnimalDialog::OnActionEvent(LONG id, void* p, DWORD event)
{
	switch(id)
	{
	case FARM_ANIMAL_GET_ITEM_BTN:
		{
			g_csFarmManager.CLI_RequestAnimalGetItem();
			break;
		}
	case FARM_ANIMAL_CLOSE_BTN:
		{
			SetActive( FALSE );
			break;
		}
	}
}

int CAnimalDialog::GetImgNum( int animalKind )
{
	return int( imageTable.GetData( animalKind ) );
}

VOID CAnimalDialog::SetAnimalImgInfo( ITEM_KIND itemKind, int imageIndex )
{
	static BOOL isUninitialize = TRUE;

	if( isUninitialize )
	{
		imageTable.Initialize( 5 );
		isUninitialize = ! isUninitialize;
	}

	if( imageTable.GetData( itemKind ) )
	{
		imageTable.Remove( itemKind );
	}

	imageTable.Add( LPINT( imageIndex ), itemKind );
}