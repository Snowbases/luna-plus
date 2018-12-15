// cTabDialog.h: interface for the cTabDialog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _cTABDIALOG_H_
#define _cTABDIALOG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cDialog.h"


// 071210 LYW --- cTabDialog : 
// 기존의 탭 다이얼로그는 탭의 수 만큼 탭을 추가는 하되,추가되는 탭이 모두 보여졌다. 
// 하지만 경우에 따라, 일정 탭은 보이고, 일정 탭은 보이지 않는 상태로 시작해야 하는 경우가 있다.
// ( 예 : 인벤토리 기본 2칸, 인벤 확장 아이템 사용시 확장 )
// 필요에 따라 탭 다이얼로그를 초기화 할 때, 정해진 수 만큼만, 
// 탭이 보이는 상태로 처리하기 위해 탭 다이얼로그의 스타일을 추가한다.
enum TABDIALOG_STYPE
{
	e_Normal = 0,		// 노멀 스타일은, 기존의 탭 다이얼로그 방식이다.
	e_VisibleOption,	// 보이기 옵션이 있는 스타일은, 스크립트에 #TAB_STYLE에 스타일 번호와 보일 수를 세팅한다.
						// #TAB_STYLE 1 2 : 보이기 여부 옵션이 있는 탭이며, 탭은 2개만 활성화 된 상태로 시작한다.
	e_Max_Style,
} ;


class cPushupButton;

class cTabDialog : public cDialog  
{
public:
	cTabDialog();
	virtual ~cTabDialog();
	virtual void InitTab(BYTE tabNum);
	virtual void Render();
	virtual void RenderTabComponent();
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	void SetAbsXY(LONG x, LONG y);
	virtual void SetActive(BOOL val);
	virtual void SetAlpha(BYTE al);
	virtual void SetOptionAlpha( DWORD dwAlpha);

	//090119 pdy hide window
	virtual void SetHide(BOOL Val);

	void AddTabBtn(BYTE idx, cPushupButton * btn);
	void AddTabSheet(BYTE idx, cWindow * sheet);
	
	cPushupButton * GetTabBtn(BYTE idx);
	cWindow * GetTabSheet(BYTE idx);

	BYTE GetCurTabNum() { return m_bSelTabNum; }
	BYTE GetTabNum(){ return m_bTabNum; }
	
	virtual void SetDisable( BOOL val );
	virtual void SelectTab(BYTE idx);
	
	virtual cWindow * GetWindowForID(LONG id);
	virtual void SetTabStyle(BYTE byStyle);
	BYTE GetTabStyle() const { return m_byTabStyle; }
	void SetVisibleTabCount(BYTE byCount);
	BYTE GetVisibleTabCount() const { return m_byVisibleTabCount; }
	void ShowTab(BYTE byTabNum, BOOL bShow);

protected:
	BYTE curIdx1;
	BYTE curIdx2;
	BYTE m_bTabNum;
	BYTE m_bSelTabNum;
	cPushupButton** m_ppPushupTabBtn;
	cWindow** m_ppWindowTabSheet;
//	DWORD	m_BtnPushstartTime;
//	DWORD	m_BtnPushDelayTime;

	// 071210 LYW --- cTabDialog : 탭 다이얼로그의 스타일을 담는 변수를 추가한다.
	BYTE m_byTabStyle ;
	// 071210 LYW --- cTabDialog : 보여 질 탭 카운트를 담는 변수를 추가한다.
	BYTE m_byVisibleTabCount ;
};

#endif // _cTABDIALOG_H_
