//--------------------------------------------------
//	TextBuffer.h
//	[8/27/2009 LeHide]
//
//	
//
//	Version		:	1.0.2009827
//--------------------------------------------------

#pragma once


#include <d3dx9.h>
#include "../4DyuchiGRX_Common/IRenderer.h"
#include "VariableBuffer.h"
#include "../4DyuchiGXGFunc/global.h"

class CoD3DDevice;

struct FONT_ITEM
{
	DWORD				dwItemSize;
	IDIFontObject*		pFont;
	DWORD				dwLen;
	RECT				rect;
	DWORD				dwColor;
	CHAR_CODE_TYPE		type;
	DWORD				dwFlag;
	TCHAR				pStr[1];
};

#define FONT_ITEM_HEADER_SIZE	40
class CTextBuffer
{
	CVariableBuffer		m_Buffer;
	DWORD				m_dwCurrentItemNum;

	LPD3DXSPRITE        m_pSprite;
	LPD3DXSPRITE        m_pSpriteTemp;

public:
	BOOL				Initialize(DWORD dwMaxBufferSize,CoD3DDevice* pRenderer);
	BOOL				PushTextItem(DPCQ_HANDLE pDPCQ,IDIFontObject* pFont,TCHAR* str,DWORD dwLen,RECT* pRect,DWORD dwColor,CHAR_CODE_TYPE type,int iZOrder,DWORD dwFlag);
	void				Process();
	BOOL				Render(FONT_ITEM* pFontItem);
	void				Clear() {m_Buffer.Clear();}

	void	__stdcall	OnLostDevice();
	void	__stdcall	Reset();

	CTextBuffer();
	~CTextBuffer();
};


