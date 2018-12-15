#include "StdAfx.h"
#include <sys/stat.h>
#include "Parser.h"
#include "State.h"
#include "Action.h"
#include "..\..\[CC]ServerModule\MHFile.h"

namespace FiniteStateMachine
{
	CParser::CParser()
	{}

	CParser::~CParser()
	{
		while(false == mStateContainer.empty())
		{
			StateContainer::iterator iterator = mStateContainer.begin();
			SAFE_DELETE(
				iterator->second);
			mStateContainer.erase(
				iterator);
		}

		while(false == mActionContainer.empty())
		{
			ActionContainer::iterator iterator = mActionContainer.begin();
			SAFE_DELETE(
				iterator->second);
			mActionContainer.erase(
				iterator);
		}
	}

	BOOL CParser::Register(LPCTSTR keyword, CParser::CloneFunction function)
	{
		TCHAR text[MAX_PATH] = {0};
		SafeStrCpy(
			text,
			keyword,
			_countof(text));
		_tcslwr(text);

		if(mCloneFunctionContainer.end() != mCloneFunctionContainer.find(text))
		{
			OutputDebug(
				"There is same keyword already. It is %s",
				text);
			return FALSE;
		}

		mCloneFunctionContainer.insert(
			std::make_pair(_tcslwr(text), function));
		return TRUE;
	}

	const CState& CParser::GetState(StateIndex index) const
	{
		static const CState emptyState(UINT_MAX, "emptyState");
		const StateContainer::const_iterator iterator = mStateContainer.find(
			index);

		return mStateContainer.end() == iterator ? emptyState : *(iterator->second);
	}

	const CAction& CParser::GetAction(ActionIndex index) const
	{
		static const CAction emptyAction;
		const ActionContainer::const_iterator iterator = mActionContainer.find(
			index);

		return mActionContainer.end() == iterator ? emptyAction : *(iterator->second);
	}

	void CParser::Release(LPCTSTR fileName)
	{
		for(FileStateContainer::iterator fileIterator = mFileStateContainer.find(fileName);
			mFileStateContainer.end() != fileIterator;
			fileIterator = mFileStateContainer.find(fileName))
		{
			const StateIndex stateIndex = fileIterator->second;
			StateContainer::iterator stateIterator = mStateContainer.find(
				stateIndex);

			if(mStateContainer.end() != stateIterator)
			{
				CState* state = stateIterator->second;
				const CState::ActionContainer& actionContainer = state->GetActionContainer();

				for(CState::ActionContainer::const_iterator actionIterator = actionContainer.begin();
					actionContainer.end() != actionIterator;
					++actionIterator)
				{
					RemoveAction(
						*actionIterator);
				}

				SAFE_DELETE(
					state);
				mStateContainer.erase(
					stateIterator);
			}

			mFileStateContainer.erase(
				fileIterator);
		}
	}

	LPCTSTR	CParser::GetText(DWORD hashCode) const
	{
		const CodeTextContainer::const_iterator iterator = mCodeTextContainer.find(
			hashCode);

		return mCodeTextContainer.end() == iterator ? "" : iterator->second.c_str();
	}

	DWORD CParser::GetHashCode(LPCTSTR text)
	{
		const TextCodeContainer::const_iterator iterator = mTextCodeContainer.find(
			text);

		if(mTextCodeContainer.end() != iterator)
		{
			return iterator->second;
		}

		const DWORD hashCode = GetHashCodeFromTxt(text);

		mCodeTextContainer.insert(
			std::make_pair(hashCode, text));
		mTextCodeContainer.insert(
			std::make_pair(text, hashCode));

		return hashCode;
	}

	BOOL CParser::IsParsedFile(LPCTSTR fileName) const
	{
		struct _tstat fileStatus = {0};

		if(_tstat(fileName, &fileStatus))
		{
			return FALSE;
		}

		const FileStampContainer::const_iterator iterator = mFileStampContainer.find(
			fileName);

		if(mFileStampContainer.end() == iterator)
		{
			return FALSE;
		}

		const FileStamp& fileStamp = iterator->second;

		if(memcmp(&fileStatus, &fileStamp.mStat, sizeof(fileStatus)))
		{
			return FALSE;
		}

		return TRUE;
	}

	void CParser::SetParsedFile(LPCTSTR fileName, StateIndex beginStateIndex)
	{
		FileStamp& fileStamp = mFileStampContainer[fileName];

		_tstat(fileName, &fileStamp.mStat);
		fileStamp.mBeginStateIndex = beginStateIndex;
	}

