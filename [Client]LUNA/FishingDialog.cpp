#include "stdafx.h"
#include "fishingdialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"
#include ".\interface\cIconDialog.h"
#include ".\interface\cGuageBar.h"
#include "ItemManager.h"
#include "ChatManager.h"
#include "FishingManager.h"
#include "GameIn.h"
#include "ObjectGuagen.h"
#include "FishingGaugeDialog.h"
#include "ObjectManager.h"
#include "InventoryExDialog.h"

CFishingDialog::CFishingDialog(void)
{
	ClearPoint();
	m_CurImage.Clear();
	ZeroMemory(m_dwMission, sizeof(m_dwMission));
	m_wMissonPos = 0;

	int i;
	for(i=0; i<MAX_FISHING_MISSION; i++)
		m_ImageMission[i].Clear();

	SetLastFishingEndTime(0);

	m_bLockStartBtn = FALSE;
	m_dwLockStartBtnTime = 0;
	m_nMissionTime = 0;
	m_dwMissionStartTime = 0;
	m_dwMissionProcessTime = 0;
}

CFishingDialog::~CFishingDialog(void)
{
}

void CFishingDialog::Linking()
{
	m_pStartBtn = (cPushupButton*)GetWindowForID(FISHING_BTN_START);
	m_pIconBait = (cIconDialog*)GetWindowForID(FISHING_ICON_BAIT);
	m_pIconBait->SetDisable(TRUE);

	m_pIconMission = (cIconDialog*)GetWindowForID(FISHING_ICON_MISSION);
	m_pIconMission->SetDisable(TRUE);

	m_pFishingExp = (CObjectGuagen*)GetWindowForID(FISHING_GAUGE_EXP);
	m_pFishingExp->SetDisable(TRUE);

	m_pExpStatic = (cStatic*)GetWindowForID(FISHING_STATIC_EXP);
	m_pPointStatic = (cStatic*)GetWindowForID(FISHING_STATIC_FISHPOINT);
	m_pProcessTime = (cStatic*)GetWindowForID(FISHING_STATIC_PROCESSTIME);


	char szfile[64] = {0,};
	sprintf(szfile,"./Data/Interface/2DImage/image/fishing_perfect.tif");
	m_ImagePerpect.LoadSprite(szfile);
	m_ImagePerpect.GetImageOriginalSize(&m_ImagePerpectRT);

	sprintf(szfile,"./Data/Interface/2DImage/image/fishing_great.tif");
	m_ImageGreat.LoadSprite(szfile);
	m_ImageGreat.GetImageOriginalSize(&m_ImageGreatRT);

	sprintf(szfile,"./Data/Interface/2DImage/image/fishing_good.tif");
	m_ImageGood.LoadSprite(szfile);
	m_ImageGood.GetImageOriginalSize(&m_ImageGoodRT);

	sprintf(szfile,"./Data/Interface/2DImage/image/fishing_miss.tif");
	m_ImageMiss.LoadSprite(szfile);
	m_ImageMiss.GetImageOriginalSize(&m_ImageMissRT);

	sprintf(szfile,"./Data/Interface/2DImage/image/fishing_O.tif");
	m_ImageSuccessMission.LoadSprite(szfile);
	m_ImageSuccessMission.GetImageOriginalSize(&m_ImageSuccessMissionRT);

	sprintf(szfile,"./Data/Interface/2DImage/image/fishing_x.tif");
	m_ImageFailMission.LoadSprite(szfile);
	m_ImageFailMission.GetImageOriginalSize(&m_ImageFailMissionRT);
}

void CFishingDialog::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	SetLastFishingEndTime(0);

	if(val == TRUE)
	{
		m_wMissonPos = 0;
		SetLockStartBtn(FALSE);
	}
	else
	{
		if(m_pStartBtn->IsPushed())
		{
			GAMEIN->GetFishingDlg()->Fishing_Cancel();
		}

		GAMEIN->GetInventoryDialog()->SetMultipleItemLock(m_ItemBait.GetItemIdx(), FALSE);

		m_pIconBait->DeleteIcon(0, 0);
		SetBaitItem(NULL);

		m_pStartBtn->SetPush(FALSE);
	}
	cDialog::SetActive(val);
}

