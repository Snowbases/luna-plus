// DamageNumber.cpp: implementation of the CDamageNumber class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DamageNumber.h"
#include "..\Engine\Engine.h"
#include "GameResourceManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cImageSelf* CDamageNumber::m_pNumberImage[eDNK_Max];
//cImageSelf* CDamageNumber::m_pMissImage;
cImageSelf* CDamageNumber::m_pCriticalImage;
VECTOR2		CDamageNumber::m_CriticalImgSize;
cImageSelf* CDamageNumber::m_pDodgeImage;
// 080910 LUJ, 블록을 나타내는 이미지가 담길 포인터 정적 선언
cImageSelf* CDamageNumber::m_pBlockImage;

void CDamageNumber::NumberData::Draw(float fAlpha,VECTOR2* pos)
{
	VECTOR2 scale;
	scale.x = 1;
	scale.y = 1;
	if(fAlpha == 2)
	{
		scale.x = 1.5f;
		scale.y = 1.5f;
	}
	DWORD Color = COLORtoDWORD(1,1,1,fAlpha);

	VECTOR2 dispos;
	dispos.x = spos.x + pos->x;
	dispos.y = spos.y + pos->y;
	
	pImage->RenderSprite(
		&scale,NULL,0,&dispos,Color);
}
void CDamageNumber::NumberData::SetImage(cImageSelf* p,VECTOR2* pos)
{
	spos = *pos;
	pImage = p;
}


CDamageNumber::CDamageNumber()
{
	m_bDraw = FALSE;
}

CDamageNumber::~CDamageNumber()
{
	SAFE_DELETE_ARRAY( m_pNumberImage[eDNK_Yellow] );
	SAFE_DELETE_ARRAY( m_pNumberImage[eDNK_Green] );
	SAFE_DELETE_ARRAY( m_pNumberImage[eDNK_Red] );
	SAFE_DELETE_ARRAY( m_pNumberImage[eDNK_Blue] );
	SAFE_DELETE( m_pCriticalImage );
	SAFE_DELETE( m_pDodgeImage );
	// 080910 LUJ, 블록을 표시할 이미지 해제
	SAFE_DELETE( m_pBlockImage );
}


BOOL CDamageNumber::LoadImage()
{
	char szfile[MAX_PATH] = {0,};

	DeleteImage();

	m_pNumberImage[eDNK_Yellow] = new cImageSelf[10];
	for(int n=0;n<10;++n)
	{
		sprintf(szfile,"./Data/Interface/2DImage/image/y%d.tif",n);
		m_pNumberImage[eDNK_Yellow][n].LoadSprite(szfile);
	}
	
	m_pNumberImage[eDNK_Green] = new cImageSelf[10];
	for(n=0;n<10;++n)
	{
		sprintf(szfile,"./Data/Interface/2DImage/image/g%d.tif",n);
		m_pNumberImage[eDNK_Green][n].LoadSprite(szfile);
	}
	
	m_pNumberImage[eDNK_Red] = new cImageSelf[10];
	for(n=0;n<10;++n)
	{
		sprintf(szfile,"./Data/Interface/2DImage/image/r%d.tif",n);
		m_pNumberImage[eDNK_Red][n].LoadSprite(szfile);
	}

	m_pNumberImage[eDNK_Blue] = new cImageSelf[10];
	for(n=0;n<10;++n)
	{
		sprintf(szfile,"./Data/Interface/2DImage/image/b%d.tif",n);
		m_pNumberImage[eDNK_Blue][n].LoadSprite(szfile);
	}

//	sprintf(szfile,"./Data/Interface/2DImage/Damage/miss.tif",n);
//	m_pMissImage = new cImageSelf;
//	m_pMissImage->LoadSprite(szfile);

	sprintf(szfile,"./Data/Interface/2DImage/image/critical.tif");
	m_pCriticalImage = new cImageSelf;
	m_pCriticalImage->LoadSprite(szfile);
	m_pCriticalImage->GetImageOriginalSize(&m_CriticalImgSize);

	sprintf(szfile,"./Data/Interface/2DImage/image/dodge.tif");
	m_pDodgeImage = new cImageSelf;
	m_pDodgeImage->LoadSprite(szfile);

	// 080910 LUJ, 블록 표시할 이미지를 저장할 공간을 힙에 할당하고 읽어옴
	sprintf(szfile,"./Data/Interface/2DImage/image/eff_s_block.tif");
	m_pBlockImage = new cImageSelf;
	m_pBlockImage->LoadSprite(szfile);

	return TRUE;
}