	DWORD CParser::Load(LPCTSTR fileName, DWORD channelIndex)
	{
		TCHAR filePath[MAX_PATH] = {0};
		_stprintf(
			filePath,
			"system\\resource\\AIScript\\%s",
			fileName);

		// 같은 파일을 두번 파싱하지 않도록 한다
		if(IsParsedFile(filePath))
		{
			if(mFileStampContainer.end() == mFileStampContainer.find(filePath))
			{
				return 0;
			}

			return mFileStampContainer[filePath].mBeginStateIndex;
		}

		Release(
			fileName);

		enum Block
		{
			BlockNone,
			BlockHeader,
			BlockBody,
			BlockSwitch,
		}
		block = BlockNone;

		BOOL isComment = FALSE;
		DWORD beginStateIndex = 0;

		CMHFile file;
		file.Init(
			filePath,
			"rb");
		std::string textSwitchValue;
		CState* currentState = 0;

		while(FALSE == file.IsEOF())
		{
			TCHAR text[MAX_PATH] = { 0 };
			file.GetLine(
				text,
				_countof(text));
			TCHAR buffer[MAX_PATH] = {0};
			SafeStrCpy(
				buffer,
				text,
				_countof(buffer));
			
			const int length = _tcslen(
				text);

			// 주석 제거
			for(int i = 0; i < length - 1; ++i)
			{
				char& first = text[i];
				char& second = text[i+1];

				if('/' == first && '/' == second)
				{
					first = 0;
					break;
				}
				else if('/' == first && '*' == second)
				{
					first = 0;
					isComment = TRUE;
					break;
				}
				else if('*' == first && '/' == second)
				{
					first = ' ';
					second = ' ';
					isComment = FALSE;
					break;
				}
			}

			if(isComment)
			{
				continue;
			}

			PCTSTR textSeparator = ",\t\n ()";
			LPCTSTR token = _tcstok(
				text,
				textSeparator);
			
			if(0 == token)
			{
				continue;
			}
			else if(0 == _tcsnicmp(token, "state", _tcslen("state")))
			{
				block = BlockHeader;

				LPCTSTR keyword = token;
				LPCTSTR textName = _tcstok(
					0,
					textSeparator);
				textName = (textName ? textName : "");
				_tcstok(
					0,
					textSeparator);
				LPCTSTR textMain = _tcstok(
					0,
					textSeparator);
				textMain = (textMain ? textMain : "");

				if(0 != _tcsicmp(keyword, "state"))
				{
					break;
				}

				CState* const state = AddState(
					fileName,
					textName);
				
				if(0 == state)
				{
					break;
				}

				currentState = state;
				const DWORD hashCode = GetHashCode(
					state->GetName());
				
				if(0 == _tcsicmp(textMain, "main"))
				{
					beginStateIndex = hashCode;
				}

				continue;
			}
			else if(0 == _tcsnicmp(token, "switch", _tcslen("switch")))
			{
				block = BlockSwitch;

				LPCTSTR actionName = _tcstok(
					0,
					textSeparator);
				LPCTSTR parameter1 = _tcstok(
					0,
					textSeparator);
				LPCTSTR parameter2 = _tcstok(
					0,
					textSeparator);
				LPCTSTR parameter3 = _tcstok(
					0,
					textSeparator);
				LPCTSTR parameter4 = _tcstok(
					0,
					textSeparator);

				const ActionIndex actionIndex = AddAction(
					actionName,
					fileName,
					channelIndex,
					*currentState,
					parameter1,
					parameter2,
					parameter3,
					parameter4);

				// 함수가 있을 경우 자동으로 변수를 생성한다
				if(actionIndex)
				{
					TCHAR variableName[MAX_PATH] = {0};
					_stprintf(
						variableName,
						"__%s.%u__",
						actionName,
						actionIndex);

					AddAction(
						"SetValue",
						fileName,
						channelIndex,
						*currentState,
						variableName);

					textSwitchValue = variableName;
				}
				else
				{
					textSwitchValue = actionName;
				}
				
				continue;
			}
			else if(0 == _tcsnicmp(token, "[", _tcslen("[")))
			{
				switch(block)
				{
				case BlockNone:
					{
						block = BlockHeader;
						break;
					}
				case BlockHeader:
					{
						block = BlockBody;
						break;
					}
				}

				continue;
			}
			else if(0 == _tcsnicmp(token, "]", _tcslen("]")))
			{
				switch(block)
				{
				case BlockHeader:
					{
						block = BlockNone;
						break;
					}
				case BlockBody:
					{
						block = BlockHeader;
						break;
					}
				case BlockSwitch:
					{
						block = BlockBody;
						break;
					}
				}

				continue;
			}

			switch(block)
			{
			case BlockBody:
				{
					LPCTSTR variableName = token;
					LPCTSTR assignmentOperatpor = _tcstok(
						0,
						textSeparator);
					LPCTSTR actionName = _tcstok(
						0,
						textSeparator);
					LPCTSTR parameter1 = _tcstok(
						0,
						textSeparator);
					LPCTSTR parameter2 = _tcstok(
						0,
						textSeparator);
					LPCTSTR parameter3 = _tcstok(
						0,
						textSeparator);
					LPCTSTR parameter4 = _tcstok(
						0,
						textSeparator);
					
					if(0 == assignmentOperatpor ||
						_tcsicmp("=", assignmentOperatpor))
					{
						parameter3 = parameter1;
						parameter4 = parameter2;
						parameter1 = assignmentOperatpor;
						parameter2 = actionName;
						actionName = variableName;

						AddAction(
							actionName,
							fileName,
							channelIndex, 
							*currentState,
							parameter1,
							parameter2,
							parameter3,
							parameter4);
					}
					else
					{
						AddAction(
							actionName,
							fileName,
							channelIndex,
							*currentState,
							parameter1,
							parameter2,
							parameter3,
							parameter4);
						AddAction(
							"SetValue",
							fileName,
							channelIndex,
							*currentState,
							variableName);
					}

					break;
				}
			case BlockSwitch:
				{
					LPCTSTR operation = token;
					LPCTSTR comparisonOperator = _tcstok(
						0,
						textSeparator);
					LPCTSTR command = _tcstok(
						0,
						textSeparator);
					LPCTSTR actionName = _tcstok(
						0,
						textSeparator);
					LPCTSTR executionRate = _tcstok(
						0,
						textSeparator);

					const ActionIndex actionIndex = AddAction(
						command,
						fileName,
						channelIndex,
						*currentState,
						actionName,
						executionRate);
					TCHAR textActionIndex[MAX_PATH] = {0};
					_stprintf(
						textActionIndex,
						"%u",
						actionIndex);

					// switch 액션이 실행해서 참이 될 경우만 실행되므로 스테이트에서 제거한다
					currentState->Remove(
						actionIndex);

					AddAction(
						"switch",
						fileName,
						channelIndex,
						*currentState,
						operation,
						textActionIndex,
						textSwitchValue.c_str(),
						comparisonOperator);
					break;
				}
			}
		}

		SetParsedFile(
			filePath,
			beginStateIndex);

		return beginStateIndex;
	}

