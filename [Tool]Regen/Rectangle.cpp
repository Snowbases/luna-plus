#include <StdAfx.h>
#include "Rectangle.h"

CRectangle::CRectangle() :
mColor(RGBA_MAKE(255, 255, 255, 0)),
mArea(Area()),
mGap(10),
mWeight(5)
{}

CRectangle::~CRectangle()
{}

void CRectangle::SetBottomRight(const CPoint& point)
{
	mArea.mTopRight.x = float(point.x);	
	mArea.mBottomLeft.y = float(point.y);
	mArea.mBottomRight.x = float(point.x);
	mArea.mBottomRight.y = float(point.y);
}

void CRectangle::SetRect(const CRect& rect)
{
	mArea.mTopLeft.x = float(rect.left);
	mArea.mTopLeft.y = float(rect.top);
	mArea.mTopRight.x = float(rect.right);
	mArea.mTopRight.y = float(rect.top);
	mArea.mBottomLeft.x = float(rect.left);
	mArea.mBottomLeft.y = float(rect.bottom);
	mArea.mBottomRight.x = float(rect.right);
	mArea.mBottomRight.y = float(rect.bottom);
}

CRect CRectangle::GetRect() const
{
	return CRect(
		CPoint(int(mArea.mTopLeft.x), int(mArea.mTopLeft.y)),
		CPoint(int(mArea.mBottomRight.x), int(mArea.mBottomRight.y)));
}

BOOL CRectangle::IsEmpty() const
{
	const Area emptyArea = {0};
	ASSERT(sizeof(emptyArea) == sizeof(mArea));

	return 0 == memcmp(
		&emptyArea,
		&mArea,
		sizeof(emptyArea));
}

void CRectangle::Draw() const
{
	DrawDotLine(
		mArea.mTopLeft,
		mArea.mTopRight);
	DrawDotLine(
		mArea.mBottomRight,
		mArea.mBottomLeft);
	DrawDotLine(
		mArea.mBottomLeft,
		mArea.mTopLeft);
	DrawDotLine(
		mArea.mTopRight,
		mArea.mBottomRight);
}

void CRectangle::DrawDot(const VECTOR2& lhs) const
{
	VECTOR2 position = lhs;
	position.x -= mWeight / 2.0f;
	position.y -= mWeight / 2.0f + 1;
	
	for(int weight = 0; weight < mWeight; ++weight)
	{
		VECTOR2 begin = {position.x++, position.y};
		VECTOR2 end = begin;
		end.y += mWeight;

		g_pExecutive->GetRenderer()->RenderLine(
			&begin,
			&end,
			mColor);
	}
}

void CRectangle::DrawLine(const VECTOR2& lhs, const VECTOR2& rhs) const
{
	VECTOR2 begin = lhs;
	begin.x -= mWeight / 2;
	begin.y -= mWeight / 2;

	VECTOR2 end = rhs;	
	end.x -= mWeight / 2;
	end.y -= mWeight / 2;

	for(int i = 0; i < mWeight; ++i)
	{
		g_pExecutive->GetRenderer()->RenderLine(
			&begin,
			&end,
			mColor);

		++begin.x;
		++begin.y;
		++end.x;
		++end.y;
	}
}

void CRectangle::DrawDotLine(const VECTOR2& lhs, const VECTOR2& rhs) const
{
	DrawDot(
		lhs);
	DrawDot(
		rhs);

	const float lineLength = sqrt(pow(lhs.x - rhs.x, 2) + pow(lhs.y - rhs.y, 2));

	if(1 > lineLength)
	{
		return;
	}

	const VECTOR2 unitVector = {
		(rhs.x - lhs.x) / lineLength * mGap,
		(rhs.y - lhs.y) / lineLength * mGap
	};
	// 두께가 있는 사선을 처리하려면 bias 값을 다른 방식으로 구해야한다
	const float biasX = (0 == unitVector.x);
	const float biasY = (0 == unitVector.y);
	const float unitLength = sqrt(pow(unitVector.x, 2) + pow(unitVector.y, 2));

	VECTOR2 begin = lhs;
	begin.x -= (biasX * mWeight / 2);
	begin.y -= (biasY * mWeight / 2);

	{
		static DWORD tick = 0;
		static float step = 0;

		if(tick < GetTickCount())
		{
			step = (mGap * 1.3f < step ? 0 : ++step);
			tick = GetTickCount() + 1000 / 20;
		}

		begin.x += (0 < unitVector.x ? step * biasY : step * biasY * -1);
		begin.y += (0 < unitVector.y ? step * biasX : step * biasX * -1);
	}

	for(float length = 0; length + unitLength < lineLength; length = sqrt(pow(lhs.x - begin.x, 2) + pow(lhs.y - begin.y, 2)))
	{
		VECTOR2 end = begin;
		end.x += unitVector.x;
		end.y += unitVector.y;

		for(int weight = 0; weight < mWeight; ++weight)
		{
			g_pExecutive->GetRenderer()->RenderLine(
				&begin,
				&end,
				mColor);

			begin.x += biasX;
			begin.y += biasY;
			end.x += biasX;
			end.y += biasY;
		}

		begin.x -= biasX * mWeight;
		begin.y -= biasY * mWeight;
		begin.x += unitVector.x * 1.5f;
		begin.y += unitVector.y * 1.5f;
	}
}