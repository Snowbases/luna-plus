#pragma once

namespace FiniteStateMachine
{
	class CMachine;
	class CAction;

	class CState
	{
	private:
		const std::string mName;
		const DWORD mIndex;
		typedef DWORD ActionIndex;

	public:
		typedef std::set< ActionIndex > ActionContainer;
	private:
		ActionContainer mActionContainer;

	public:
		CState(DWORD index, LPCTSTR name);
		virtual ~CState();
		virtual void Run(CMachine&) const;
		void Add(ActionIndex index) { mActionContainer.insert(index); }
		void Remove(ActionIndex index) { mActionContainer.erase(index); }
		LPCTSTR GetName() const { return mName.c_str(); }
		const ActionContainer& GetActionContainer() const { return mActionContainer; }
	};
}