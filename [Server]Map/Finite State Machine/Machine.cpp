#include "StdAfx.h"
#include "Machine.h"
#include "Memory.h"
#include "Parser.h"
#include "State.h"
#include "Action.h"
#include "..\UserTable.h"
#include "..\Object.h"

namespace FiniteStateMachine
{
	const std::auto_ptr< CParser > CMachine::mParser(new CParser);

	CMachine::CMachine() :
	mMemory(new CMemory),
	mObjectIndex(0),
	mCheckedTick(0)
	{
		ZeroMemory(
			mLastRunState,
			sizeof(mLastRunState));
	}

	CMachine::~CMachine()
	{}

	BOOL CMachine::Initialize(LPCTSTR fileName, DWORD objectIndex, DWORD channelIndex)
	{
		const DWORD beginStateIndex = mParser.get()->Load(
			fileName,
			channelIndex);

		if(0 == beginStateIndex)
		{
			return FALSE;
		}

		mObjectIndex = objectIndex;
		mCheckedTick = 0;
		
		mAlias.clear();
		mStateContainer.push(
			beginStateIndex);
		mFileName = fileName;
		GetMemory().Initialize();
		return TRUE;
	}

	void CMachine::Run()
	{
		if(mCheckedTick > gCurTime)
		{
			return;
		}

		const DWORD nextCheckedTick = 500;
		mCheckedTick = gCurTime + nextCheckedTick;

		if(mStateContainer.empty())
		{
			mCheckedTick = UINT_MAX;
			return;
		}

		GetMemory().Clear();
		mLogContainer.clear();
		
		StateIndex stateIndex = mStateContainer.top();
		const StateIndex beginStateIndex = stateIndex;
		size_t stateCount = 0;
		StateLog runState[MaxStateCount];
		TCHAR textDebug[MaxStateCount][MAX_PATH] = {0};

		do
		{
			GetMemory().SetCurrentState(
				stateIndex);

			_stprintf(
				textDebug[stateCount],
				"%s",
				GetParser().GetText(stateIndex));

			const CState& state = GetParser().GetState(
				stateIndex);
			state.Run(
				*this);

			runState[stateCount] = StateLog(
				stateIndex,
				mLogContainer[stateIndex].size());
			stateIndex = GetMemory().GetNextState();

			if(beginStateIndex == stateIndex)
			{
				CObject* const object = g_pUserTable->FindUser(
					GetObjectIndex());

				if(0 == object)
				{
					return;
				}

				// 지속적으로 부하를 일으키므로 종료시킨다
				mStateContainer.empty();

				OutputDebug(
					"%s(%u)%s ...... %s",
					object->GetObjectName(),
					GetObjectIndex(),
					mAlias.empty() ? "": std::string(" alias: " + mAlias).c_str(),
					mFileName.c_str());
				OutputDebug(
					"error: state %s is recursive. Process is canceled.",
					GetParser().GetText(stateIndex));
				break;
			}
			else if(MaxStateCount < ++stateCount)
			{
				CObject* const object = g_pUserTable->FindUser(
					GetObjectIndex());

				if(0 == object)
				{
					return;
				}

				OutputDebug(
					"%s(%u)%s ...... %s",
					object->GetObjectName(),
					GetObjectIndex(),
					mAlias.empty() ? "": std::string(" alias: " + mAlias).c_str(),
					mFileName.c_str());
				OutputDebug(
					"error: It's unable over %d each a turn.",
					MaxStateCount);
				break;
			}
		}
		while(0 < stateIndex);

		if(memcmp(mLastRunState, runState, sizeof(runState)))
		{
			CObject* const object = g_pUserTable->FindUser(
				GetObjectIndex());

			if(0 == object)
			{
				return;
			}

			OutputDebug(
				"");
			OutputDebug(
				"%s(%u)%s ...... %s",
				object->GetObjectName(),
				GetObjectIndex(),
				mAlias.empty() ? "": std::string(" alias: " + mAlias).c_str(),
				mFileName.c_str());
			memcpy(
				mLastRunState,
				runState,
				sizeof(runState));

			for(size_t i = 0; i < stateCount; ++i)
			{
				OutputDebug(
					"\t%02u %s",
					i + 1,
					textDebug[i] + _tcslen(mFileName.c_str()));

				const StateLog& stateLog = runState[i];
				const StateIndex stateIndex = stateLog.first;
				const ActionContainer& actionContainer = mLogContainer[stateIndex];

				for(ActionContainer::const_iterator iterator = actionContainer.begin();
					actionContainer.end() != iterator;
					++iterator)
				{
					const ActionIndex actionIndex = *iterator;
					const CAction& action = GetParser().GetAction(
						actionIndex);

					OutputDebug(
						"\t\t%s",
						action.GetName());
				}
			}
		}
	}

	void CMachine::Clear()
	{
		while(false == mStateContainer.empty())
		{
			mStateContainer.pop();
		}
	}

	void CMachine::SetMainState(StateIndex index)
	{
		while(false == mStateContainer.empty())
		{
			mStateContainer.pop();
		}

		mStateContainer.push(
			index);
	}

	DWORD CMachine::GetObjectIndex(LPCTSTR alias) const
	{
		LPCTSTR keyword = "_this_";

		if(0 == _tcsnicmp(keyword, alias, _tcslen(keyword)))
		{
			return GetObjectIndex();
		}

		return g_pUserTable->GetObjectIndex(
			alias);
	}

	void CMachine::PopState()
	{
		if(false == mStateContainer.empty())
		{
			mStateContainer.pop();
		}
	}

	void CMachine::PushState(StateIndex index)
	{
		mStateContainer.push(
			index);
	}

	void CMachine::Log(StateIndex stateIndex, ActionIndex actionIndex)
	{
		ActionContainer& actionContainer = mLogContainer[stateIndex];
		actionContainer.push_back(
			actionIndex);
	}
}