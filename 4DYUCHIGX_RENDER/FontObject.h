

#pragma once

#include "d3dtexture.h"
#include "BaseFontObject.h"


class CFontCache;


class CFontObject : public CBaseFontObject
{
public:
	CFontObject();
	virtual ~CFontObject();

	BOOL	__stdcall	CreateFont(CoD3DDevice* pRenderer,LOGFONT* pLogFont);
	void	__stdcall	BeginRender();
	void	__stdcall	EndRender();
	BOOL	__stdcall	DrawText(TCHAR* str,DWORD dwLen,RECT* pRect,DWORD dwColor,CHAR_CODE_TYPE type,DWORD dwFlag,void* sprite);

private:
	int					m_iHeight;

	CFontCache*			m_pFontCache;
	LPDIRECT3DDEVICE9	m_pD3DDevice;
	LPDIRECT3DTEXTURE9	m_pTexture;

	HFONT				m_hFont;
	
	BYTE				m_bItalc;
	BYTE				m_bUnderLine;
	BYTE				m_bStrikeOut;
			
	FONT_PROPERTY_DESC	m_propertyDesc;
};


