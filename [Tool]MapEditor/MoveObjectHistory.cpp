#include "stdafx.h"
#include "MoveObjectHistory.h"
#include "Tool.h"

CMoveObjectHistory::CMoveObjectHistory()
{}

void CMoveObjectHistory::SetValue(const GXOBJECT_PROPERTY& property, GXOBJECT_HANDLE handle)
{
	mParameter.mProperty = property;
	mParameter.mHandle = handle;
}

CMoveObjectHistory::~CMoveObjectHistory(void)
{}

void CMoveObjectHistory::Undo()
{
	Run();
}

void CMoveObjectHistory::Redo()
{
	Run();
}

void CMoveObjectHistory::Run()
{
	I4DyuchiGXExecutive* const executive = g_pTool->GetExecutive();

	if(0 == executive)
	{
		return;
	}

	LPCTSTR fileName = LPCTSTR(executive->GetData(mParameter.mHandle));

	if(0 == fileName)
	{
		mParameter = Parameter();
		return;
	}

	const GXOBJECT_PROPERTY property = mParameter.mProperty;
	RefreshProperty();	

	executive->GXOSetDirection(
		mParameter.mHandle,
		&(VECTOR3(property.v3Axis)),
		property.fRad);
	executive->GXOSetPosition(
		mParameter.mHandle,
		&(VECTOR3(property.v3Pos)),
		FALSE);
	executive->GXOSetScale(
		mParameter.mHandle,
		&(VECTOR3(property.v3Scale)));

	if(property.bApplyHField)
	{
		executive->GXOEnableHFieldApply(
			mParameter.mHandle);
	}
	else
	{
		executive->GXODisableHFieldApply(
			mParameter.mHandle);
	}

	if(property.bLock)
	{
		executive->LockTransform(
			mParameter.mHandle);
	}
	else
	{
		executive->UnLockTransform(
			mParameter.mHandle);
	}

	if(property.bAsEffect)
	{
		executive->GXOEnableAsEffect(
			mParameter.mHandle);
	}
	else
	{
		executive->GXODisableAsEffect(
			mParameter.mHandle);
	}
}

void CMoveObjectHistory::RefreshProperty()
{
	I4DyuchiGXExecutive* const executive = g_pTool->GetExecutive();

	if(0 == executive)
	{
		return;
	}

	LPCTSTR fileName = LPCTSTR(executive->GetData(
		mParameter.mHandle));

	if(0 == fileName)
	{
		mParameter = Parameter();
		return;
	}	

	GXOBJECT_PROPERTY& property = mParameter.mProperty;

	property.dwID = executive->GetID(
		mParameter.mHandle);
	executive->GXOGetDirection(
		mParameter.mHandle,
		&property.v3Axis,
		&property.fRad);
	executive->GXOGetPosition(
		mParameter.mHandle,
		&property.v3Pos);
	executive->GXOGetScale(
		mParameter.mHandle,
		&property.v3Scale);
	property.bApplyHField = executive->GXOIsHFieldApply(
		mParameter.mHandle);
	property.bAsEffect = executive->GXOIsAsEffect(
		mParameter.mHandle);
	property.bLock = executive->IsLockTransform(
		mParameter.mHandle);
}

