/*********************************************************************

	 파일		: SHProgressBar.cpp
	 작성자		: hseos
	 작성일		: 2007/09/20

	 파일설명	: 게이지 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "SHProgressBar.h"
#include "cScriptManager.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CSHProgressBar
//

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHProgressBar Method																										  생성자
//
CSHProgressBar::CSHProgressBar()
{
	m_nShowTime = NULL;
	m_nShowTimeTick = NULL;
	m_nDeltaTime = NULL;
	m_nBarType = BARTYPE_01;
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHProgressBar Method																										  파괴자
//
CSHProgressBar::~CSHProgressBar()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_Start Method																												   시작
//
VOID CSHProgressBar::CLI_Start(int nBarType, int nWidth, DWORD nShowTime, DWORD nDeltaTime)
{
  	int nImgNum = 114;
	int nBgImgNum = 115;

 	m_nBarType = nBarType;
 	switch(nBarType)
	{
	case BARTYPE_FARM_REPLANT:	nImgNum = 2; nBgImgNum = 116;		break;
 	case BARTYPE_FARM_REMANURE:	nImgNum = 2; nBgImgNum = 117;		break;
	default:					nImgNum = 114; nBgImgNum = 115;		m_nBarType = BARTYPE_01; break;
	}

	SCRIPTMGR->GetImage(nImgNum,	&m_csImage, PFT_HARDPATH);
	SCRIPTMGR->GetImage(nBgImgNum,	&m_csBgImage, PFT_HARDPATH);
	m_nWidth = nWidth;
	m_nShowTime = nShowTime;
	m_nShowTimeTick = gCurTime;
	m_nDeltaTime = 0;
	if (nDeltaTime)
	{
		m_nDeltaTime = m_nShowTime - nDeltaTime;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------
// CLI_Render Method																											    렌더
//
VOID CSHProgressBar::CLI_Render(VECTOR2* pPos)
{
  	if (m_nShowTime)
	{
 		DWORD nPastTime = gCurTime - m_nShowTimeTick;

 		if (nPastTime+m_nDeltaTime > m_nShowTime)
		{
			m_nShowTime = NULL;
			return;
		}

 		VECTOR2 stPos = *pPos;

		switch(m_nBarType)
		{
		case BARTYPE_01:
			{
				stPos.x -= m_nWidth/2;
				for(int i=0; i<m_nWidth; i++)
				{
					m_csBgImage.RenderSprite(NULL, NULL, 0.0f, &stPos, 0xffffffff);
 					stPos.x++;
				}

  				stPos.x = pPos->x - m_nWidth/2;
				int nNum = m_nWidth*nPastTime/m_nShowTime;
				for(int i=0; i<nNum; i++)
				{
					m_csImage.RenderSprite(NULL, NULL, 0.0f, &stPos, 0xffffffff);
 					stPos.x++;
				}
/*
 				VECTOR2 stScale = { (float)m_nWidth, 1.0f };
				stPos.x -= m_nWidth/2;
				float nNum = m_nWidth*nPastTime/m_nShowTime;

				m_csBgImage.RenderSprite(&stScale, NULL, 0.0f, &stPos, 0xffffffff);
				stScale.x = (float)nNum;
				m_csImage.RenderSprite(&stScale, NULL, 0.0f, &stPos, 0xffffffff);
*/
			}
			break;
		case BARTYPE_FARM_REPLANT:
		case BARTYPE_FARM_REMANURE:
		case BARTYPE_FARM_ANIMAL_REFEED:
		case BARTYPE_FARM_ANIMAL_RECLEANING:
			{
				m_csBgImage.RenderSprite(NULL, NULL, 0.0f, &stPos, 0xffffffff);

 				m_nWidth = m_csBgImage.GetImageRect()->bottom - m_csBgImage.GetImageRect()->top;
				int nNum = m_nWidth - m_nWidth*(nPastTime+m_nDeltaTime)/m_nShowTime;
				stPos.y += m_nWidth - nNum;
				VECTOR2 stScale = {1.0f, (float)nNum};
				m_csImage.RenderSprite(&stScale, NULL, 0.0f, &stPos, 0xffffffff);
			}
			break;
		}
	}
}
