#pragma once

#include "Action.h"

namespace FiniteStateMachine
{
	class CActionSwitch : public CAction
	{
	public:
		typedef BOOL (*IsValid)(int, int);
		static BOOL IsLess(int lhs, int rhs) { return lhs > rhs; }
		static BOOL IsMore(int lhs, int rhs) { return lhs < rhs; }
		static BOOL IsEqual(int lhs, int rhs) { return lhs == rhs; }
		static BOOL IsLessEqual(int lhs, int rhs) { return lhs >= rhs; }
		static BOOL IsMoreEqual(int lhs, int rhs) { return lhs <= rhs; }
		static BOOL IsNoEqual(int lhs, int rhs) { return lhs != rhs; }
		static BOOL IsNone(int, int) { return true; }

	private:
		const IsValid mIsValid;
		const int mOperation;
		const DWORD mActionIndex;
		const std::string mVaraible;

	public:
		CActionSwitch(IsValid, int operation, DWORD actionIndex, LPCTSTR variable);
		virtual ~CActionSwitch() {}
		virtual int Run(CMachine&) const;
	};
}