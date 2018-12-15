/*********************************************************************

	 파일		: SHFarmRenderObj.h
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: CSHFarmRenderObj 클래스의 헤더
				  클라이언트에서 단지 화면 출력과 조작을 위한 농장 오브젝트 클래스임

 *********************************************************************/

#pragma once

#if defined(_AGENTSERVER)
	class CObject
	{
	};

	struct BASEOBJECT_INFO;
	enum EObjectKind;
#else
	#include "Object.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHFarmRenderObj : public CObject
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef struct
	{
		DWORD	nID;
		WORD	nKind;
		WORD	nMapNum;
	} stFarmRenderObjInfo;

private:
	//----------------------------------------------------------------------------------------------------------------
	stFarmRenderObjInfo		m_stFarmRenderObjInfo;						// 농장 오브젝트 정보

public:
	//----------------------------------------------------------------------------------------------------------------
	CSHFarmRenderObj();
	virtual ~CSHFarmRenderObj();

	//----------------------------------------------------------------------------------------------------------------
	// 초기화
	virtual BOOL	Init(EObjectKind kind, BASEOBJECT_INFO* pBaseObjectInfo, stFarmRenderObjInfo* pstFarmRenderObjInfo);
	virtual void	Release();

	// 농장 오브젝트 정보 얻기
	stFarmRenderObjInfo*	GetInfo()	{ return &m_stFarmRenderObjInfo; }

	// 이름 변경
	void			SetName(char* pszName)		
	{
		#if !defined(_AGENTSERVER)
			SafeStrCpy(m_BaseObjectInfo.ObjectName, pszName, MAX_NAME_LENGTH); 
		#endif
	}
};