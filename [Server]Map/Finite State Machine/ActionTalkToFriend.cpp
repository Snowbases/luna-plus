#include "StdAfx.h"
#include "ActionTalkToFriend.h"
#include "ActionTalk.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "..\UserTable.h"
#include "..\Monster.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			return new CActionTalkToFriend(
				_ttoi(parameter1));
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"TalkToFriend",
			Clone);
	}

	CActionTalkToFriend::CActionTalkToFriend(DWORD speechIndex) :
	CAction("TalkToFriend"),
	mSpeechIndex(speechIndex)
	{}

	int CActionTalkToFriend::Run(CMachine& machine) const
	{
		CActionTalk action(
			mSpeechIndex);
		action.Run(
			machine);

		const CMemory::TextContainer& nameContainer = machine.GetMemory().GetFriend();

		for(CMemory::TextContainer::const_iterator iterator = nameContainer.begin();
			nameContainer.end() != iterator;
			++iterator)
		{
			const std::string& name = *iterator;
			const DWORD objectIndex = g_pUserTable->GetObjectIndex(
				name.c_str());
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