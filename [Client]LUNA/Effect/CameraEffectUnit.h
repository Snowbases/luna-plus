// CameraEffectUnit.h: interface for the CCameraEffectUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAEFFECTUNIT_H__893E1529_5038_4B04_BE57_C57A14BF06D1__INCLUDED_)
#define AFX_CAMERAEFFECTUNIT_H__893E1529_5038_4B04_BE57_C57A14BF06D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffectUnit.h"

// 100607 ShinJS --- Camera Effect의 종류
enum eCameraEffectUnitType{
	eCameraEffectUnitType_AngleDist,
	eCameraEffectUnitType_Filter,
};

class CCameraEffectUnit : public CEffectUnit  
{
	eCameraEffectUnitType m_CameraEffectUnitType;

	float m_AngleX;
	float m_AngleY;
	float m_Dist;

	DWORD m_dwCameraFilterObjectIndex;		// Filter Attach시 생성된 필터 ObjectIndex
	DWORD m_dwFilterIndex;					// CameraFilterList.bin 의 Index
	BOOL m_bFilterRepeat;
	float m_fFilterFadeTime;
	BOOL m_bFilterDetachAll;				// Off시 관련 FilterIndex들을 모두 제거할지 여부
	BOOL m_bFilterOnlyOne;					// On시 관련 FilterIndex들을 모두 제거할지 여부
	BOOL m_bFilterRelease;					// Unit Release시 Filter Rlease 여부
	DWORD m_dwFilterRemainTime;				// Filter 유지 시간
public:	
	virtual void Initialize(CEffectUnitDesc* pEffectUnitDesc,EFFECTPARAM* pEffectParam);
	virtual void Release();

	virtual void Reset();

	CCameraEffectUnit();
	virtual ~CCameraEffectUnit();

	virtual BOOL On(EFFECTPARAM* pParam);
	virtual BOOL Off(EFFECTPARAM* pParam);
	virtual BOOL CameraRotate(EFFECTPARAM* pParam,float fRotateAngle,DWORD Duration);
	virtual BOOL CameraZoom(EFFECTPARAM* pParam,float fZoomDistance,DWORD Duration);
	virtual BOOL CameraAngle(EFFECTPARAM* pParam,float fTargetAngle,DWORD Duration);
	virtual BOOL ChangeCamera(EFFECTPARAM* pParam,int Camera,DWORD Duration);


	void DoProcess(DWORD tickTime,EFFECTPARAM* pParam);

	friend class CCameraEffectUnitDesc;

};

#endif // !defined(AFX_CAMERAEFFECTUNIT_H__893E1529_5038_4B04_BE57_C57A14BF06D1__INCLUDED_)
