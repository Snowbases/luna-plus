#include "StdAfx.h"
#include "ActionCallTeam.h"
#include "Machine.h"
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
			LPCTSTR channel = parameterContainer["channel"].c_str();
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			LPCTSTR alias = CMachine::GetParser().GetAlias(
				parameter1,
				_ttoi(channel));

			return new CActionCallTeam(
				alias);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"CallTeam",
			Clone);
	}

	CActionCallTeam::CActionCallTeam(LPCTSTR teamName) :
	CAction("CallTeam"),
	mTeamName(teamName)
	{}

	int CActionCallTeam::Run(CMachine& machine) const
	{
		CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
			machine.GetObjectIndex());

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

		const CUserTable::Team& team = g_pUserTable->GetTeam(
			mTeamName.c_str());

		for(CUserTable::Team::ObjectIndexContainer::const_iterator iterator = team.mMemberContainer.begin();
			team.mMemberContainer.end() != iterator;
			++iterator)
		{
			const DWORD objectIndex = *iterator;

			if(machine.GetObjectIndex() == objectIndex)
			{
				continue;
			}

			CMonster* const teamMonsterObject = (CMonster*)g_pUserTable->FindUser(
				objectIndex);

			if(0 == teamMonsterObject)
			{
				continue;
			}
			else if(FALSE == (eObjectKind_Monster & teamMonsterObject->GetObjectKind()))
			{
				continue;
			}
			else if(eMA_PERSUIT == teamMonsterObject->mStateParamter.stateCur ||
				eMA_ATTACK == teamMonsterObject->mStateParamter.stateCur)
			{
				continue;
			}

			teamMonsterObject->SetTObject(
				monsterObject->GetTObject());
			GSTATEMACHINE.SetState(
				teamMonsterObject,
				eMA_PERSUIT);
		}

		return 1;
	}
}