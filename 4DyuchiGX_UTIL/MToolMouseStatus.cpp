#include "MToolMouseStatus.h"
#include "../4dyuchigrx_common/typedef.h"
#include "../4DyuchiGXGFunc/global.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MToolMouseStatus::MToolMouseStatus()
{

}

MToolMouseStatus::~MToolMouseStatus()
{

}

BOOL MToolMouseStatus::Initialize(I4DyuchiGXExecutive* pExecutive)
{
	mfMoveHorizonStep		=	3.0f;
	mfMoveVerticalStep		=	20.0f;
	mfMoveStep				=	0.0007f;
	mfZoomStep				=	20.0f;
	mfPickDist				=	0.0f;
//	fRotateStep		=	0.2f;	// Shift에 의해 상태가 달라지는 회전, 이동 속도값.
	mfRotateStep		=	0.13f * PI / 180.0f;	// Shift에 의해 상태가 달라지는 회전, 이동 속도값.
	mpExecutive = pExecutive;
	mpGeometry = pExecutive->GetGeometry();
	mbDoNotRotate		=	FALSE;

	miMouseX	=	-1;
	miMouseY	=	-1;
	miMouseZ	=	-1;

	miOldMouseX	=	-1;
	miOldMouseY	=	-1;

	mbLButtonDown	=	false;
	mbRButtonDown	=	false;
	mbMButtonDown	=	false;
	mbCtrlDown		=	false;
	mbShiftDown		=	false;		// 쉬프트 버튼은 고속 이동.
	mbAltDown		=	false;

	mdwMouseMode	=	MOUSE_NONE;



	return TRUE;
}
bool MToolMouseStatus::MoveMouse(DWORD dwFlag, int iMouseX, int iMouseY, DWORD &viewType)
{
	if( mpGeometry == 0)
	{
		__asm int 3
		// 랜더러가 아직 설정 안됬다.
	}
	if( miMouseX == -1 || miMouseY == -1)
	{
		miOldMouseX	=	iMouseX;
		miOldMouseY	=	iMouseY;
	}
	miMouseX	=	iMouseX;
	miMouseY	=	iMouseY;

	mbAltDown = false;
	if (GetAsyncKeyState(VK_LMENU)&0x8000)
	{
		mbAltDown	=	 true;
	}
	//	컨트롤키.
	mbCtrlDown	=	false;
	if( dwFlag & MK_CONTROL)
	{
		mbCtrlDown	=	true;
	}
	//	시프트키.
	mbShiftDown	=	false;
	if( dwFlag & MK_SHIFT)
	{
		mbShiftDown	=	true;
//		fMoveHorizonStep		=	5.0f;
//		fMoveVerticalStep		=	50.0f;
//		fRotateStep		=	0.5f;	// Shift에 의해 상태가 달라지는 회전, 이동 속도값.
	}
	else
	{
//		fMoveHorizonStep		=	1.0f;
//		fMoveVerticalStep		=	10.0f;
//		fRotateStep		=	0.2f;	// Shift에 의해 상태가 달라지는 회전, 이동 속도값.
	}
	//	마우스 왼쪽 버튼.
	mbLButtonDown	=	false;
	if( dwFlag & MK_LBUTTON)
	{
		mbLButtonDown	=	true;
	}
	//	마우스 중간 버튼.
	mbMButtonDown	=	false;
	if( dwFlag & MK_MBUTTON)
	{
		mbMButtonDown	=	true;
	}
	//	마우스 오른쪽 버튼.
	mbRButtonDown	=	false;
	if( dwFlag & MK_RBUTTON)
	{
		mbRButtonDown	=	true;
	}

	// 모드에 따른 스위치, 함수 콜.
	switch( mdwMouseMode)
	{
		case	MOUSE_NONE:
				miOldMouseX = iMouseX;
				miOldMouseY = iMouseY;
			// 마우스 오른쪽 버튼만 눌린 경우.
			// 이동 모드로.
			if( !mbLButtonDown && mbMButtonDown && !mbRButtonDown && !mbCtrlDown && !mbShiftDown && !mbAltDown)	
			{
				CAMERA_DESC Desc;
				mpGeometry->GetCameraDesc( &Desc, 0);

				VECTOR3 vPickPos;
				POINT pt;
				pt.x = iMouseX;
				pt.y = iMouseY;

				if (!mpExecutive->GXMGetHFieldCollisionPointWithScreenCoord(&vPickPos,&mfPickDist,(POINT*)&pt))
				{
					vPickPos.x = 0.0f;
					vPickPos.y = 0.0f;
					vPickPos.z = 0.0f;

					mfPickDist = 10000.0f;
				}

				mdwMouseMode = MOUSE_MOVE_CAM;	
			}
			// 마우스 오른쪽 버튼과 알트키가 눌린 경우.
			// 회전 모드로.
			if( !mbLButtonDown && mbMButtonDown && !mbRButtonDown && !mbCtrlDown && !mbShiftDown && mbAltDown)	
			{
				if (viewType == 0)
				{
					PushCameraAngleAndPos(0);

					viewType = 6;
				}

				mdwMouseMode = MOUSE_ROTATE_CAM;	
			}
			// 마우스 휠 버튼 드래그 -> 다시 원래의 것으로 바꿈
			//	수평 이동 모드로.
			else if( !mbLButtonDown && mbMButtonDown && !mbRButtonDown && mbCtrlDown && !mbShiftDown && !mbAltDown)	
			{
				mdwMouseMode	=	MOUSE_MOVEHORIZON_CAM;
			}
			// 마우스 오른쪽 버튼과 시프트가 눌려진 경우.
			//	수직(높이) 이동 모드로.
			else if( !mbLButtonDown && mbMButtonDown && !mbRButtonDown && !mbCtrlDown && mbShiftDown && !mbAltDown)	
			{
				mdwMouseMode	=	MOUSE_MOVEVERTICAL_CAM;
			}
			// 마우스 오른쪽 버튼과 시프트,컨트롤이 동시에 눌려진 경우.
			//	주어진 피봇을 중심으로 회전한다.
			else if( !mbLButtonDown && mbMButtonDown && !mbRButtonDown && mbCtrlDown && mbShiftDown && !mbAltDown)
			{
				mdwMouseMode	=	MOUSE_ROTATE_WITH_PIVOT_CAM;
			}
			else if( !mbLButtonDown && mbMButtonDown && !mbRButtonDown && mbCtrlDown && !mbShiftDown && mbAltDown)
			{
				mdwMouseMode	=	MOUSE_ZOOM_CAM;
			}

			return this->MouseNone();
			break;
		case	MOUSE_MOVE_CAM:
			if( mbLButtonDown || !mbMButtonDown || mbRButtonDown || mbCtrlDown || mbShiftDown || mbAltDown)	// 어떻게든 상태가 달라지면 모드 해제.
			{
				mdwMouseMode	=	MOUSE_NONE;
			}
			else
			{
				return	this->MouseMove();
			}

			break;
		case	MOUSE_ROTATE_CAM:
			if( mbLButtonDown || !mbMButtonDown || mbRButtonDown || mbCtrlDown || mbShiftDown || !mbAltDown)	// 어떻게든 상태가 달라지면 모드 해제.
			{
				mdwMouseMode	=	MOUSE_NONE;
			}
			else
			{
				return	this->MouseRotate();
			}

			break;
		case	MOUSE_MOVEHORIZON_CAM:
			if( mbLButtonDown || !mbMButtonDown || mbRButtonDown || !mbCtrlDown || mbShiftDown || mbAltDown)	// 어떻게든 상태가 달라지면 모드 해제.
			{
				mdwMouseMode	=	MOUSE_NONE;
			}
			else
			{
				return	this->MouseMoveHorizon();
			}
			break;
		case	MOUSE_MOVEVERTICAL_CAM:
			if( mbLButtonDown || !mbMButtonDown || mbRButtonDown || mbCtrlDown || !mbShiftDown || mbAltDown)	// 어떻게든 상태가 달라지면 모드 해제.
			{
				mdwMouseMode	=	MOUSE_NONE;
			}
			else
			{
				return	this->MouseMoveVertical();
			}
			break;
		case	MOUSE_ROTATE_WITH_PIVOT_CAM:
			if( mbLButtonDown || !mbMButtonDown || mbRButtonDown || !mbCtrlDown || !mbShiftDown || mbAltDown)	// 어떻게든 상태가 달라지면 모드 해제.
			{
				mdwMouseMode	=	MOUSE_NONE;
			}
			else
			{
				return	this->MouseRotateWithPivot();
			}
			break;
		case	MOUSE_ZOOM_CAM:
			if( mbLButtonDown || !mbMButtonDown || mbRButtonDown || !mbCtrlDown || mbShiftDown || !mbAltDown)	// 어떻게든 상태가 달라지면 모드 해제.
			{
				mdwMouseMode	=	MOUSE_NONE;
			}
			else
			{
				return	this->MouseMoveForward();
			}
			break;
		default:
			break;
	}

return false;
}

