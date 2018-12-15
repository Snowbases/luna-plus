#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionDungeonSwitchOn :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonSwitchOn;
		}
	};

	class CActionDungeonSwitchOff :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonSwitchOff;
		}
	};

	class CActionDungeonSwitchToggle :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonSwitchToggle;
		}
	};

	class CActionDungeonWarpOn :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonWarpOn;
		}
	};

	class CActionDungeonWarpOff :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonWarpOff;
		}
	};

	class CActionDungeonSetBossMonster :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonSetBossMonster;
		}
	};

	class CActionDungeonAddPoint :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonAddPoint;
		}
	};

	class CActionDungeonSummonMonsterGroup :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonSummonMonsterGroup;
		}
	};

	class CActionDungeonSwitchNpcOn :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonSwitchNpcOn;
		}
	};

	class CActionDungeonSwitchNpcOff :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonSwitchNpcOff;
		}
	};

	class CActionDungeonZoneBlock :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonZoneBlock;
		}
	};

	class CActionDungeonZoneFree :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonZoneFree;
		}
	};

	class CActionDungeonEdgeBlock :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonEdgeBlock;
		}
	};

	class CActionDungeonEdgeFree :
		public CAction
	{
		public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionDungeonEdgeFree;
		}
	};
}