void CDamageNumber::DeleteImage()
{
	SAFE_DELETE_ARRAY(m_pNumberImage[eDNK_Yellow]);
	SAFE_DELETE_ARRAY(m_pNumberImage[eDNK_Green]);
	SAFE_DELETE_ARRAY(m_pNumberImage[eDNK_Red]);
//	SAFE_DELETE(m_pMissImage);
	SAFE_DELETE(m_pCriticalImage);
	SAFE_DELETE(m_pDodgeImage);
	// 080910 LUJ, 블록 이미지를 메모리에서 해제
	SAFE_DELETE(m_pBlockImage);
}

void CDamageNumber::SetDodge(VECTOR3* pPos,VECTOR3* pVelocity)
{
	m_bDodge = TRUE;
	m_bCritical = FALSE;
	m_bDecisive = FALSE;
	// 080910 LUJ, 블록 여부를 나타내는 플래그 초기화
	m_bBlock	= FALSE;
		
	m_CreatedTime = gCurTime;
	m_fAlpha = 1;

	m_Position = *pPos;
	m_Direction = *pVelocity;
	m_Damage = 0;
	m_Jarisu = 0;

	m_bDraw = TRUE;
}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CDamageNumber::SetDamage(DWORD Damage,VECTOR3* pPos,VECTOR3* pVelocity,BYTE nDamageNumberKind,const RESULTINFO& result)
{
	// desc_hseos_ASSERT수정_01
	// S ASSERT수정 추가 added by hseos 2007.06.01
	// ..데미지 수치가 비정상적으로 클 경우 아래의 숫자를 찍는 부분에서 무한루프등의 오류가 발생할 수 있다.
	// ..데미지가 9999999 를 넘어가면 화면상의 표시는 무조건 9999999 로 해서 클라이언트 오류는 나지 않게 한다.
 	if (Damage > 9999999)
 	{
		Damage = 9999999;
	}
	// E ASSERT수정 추가 added by hseos 2007.06.01

	m_bDodge	= result.bDodge;
	m_bCritical = result.bCritical;
	m_bDecisive = result.bDecisive;
	m_bBlock	= result.bBlock;

	m_CreatedTime = gCurTime;
	m_fAlpha = 1;

	m_Position = *pPos;
	
	if(m_bCritical)
	{		
//		VECTOR3 temp;
		m_PositionCritical = m_Position;
	}

	m_Direction = *pVelocity;

	float charwidth = 24;
	float charheight = 12;

	m_Damage = Damage;

	// 몇자리 수인지 구한다
	m_Jarisu = 0;
	DWORD tens=1;
	while(tens<=m_Damage)
	{
		tens*=10;
		++m_Jarisu;
	}
	if(m_Damage == 0)
	{
		/*
		VECTOR2 ScreenPos;
		float MissWidth = 50;
		float MissHeight= 15;
		ScreenPos.x = -MissWidth*m_Jarisu*0.5f;
		ScreenPos.y = -MissHeight*0.5f;

		m_Jarisu = 1;
		m_Numbers[0].SetImage(m_pMissImage,&ScreenPos);
		m_bDraw = TRUE;
		*/
		return;
	}

	ASSERT(m_Jarisu <= MAX_DAMAGE_JARISU);


	// 각자리의 수를 구해서 image와 좌표를 셋팅한다.
	tens /= 10;
	DWORD eachnum;
	VECTOR2 ScreenPos;
	ScreenPos.x = -charwidth*m_Jarisu*0.5f;
	ScreenPos.y = -charheight*0.5f;

	for(DWORD n=0;n<m_Jarisu;++n)
	{
		eachnum = Damage / tens;
		ASSERT(eachnum<10);
		m_Numbers[n].SetImage(&m_pNumberImage[nDamageNumberKind][eachnum],&ScreenPos);

		Damage = Damage % tens;
		tens /= 10;
		ScreenPos.x += charwidth;
	}

	m_bDraw = TRUE;
}