bool MToolMouseStatus::MouseNone()
{
	// 아무것도 안한다. -_-;
	return false;
}

bool MToolMouseStatus::MouseMove(void)
{
	CAMERA_DESC Desc;
	mpGeometry->GetCameraDesc( &Desc, 0);

	float	x = float(miMouseX - miOldMouseX) * mfPickDist * mfMoveStep;
	float	y = float(miMouseY - miOldMouseY) * mfPickDist * mfMoveStep;

	VECTOR3	vAt, vUp, vRight, vMove;

	vUp = Desc.v3Up;
	vAt = Desc.v3EyeDir;

	Normalize(&vUp, &vUp);
	Normalize(&vAt, &vAt);

	CrossProduct(&vRight, &vUp, &vAt);
	Normalize(&vRight, &vRight);

	vMove = (vUp * y) + (vRight * -x);

	mpGeometry->MoveCamera(&vMove,0);

	miOldMouseX	=	miMouseX;
	miOldMouseY	=	miMouseY;

	return true;
}

bool MToolMouseStatus::MouseRotate()
{
	if( mbDoNotRotate == TRUE)	return true;		// 마우스 로테이트 금지 모드일경우...

	float	x = float(miMouseX - miOldMouseX) * mfRotateStep;
	float	y = float(miMouseY - miOldMouseY) * mfRotateStep;
	VECTOR3	Rotate;
	Rotate.x = -y;
	Rotate.y = x;
	Rotate.z = 0;


	mpGeometry->RotateCameraRad(&Rotate,0);

	miOldMouseX	=	miMouseX;
	miOldMouseY	=	miMouseY;

	return true;
}

