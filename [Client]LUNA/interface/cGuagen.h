#pragma once

#include "cWindow.h"

typedef float	GUAGEVAL;
typedef BOOL	GUAGEBOOL;
typedef VECTOR2 GUAGEPOS;
typedef LONG	GUAGEXY;

class cGuagen : public cWindow  
{
public:
	cGuagen();
	virtual ~cGuagen();
	virtual void Render();
	void SetGuageImagePos(GUAGEXY x, GUAGEXY y) { m_imgRelPos.x = float(x); m_imgRelPos.y = float(y); }
	void SetValue(GUAGEVAL val) { m_fPercentRate = min(1, val); }
	GUAGEVAL GetValue() const { return m_fPercentRate; }
	void SetPieceImage(cImage * piece) { m_GuagePieceImage = *piece; }
	void SetGuageWidth(float width)	{ m_fGuageWidth = width; }
	void SetGuagePieceWidth(float width) {	m_fGuagePieceWidth = width;	}
	void SetGuagePieceHeightScale(float hei) { m_fGuagePieceHeightScaleY = hei;	}
	virtual void Add(cWindow*) {}

protected:
	cImage m_GuagePieceImage;
	GUAGEPOS m_imgRelPos;
	float m_fGuageWidth;
	float m_fGuagePieceWidth;
	GUAGEVAL m_fPercentRate;
	float m_fGuagePieceHeightScaleY;
};