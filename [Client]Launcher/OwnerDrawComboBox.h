#pragma once
// 
// http://www.codersource.net/mfc_ccombobox_ownerdraw_color.html
//


// COwnerDrawComboBox

class COwnerDrawComboBox : public CComboBoxEx
{
public:
	struct ImageName
	{
		CString	mSelect;
		CString mUnselect;
	};

	typedef std::list< ImageName > ImageNameList;

	// 080507 LUJ, 이미지 설정
	void SetImage( const ImageNameList& );

	COwnerDrawComboBox();
	virtual ~COwnerDrawComboBox();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int OnCreate(LPCREATESTRUCT);
	void SetLogFont(LPCTSTR name, LONG height, BYTE charSet);
	CxImage& GetCurrentImage();
	
protected:
	void InitStorage(int nItems, UINT nBytes);
	CFont m_Font;
	CArray< CxImage, CxImage& > m_unselectedImageArray;
	CArray< CxImage, CxImage& > m_selectedImageArray;


protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};