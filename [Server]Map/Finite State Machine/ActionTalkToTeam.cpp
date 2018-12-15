#include "StdAfx.h"
#include "ActionTalkToTeam.h"
#include "ActionTalk.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\Monster.h"
#include "..\UserTable.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			const DWORD speechIndex = _ttoi(
				parameter1);

			return new CActionTalkToTeam(
				speechIndex);
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"TalkToTeam",
			Clone);
	}

	CActionTalkToTeam::CActionTalkToTeam(DWORD speechIndex) :
	CAction("TalkToTeam"),
	mSpeechIndex(speechIndex)
	{}

	int CActionTalkToTeam::Run(CMachine& machine) const
	{
		CActionTalk action(
			mSpeechIndex);
		action.Run(
			machine);

		const CUserTable::Team& team = g_pUserTable->GetTeam(
			machine.GetMemory().GetTeam());

		for(CUserTable::Team::ObjectIndexContainer::const_iterator iterator = team.mMemberContainer.begin();
			team.mMemberContainer.end() != iterator;
			++iterator)
		{
			const DWORD objectIndex = *iterator;

			if(machine.GetObjectIndex() == objectIndex)
			{
				continue;
			}

			CMonster* const monsterObject = (CMonster*)g_pUserTable->FindUser(
				objectIndex);

			if(0 == monsterObject)
			{
				continue;
			}
			else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
			{
				continue;
			}

			monsterObject->GetFiniteStateMachine().GetMemory().AddHeardSpeech(
				mSpeechIndex);
		}

		return 1;
	}
}