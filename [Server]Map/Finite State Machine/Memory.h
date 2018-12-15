#pragma once

namespace FiniteStateMachine
{
	class CMemory
	{
	private:
		typedef int Value;
		DWORD mNextState;
		DWORD mCurrnetState;
		BOOL mIsExitState;
		Value mResult;
		DWORD mFoundObjectIndex;
		std::string mTeamName;
		
		typedef std::map< std::string, Value > VariableContainer;
		VariableContainer mVariableContainer;

	public:
		typedef std::set< std::string > TextContainer;
		typedef std::set< DWORD > IndexContainer;

	private:
		TextContainer mFriendContainer;
		IndexContainer mHeardSpeechContainer;
		IndexContainer mFoundObjectContainer;

		typedef std::map< UNITKIND, float > AggroRateBySkillKindContainer;
		AggroRateBySkillKindContainer mAggroRateBySkillKindContainer;
		typedef std::map< eStatusKind, float > AggroRateBySkillStatusContainer;
		AggroRateBySkillStatusContainer mAggroRateBySkillStatusContainer;

	public:
		CMemory();
		virtual ~CMemory();
		void Initialize();
		void Clear();
		void SetResult(Value value) { mResult = value; }
		Value GetResult() const { return mResult; }
		void SetFoundObject(DWORD index) { mFoundObjectIndex = index; }
		DWORD GetFoundObject() const { return mFoundObjectIndex; }
		void SetVariable(LPCTSTR, Value);
		Value GetVariable(LPCTSTR) const;
		void SetNextState(DWORD state) { mNextState = state; }
		DWORD GetNextState() const { return mNextState; }
		void SetCurrentState(DWORD state) { mCurrnetState = state; }
		DWORD GetCurrentState() const { return mCurrnetState; }
		BOOL IsExitState() const { return mIsExitState; }
		void SetExitState(BOOL isExit) { mIsExitState = isExit; }
		TextContainer& GetFriend() { return mFriendContainer; }
		void AddHeardSpeech(DWORD speechIndex) { mHeardSpeechContainer.insert(speechIndex); }
		void RemoveHeardSpeech(DWORD speechIndex) { mHeardSpeechContainer.erase(speechIndex); }
		BOOL IsHeardSpeech(DWORD speechIndex) const { return mHeardSpeechContainer.end() != mHeardSpeechContainer.find(speechIndex); }
		void AddFoundObject(DWORD objectIndex) { mFoundObjectContainer.insert(objectIndex); }
		IndexContainer& GetFoundObject() { return mFoundObjectContainer; }
		void SetTeam(LPCTSTR name) { mTeamName = name; }
		LPCTSTR GetTeam() const { return mTeamName.c_str(); }
		float GetAggroRate(UNITKIND) const;
		float GetAggroRate(eStatusKind) const;
		void AddAggroRate(UNITKIND kind, float rate) { mAggroRateBySkillKindContainer[kind] = rate; }
		void AddAggroRate(eStatusKind kind, float rate) { mAggroRateBySkillStatusContainer[kind] = rate; }
	};
}