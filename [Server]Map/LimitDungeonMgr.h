#pragma once
#include <queue>

#define LIMITDUNGEONMGR	CLimitDungeonManager::GetInstance()

class CPlayer;

class CLimitDungeonManager
{
private:
	// 091106 LUJ, 일정이 순차적으로 저장되어 있다
	struct Event
	{
		enum Type
		{
			TypeNone,
			TypeOpen,
			TypeOpenIlregulary,
			TypeClose,
			TypeCloseIlregulary,
		}
		mType;
		MAPTYPE mMapType;
		DWORD mChannelIndex;
		__time64_t mPlayTime;
	};
	typedef std::multimap< __time64_t, Event > ScheduleMap;
	ScheduleMap mScheduleMap;
	typedef DWORD ChannelIndex;
	// 091106 LUJ, 게임을 위해 열린 공간이 저장되어 있다
	typedef std::pair< MAPTYPE, ChannelIndex > RoomKey;
	struct Room
	{
		typedef stdext::hash_set< DWORD > IndexSet;
		IndexSet mPlayerIndexSet;
		__time64_t mCloseTime;
	};
	typedef std::map< RoomKey, Room > RoomMap;
	RoomMap mRoomMap;
	// 091106 LUJ, 몬스터 개수를 업데이트할 대상. 큐에 저장되어 순차적으로 처리된다
	typedef std::queue< DWORD > IndexQueue;
	IndexQueue mPlayerQueue;
	// 091106 LUJ, 현재 맵 번호
	MAPTYPE mMapType;
	// 091106 LUJ, 스크립트 파일 저장 일자
	struct stat mFileStatus;
	
public:
	CLimitDungeonManager(void);
	virtual ~CLimitDungeonManager(void);
	static CLimitDungeonManager* GetInstance();
	void NetworkMsgParser(MSGBASE*);
	void Process();
	void AddPlayer(CPlayer&);

private:
	void UpdateSchedule();
	void RefreshScript();
	BOOL IsReady() const;
	void PostponeSchedule();
	__time64_t GetSecondForOneWeek() const;
	void Run();
	void AskJoin(DWORD playerIndex, MAPTYPE, DWORD channelIndex, POSTYPE);
	void OpenRoom(const Event&);
	void CloseRoom(const Event&);
	BOOL IsOpen(DWORD channelIndex) const;
	Room& GetRoom(MAPTYPE, DWORD channelIndex);
	BOOL IsNoRoom(MAPTYPE, DWORD channelIndex);
	void UpdatePlayer();
	BOOL UseItem(const LimitDungeonScript&, CPlayer&, POSTYPE);
};