void CFishingDialog::Render()
{
	if(!m_bActive) return;	
	cDialog::RenderWindow();
	cDialog::RenderComponent();

	if(m_CurImage.pImage)
	{
		//////////////////////////////////////////////////////////////////////////
		// 움직임
		float fVelocity = 1.8f;
		float TotalTime = 700.f;
		float HighestTime = 150.f;
		float AlphaStartTime = 150.f;
		float LeanAngle = 0.001f;
		float Elapsedtime = (float)(gCurTime - m_CurImage.dwCreateTime);
		float RealElapsedtime = Elapsedtime;

		if(Elapsedtime > TotalTime)
		{
			m_CurImage.Clear();
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// 알파
		if(Elapsedtime > AlphaStartTime+ 200.f)
		{
			float ttt = Elapsedtime - AlphaStartTime;
			m_CurImage.fAlpha = 1 - ttt / (TotalTime - AlphaStartTime);
		}
		else
		{
			m_CurImage.fAlpha = 1;
		}

		if(Elapsedtime > AlphaStartTime )
		{
			float Des = Elapsedtime - AlphaStartTime;
			Elapsedtime = AlphaStartTime + Des*0.1f;
		}

		VECTOR3 Temp;
		VECTOR3 pos, heropos;
		float cury = (float)(-fVelocity*LeanAngle*(Elapsedtime - HighestTime)*(Elapsedtime - HighestTime) + 10.f*Elapsedtime*0.02);
		HERO->GetPosition(&heropos);
		pos = heropos;
		pos.y += cury;
		pos.x = heropos.x + Elapsedtime * m_CurImage.vDir.x * fVelocity;
		pos.z = heropos.z + Elapsedtime * m_CurImage.vDir.z * fVelocity;
		//
		//VECTOR3 Temp;
		//GetScreenXYFromXYZ(g_pExecutive->GetGeometry(),0,&pos,&Temp);
		//if(Temp.x < 0 || Temp.x > 1 || Temp.y < 0 || Temp.y > 1)
		//{
		//	return TRUE;
		//}

		VECTOR2 ScreenPos;
		static float CriticalOverHeight = 85 + 20;
		GetScreenXYFromXYZ(g_pExecutive->GetGeometry(),0,&pos,&Temp);
		ScreenPos.x = GAMERESRCMNGR->m_GameDesc.dispInfo.dwWidth*Temp.x;
		ScreenPos.y = GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight*Temp.y;
		ScreenPos.y -= CriticalOverHeight;
		static float CRISCALETIME = 100;
		static float CRIALPHTTIME = 1000;
		static float SCALE = 0.5;
		DWORD Color;
		if(RealElapsedtime < CRIALPHTTIME)
			Color = COLORtoDWORD(1,1,1,(CRIALPHTTIME-RealElapsedtime)/CRIALPHTTIME);
		else
			Color = 0;
		VECTOR2 scale;
		if(RealElapsedtime < CRISCALETIME)
			scale.x = scale.y = 1 + SCALE * (CRISCALETIME-RealElapsedtime)/CRISCALETIME;
		else
			scale.x = scale.y = 1;
		ScreenPos.x = ScreenPos.x - (m_CurImage.pImageRect->x*0.5f*scale.x);
		ScreenPos.y = ScreenPos.y - (m_CurImage.pImageRect->y*0.5f*scale.y);

		////////
		//UI에 붙이기 테스트 
		////////
		scale.x = scale.y = 1;
		ScreenPos.x = GAMEIN->GetFishingGaugeDlg()->GetAbsX();
		ScreenPos.y = (GAMEIN->GetFishingGaugeDlg()->GetAbsY()-15) - (0.18f * RealElapsedtime);

		m_CurImage.pImage->RenderSprite(&scale,NULL,0,&ScreenPos,Color);
	}

	VECTOR2 Scale;
	Scale.x = Scale.y = 1;
	VECTOR2 ScreenPos;
	RECT rt;

	for(WORD i=0; i<MAX_FISHING_MISSION; ++i)
	{
		if(m_ImageMission[i].pImage)
		{
			if(m_pIconMission->GetIconCellRect(i, rt))
			{
				ScreenPos.x = m_pIconMission->GetAbsX() + rt.left + 3;
				ScreenPos.y = m_pIconMission->GetAbsY() + rt.top + 3;
				m_ImageMission[i].pImage->RenderSprite(&Scale, NULL, 0, &ScreenPos, 0xffffffff);
			}
		}
	}

	// 미션남은 시간 출력
	if(0<m_dwMissionStartTime && 0<m_dwMissionProcessTime)
	{
		DWORD dwDiff = m_dwMissionProcessTime - (gCurTime - m_dwMissionStartTime);
		int nTime = dwDiff / 1000;

		if(m_nMissionTime != nTime && nTime < 10000)
		{
			m_nMissionTime = nTime;
			char szValue[50];
			sprintf( szValue, "%d%s", m_nMissionTime, CHATMGR->GetChatMsg(795));
			SetProcessTimeText(szValue);
		}
	}


	// 낚시시작버튼 사용가능
	if(m_bLockStartBtn && m_dwLockStartBtnTime+FISHING_STARTBTN_DELAY<gCurTime)
		SetLockStartBtn(FALSE);
}

BOOL CFishingDialog::FakeMoveIcon(LONG x, LONG y, cIcon * pOrigIcon)
{
	if( WT_ITEM != pOrigIcon->GetType() || pOrigIcon->IsLocked() || m_bDisable)
	{
		return FALSE;
	}

	if(FISHINGMGR->IsActive())
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(1529) );
		return FALSE;
	}

	CItem * pOrigItem = (CItem *)pOrigIcon;

	// 인벤토리의 아이템만 허용된다
	if( ! ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, pOrigItem->GetPosition() ) )
	{
		return FALSE;
	}

	const DWORD				itemIndex	= pOrigItem->GetItemIdx();
	const ITEM_INFO*		info		= ITEMMGR->GetItemInfo( itemIndex );
	
	// 미끼만 처리
	if(ITEM_KIND_FISHING_BAIT == info->SupplyType)
	{
		if(FISHINGMGR->GetFishingLevel() < info->Grade)
		{
			//등급제한
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(1530) );
			return FALSE;
		}

		CBaseItem* pPrevItem = (CBaseItem*)m_pIconBait->GetIconForIdx(0);
		if(pPrevItem)
		{
			GAMEIN->GetInventoryDialog()->SetMultipleItemLock(m_ItemBait.GetItemIdx(), FALSE);

			SetBaitItem(NULL);
		}

		if(!m_pIconBait->IsAddable(0))
			return FALSE;

		SetBaitItem(pOrigItem);

		ITEMMGR->AddToolTip( &m_ItemBait );
		m_pIconBait->AddIcon( 0, &m_ItemBait);

		GAMEIN->GetInventoryDialog()->SetMultipleItemLock(pOrigItem->GetItemIdx(), TRUE);

		return FALSE;
	}
	
	return FALSE;
}

