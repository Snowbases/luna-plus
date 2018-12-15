#pragma once

#define LIMITDUNGEONMGR	CLimitDungeonManager::GetInstance()

class CLimitDungeonManager
{
private:
	struct Event
	{
		enum Type
		{
			TypeNone,
			TypeOpenAlarm,
			TypeClose,
			TypeCloseAlarm,
		}
		mType;
		__time64_t mAlarmTime;
		DWORD mMessage;
		MAPTYPE mMapType;
		DWORD mChannelIndex;
		LimitDungeonScript::Date mDate;

		Event() :
		mType(TypeNone),
		mAlarmTime(0),
		mMessage(0),
		mMapType(0),
		mChannelIndex(0)
		{
			ZeroMemory(&mDate, sizeof(mDate));
		}
	};
	typedef std::multimap< __time64_t, Event > ScheduleMap;
	ScheduleMap mScheduleMap;
	typedef DWORD ChannelIndex;
	struct stat mFileStatus;

public:
	CLimitDungeonManager(void);
	virtual ~CLimitDungeonManager(void);
	static CLimitDungeonManager* GetInstance();
	void AskJoin(MAPTYPE, DWORD channelIndex);
	typedef std::list< std::string > ToolTipList;
	void SetToolTip(const LimitDungeonScript&, ToolTipList&);
	void NetworkMsgParser(BYTE Protocol, LPVOID);
	void Process();

private:
	void SetTipCondition(const LimitDungeonScript&, ToolTipList&);
	void SetTipTime(const LimitDungeonScript&, ToolTipList&);
	void SetTipDescription(const LimitDungeonScript&, ToolTipList&);
	void RefreshScript();
	void UpdateSchedule();
	void UpdateWindow();
	BOOL IsReady();
	BOOL IsPlayMap();
	void PostponeSchedule();
	__time64_t GetSecondForOneWeek() const;
	LPCTSTR GetDayString(__time64_t todayTime, __time64_t time);
	void SetMonsterCount(DWORD monsterCount);
	void SetPlayTime(__time64_t);
	void PutMessage();
	void PutMessage(MSG_LIMITDUNGEON_ERROR::Error);
};