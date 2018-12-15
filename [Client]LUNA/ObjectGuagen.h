#pragma once

#include "./Interface/cGuagen.h"

class CObjectGuagen : public cGuagen  
{
public:
	CObjectGuagen();
	virtual ~CObjectGuagen();
	virtual void Render();
	void SetValue(GUAGEVAL, DWORD animateTick);
	void SetEffectPieceImage(cImage* piece) { m_GuageEffectPieceImage = *piece; }
	void SetGuageEffectPieceWidth(float width) { m_fGuageEffectPieceWidth = width; }
	void SetGuageEffectPieceHeightScale(float hei) { m_fGuageEffectPieceHeightScaleY = hei;	}

protected:
	cImage m_GuageEffectPieceImage;
	float m_fGuageEffectPieceWidth;
	float m_fGuageEffectPieceHeightScaleY;
	DWORD mTick;
	GUAGEVAL mTargetValue;
	GUAGEVAL mValue;
	GUAGEVAL mBias;
	float mWidthScale;
};