void CFishingDialog::ChangeBaitItem(CItem* pItem)
{
	if( WT_ITEM != pItem->GetType() || pItem->IsLocked() || m_bDisable)
	{
		return;
	}

	if(FISHINGMGR->IsActive())
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(1529) );
		return;
	}

	// 인벤토리의 아이템만 허용된다
	if( ! ITEMMGR->IsEqualTableIdxForPos(eItemTable_Inventory, pItem->GetPosition() ) )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 787 ) );
		return;
	}

	const DWORD				itemIndex	= pItem->GetItemIdx();
	const ITEM_INFO*		info		= ITEMMGR->GetItemInfo( itemIndex );
	
	// 미끼만 처리
	if(ITEM_KIND_FISHING_BAIT == info->SupplyType)
	{
		if(FISHINGMGR->GetFishingLevel() < info->Grade)
		{
			//등급제한
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(1530) );
			return;
		}

		CBaseItem* pPrevItem = (CBaseItem*)m_pIconBait->GetIconForIdx(0);
		if(pPrevItem)
		{
			GAMEIN->GetInventoryDialog()->SetMultipleItemLock(m_ItemBait.GetItemIdx(), FALSE);

			SetBaitItem(NULL);
		}

		if(!m_pIconBait->IsAddable(0))
			return;

		SetBaitItem(pItem);

		ITEMMGR->AddToolTip( &m_ItemBait );
		m_pIconBait->AddIcon( 0, &m_ItemBait);

		GAMEIN->GetInventoryDialog()->SetMultipleItemLock(itemIndex, TRUE);

		SetPushStartBtn(FALSE);

		return;
	}
}

