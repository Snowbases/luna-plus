#include "font.h"
#include "CoD3DDevice.h"

CFontBuffer::CFontBuffer()
{
	memset((char*)this+4,0,sizeof(CFontBuffer)-4);
}


void __stdcall CFontBuffer::BeginRender()
{
	/// LeHide Edit, Check - m_pD3DXFont->Begin(); 이제 안쓰나보다
	
}
void __stdcall CFontBuffer::EndRender()
{
	/// LeHide Edit, Check - m_pD3DXFont->End(); 이제 안쓰나보다
	
}

INT __stdcall CFontBuffer::DrawText(TCHAR* str,DWORD dwLen,RECT* pRect,DWORD dwColor,CHAR_CODE_TYPE type,DWORD dwFlag,void* sprite)
{
	INT iResult = 0xffffffff;

	if (type == CHAR_CODE_TYPE_UNICODE)
		iResult = m_pD3DXFont->DrawTextW((LPD3DXSPRITE)sprite,(LPCWSTR)str,dwLen,pRect,DT_LEFT | DT_TOP | DT_NOCLIP,dwColor);
	else
		iResult = m_pD3DXFont->DrawTextA((LPD3DXSPRITE)sprite,(LPCSTR)str,dwLen,pRect,DT_LEFT | DT_TOP | DT_NOCLIP,dwColor);

	return iResult;


// 	INT iResult = 0xffffffff;
// 
// 	LPDIRECT3DDEVICE9 pDevice = m_pRenderer->GetDevice();
// 	pDevice->SetRenderState(D3DRS_ZWRITEENABLE ,FALSE);
// 	pDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
// 	DWORD dwAlpha = (dwColor & 0xff000000)>>24;
// 	BeginAlphaMeshObject(pDevice,0,255,dwAlpha);
// 
// 	if (type == CHAR_CODE_TYPE_UNICODE)
// 	{
// 		iResult = m_pD3DXFont->DrawTextW((LPD3DXSPRITE)sprite, (LPCWSTR)str,dwLen,pRect,DT_LEFT | DT_TOP | DT_NOCLIP ,dwColor);
// 	}
// 	else
// 	{
// 		iResult = m_pD3DXFont->DrawTextA((LPD3DXSPRITE)sprite, (LPCSTR)str,dwLen,pRect,DT_LEFT | DT_TOP | DT_NOCLIP ,dwColor);
// 	}
// 
// 	EndAlphaMeshObject(pDevice);
// 	
// 	pDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
// 	pDevice->SetRenderState(D3DRS_ZWRITEENABLE ,TRUE);
// 
// 	return iResult;
}

BOOL __stdcall CFontBuffer::CreateFont(CoD3DDevice* pRenderer,LOGFONT* pLogFont)
{
	BOOL bResult = FALSE;

	D3DXFONT_DESC fontdesc;

	if( pLogFont )
	{
		fontdesc.CharSet = pLogFont->lfCharSet;
		::lstrcpy( fontdesc.FaceName, pLogFont->lfFaceName );
		fontdesc.Height = pLogFont->lfHeight;
		fontdesc.Italic = pLogFont->lfItalic;
		fontdesc.MipLevels = D3DX_DEFAULT;
		fontdesc.OutputPrecision = pLogFont->lfOutPrecision;
		fontdesc.PitchAndFamily = pLogFont->lfPitchAndFamily;
		fontdesc.Quality = pLogFont->lfQuality;
		fontdesc.Weight = pLogFont->lfWeight;
		fontdesc.Width = pLogFont->lfWidth;
	}
	else
	{
		fontdesc.CharSet = DEFAULT_CHARSET;
		::lstrcpy( fontdesc.FaceName, "굴림체" );
		fontdesc.Height = -12;
		fontdesc.Italic = FALSE;
		fontdesc.MipLevels = D3DX_DEFAULT;
		fontdesc.OutputPrecision = 0;
		fontdesc.PitchAndFamily = 0;
		fontdesc.Quality = PROOF_QUALITY;
		fontdesc.Weight = FW_NORMAL;
		fontdesc.Width = 0;
	}

	m_pRenderer = pRenderer;
	HRESULT hr = m_pRenderer->GetD3DResourceManager()->CreateFont(&fontdesc,&m_pD3DXFont);
	
	if (hr != D3D_OK)
		goto lb_return;

	bResult = TRUE;

lb_return:
	return bResult;
}

CFontBuffer::~CFontBuffer()
{
	if (m_pD3DXFont)
	{
		m_pRenderer->GetD3DResourceManager()->Release(m_pD3DXFont);
		m_pD3DXFont = NULL;
	}

}

