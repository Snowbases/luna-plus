#pragma once

namespace FiniteStateMachine
{
	class CState;
	class CAction;

	class CParser
	{
	private:
		typedef std::string StateName;
		typedef DWORD StateIndex;
		typedef stdext::hash_map< StateIndex, CState* > StateContainer;
		StateContainer mStateContainer;

		typedef DWORD ActionIndex;
		typedef stdext::hash_map< ActionIndex, CAction* > ActionContainer;
		ActionContainer mActionContainer;

		typedef std::string FileName;
		typedef std::multimap< FileName, StateIndex > FileStateContainer;
		FileStateContainer mFileStateContainer;

		typedef std::map< std::string, DWORD > TextCodeContainer;
		TextCodeContainer mTextCodeContainer;
		typedef std::map< DWORD, std::string > CodeTextContainer;
		CodeTextContainer mCodeTextContainer;

		typedef std::string ParameterKey;
		typedef std::string ParameterValue;
		typedef std::map< ParameterKey, ParameterValue > _ParameterContainer;
		typedef CAction* (*_CloneFunction)(_ParameterContainer&);
		typedef std::string Keyword;
		typedef std::map< Keyword, _CloneFunction > CloneFunctionContainer;
		CloneFunctionContainer mCloneFunctionContainer;

		struct FileStamp
		{
			struct _tstat mStat;
			DWORD mBeginStateIndex;

			FileStamp()
			{
				ZeroMemory(
					this,
					sizeof(*this));
			}
		};
		typedef std::map< FileName, FileStamp > FileStampContainer;
		FileStampContainer mFileStampContainer;

	public:
		CParser();
		virtual ~CParser();
		DWORD Load(LPCTSTR fileName, DWORD channelIndex);
		const CState& GetState(StateIndex) const;
		const CAction& GetAction(ActionIndex) const;
		LPCTSTR GetText(DWORD hashCode) const;
		LPCTSTR	GetAlias(LPCTSTR alias, DWORD channelIndex) const;

		typedef _ParameterContainer ParameterContainer;
		typedef _CloneFunction CloneFunction;
		BOOL Register(LPCTSTR keyword, CloneFunction);
		LPCTSTR GetStateName(LPCTSTR fileName, LPCTSTR stateName) const;
		DWORD GetHashCode(LPCTSTR);

	private:
		CState* AddState(LPCTSTR fileName, LPCTSTR stateName);
		void Release();
		void Release(LPCTSTR);
		ActionIndex AddAction(LPCTSTR actionName, LPCTSTR fileName, DWORD channelIndex, CState&, LPCTSTR parameter1 = "", LPCTSTR parameter2 = "", LPCTSTR parameter3 = "", LPCTSTR parameter4 = "");
		void RemoveAction(ActionIndex);
		BOOL IsParsedFile(LPCTSTR fileName) const;
		void SetParsedFile(LPCTSTR fileName, StateIndex beginStateIndex);
	};
}