void CFishingDialog::SetBaitItem(CBaseItem* pBait)
{
	m_ItemBait.SetData(0);
	m_ItemBait.Init(0, 0);
	m_pIconBait->DeleteIcon(0, NULL);

	if(pBait)
	{
		m_ItemBait.SetData(pBait->GetItemIdx());
		DURTYPE itemDurability = GAMEIN->GetInventoryDialog()->GetTotalItemDurability( pBait->GetItemIdx() );
		m_ItemBait.Init( pBait->GetItemIdx(), itemDurability );

		ITEMMGR->AddToolTip( &m_ItemBait );
		m_pIconBait->AddIcon( 0, &m_ItemBait);
	}

	RefreshBaitItem();
}

void CFishingDialog::UpdateBaitItem()
{
	CItem* pItem = NULL;
	pItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(m_ItemBait.GetItemIdx());
	SetBaitItem(pItem);
	
	RefreshBaitItem();
}

void CFishingDialog::RefreshBaitItem()
{
	DURTYPE prevDur, dur = 0;
	DWORD dwItemIdx = m_ItemBait.GetItemIdx();
	if( dwItemIdx )
	{
		prevDur = m_ItemBait.GetDurability();
		dur	= (WORD)(GAMEIN->GetInventoryDialog()->GetTotalItemDurability(m_ItemBait.GetItemIdx()));

		// 090811 ShinJS --- 개수가 변경된 경우 Item Lock처리를 한다
		if( prevDur != dur )
			GAMEIN->GetInventoryDialog()->SetMultipleItemLock(m_ItemBait.GetItemIdx(), TRUE);
	}

	if(dur <= 0)
		m_ItemBait.SetZeroCount(TRUE);
	else
		m_ItemBait.SetZeroCount(FALSE);

	m_ItemBait.SetDurability(dur);
}

void CFishingDialog::SetMissionItem(DWORD* pItemIdx)
{
	ITEM_INFO* pInfo = NULL;
	cImage image;
	m_wMissonPos = 0;

	for(WORD i=0; i<MAX_FISHING_MISSION; ++i)
	{
		m_pIconMission->DeleteIcon(i, NULL);
		m_dwMission[i] = pItemIdx[i];
		pInfo = ITEMMGR->GetItemInfo( m_dwMission[i] );

		if(pInfo)
		{
			m_Icons[i].Init(0,
			0,
			DEFAULT_ICONSIZE,
			DEFAULT_ICONSIZE,
			ITEMMGR->GetIconImage( m_dwMission[i], &image ),
			0);


			m_Icons[i].SetData(m_dwMission[i]);
			ITEMMGR->AddToolTip(&m_Icons[i]);
			m_pIconMission->AddIcon(i, &m_Icons[i]);
		}

		m_ImageMission[i].Clear();
	}
}

void CFishingDialog::CheckMissionState(DWORD dwItemIdx)
{
	if(dwItemIdx == 0)
		return;

	if(!FISHINGMGR->IsMission() || MAX_FISHING_MISSION <= m_wMissonPos)
		return;

	VECTOR3 vDir;
	vDir.x = vDir.y = vDir.z = 0;
	if(m_dwMission[m_wMissonPos] == dwItemIdx)
	{
		m_ImageMission[m_wMissonPos].SetImage(&m_ImageSuccessMission, &m_ImageSuccessMissionRT, vDir);
	}
	else
	{
		m_ImageMission[m_wMissonPos].SetImage(&m_ImageFailMission, &m_ImageFailMissionRT, vDir);
	}
	m_wMissonPos++;
}

