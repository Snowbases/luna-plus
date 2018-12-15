#include "StdAfx.h"
#include "ActionTalk.h"
#include "Machine.h"
#include "Parser.h"
#include "..\Object.h"
#include "..\UserTable.h"
#include "..\PackedData.h"

namespace FiniteStateMachine
{
	namespace
	{
		CAction* Clone(CParser::ParameterContainer& parameterContainer)
		{
			LPCTSTR parameter1 = parameterContainer["parameter1"].c_str();

			return new CActionTalk(
				_ttoi(parameter1));
		}

		const BOOL isRegistered = CMachine::GetParser().Register(
			"Talk",
			Clone);
	}

	CActionTalk::CActionTalk(DWORD speechIndex) :
	CAction("Talk"),
	mSpeechIndex(speechIndex)
	{}

	int CActionTalk::Run(CMachine& machine) const
	{
		CObject* const object = g_pUserTable->FindUser(
			machine.GetObjectIndex());

		if(0 == object)
		{
			return 0;
		}

		MSG_DWORD4 message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_CHAT;
		message.Protocol = MP_CHAT_MONSTERSPEECH;
		message.dwData2 = mSpeechIndex;
		message.dwObjectID = machine.GetObjectIndex();
		PACKEDDATA_OBJ->QuickSend(
			object,
			&message,
			sizeof(message));

		return 1;
	}
}