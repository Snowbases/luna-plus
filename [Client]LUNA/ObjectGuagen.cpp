#include "StdAfx.h"
#include "ObjectGuagen.h"

CObjectGuagen::CObjectGuagen()
{
	m_type = WT_GUAGENE;
	m_fGuageEffectPieceWidth = 0.f;
	m_fGuageEffectPieceHeightScaleY	= 1.f;
	mTick = UINT_MAX;
	mTargetValue = 0;
	mBias = 0;
	mValue = 0;
	mWidthScale = 0;
}

CObjectGuagen::~CObjectGuagen()
{

}
void CObjectGuagen::SetValue(GUAGEVAL val, DWORD animatedTick)
{
	val = min(
		1,
		val);

	if(mTargetValue == val)
	{
		return;
	}
	else if(mTargetValue != val)
	{
		mValue = mTargetValue;
		cGuagen::SetValue(
			mTargetValue);
	}

	if(0 == animatedTick)
	{
		mTick = UINT_MAX;
		mTargetValue = val;
		mValue = mTargetValue;

		cGuagen::SetValue(
			mTargetValue);
	}
	else
	{
		mTick = 0;
		mTargetValue = val;
		mValue = GetValue();
		mBias = (mTargetValue - GetValue()) / float(animatedTick / 50);
	}

	if(mTargetValue < mValue)
	{
		mWidthScale = ceil(
			m_fGuageWidth * mTargetValue / m_fGuageEffectPieceWidth);
	}
	else if(mTargetValue > mValue)
	{
		mWidthScale = ceil(
			m_fGuageWidth * mValue / m_fGuageEffectPieceWidth);
	}
	else
	{
		mWidthScale = ceil(
			m_fGuageWidth * GetValue() / m_fGuageEffectPieceWidth);
	}
}

void CObjectGuagen::Render()
{
	cGuagen::Render();

	VECTOR2 scale = {
		mWidthScale,
		m_fGuageEffectPieceHeightScaleY,
	};

	if(gCurTime > mTick)
	{
		mTick = gCurTime + 10;

		if(mValue < mTargetValue)
		{
			mValue = min(
				mTargetValue,
				mValue + mBias);
			cGuagen::SetValue(
				mTargetValue);

			mWidthScale = ceil(m_fGuageWidth * mValue / m_fGuageEffectPieceWidth);
		}
		else if(mValue > mTargetValue)
		{
			mValue = max(
				mTargetValue,
				mValue + mBias);
			cGuagen::SetValue(
				mValue);

			mWidthScale = ceil(0.5f + m_fGuageWidth * mTargetValue / m_fGuageEffectPieceWidth);
		}
		else
		{
			mTick = UINT_MAX;

			cGuagen::SetValue(
				mValue);
			mValue = GetValue();
			mWidthScale = ceil(0.5f + m_fGuageWidth * mTargetValue / m_fGuageEffectPieceWidth);
		}
	}

	VECTOR2 imgPosRect = {
		m_absPos.x + m_imgRelPos.x,
		m_absPos.y + m_imgRelPos.y,
	};
	m_GuageEffectPieceImage.RenderSprite(&scale, NULL, 0, &imgPosRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
}