bool MToolMouseStatus::MouseRotateWithPivot()
{
	if( mbDoNotRotate == TRUE)	return true;		// 마우스 로테이트 금지 모드일경우...
//	여기

	mRotationPivot.x	=	50.0f;
	mRotationPivot.y	=	0.0f;
	mRotationPivot.z	=	0.0f;


	float	fRotateYaw		=	float(miMouseX - miOldMouseX) * mfRotateStep;
	float	fRotatePitch	=	-1.0f * float(miMouseY - miOldMouseY) * mfRotateStep;
	VECTOR3	Rotate;
	VECTOR3		NewPos, CurrentPos;
	CAMERA_DESC Desc;
	mpGeometry->GetCameraDesc( &Desc, 0);
	CurrentPos	=	Desc.v3From;

	VECTOR3		CameraAngle;
	mpGeometry->GetCameraAngleRad( &CameraAngle, 0);
/*
	if( CameraAngle.x >= (PI-0.01)/2.0f)	// 카메라가 아래를 보고 있을 경우.
	{
		_asm int 3;
	}
	else if( CameraAngle.x <= -1.0f * (PI-0.01)/2.0f)	// 카메라가 아래를 보고 있을 경우.
	{
		_asm int 3;
	}
*/
	// 피봇을 기준으로 위치를 옮기고,
	RotatePositionWithPivot( &NewPos, &mRotationPivot, &CurrentPos, &CameraAngle, fRotateYaw, fRotatePitch);
	mpGeometry->SetCameraPos( &NewPos, 0);

	// 마우스를 로테이트.	(위치 옮기는것과의 순서는 관계 없다.)
	Rotate.x = fRotatePitch;
	Rotate.y = fRotateYaw;
	Rotate.z = 0;
	mpGeometry->RotateCameraRad(&Rotate,0);

	miOldMouseX	=	miMouseX;
	miOldMouseY	=	miMouseY;

	return true;
}

