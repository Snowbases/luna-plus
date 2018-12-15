#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "./Interface/cTabDialog.h"


class cListCtrl;
class cPrtList;

enum eBonusListDlgType
{
	eBonusNameList = 0,
	eDecoPointList = 1,
	eUsePointList = 2,
	eBonusListMaxType,
};

//꾸미기 보너스 사용 UI
//090818 pdy 하우징 UI기능변경 cHousingDecoPointDlg가 탭다이알로그로 변경
class cHousingDecoPointDlg : public cTabDialog
{
	cStatic*		m_pDecoPointStatic;
	cStatic*		m_pStarPointStatic;
	int				m_nSelectBonus;

	cPtrList		m_DecoNormalBonusList;
	cPtrList		m_DecoSpacialBonusList;

public:
	cHousingDecoPointDlg();
	virtual ~cHousingDecoPointDlg();
	virtual DWORD ActionEvent(CMouse * mouseInfo);

	void Linking();
	void SetActive( BOOL val );

	void RefreshBonusList(DWORD dwDecoPoint,DWORD dwStarPoint);
	void InitBonusList();

	stHouseBonusInfo* GetCurSelectBonusInfo();

	void Add(cWindow * window);
	virtual void Render();
	virtual void SelectTab(BYTE idx);

	// 091211 pdy 폰트 문제로 ListDlg를 3개로 확장 
	void OnActionEvent( LONG id, void* p, DWORD event );
	void GetListDialogArrByTabNum(cListDialog** pArr,BYTE TabNum);
};

