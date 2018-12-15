#include "HousingRevolDlg.h"
#include "WindowIDEnum.h"
#include "cButton.h"
#include "cGuageBar.h"
#include "cHousingMgr.h"

CHousingRevolDlg::CHousingRevolDlg()
{
	m_type				= WT_HOUSING_REVOL_DLG;
}
CHousingRevolDlg::~CHousingRevolDlg()
{

}

void CHousingRevolDlg::Linking()
{
	m_BtReset		= ( cButton* )GetWindowForID( HOUSING_REVOL_RESET_BTN );
	m_BtSet			= ( cButton* )GetWindowForID( HOUSING_REVOL_SET_BTN );
	m_BtDel			= ( cButton* )GetWindowForID( HOUSING_REVOL_DEL_BTN );
	m_BtLArrow		= ( cButton* )GetWindowForID( HOUSING_REVOL_LEFT_BTN );
	m_BtRArrow		= ( cButton* )GetWindowForID( HOUSING_REVOL_RIGHT_BTN );

	m_Bar			= ( cGuageBar* )GetWindowForID( HOUSING_REVOL_GUAGE_BAR );
}

DWORD CHousingRevolDlg::ActionEvent(CMouse *mouseInfo)
{
	return cDialog::ActionEvent(mouseInfo);
}

void CHousingRevolDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	//#define Seta_45 0.785f	

	/*
	#define PI			3.14159265358979323846f // Pi
	#define PI_MUL_2	6.28318530717958623200f // 2 * Pi
	#define PI_DIV_2	1.57079632679489655800f // Pi / 2
	#define PI_DIV_4	0.78539816339744827900f // Pi / 4
	*/

	switch(lId)
	{
		case HOUSING_REVOL_RESET_BTN :
			{
				//리셋이아니라 픽킹이동 시작..
				HOUSINGMGR->StartDecoMove();
				HOUSINGMGR->EndDecoAngle();
				SetActive(FALSE);
			}
			break;
		case HOUSING_REVOL_SET_BTN :
			{
				if( HOUSINGMGR->DecideSettingCurDeco() )
				{
					SetActive(FALSE);
				}
			}
			break;
		case HOUSING_REVOL_DEL_BTN :
			{
				if( HOUSINGMGR->RequestUninstallCurDeco() )
				{
					SetActive(FALSE);
				}
			}
			break; 
		case HOUSING_REVOL_LEFT_BTN :
			{
				float fSetaRate = ( m_Bar->GetCurRate() - 0.5f ) * 2.0f;	// 0.0~1.0 -> -1.0~1.0 Rate변환

				//45도 단위로 맞추자
				int Rate = DWORD( fSetaRate * 1000 );
				Rate = 250 * (Rate / 250);
				Rate -= 250;

				if( Rate < -1000 )	 //-180도를 넘으면 -180도
					Rate = -1000;

				fSetaRate = float(Rate / 1000.0f);

				m_Bar->SetCurRate( (fSetaRate * 0.5f) + 0.5f );	// 0 ~ -1 -> 0.5 ~ 0.0f 

			}
			break;
		case HOUSING_REVOL_RIGHT_BTN :
			{
				float fSetaRate = ( m_Bar->GetCurRate() - 0.5f ) * 2.0f;	// 0.0~1.0 -> -1.0~1.0 Rate변환

					//45도 단위로 맞추자
				int Rate = DWORD( fSetaRate * 1000 );
				Rate = 250 * (Rate / 250);
				Rate += 250;

				if( Rate > 1000)	//-180도를 넘으면 -180도
					Rate = 1000;

				fSetaRate = float(Rate / 1000.0f);


				m_Bar->SetCurRate( (fSetaRate * 0.5f) + 0.5f );	// 1 ~ 0 -> 0.5 ~ 1.0f 
			}
			break;
	}
}
void CHousingRevolDlg::SetActive(BOOL val)
{
	cDialog::SetActive(val);

	m_Bar->SetDisable(TRUE);
	m_BtLArrow->SetDisable(TRUE);
	m_BtRArrow->SetDisable(TRUE);
	//m_BtSet->SetDisable(TRUE);
}

void CHousingRevolDlg::SetBarRateFromAngle(float fAngle)
{
	float fRate = 0.5f;

	if( fAngle != 0)
	{
		fRate = (( fAngle / PI ) * 0.5f) + 0.5f;

		if( fRate < 0 )
			fRate = 0.0f;

		if( fRate > 1 ) 
			fRate = 1.0f;
	}

	m_Bar->SetCurRate(fRate);
}

void CHousingRevolDlg::StartRotate()
{
	m_Bar->SetDisable(FALSE);
	m_BtLArrow->SetDisable(FALSE);
	m_BtRArrow->SetDisable(FALSE);
	//m_BtSet->SetDisable(FALSE);
}

float CHousingRevolDlg::GetRotateAngleFromBarRate()
{
	//중간점이 0퍼센트가 되야하므로 - 50퍼센트를 해준다
	float CurRate = ( m_Bar->GetCurRate() - 0.5f ) * 2.0f;	// 0.0~1.0 -> -1.0~1.0 Rate변환

	return (PI * CurRate);
}

