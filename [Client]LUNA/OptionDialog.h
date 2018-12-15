#ifndef _OPTIONDIALOG_H
#define _OPTIONDIALOG_H


#include "./Interface/cTabDialog.h"
#include "OptionManager.h"

//class cButton;

class COptionDialog : public cTabDialog
{
protected:
	sGAMEOPTION		m_GameOption;

public :
	void UpdateData( BOOL bSave ) ;

public :
	COptionDialog() ;
	virtual ~COptionDialog() ;

	virtual void Add( cWindow* window ) ;
	virtual void SetActive( BOOL val ) ;

	void Linking() ;
	void OnActionEvent( LONG lId, void* p, DWORD we ) ;
	
	// 100112 ONS 배경, 효과음 게이지바 마우스 이벤트 처리를 위해 추가
	virtual DWORD ActionEvent( CMouse* mouseInfo );

	// 090116 ShinJS --- 캐릭터 안보이기 체크시 펫의 체크박스내용을 Disable 시키는 함수
	void DisableOtherPetRender(BOOL bDisable);


#ifndef _JAPAN_LOCAL_
	void DisableGraphicTab(BOOL bDisable) ;
	void DisableGraphicHideParts(BOOL bDisable) ;
#endif //_JAPAN_LOCAL_
};

////class cButton;
//
//class COptionDialog : public cTabDialog
//{
//protected:
//
//	sGAMEOPTION		m_GameOption;
////	cButton*		m_pBtnPreview;
//	// 061206 LYW --- Add Flag For CheckBox. ( Chatting Option Part )
//	BOOL			m_bCheckSystem ;
//
//protected:
//
//	void UpdateData( BOOL bSave );
//public:
//
//	COptionDialog();
//	virtual ~COptionDialog();
//
//	virtual void Add(cWindow * window);
//	virtual void SetActive(BOOL val);
//
//	void Linking();
//	void OnActionEvent(LONG lId, void * p, DWORD we);
//
//	void DisableGraphicTab(BOOL bDisable);
//
//	// 2005.12.28일 추가 눈 On/Off
//	int GetEffectSnow() { return m_GameOption.nEffectSnow;}
//
//	// 061206 LYW --- Add Function to Setting checkbox.
//	void SetCheckBox( int nTabNum, LONG lId, BOOL* bOptionValue ) ;
///*
//	/// 061206 LYW --- Processing Event Functions.
//	virtual DWORD ActionEvent( CMouse* mouseInfo ) ;
//	*/
//};





#endif
