// OwnerDrawComboBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OwnerDrawComboBox.h"
#include "resource.h"
#include "common.h"

#define COLOR_HEIGHT 16


COwnerDrawComboBox::COwnerDrawComboBox()
{
	EnableAutomation();
}

COwnerDrawComboBox::~COwnerDrawComboBox()
{
}

BEGIN_MESSAGE_MAP(COwnerDrawComboBox, CComboBoxEx)
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(COwnerDrawComboBox, CComboBoxEx)
END_DISPATCH_MAP()

// 참고: IID_IOwnerDrawComboBox에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {2EEE0612-9DC6-46AC-A00C-6D40A8F2531E}
static const IID IID_IOwnerDrawComboBox =
{ 0x2EEE0612, 0x9DC6, 0x46AC, { 0xA0, 0xC, 0x6D, 0x40, 0xA8, 0xF2, 0x53, 0x1E } };

BEGIN_INTERFACE_MAP(COwnerDrawComboBox, CComboBoxEx)
	INTERFACE_PART(COwnerDrawComboBox, IID_IOwnerDrawComboBox, Dispatch)
END_INTERFACE_MAP()


// COwnerDrawComboBox 메시지 처리기입니다.



void COwnerDrawComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemID==(UINT)-1) return;

	const CRect rect( lpDrawItemStruct->rcItem );

	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState  & ODS_SELECTED))
	{
		CxImage& image = m_selectedImageArray[ lpDrawItemStruct->itemID ];
		image.Draw( lpDrawItemStruct->hDC, rect.left, rect.top );
	}
	else
	{
		CxImage& image = m_unselectedImageArray[ lpDrawItemStruct->itemID ];
		image.Draw( lpDrawItemStruct->hDC, rect.left, rect.top );
	}
}


void COwnerDrawComboBox::SetLogFont(LPCTSTR name, LONG height, BYTE charSet)
{
	LOGFONT logFont;
	
	ZeroMemory( &logFont, sizeof(LOGFONT));
	logFont.lfHeight = height;
	logFont.lfCharSet = charSet;
	strcpy(logFont.lfFaceName, name);
	m_Font.CreateFontIndirect(&logFont);

	CComboBoxEx::SetFont(&m_Font);
}


void COwnerDrawComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpDrawItemStruct)
{
	lpDrawItemStruct->itemHeight = COLOR_HEIGHT;
}


int COwnerDrawComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBoxEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( ! m_selectedImageArray.IsEmpty() )
	{
		const CxImage& image = m_selectedImageArray[ 0 ];
		
		SetWindowPos( 
			0,
			0,
			0,
			image.GetWidth(),
			image.GetHeight(),
			SWP_NOZORDER );
	}

	return 0;
}


void COwnerDrawComboBox::InitStorage(int nItems, UINT nBytes)
{
	CComboBoxEx::InitStorage( nItems, nBytes );

	AddString( "4" );
}


CxImage& COwnerDrawComboBox::GetCurrentImage()
{
	const int index = GetCurSel();

	if( 0 > index ||
		index > m_selectedImageArray.GetSize() )
	{
		static CxImage image;

		return image;
	}

	return m_selectedImageArray.GetAt( index );
}


// 080507 LUJ, 이미지 설정
void COwnerDrawComboBox::SetImage( const COwnerDrawComboBox::ImageNameList& nameList )
{
	for(
		ImageNameList::const_iterator it = nameList.begin();
		nameList.end() != it;
		++it )
	{
		const ImageName& name = *it;

		m_selectedImageArray.InsertAt(
			m_selectedImageArray.GetCount(),
			CxImage( name.mSelect, CXIMAGE_FORMAT_TIF ) );

		m_unselectedImageArray.InsertAt(
			m_unselectedImageArray.GetCount(),
			CxImage( name.mUnselect, CXIMAGE_FORMAT_TIF ) );
	}
}
