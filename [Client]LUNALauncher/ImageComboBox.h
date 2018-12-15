#pragma once

class CImageComboBox : public CComboBox
{
public:
	CImageComboBox();
	virtual ~CImageComboBox();
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void AddString(LPCTSTR);
	void DrawSelectItem();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT);
	void SetImage(CxImage&);
	void SetPosition(const CPoint&);

protected:
	struct Image
	{
		struct Item
		{
			CxImage mOver;
			CxImage mNormal;
		};
		Item mTop;
		Item mMiddle;
		Item mBottom;
		CxImage mCover;
		CxImage mSelect;
	};
	Image mImage;
	std::vector< Image::Item > mImageVector;

protected:
	DECLARE_MESSAGE_MAP()
public:
	void OnCbnSelchange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};