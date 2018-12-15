#ifndef _IMAGENUMBER_H
#define _IMAGENUMBER_H


#include "./interface/cImage.h"
// desc_hseos_몬스터미터01
// S 몬스터미터 추가 added by hseos 2007.05.29
// ..정렬 기준(좌, 우, 중앙)
class cImageSelf;
// E 몬스터미터 추가 added by hseos 2007.05.29


class CImageNumber
{
protected:

	VECTOR2 m_vPos;
	VECTOR2 m_vScale;
//	float	m_x;
//	float	m_y;
	cImage	m_ImageNum[10];
	DWORD	m_dwNumber;
	DWORD	m_dwLimitCipher;	//ÀÚ¸®¼ö
	BOOL	m_bFillZero;

	int		m_nNumWidth;
	int		m_nGap;

///È¿°ú
	BOOL	m_bNumberChange;
	DWORD	m_dwFadeOutStartTime;
	DWORD	m_dwFadeOutTime;
	BOOL	m_bFadeOut;

	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.05.29
	cImageSelf*		m_pImg[10];						// 데미지 숫자 이미지를 사용할 경우 이미지 포인터
	// E 몬스터미터 추가 added by hseos 2007.05.29

public:

	CImageNumber();
	virtual ~CImageNumber();

	void Init( int nNumWidth, int nGap );
	void SetNumber( DWORD dwNum );
	void SetLimitCipher( DWORD dwCipher );
	void SetFillZero( BOOL bFillZero ) { m_bFillZero = bFillZero; }
	void SetPosition( int x, int y );
	void Render();

	void SetScale( float x, float y ) { m_vScale.x = x; m_vScale.y = y;	}
//ÀÓ½Ã
	void SetFadeOut( DWORD dwTime );
	BOOL IsNumberChanged() { return m_bNumberChange; }

	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.05.29
	//----------------------------------------------------------------------------------------------------------------
	//		데미지 숫자 이미지를 사용할 경우 추가적으로 호출해야할 초기화 함수
	void	InitDamageNumImage(cImageSelf* pImg, int nNum);
	//----------------------------------------------------------------------------------------------------------------
	//		데미지 숫자 이미지를 사용할 경우 화면 출력 함수
	void	RenderWithDamageNumImage();
	// E 몬스터미터 추가 added by hseos 2007.05.29
};


#endif