void CFishingDialog::SetExpValue(GUAGEVAL val, DWORD estTime)
{
	if(m_pFishingExp)
	{
		m_pFishingExp->SetValue(val, estTime);
	}
}

void CFishingDialog::SetExpText(char* pText)
{
	if(m_pExpStatic)
	{
		m_pExpStatic->SetStaticText(pText);
	}
}

void CFishingDialog::SetPointText(char* pText)
{
	if(m_pPointStatic)
	{
		m_pPointStatic->SetStaticText(pText);
	}
}

void CFishingDialog::SetProcessTimeText(char* pText)
{
	if(m_pProcessTime)
	{
		m_pProcessTime->SetStaticText(pText);
	}
}

void CFishingDialog::SetLockStartBtn(BOOL bVal)
{
	m_bLockStartBtn = bVal;

	if(m_bLockStartBtn)
	{
		SetLockStartBtnTime(gCurTime);

		if(m_pStartBtn)
			m_pStartBtn->SetDisable(TRUE);
	}
	else
	{
		SetLockStartBtnTime(0);

		if(m_pStartBtn)
			m_pStartBtn->SetDisable(FALSE);
	}
}

void CFishingDialog::Fishing_Start()
{
	DWORD dwBaitIndex = 0;
	POSTYPE BaitPos = 0;

	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForIdx(m_ItemBait.GetItemIdx());
	if(pItem)
	{
		dwBaitIndex = pItem->GetItemIdx();
		BaitPos = pItem->GetPosition();
	}

	ClearPoint();
	FISHINGMGR->SendFishing_Ready(dwBaitIndex, BaitPos);
}

void CFishingDialog::Fishing_Pullling()
{
	if(!GAMEIN->GetFishingGaugeDlg() || !GAMEIN->GetFishingGaugeDlg()->IsActive())
		return;

	if(0 < GAMEIN->GetFishingGaugeDlg()->GetHoldTime())
	{
		return;
	}

	if(FISHINGMGR->GetGaugeRepeatCount() <= m_nPerpect+m_nGreat+m_nGood+m_nMiss)
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg(1531) );
		return;
	}


	VECTOR3 vDir;
	vDir.x=0.0f; vDir.y=0.5f; vDir.z=0.0f;
	float angle = HERO->GetAngle();
	// 게이지의 적중영역은 하드코딩 되어있음.
	cGuageBar* pGaugeBar = GAMEIN->GetFishingGaugeDlg()->GetFishingGB();
	if(pGaugeBar)
	{
		LONG Value = pGaugeBar->GetCurValue();

		if(22<=Value && Value<29)
		{
			m_nPerpect++;
			
			TransToRelatedCoordinate(&vDir,&vDir,angle);
			m_CurImage.SetImage(&m_ImagePerpect, &m_ImagePerpectRT, vDir);
		}
		else if(39<=Value && Value<47)
		{
			m_nGreat++;
			
			TransToRelatedCoordinate(&vDir,&vDir,angle);
			m_CurImage.SetImage(&m_ImageGreat, &m_ImageGreatRT, vDir);
		}
		else if(59<=Value && Value<72)
		{
			m_nGood++;

			TransToRelatedCoordinate(&vDir,&vDir,angle);
			m_CurImage.SetImage(&m_ImageGood, &m_ImageGoodRT, vDir);
		}
		else
		{
			m_nMiss++;
			
			TransToRelatedCoordinate(&vDir,&vDir,angle);
			m_CurImage.SetImage(&m_ImageMiss, &m_ImageMissRT, vDir);
		}

		GAMEIN->GetFishingGaugeDlg()->SetHoldTime();
	}
}

void CFishingDialog::Fishing_Cancel()
{
	ClearPoint();

	FISHINGMGR->SendFishing_Cancel();
	SetLastFishingEndTime(0);
}