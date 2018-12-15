#pragma once

namespace FiniteStateMachine
{
	class CMachine;

	class CAction
	{
	private:
		LPCTSTR mName;

	public:
		CAction() : mName(__FUNCTION__) {}
		CAction(LPCTSTR name) : mName(name) {}
		LPCTSTR GetName() const { return mName; }
		virtual ~CAction() {}
		virtual int Run(CMachine&) const { return 0; }
	};
}