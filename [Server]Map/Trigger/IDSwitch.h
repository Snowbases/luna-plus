#pragma once
#include "common.h"

namespace Trigger
{
	class CIDSwitch
	{
	public:
		CIDSwitch(void);
		virtual ~CIDSwitch(void);

	private:
		int mSwitch[MAX_TRIGGER_SWITCH];

	public:
		void SetSwitch(WORD num, BOOL val);
		int GetSwitch(WORD num);

		void ResetSwitch();
	};
}
