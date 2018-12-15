#include "StdAfx.h"
#include "ActionDistance.h"
#include "Machine.h"
#include "Parser.h"
#include "..\UserTable.h"
#include "..\Object.h"
#include "..\CharMove.h"
#include "..\..\4DyuchiGXGFunc\global.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionDistance(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Distance",
			Clone);
	}

	CActionDistance::CActionDistance(LPCTSTR friendName) :
	CAction("Distance"),
	mFriendName(friendName)
	{}

	int CActionDistance::Run(CMachine& machine) const
	{
		CObject* const friendObject = g_pUserTable->FindUser(
			machine.GetObjectIndex(mFriendName.c_str()));

		if(0 == friendObject)
		{
			return INT_MAX;
		}

		VECTOR3 friendPosition = *(CCharMove::GetPosition(
			friendObject));

		CObject* const object = g_pUserTable->FindUser(
			machine.GetObjectIndex());

		if(0 == object)
		{
			return INT_MAX;
		}

		VECTOR3 objectPosition = *(CCharMove::GetPosition(
			object));

		return int(CalcDistance(&friendPosition, &objectPosition));
	}
}