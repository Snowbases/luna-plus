#pragma once
#include "Object.h"
#include "CommonStruct.h"

struct stMarkingInfo
{
	BOOL bUseMaking;
	BOOL bUseResize;
	BOUNDING_BOX stResizeBoudingBox;
	float f2DRadius;
	float fHeightSize;

	stMarkingInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(stMarkingInfo));
	}
};

class CFurniture :
	public CObject
{
	stFurniture	m_FurnitureInfo;
	stMarkingInfo m_MarkingInfo;

protected:
	virtual void Release();

public:
	CFurniture(void);
	virtual ~CFurniture(void);

	void InitFurniture(stFurniture* pFurniture);

	void InitMarkingInfo();

	stFurniture* GetFurnitureInfo()				{return &m_FurnitureInfo;}
	stMarkingInfo* GetMarkingInfo()				{return &m_MarkingInfo;}

	void UpdateMarkingInfo();

	BOOL GetRideNodeNumByMousePoint(LONG MouseX,LONG MouseY , DWORD* pBoneNumArr );

public:

};
