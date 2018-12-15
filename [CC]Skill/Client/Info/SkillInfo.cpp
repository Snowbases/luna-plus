#include "stdafx.h"
#include ".\skillinfo.h"
#include "../Object/SkillObject.h"
#include "ActionTarget.h"

cSkillInfo::cSkillInfo(Type type) : 
mType(type)
{
}

cSkillInfo::~cSkillInfo(void)
{}

void cSkillInfo::SendMsgToServer(CObject* pOperator, CActionTarget* pTarget, const ITEMBASE& itemBase)
{
	MSG_SKILL_START_SYN msg;
	ZeroMemory( &msg, sizeof( msg ) );
	MAINTARGET MainTarget;
	
	pTarget->GetMainTarget( &MainTarget );

	ROTATEINFO* pRotateInfo = pOperator->GetBaseRotateInfo();
	if( !pRotateInfo )	return;
	const float fOperatorAngle = (pRotateInfo->bRotating == TRUE ? pRotateInfo->Angle.GetTargetAngleRad() : pOperator->GetAngle());
	const VECTOR3 directionVector = GetRadToVector( fOperatorAngle );
	
	msg.InitMsg( GetIndex(), &MainTarget, directionVector, pOperator->GetID() );
	msg.mConsumeItem = itemBase;
	
	pTarget->GetTargetArray(&msg.TargetList);

	NETWORK->Send(&msg,msg.GetMsgLength());
}