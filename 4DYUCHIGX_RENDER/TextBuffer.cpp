#include "TextBuffer.h"
#include "CoD3DDevice.h"

BOOL __stdcall RenderFontItem(CTextBuffer* pBuffer,FONT_ITEM* pFontItem)
{
	return pBuffer->Render(pFontItem);
}

CTextBuffer::CTextBuffer()
{
	m_dwCurrentItemNum = 0;
	m_pSprite = NULL;
	m_pSpriteTemp = NULL;
	memset(this,0,sizeof(CTextBuffer));
}

BOOL CTextBuffer::Initialize(DWORD dwMaxBufferSize,CoD3DDevice* pRenderer)
{
	// 폰트용 sprite 생성...
	D3DXCreateSprite( pRenderer->GetDevice(), &m_pSprite );

	return m_Buffer.Initialize(dwMaxBufferSize);
}

BOOL CTextBuffer::PushTextItem(DPCQ_HANDLE pDPCQ,IDIFontObject* pFont,TCHAR* str,DWORD dwLen,RECT* pRect,DWORD dwColor,CHAR_CODE_TYPE type,int iZOrder,DWORD dwFlag)
{
	BOOL	bResult = FALSE;

	DWORD	dwCharSize;
	DWORD	dwBufferSize;
	DWORD	dwStrSize;
	

	if (type == CHAR_CODE_TYPE_ASCII)
		dwCharSize = 1;
	else 
		dwCharSize = 2;

	dwStrSize = dwCharSize*dwLen;
	dwBufferSize = dwStrSize + FONT_ITEM_HEADER_SIZE;

	FONT_ITEM* pFontItem = (FONT_ITEM*)m_Buffer.AllocBuffer(dwBufferSize);

	if (!pFontItem)
		goto lb_return;
	
	pFontItem->dwItemSize = dwBufferSize;
	pFontItem->pFont = pFont;
	pFontItem->dwLen = dwLen;
	pFontItem->rect = *pRect;
	pFontItem->dwColor = dwColor;
	pFontItem->type = type;
	pFontItem->dwFlag = dwFlag;
	memcpy(pFontItem->pStr,str,dwStrSize);

	DWORD	dwArgList[2];
	dwArgList[0] = (DWORD)this;
	dwArgList[1] = (DWORD)pFontItem;

	DPCQPushDPC(pDPCQ,RenderFontItem,2,dwArgList,NULL,iZOrder);

	m_dwCurrentItemNum++;

	bResult = TRUE;
lb_return:

	return bResult;
}
BOOL CTextBuffer::Render(FONT_ITEM* pFontItem)
{
	BOOL rt = FALSE;

	m_pSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
	m_pSpriteTemp = m_pSprite;

	rt = pFontItem->pFont->DrawText(pFontItem->pStr, pFontItem->dwLen, &pFontItem->rect, pFontItem->dwColor, pFontItem->type, pFontItem->dwFlag,m_pSpriteTemp);

	m_pSprite->End();
	m_pSpriteTemp = NULL;

	return rt;
}

void __stdcall CTextBuffer::OnLostDevice()
{
	m_pSprite->OnLostDevice();
}

void __stdcall CTextBuffer::Reset()
{
	m_pSprite->OnResetDevice();
}

CTextBuffer::~CTextBuffer()
{

}