bool MToolMouseStatus::MouseMoveForward()
{
	float	y	=	float(miMouseY - miOldMouseY) * -mfZoomStep;

	CAMERA_DESC Desc;
	mpGeometry->GetCameraDesc( &Desc, 0);

	VECTOR3	vAt = Desc.v3EyeDir;

	Normalize(&vAt, &vAt);

	vAt = vAt * y;
	mpGeometry->MoveCamera( &vAt,0);

	miOldMouseX	=	miMouseX;
	miOldMouseY	=	miMouseY;

	return true;
}

bool MToolMouseStatus::MouseMoveHorizon()
{
	float	x	=	float(miMouseX - miOldMouseX) * mfMoveHorizonStep * -1.0f;
	float	y	=	float(miMouseY - miOldMouseY) * mfMoveHorizonStep * -1.0f;
	
	VECTOR3	Rot;
	mpGeometry->GetCameraAngleRad(&Rot,0);

	VECTOR3	To;
	To.x	=	(-1 * x * (float)cosf(Rot.y) *10.0f + y * (float)sinf(Rot.y)*10.0f);
	To.y	=	0.0f;
	To.z	=	x * (float)sinf(Rot.y) *10.0f + y * (float)cosf(Rot.y)*10.0f;

	mpGeometry->MoveCamera( &To,0);

	miOldMouseX	=	miMouseX;
	miOldMouseY	=	miMouseY;
						
	return true;
}

bool MToolMouseStatus::MouseMoveVertical()
{
	VECTOR3		To;
	To.x	=	0;
	To.y	=	float(miMouseY - miOldMouseY) * mfMoveVerticalStep * -1;
	To.z	=	0;
	mpGeometry->MoveCamera(&To,0);

	miOldMouseX	=	miMouseX;
	miOldMouseY	=	miMouseY;

	return true;
}


void	MToolMouseStatus::DoNotRotate(void)
{
	mbDoNotRotate	=	TRUE;
}


void	MToolMouseStatus::AllowRotate(void)
{
	mbDoNotRotate	=	FALSE;
}

void	MToolMouseStatus::PushCameraAngleAndPos(DWORD dwStackNum)
{
	CAMERA_DESC		Desc;
	mpGeometry->GetCameraDesc(&Desc,0);
	mpCameraStackPos[dwStackNum]		=	Desc.v3From;
	mpfCameraStackFromTo[dwStackNum]	=	CalcDistance( &(Desc.v3From), &(Desc.v3To));
	mpGeometry->GetCameraAngleRad(&mpCameraStackAngle[dwStackNum],0);
}

void	MToolMouseStatus::PopCameraAngleAndPos(DWORD dwStackNum)
{
	CAMERA_DESC		Desc;
	mpGeometry->GetCameraDesc(&Desc,0);

	mpCameraStackPos[dwStackNum]		=	Desc.v3From;

	VECTOR3	To	=	mpCameraStackPos[dwStackNum];
	To.z	=	To.z + mpfCameraStackFromTo[dwStackNum];
	VECTOR3 Up;
	Up.x = Up.z = 0;
	Up.y = 1;

	if (dwStackNum == 0 /*VIEW_TYPE_TOP*/)
	{
		mpCameraStackAngle[dwStackNum].x	=	-90.0f * (PI / 180.0f);
		mpCameraStackAngle[dwStackNum].y	=	0.0f;
		mpCameraStackAngle[dwStackNum].z	=	0.0f;
	}

	float fNear = DEFAULT_NEAR;
	float fFar = DEFAULT_FAR;

	mpGeometry->ResetCamera( &(mpCameraStackPos[dwStackNum]),fNear,fFar,DEFAULT_FOV,0);
//	mpGeometry->ResetCamera( &(mpCameraStackPos[dwStackNum]),fNear,5000.0f,DEFAULT_FOV,0);

	mpGeometry->SetCameraAngleRad( &(mpCameraStackAngle[dwStackNum]),0 );
}

