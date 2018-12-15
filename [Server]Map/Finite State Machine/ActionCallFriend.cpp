#include "StdAfx.h"
#include "ActionCallFriend.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\UserTable.h"
#include "..\Monster.h"
#include "..\StateMachinen.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			std::string channel = parameterContainer["channel"];
			std::string parameter1 = parameterContainer["parameter1"];

			return new CActionCallFriend(
				CMachine::GetParser().GetAlias(parameter1.c_str(), _ttoi(channel.c_str())));
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"CallFriend",
			Clone);
	}

	CActionCallFriend::CActionCallFriend(LPCTSTR alias) :
	CAction("CallFriend"),
	mAlias(alias)
	{}

	int CActionCallFriend::Run(CMachine& machine) const
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
		else if(0 == monsterObject->GetTObject())
		{
			return 0;
		}

		const CMemory::TextContainer& nameContainer = monsterObject->GetFiniteStateMachine().GetMemory().GetFriend();

		for(CMemory::TextContainer::const_iterator iterator = nameContainer.begin();
			nameContainer.end() != iterator;
			++iterator)
		{
			const std::string& name = *iterator;
			const DWORD objectIndex = g_pUserTable->GetObjectIndex(
				name.c_str());
			CMonster* const friendMonsterObject = (CMonster*)g_pUserTable->FindUser(
				objectIndex);

			if(0 == friendMonsterObject)
			{
				continue;
			}
			else if(FALSE == (eObjectKind_Monster & friendMonsterObject->GetObjectKind()))
			{
				continue;
			}
			else if(monsterObject->GetID() == friendMonsterObject->GetID())
			{
				continue;
			}
			else if(eMA_PERSUIT == friendMonsterObject->mStateParamter.stateCur ||
				eMA_ATTACK == friendMonsterObject->mStateParamter.stateCur)
			{
				continue;
			}

			friendMonsterObject->SetTObject(
				monsterObject->GetTObject());
			GSTATEMACHINE.SetState(
				friendMonsterObject,
				eMA_PERSUIT);
		}

		return 1;
	}
}