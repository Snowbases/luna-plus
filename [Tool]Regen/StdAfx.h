#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxisapi.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

extern BOOL g_bUsingEnginePack;

#include "resource.h"
#include "PtrList.h"
#include "DefineStruct.h"
#include "DirectoryManager.h"
#include "../[CC]Header/CommonDefine.h"
#include "../4DYuchiGRX_Common/IExecutive.h"

extern I4DyuchiGXExecutive* g_pExecutive;
#define LOGEX(a,b)	MessageBox(NULL,a,NULL,NULL);

#pragma warning(disable:4100) // 참조하지 않는 매개변수

#include "..\[CC]Header\AESFile.h"
#include <fcntl.h>
#include <io.h>
#include <map>
#include <set>
#include <list>
#include <hash_map>
#include <hash_set>
#include <afxdlgs.h>
#include <float.h>
#include "Grid_Control/GridCtrl.h"


enum EObjectKind
{
	// 080616 LUJ, 초기화용 값 추가
	eObjectKind_None,
	eObjectKind_Player = 1,
	eObjectKind_Npc		=	2,
	eObjectKind_Item	=	4,
	eObjectKind_SkillObject=16,
	eObjectKind_Monster	=	32,
	eObjectKind_BossMonster=33,
	eObjectKind_SpecialMonster=34,

	// 필드보스 - 05.12 이영준
	eObjectKind_FieldBossMonster=35,
	eObjectKind_FieldSubMonster=36,
	eObjectKind_ToghterPlayMonster=37,
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.23
	eObjectKind_ChallengeZoneMonster = 38,
	// E 데이트 존 추가 added by hseos 2007.11.23
	// 080616 LUJ, 함정 추가
	eObjectKind_Trap = 39,
	// 090316 LUJ, 탈것
	eObjectKind_Vehicle		= 40,
	eObjectKind_MapObject	= 64,
	eObjectKind_Pet			= 128,
};

DWORD GetHashCodeFromText(LPCTSTR text);

#define MAKE_COLORREF(r, g, b, a)   ((DWORD) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))

struct Script
{
	enum Type
	{
		TypeNone,
		TypeUnique,
	}
	mType;
	DWORD mGroupIndex;
	CString mGroupName;
	struct Condition
	{
		DWORD mGroupIndex;
		float mRatio;
		DWORD mDelay;
		BOOL mIsRegen;
		DWORD mRange;

		// Regen Tool Data
		DWORD dwConditionIdx;

		BOOL operator > ( const Condition& condition )		{ return this->dwConditionIdx > condition.dwConditionIdx; }
		BOOL operator < ( const Condition& condition )		{ return this->dwConditionIdx < condition.dwConditionIdx; }
		BOOL operator == ( const Condition& condition )		{ return this->dwConditionIdx == condition.dwConditionIdx; }
	};
	typedef std::list< Condition > ConditionList;
	ConditionList mConditionList;

	struct Monster
	{
		EObjectKind mObjectKind;
		WORD mMonsterKind;
		VECTOR3 mPosition;
		CString mFiniteStateMachine;
		GXOBJECT_HANDLE hGXO;

		Monster()
		{
			mObjectKind = eObjectKind_None;
			mMonsterKind = 0;
			mPosition.x = mPosition.y = mPosition.z = 0;
			hGXO = NULL;
		}
	};
	typedef std::list< Monster > MonsterList;
	MonsterList mMonsterList;

	typedef std::list< DWORD > UniqueGroupIndexList;
	UniqueGroupIndexList mUniqueGroupIndexList;
	typedef std::list< VECTOR3 > FieldBossPositionList;
	FieldBossPositionList mFieldBossPositionList;

	struct Delay
	{
		DWORD mMinTick;
		DWORD mMaxTick;
	};
	typedef std::list< Delay > DelayList;
	DelayList mDelayList;

	Script() :
	mGroupIndex(0),
		mType(TypeNone)
	{}
};

#ifdef NDEBUG

	#undef ASSERT
	#undef ASSERTMSG
	#undef DEBUGMSG
	#undef DEBUG

	#define ASSERT(a)			void(0)
	#define ASSERTMSG(a,b)		void(0)
	#define DEBUGMSG(a, b)		void(0)
	#define DEBUG(a)			void(0)
#endif