void MToolMouseStatus::SetRotationPivot(VECTOR3 *pPivot)
{
	mRotationPivot	=	*pPivot;
}


/// 키보드로 카메라를 움직이기 위한 부분
// 카메라의 전방으로 이동한다
void MToolMouseStatus::MoveCamera_Forward( float p_Value )
{
	CAMERA_DESC Desc;
	mpGeometry->GetCameraDesc( &Desc, 0);

	VECTOR3	vAt = Desc.v3EyeDir;

	Normalize(&vAt, &vAt);

	vAt = vAt * p_Value * 5.0f;

	mpGeometry->MoveCamera( &vAt,0);
}

void MToolMouseStatus::MoveCamera_Backward( float p_Value )
{
	CAMERA_DESC Desc;
	mpGeometry->GetCameraDesc( &Desc, 0);

	VECTOR3	vAt = Desc.v3EyeDir;

	Normalize(&vAt, &vAt);

	vAt = vAt * -p_Value * 5.0f;

	mpGeometry->MoveCamera( &vAt,0);
}

void MToolMouseStatus::MoveCamera_LeftSideStep( float p_Value )
{
	// 회전각으로부터 전방벡터를 얻는다
	VECTOR3	Rot;
	mpGeometry->GetCameraAngleRad(&Rot,0);

	VECTOR3	t_Front;
	t_Front.x	=	p_Value * (float)sinf(Rot.y);
	t_Front.y	=	p_Value * (float)sinf(Rot.x);
	t_Front.z	=	p_Value * (float)cosf(Rot.y);

	// 전방벡터와 (0,1,0) 벡터를 외적, 사이드벡터를 얻는다
	VECTOR3 t_UpVec = { 0.0f, 1.0f, 0.0f };
	VECTOR3 t_SideVec;
	CrossProduct( &t_SideVec, &t_Front, &t_UpVec );

	mpGeometry->MoveCamera( &t_SideVec,0);
}

void MToolMouseStatus::MoveCamera_RightSideStep( float p_Value )
{
	// 회전각으로부터 전방벡터를 얻는다
	VECTOR3	Rot;
	mpGeometry->GetCameraAngleRad(&Rot,0);

	VECTOR3	t_Front;
	t_Front.x	=	p_Value * (float)sinf(Rot.y);
	t_Front.y	=	p_Value * (float)sinf(Rot.x);
	t_Front.z	=	p_Value * (float)cosf(Rot.y);

	// 전방벡터와 (0,1,0) 벡터를 외적, 사이드벡터를 얻는다
	VECTOR3 t_UpVec = { 0.0f, 1.0f, 0.0f };
	VECTOR3 t_SideVec;
	CrossProduct( &t_SideVec, &t_UpVec, &t_Front );	// 오른쪽이니까 반대로

	mpGeometry->MoveCamera( &t_SideVec,0);
}

void MToolMouseStatus::MoveCamera_Up( float p_Value )
{
	// 그냥 위로
	VECTOR3 t_Vec = { 0.0f, p_Value, 0.0f };

	mpGeometry->MoveCamera( &t_Vec,0);
}

void MToolMouseStatus::MoveCamera_Down( float p_Value )
{
	// 그냥 아래로
	VECTOR3 t_Vec = { 0.0f, -1 * p_Value, 0.0f };

	mpGeometry->MoveCamera( &t_Vec,0);
}