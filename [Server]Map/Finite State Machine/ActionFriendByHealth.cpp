#include "StdAfx.h"
#include "ActionFriendByHealth.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\Monster.h"
#include "..\UserTable.h"
#include "..\Object.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();
			LPCTSTR parameter2 = parameterContainer["parameter2"].c_str();

			const float rate = float(_tstof(
				parameter2)) / 100.0f;
			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionFriendByHealth(
				alias,
				rate);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"FriendByHealth",
			Clone);
	}

	CActionFriendByHealth::CActionFriendByHealth(LPCTSTR alias, float rate) :
	CAction("FriendByHealth"),
	mAlias(alias),
	mMinimumRate(rate)
	{}

	int CActionFriendByHealth::Run(CMachine& machine) const
	{
		CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
			machine.GetObjectIndex(mAlias.c_str()));

		if(0 == monsterObject)
		{
			return 0;
		}
		else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
		{
			return 0;
		}

		CMachine& targetMachine = monsterObject->GetFiniteStateMachine();
		targetMachine.GetMemory().GetFoundObject().clear();

		const CMemory::TextContainer& friendContainer = targetMachine.GetMemory().GetFriend();

		for(CMemory::TextContainer::const_iterator iterator = friendContainer.begin();
			friendContainer.end() != iterator;
			++iterator)
		{
			const std::string& friendName = *iterator;
			CObject* const object = g_pUserTable->FindUser(
				machine.GetObjectIndex(friendName.c_str()));

			if(0 == object)
			{
				continue;
			}

			const float rate = float(object->GetLife()) / object->GetMaxLife();

			if(mMinimumRate < rate)
			{
				continue;
			}

			machine.GetMemory().AddFoundObject(
				object->GetID());
			break;
		}

		return false == machine.GetMemory().GetFoundObject().empty();
	}
}