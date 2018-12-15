#include "stdafx.h"
#include "Target.h"
#include "UserTable.h"
#include "Object.h"

cTarget::cTarget(void)
{
}

cTarget::~cTarget(void)
{
}

CObject* cTarget::GetData()
{
	if(mObjectIndexContainer.end() == mIterator)
	{
		return 0;
	}

	const ObjectIndex objectIndex = *mIterator++;

	CObject* const object = g_pUserTable->FindUser(
		objectIndex);

	if(0 == object)
	{
		return 0;
	}
	else if(FALSE == object->GetInited())
	{
		return 0;
	}

	return object;
}