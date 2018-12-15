#pragma once
#include "./Interface/cDialog.h"											

class cStatic ;
class cTextArea;


class cItemPopupDlg : public cDialog										
{
	cStatic*	mpMiddle;
	cStatic*	mpBottom;
	cTextArea*	mpTextArea;

	DWORD		mActiveTime;

	DWORD		mShadow;
	DWORD		mFont;

	int			mTextPosX;
	int			mTextPosY;

public:
	// BASE PART.
	cItemPopupDlg(void);													
	virtual ~cItemPopupDlg(void);											

	void Linking() ;														

	virtual void Render() ;
	virtual void SetActive( BOOL val );
	virtual DWORD ActionEvent(CMouse * mouseInfo);

	void	SetText( char* text );
};
