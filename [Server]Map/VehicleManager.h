/*
090316 LUJ, 탈것. 아이템으로 소환된다
*/
#pragma once

#define VEHICLEMGR ( &CVehicleManager::GetInstance() )

class CVehicle;
class CPlayer;

class CVehicleManager
{
public:
	struct Option
	{
		BOOL mIsNeedAllow;
		MONEYTYPE mChargedMoney;
		DWORD mKey;
	};
private:
	TCHAR mFileName[MAX_PATH];
	typedef DWORD PlayerIndex;
	typedef stdext::hash_map< PlayerIndex, Option > OptionContainer;
	OptionContainer mOptionContainer;
	
	class SortByLocation
	{
	public:
		bool operator()(const VECTOR3& lhs, const VECTOR3& rhs) const
		{
			LONGLONG leftValue = 0;
			memcpy(
				&leftValue,
				&lhs.x,
				sizeof(lhs.x));
			memcpy(
				&leftValue + sizeof(lhs.z),
				&lhs.z,
				sizeof(lhs.z));

			LONGLONG rightValue = 0;
			memcpy(
				&rightValue,
				&rhs.x,
				sizeof(rhs.x));
			memcpy(
				&rightValue + sizeof(rhs.z),
				&rhs.z,
				sizeof(rhs.z));

			return leftValue < rightValue;
		}
	};

	typedef DWORD ObjectIndex;
	typedef std::set< ObjectIndex > ObjectIndexContainer;
	typedef std::map< VECTOR3, ObjectIndexContainer, SortByLocation > LocationContainer;
	LocationContainer mLocationContainer;

public:
	static CVehicleManager& GetInstance();
	CVehicleManager(void);
	virtual ~CVehicleManager(void);
	void NetworkMsgParse( const DWORD connectionIndex, const MSGBASE* const, const DWORD receivedMessageSize );
	void Summon(const DWORD playerIndex, const ICONBASE&, VECTOR3& vehiclePosition, const BOOL summonByPlayer ) const;
	void Summon(DWORD monsterKind, VECTOR3& position, DWORD playerIndex);
	void Unsummon(const DWORD playerIndex, const BOOL unsummonByPlayer) const;
	void Unsummon(const VECTOR3&);
	BOOL Teleport( const DWORD playerIndex, const MAPTYPE ) const;
	BOOL Teleport( const DWORD playerIndex, const MAPCHANGE_INFO* const mapChangeInfo ) const;
	void MountAllow( const DWORD playerIndex, const DWORD vehicleIndex, const DWORD seatIndex, const BOOL needBuff ) const;
	BOOL Dismount( const DWORD playerIndex, const BOOL dismountByPlayer ) const;
	BOOL IsInvalidState( const EObjectState ) const;
	BOOL IsInvalidShowdown( CPlayer& );
	BOOL Recall( const DWORD playerIndex );
	void SetOption(DWORD playerIndex, BOOL isNeedAllow, MONEYTYPE);
	void Save() const;
	void Load();
	void GetSummonedVehicle(DWORD playerIndex) const;
	
private:
	MSG_VEHICLE_ERROR::Error MountToll(DWORD askedPlayerIndex, DWORD vehicleIndex, DWORD key) const;
	void MountAsk(DWORD askedPlayerIndex, DWORD vehicleIndex, DWORD key);
	MSG_VEHICLE_ERROR::Error MountCheck( CPlayer& ownedPlayer, CPlayer& askedPlayer, CVehicle& ) const;
	CPlayer* GetPlayer( DWORD playerIndex ) const;
	CVehicle* GetVehicle( DWORD vehicleIndex ) const;
	BOOL IsInvalidDistance( CObject&, CVehicle& ) const;
	void SendErrorMessage(CObject&, MSG_VEHICLE_ERROR::Error, DWORD dwData=0) const;
	void SendErrorMessage(DWORD objectIndex, MSG_VEHICLE_ERROR::Error, DWORD dwData=0) const;
	const Option& GetOption(DWORD playerIndex) const;
};