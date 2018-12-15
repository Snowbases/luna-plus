#include "StdAfx.h"
#include "ImageComboBox.h"
#include "Application.h"
#include "resource.h"
#include ".\imagecombobox.h"

CImageComboBox::CImageComboBox()
{
	EnableAutomation();
}

CImageComboBox::~CImageComboBox()
{}

BEGIN_MESSAGE_MAP(CImageComboBox, CComboBox)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CImageComboBox::DrawSelectItem()
{
	if(0 > GetCurSel())
	{
		return;
	}
	else if(int(mImageVector.size()) <= GetCurSel())
	{
		return;
	}

	mImage.mSelect.Draw(
		GetDC()->m_hDC,
		0,
		0,
		mImage.mSelect.GetWidth(),
		mImage.mSelect.GetHeight(),
		0,
		true);
}

void CImageComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(UINT(-1) == lpDrawItemStruct->itemID)
	{
		return;
	}
	else if(mImageVector.size() <= lpDrawItemStruct->itemID)
	{
		return;
	}

	Image::Item& imageItem = mImageVector[lpDrawItemStruct->itemID];

	if((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		imageItem.mOver.Draw(
			lpDrawItemStruct->hDC,
			lpDrawItemStruct->rcItem.left,
			lpDrawItemStruct->rcItem.top,
			imageItem.mOver.GetWidth(),
			imageItem.mOver.GetHeight(),
			0,
			true);
	}
	else
	{
		imageItem.mNormal.Draw(
			lpDrawItemStruct->hDC,
			lpDrawItemStruct->rcItem.left,
			lpDrawItemStruct->rcItem.top,
			imageItem.mNormal.GetWidth(),
			imageItem.mNormal.GetHeight(),
			0,
			true);
	}
}

void CImageComboBox::SetImage(CxImage& image)
{
	image.Crop(
		theApp.GetConfig().mTopCombo.mRect,
		&mImage.mTop.mNormal);
	image.Crop(
		theApp.GetConfig().mTopOverCombo.mRect,
		&mImage.mTop.mOver);
	image.Crop(
		theApp.GetConfig().mMiddleCombo.mRect,
		&mImage.mMiddle.mNormal);
	image.Crop(
		theApp.GetConfig().mMiddleOverCombo.mRect,
		&mImage.mMiddle.mOver);
	image.Crop(
		theApp.GetConfig().mBottomCombo.mRect,
		&mImage.mBottom.mNormal);
	image.Crop(
		theApp.GetConfig().mBottomOverCombo.mRect,
		&mImage.mBottom.mOver);
	image.Crop(
		theApp.GetConfig().mCoverCombo.mRect,
		&mImage.mCover);

	mImage.mSelect = mImage.mCover;
}

void CImageComboBox::AddString(LPCTSTR text)
{
	CComboBox::AddString(
		text);

	mImageVector.clear();
	
	for(int i = 0; i < GetCount(); ++i)
	{
		Image::Item* imageItem = 0;

		// 100216 LUJ, 첫번째 아이템
		if(0 == i)
		{
			mImageVector.push_back(
				Image::Item());
			imageItem = &mImageVector.back();
			imageItem->mNormal = mImage.mTop.mNormal;
			imageItem->mOver = mImage.mTop.mOver;
		}
		// 100216 LUJ, 마지막 아이템
		else if(GetCount() - 1 == i)
		{
			mImageVector.push_back(
				Image::Item());
			imageItem = &mImageVector.back();
			imageItem->mNormal = mImage.mBottom.mNormal;
			imageItem->mOver = mImage.mBottom.mOver;
		}
		// 100216 LUJ, 중간 아이템
		else
		{
			mImageVector.push_back(
				Image::Item());
			imageItem = &mImageVector.back();
			imageItem->mNormal = mImage.mMiddle.mNormal;
			imageItem->mOver = mImage.mMiddle.mOver;
		}

		CString itemText;
		GetLBText(
			i,
			itemText);		
		DrawText(
			imageItem->mNormal,
			theApp.GetConfig().mCoverCombo.mPoint,
			itemText,
			theApp.GetConfig().mControlFont.mColor,
			theApp.GetConfig().mControlFont.mSize,
			theApp.GetConfig().m_fontName,
			theApp.GetConfig().mControlFont.mWeight,
			theApp.GetConfig().mControlFont.mBorderSize,
			theApp.GetConfig().mControlFont.mBorderColor);
		DrawText(
			imageItem->mOver,
			theApp.GetConfig().mCoverCombo.mPoint,
			itemText,
			theApp.GetConfig().mControlFont.mColor,
			theApp.GetConfig().mControlFont.mSize,
			theApp.GetConfig().m_fontName,
			theApp.GetConfig().mControlFont.mWeight,
			theApp.GetConfig().mControlFont.mBorderSize,
			theApp.GetConfig().mControlFont.mBorderColor);
		SetItemHeight(
			i,
			imageItem->mNormal.GetHeight() - 1);
	}
}
void CImageComboBox::OnCbnSelchange()
{
	CString text;
	GetLBText(
		GetCurSel(),
		text);

	mImage.mSelect = mImage.mCover;
	DrawText(
		mImage.mSelect,
		theApp.GetConfig().mCoverCombo.mPoint,
		text,
		theApp.GetConfig().mControlFont.mColor,
		theApp.GetConfig().mControlFont.mSize,
		theApp.GetConfig().m_fontName,
		theApp.GetConfig().mControlFont.mWeight,
		theApp.GetConfig().mControlFont.mBorderSize,
		theApp.GetConfig().mControlFont.mBorderColor);
	mImage.mSelect.Draw(
		GetDC()->m_hDC,
		0,
		0,
		mImage.mSelect.GetWidth(),
		mImage.mSelect.GetHeight(),
		0,
		true);
}

void CImageComboBox::SetPosition(const CPoint& point)
{
	int height = 0;

	for(int i = 0; i < GetCount(); ++i)
	{
		height += GetItemHeight(i);
	}

	SetWindowPos(
		0,
		point.x,
		point.y,
		mImage.mCover.GetWidth(),
		point.y + height,
		SWP_NOREDRAW);
}

void CImageComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if(UINT(-1) == lpMeasureItemStruct->itemID)
	{
		return;
	}
	else if(mImageVector.size() <= lpMeasureItemStruct->itemID) 
	{
		return;
	}

	const Image::Item& imageItem = mImageVector[lpMeasureItemStruct->itemID];
	lpMeasureItemStruct->itemWidth = imageItem.mNormal.GetWidth();
	lpMeasureItemStruct->itemHeight = imageItem.mOver.GetHeight();
}

void CImageComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	GetParent()->PostMessage(
		WM_MOUSEMOVE_COMBO,
		GetDlgCtrlID());

	CComboBox::OnMouseMove(nFlags, point);
}
