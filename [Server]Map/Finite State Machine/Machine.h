#pragma once

namespace FiniteStateMachine
{
	class CMemory;
	class CParser;

	class CMachine
	{
	private:
		enum { MaxStateCount = 10 };
		typedef DWORD StateIndex;
		typedef std::pair< StateIndex, size_t > StateLog;
		StateLog mLastRunState[MaxStateCount];
		std::string mFileName;
		std::string mAlias;
		DWORD mCheckedTick;
		DWORD mObjectIndex;
		typedef std::stack< StateIndex > StateContainer;
		StateContainer mStateContainer;
		const std::auto_ptr< CMemory > mMemory;
		static const std::auto_ptr< CParser > mParser;
		typedef DWORD ActionIndex;
		typedef std::list< ActionIndex > ActionContainer;
		typedef std::map< StateIndex, ActionContainer > LogContainer;
		LogContainer mLogContainer;
 
	public:
		CMachine();
		virtual ~CMachine();
		void Run();
		void PushState(StateIndex);
		void PopState();
		void SetMainState(StateIndex);
		void Clear();
		BOOL Initialize(LPCTSTR fileName, DWORD objectIndex, DWORD channelIndex);
		DWORD GetObjectIndex() const { return mObjectIndex; }
		CMemory& GetMemory() { return *mMemory.get(); }
		static CParser& GetParser() { return *mParser.get(); }
		DWORD GetObjectIndex(LPCTSTR) const;
		void SetAlias(LPCTSTR alias) { mAlias = alias; }
		void Log(StateIndex, ActionIndex);
	};
}