BOOL CDamageNumber::Render()
{
	if(m_bDraw == FALSE)
		return FALSE;
	if(m_fAlpha == 0)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	// 움직임
	float fVelocity = 1.8f;
	float TotalTime = 700.f;
	float HighestTime = 150.f;
	float AlphaStartTime = 150.f;
	float LeanAngle = 0.001f;
	float Elapsedtime = (float)(gCurTime - m_CreatedTime);
	float RealElapsedtime = Elapsedtime;

	if(Elapsedtime > TotalTime)
	{
		m_bDraw = FALSE;
		return FALSE;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 알파
	if(Elapsedtime > AlphaStartTime+ 200.f)
	{
		float ttt = Elapsedtime - AlphaStartTime;
		m_fAlpha = 1 - ttt / (TotalTime - AlphaStartTime);
	}
	else
	{
		m_fAlpha = 1;
	}

	if(Elapsedtime > AlphaStartTime )
	{
		float Des = Elapsedtime - AlphaStartTime;
		Elapsedtime = AlphaStartTime + Des*0.1f;
	}
	
	VECTOR3 pos;
	float cury = (float)(-fVelocity*LeanAngle*(Elapsedtime - HighestTime)*(Elapsedtime - HighestTime) + 10.f*Elapsedtime*0.02);
	pos = m_Position;
	pos.y += cury;
	pos.x = m_Position.x + Elapsedtime * m_Direction.x * fVelocity;
	pos.z = m_Position.z + Elapsedtime * m_Direction.z * fVelocity;
	
	VECTOR3 Temp;
	GetScreenXYFromXYZ(g_pExecutive->GetGeometry(),0,&pos,&Temp);
	if(Temp.x < 0 || Temp.x > 1 || Temp.y < 0 || Temp.y > 1)
	{
		return TRUE;
	}

	VECTOR2 ScreenPos;
	ScreenPos.x = GAMERESRCMNGR->m_GameDesc.dispInfo.dwWidth*Temp.x;
	ScreenPos.y = GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight*Temp.y;

	if(m_bDodge)
	{
		DWORD Color = COLORtoDWORD(1,1,1,m_fAlpha);
		m_pDodgeImage->RenderSprite(NULL,NULL,0,&ScreenPos,Color);
	}
	for(DWORD n=0;n<m_Jarisu;++n)
	{
		m_Numbers[n].Draw(m_fAlpha,&ScreenPos);
	}
	

	//////////////////////////////////////////////////////////////////////////
	// Critical
	// 06. 07. 내공 적중(일격) - 이영준
	// 080910 LUJ, 블록 상태 때 표시하도록 조건을 추가
	if(m_bCritical || m_bDecisive || m_bBlock)
	{
		static float CriticalOverHeight = 45 + 20;
		GetScreenXYFromXYZ(g_pExecutive->GetGeometry(),0,&m_Position,&Temp);
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
		ScreenPos.x = ScreenPos.x - (m_CriticalImgSize.x*0.5f*scale.x);
		ScreenPos.y = ScreenPos.y - (m_CriticalImgSize.y*0.5f*scale.y);

		// 080910 LUJ, 크리티컬일 때 표시할 이미지가 다르다
		if( m_bCritical )
		{
			m_pCriticalImage->RenderSprite(&scale,NULL,0,&ScreenPos,Color);
			
			// 080910 LUJ, 블록과 함께 표시될 때 겹쳐지지 않기 위해 위치를 변경한다
			const float relativePosition = 0.25f;
			ScreenPos.x	-= m_CriticalImgSize.x * relativePosition;
			ScreenPos.y -= m_CriticalImgSize.y * relativePosition;
		}

		// 080910 LUJ, 블록 때 표시할 이미지가 다르다
		if( m_bBlock )
		{
			m_pBlockImage->RenderSprite(&scale,NULL,0,&ScreenPos,Color);
		}
	}

	return TRUE;
}