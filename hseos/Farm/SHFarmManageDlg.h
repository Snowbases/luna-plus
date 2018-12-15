#ifndef _FARMMANAGE_H
#define _FARMMANAGE_H

#include "./Interface/cDialog.h"
#include "../hseos/Farm/SHFarm.h"
#include "cImage.h"

class cStatic;
class CObjectGuagen;
class cPushupButton;

class CSHFarmManageGardenDlg : public cDialog
{
public:
	const static int	MAX_CROP_NUM	= 15;

protected:
	cPushupButton*		m_pcsCrop[MAX_CROP_NUM];

	cImage				m_csMineSign;
	BOOL				m_bMine[MAX_CROP_NUM];

public:
	CSHFarmManageGardenDlg() ;
	virtual ~CSHFarmManageGardenDlg() ;
	static void		SetCropImgInfo( ITEM_KIND, int nCropImg);
	static int		GetImgNum( int nCropKind );
	void			SetCropImg(int nCropIndex, int nCropKind, DWORD nCropOwner);
	cPushupButton*	GetCrop(int nCropIndex)		{ return m_pcsCrop[nCropIndex]; }

	void		 	Linking();
	virtual void 	Render();
};


class CSHFarmManageDlg : public cDialog
{
protected:
	cStatic*				m_pcsMapName;			
	cStatic*				m_pcsFarmName;			
	cStatic*				m_pcsCropNum;			
	cStatic*				m_pcsCropName;			
	cStatic*				m_pcsCropOwner; 			
	cStatic*				m_pcsCropGrowTime;		
	cStatic*				m_pcsCropGrowRemainTime1;
	cStatic*				m_pcsCropGrowRemainTime2;
	cStatic*				m_pcsCropGrowStep1;		
	cStatic*				m_pcsCropGrowStep2;		
	cStatic*				m_pcsCropLife; 			
	cStatic*				m_pcsCropImg;

	CObjectGuagen*			m_pcsCropGrowRemainTimeGauge;
	CObjectGuagen*			m_pcsCropGrowStepGauge;
	CObjectGuagen*			m_pcsCropLifeGauge;
	
	cPushupButton*			m_pcsGardenTabBtn;
	cPushupButton*			m_pcsAnimalCageTabBtn;


	CSHFarmManageGardenDlg*	m_pcsGardenDlg;

	int						m_nFarmZone;
	CSHFarm					m_csFarm;

	CSHCrop*				m_pcsSelCrop;

public:
	CSHFarmManageDlg() ;
	~CSHFarmManageDlg() ;

	void		 	Linking();
	virtual void 	SetActive( BOOL val );
	void			OnActionEvent( LONG lId, void* p, DWORD we );

	char*			GetFarmMapName(int nFarmZone);
	// 091204 ONS UI정보중에서 텃밭레벨도 저장한다.
	void			SetFarm(int nFarmZone, int nFarmID, WORD wGrade);
	void			Clear();
	CSHCrop*		GetCrop(int nCropIndex)				{ return m_csFarm.GetGarden(0)->GetCrop(nCropIndex); }
	CSHFarm*		GetFarm()							{ return &m_csFarm; }

	void			SetCropImg(int nCropIndex, int nCropKind, DWORD nCropOwner)	{ m_pcsGardenDlg->SetCropImg(nCropIndex, nCropKind, nCropOwner); }
	void			SetCropViewInfo(int nCropindex, BOOL bUpdate = FALSE);

// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 추가.
private:
	WORD					m_wFarmGrade;
public:
	int				GetFarmZoneNum()  { return m_nFarmZone; }
	WORD			GetFarmID()  { return m_csFarm.GetID(); }
	WORD			GetFarmGrade() { return m_wFarmGrade; }
};



#endif