	LPCTSTR CParser::GetStateName(LPCTSTR fileName, LPCTSTR stateName) const
	{
		static TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"%s %s",
			fileName,
			stateName);

		return text;
	}

	LPCTSTR CParser::GetAlias(LPCTSTR alias, DWORD channelIndex) const
	{
		static TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			"%s(%u)",
			alias,
			channelIndex);

		return text;
	}

	CState* CParser::AddState(LPCTSTR fileName, LPCTSTR stateName)
	{
		LPCTSTR name = GetStateName(
			fileName,
			stateName);
		const DWORD hashCode = GetHashCode(
			name);

		CState* const state = new CState(
			hashCode,
			name);

		if(state)
		{
			mFileStateContainer.insert(
				std::make_pair(fileName, hashCode));
			mStateContainer.insert(
				std::make_pair(hashCode, state));
		}

		return state;
	}

	void CParser::RemoveAction(ActionIndex actionIndex)
	{
		const ActionContainer::iterator iterator = mActionContainer.find(
			actionIndex);

		if(mActionContainer.end() == iterator)
		{
			return;
		}

		SAFE_DELETE(
			iterator->second);
		mActionContainer.erase(
			iterator);
	}

	CParser::ActionIndex CParser::AddAction(LPCTSTR actionName, LPCTSTR fileName, DWORD channelIndex, CState& state, LPCTSTR parameter1, LPCTSTR parameter2, LPCTSTR parameter3, LPCTSTR parameter4)
	{
		TCHAR keyword[MAX_PATH] = {0};
		SafeStrCpy(
			keyword,
			actionName,
			_countof(keyword));

		const CloneFunctionContainer::const_iterator iterator = mCloneFunctionContainer.find(
			_tcslwr(keyword));

        if(mCloneFunctionContainer.end() == iterator)
		{
			return 0;
		}

		TCHAR textChannel[MAX_PATH] = {0};

		ParameterContainer parameterContainer;
		parameterContainer["channel"] = _itot(
			channelIndex,
			textChannel,
			10);
		parameterContainer["fileName"] = fileName;
		parameterContainer["parameter1"] = (parameter1 ? parameter1 : "");
		parameterContainer["parameter2"] = (parameter2 ? parameter2 : "");
		parameterContainer["parameter3"] = (parameter3 ? parameter3 : "");
		parameterContainer["parameter4"] = (parameter4 ? parameter4 : "");

		const CloneFunction cloneFunction = iterator->second;
		CAction* const action = (*cloneFunction)(parameterContainer);

		if(0 == action)
		{
			return 0;
		}

		static ActionIndex uniqueIndex = 0;

		mActionContainer.insert(
			std::make_pair(++uniqueIndex, action));
		state.Add(
			uniqueIndex);

		return uniqueIndex;
	}
}