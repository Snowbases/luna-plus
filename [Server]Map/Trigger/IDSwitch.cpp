#include "StdAfx.h"
#include "IDSwitch.h"

namespace Trigger
{
	CIDSwitch::CIDSwitch(void)
	{
		ResetSwitch();
	}

	CIDSwitch::~CIDSwitch(void)
	{
	}

	void CIDSwitch::SetSwitch(WORD num, BOOL val)
	{
		if(num<0 || MAX_TRIGGER_SWITCH<num)
			return;

		mSwitch[num] = val;
	}

	int CIDSwitch::GetSwitch(WORD num)
	{
		if(num<0 || MAX_TRIGGER_SWITCH<num)
			return 0;

		return mSwitch[num];
	}

	void CIDSwitch::ResetSwitch()
	{
		ZeroMemory(mSwitch, sizeof(mSwitch));
	}
}