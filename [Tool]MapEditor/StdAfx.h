// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0DBADFB5_CAA6_417B_88B4_4ABFDAE9A7EB__INCLUDED_)
#define AFX_STDAFX_H__0DBADFB5_CAA6_417B_88B4_4ABFDAE9A7EB__INCLUDED_

#define WINVER 0x0501

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <crtdbg.h>
#include <io.h>
#include <direct.h>
#include <stack>
#include <hash_set>
#include "Voidlist.h"
#include "../4DyuchiGX_UTIL/MToolMouseStatus.h"
#include "../4DyuchiGRX_common/stdafx.h"
#include "../4DyuchiGRX_common/IGeometry.h"
#include "../4DyuchiGRX_common/IRenderer.h"
#include "../4DyuchiGRX_common/IExecutive.h"
#include "resource.h"

#define MAX_LIGHT_NUM		64
#define MAX_CAMERA_NUM		16
#define MAX_OBJECTS_NUM		4096
#define MAX_ZONE_NUM		32 // @@@ iros  32개까지로 제한

//Added by KBS
#define MOVE_SPOT_START_TILE_NO		9	
#define MAX_MOVE_SPOT_NUM			10
#define START_SPOT_START_TILE_NO	19	
#define MAX_START_SPOT_NUM			5

enum VIEW_TYPE
{
	VIEW_TYPE_TOP		=	0x00000000,
	VIEW_TYPE_LEFT		=	0x00000001,
	VIEW_TYPE_RIGHT		=	0x00000002,
	VIEW_TYPE_BOTTOM	=	0x00000003,
	VIEW_TYPE_FRONT		=	0x00000004,
	VIEW_TYPE_BACK		=	0x00000005,
	VIEW_TYPE_FREE		=	0x00000006
};

enum EDIT_TYPE
{
	EDIT_TYPE_MOVE		= 0x00000001,
	EDIT_TYPE_ROT		= 0x00000002,
	EDIT_TYPE_SCALE		= 0x00000004,
	EDIT_TYPE_CREATE	= 0x00000008
};

enum EDIT_MODE
{
	EDIT_MODE_TILE				= 0x10000000,
	EDIT_MODE_VERTEX			= 0x20000000,
	EDIT_MODE_GXOBJECT			= 0x30000000,
	EDIT_MODE_BRUSH				= 0x40000000,
	EDIT_MODE_GET_TILE_INFO		= 0x50000000,
	EDIT_MODE_GET_HFIELDOBJECT	= 0x60000000,
	EDIT_MODE_LIGHT				= 0x70000000,
	EDIT_MODE_TRIGGER			= 0x80000000,
	EDIT_MODE_DRAW_LIGHTMAP		= 0x90000000,
	EDIT_MODE_DRAW_VERTEXCOLOR	= 0xA0000000,
	EDIT_MODE_DRAW_HFIELD_ALPHAMAP = 0xB0000000,
	//yh
	EDIT_MODE_TILESET			= 0xC0000000	
};

#define EDIT_MODE_MASK			0xf0000000
#define EDIT_MODE_MASK_INVERSE	0x0fffffff
enum MOUSE_STATUS
{
	LBUTTON_DOWN		=		0x00000001,
	LBUTTON_UP			=		0x00000002,
	MOVE				=		0x00000004

};

#define DEFAULT_PICK_DISTANCE 1000.0f

enum AXIS_TYPE
{
	NOT_SET			= 0x00000000,
	X_AXIS_LOCK		= 0x00000001,
	Y_AXIS_LOCK		= 0x00000002,
	Z_AXIS_LOCK		= 0x00000004
};
struct TILE_INFO
{
	DWORD		dwPosX;
	DWORD		dwPosY;

};

enum eUndoStatus
{
	eUndoStatus_None,		// 언두가 불가능함
	eUndoStatus_Writing,	// 언두 가능한 어떤 작업을 기록중인 상태
	eUndoStatus_CanUndo,	// 언두 가능한 상태

	eUndoLevels = 20,		// 이건 언두 단계
};

#define DEFAULT_SS3D_MAP_FILE_NAME		"제목없음"
#define LOAD_FAILED_TEXTURE_TABLE_SIZE	512

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0DBADFB5_CAA6_417B_88B4_4ABFDAE9A7EB__INCLUDED_)
