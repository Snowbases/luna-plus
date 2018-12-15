#pragma once

#define TRIGGERMGR (&Trigger::CManager::GetInstance())

class CEngineEffect;

namespace Trigger
{
	class CManager
	{
		struct stEffectInfo
		{
			DWORD dwObjectIndex;
			DWORD dwEffectIndex;
		};
		typedef stdext::hash_map< DWORD, stEffectInfo > EffectInfoContainer;
		EffectInfoContainer mEffectInfoContainer;
		typedef DWORD HashCode;
		typedef std::map< HashCode, CEngineEffect* > EngineEffectContainer;
		EngineEffectContainer mEngineEffectContainer;
		std::queue< HashCode > mEngineEffectQueue;
		typedef std::pair< float, float > TilePosition;
		typedef std::set< TilePosition > TilePositionSet;
		typedef std::map< HashCode, TilePositionSet > BlockContainer;
		BlockContainer mBlockContainer;
		struct Flag
		{
			DWORD mHashCode;
			DWORD mOwnerType;
			DWORD mOwnerIndex;
			DWORD mValueAccept;
			DWORD mValueReject;
		}
		mFlag;

	private:
		CManager(void);
		virtual ~CManager(void);
		void AddEngineEffect(LPCTSTR fileName, const VECTOR3&, int motionIndex, float angle, BOOL isRepeat);
		void RemoveEngineEffect();
		void SetCollisionTile(LPCTSTR, const RECT&, float angle, BOOL isCollide);
		void RestoreTile(HashCode);
		void PutMessageBox(DWORD messageIndex, DWORD waitSecond);

	public:
		static CManager& GetInstance();
		void Process();
		void NetworkMsgParse(BYTE Protocol,LPVOID);
		void AttachEffect(DWORD objectIndex, WORD effectIndex);
		void DetatchEffect(DWORD objectIndex, WORD effectIndex);
		void RegenEffect(DWORD objectIndex);
		void RestoreTile();
		void OnActionEvent(LONG, LPVOID, DWORD);
	};
}