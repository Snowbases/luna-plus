// FamilyMark.h: interface for the CFamilyMark class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAMILYMARK_H__452B0AB3_F6FD_4A8C_BBA5_6663675C5FFD__INCLUDED_)
#define AFX_FAMILYMARK_H__452B0AB3_F6FD_4A8C_BBA5_6663675C5FFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFamilyMarkImg;
class CFamilyMark  
{
	CFamilyMarkImg* m_pMarkImg;


public:
	CFamilyMark();
	virtual ~CFamilyMark();
	
	void Clear();

	void SetFamilyMarkImg(CFamilyMarkImg* pMarkImg);
	void Render(VECTOR2* pos, DWORD dwColor = 0xffffffff);
};

#endif // !defined(AFX_FAMILYMARK_H__452B0AB3_F6FD_4A8C_BBA5_6663675C5FFD__INCLUDED_)
