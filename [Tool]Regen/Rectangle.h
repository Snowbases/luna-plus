#pragma once

class CRectangle
{
private:
	struct Area
	{
		VECTOR2 mTopLeft;
		VECTOR2 mTopRight;
		VECTOR2 mBottomLeft;
		VECTOR2 mBottomRight;
	}
	mArea;
	COLORREF mColor;
	float mGap;
	float mWeight;

public:
	CRectangle();
	virtual ~CRectangle();
	void SetRect(const CRect& rect);
	CRect GetRect() const;
	void SetBottomRight(const CPoint&);
	void Draw() const;
	void SetColor(COLORREF color) { mColor = color; }
	BOOL IsEmpty() const;
	
private:
	void DrawDot(const VECTOR2&) const;
	void DrawLine(const VECTOR2&, const VECTOR2&) const;
	void DrawDotLine(const VECTOR2&, const VECTOR2&) const;
};