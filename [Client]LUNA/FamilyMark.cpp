// FamilyMark.cpp: implementation of the CFamilyMark class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FamilyMark.h"

#include "FamilyMarkImg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFamilyMark::CFamilyMark()
{
	m_pMarkImg = NULL;
}

CFamilyMark::~CFamilyMark()
{
}

void CFamilyMark::Clear()
{
	if(m_pMarkImg)
	{
		if(m_pMarkImg->GetMarkName() != 0)
		{
			m_pMarkImg->Release();
			delete m_pMarkImg;
			m_pMarkImg = NULL;
		}
	}
}

void CFamilyMark::SetFamilyMarkImg(CFamilyMarkImg* pMarkImg)
{
	Clear();

	m_pMarkImg = pMarkImg;
}

void CFamilyMark::Render(VECTOR2* pos, DWORD dwColor)
{
	if(m_pMarkImg == NULL)
		return;
	
	m_pMarkImg->Render(pos,dwColor);
}
