#include "stdafx.h"
#include "Consignmentguidedlg.h"

CConsignmentGuideDlg::CConsignmentGuideDlg(void)
{
}

CConsignmentGuideDlg::~CConsignmentGuideDlg(void)
{
}

void CConsignmentGuideDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
}

void CConsignmentGuideDlg::Linking()
{

}

void CConsignmentGuideDlg::SetActive( BOOL val )
{
	cDialog::SetActive( val );
}