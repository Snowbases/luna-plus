// FamilyMarkImg.h: interface for the CFamilyMarkImg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAMILYMARKIMG_H__B4839C00_8346_4E60_9B0A_77D8069BF82D__INCLUDED_)
#define AFX_FAMILYMARKIMG_H__B4839C00_8346_4E60_9B0A_77D8069BF82D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class cImageSelf;

class CFamilyMarkImg  
{
	MARKNAMETYPE m_MarkName;
	cImageSelf* m_pImg;

public:
	CFamilyMarkImg(MARKNAMETYPE markname,cImageSelf* pImg);
	virtual ~CFamilyMarkImg();

	MARKNAMETYPE GetMarkName()	{	return m_MarkName;	}
	void Render(VECTOR2* pos, DWORD dwColor);
	void Release();
};

#endif // !defined(AFX_FAMILYMARKIMG_H__B4839C00_8346_4E60_9B0A_77D8069BF82D__INCLUDED_)
