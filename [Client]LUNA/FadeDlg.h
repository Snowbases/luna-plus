#pragma once
// 080930 LYW --- FadeDlg : 페이드 인/아웃 다이얼로그 추가.
#include "./Interface/cDialog.h"
#include "cImageSelf.h"

enum ENUM_FADEMODE
{
	eFadeNone = 0,
	eFadeIn,
	eFadeOut,
} ;

class CFadeDlg : public cDialog
{
	cImage	m_ImgBackGround ;

	VECTOR2 mScale ;
	VECTOR2 mPos ;

	DWORD dwColor ;

	int m_nDispWidth ;
	int m_nDispHeight ;

	BYTE m_byAlpha ;

	BYTE m_byFadeMode ;

public:
	CFadeDlg(void);
	~CFadeDlg(void);

	void Initialize() ;

	void FadeIn() ;
	void FadeOut() ;

	virtual void Render() ;
};
