#include "StdAfx.h"
#include "ActionFriend.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"

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

			return new CActionFriend(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Friend",
			Clone);
	}

	CActionFriend::CActionFriend(LPCTSTR friendName) :
	CAction("Friend"),
	mFriendName(friendName)
	{}

	int CActionFriend::Run(CMachine& machine) const
	{
		machine.GetMemory().GetFriend().insert(
			mFriendName.c_str());
		return 1;
	}
}