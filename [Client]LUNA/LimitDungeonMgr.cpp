#include "stdafx.h"
#include <sys/stat.h>
#include "LimitDungeonMgr.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "GMNotifyManager.h"
#include "GameIn.h"
#include "InventoryExDialog.h"
#include "ItemManager.h"
#include "LimitDungeonDialog.h"
#include "WindowIDEnum.h"
#include "interface/cWindowManager.h"
#include "MHMap.h"
#include "../[CC]Header/GameResourceManager.h"

CLimitDungeonManager::CLimitDungeonManager(void)
{
	ZeroMemory(&mFileStatus, sizeof(mFileStatus));

	// 091106 LUJ, 크기가 0인지 검사를 피하기 위해 무의미한 값을 넣는다
	__time64_t todayTime = 0;
	_time64(&todayTime);
	mScheduleMap.insert(std::make_pair(todayTime, Event()));
	RefreshScript();
}

void CLimitDungeonManager::SetToolTip(const LimitDungeonScript& script, ToolTipList& tipList)
{
	SetTipCondition(
		script,
		tipList);

	if(false == tipList.empty())
	{
		tipList.push_back("");
	}

	SetTipTime(
		script,
		tipList);

	if(false == script.mDescriptionList.empty())
	{
		tipList.push_back("");
	}

	SetTipDescription(
		script,
		tipList);
}

void CLimitDungeonManager::SetTipCondition(const LimitDungeonScript& script, ToolTipList& tipList)
{
	// 091106 LUJ, 입장 레벨 구간
	{
		const LimitDungeonScript::Level emptyLevel = {0, USHRT_MAX};

		if(memcmp(&emptyLevel, &script.mLevel, sizeof(emptyLevel)))
		{
			TCHAR tipText[MAX_PATH] = {0};
			_stprintf(
				tipText,
				"%s%u ~ %u",
				CHATMGR->GetChatMsg(670),
				script.mLevel.mMin,
				script.mLevel.mMax);
			tipList.push_back(tipText);
		}
	}

	// 091106 LUJ, 소요 아이템
	{
		const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(script.mItem.mItemIndex);

		if(itemInfo)
		{
			TCHAR itemText[MAX_PATH] = {0};
			_stprintf(
				itemText,
				"%s(%d)",
				itemInfo ? itemInfo->ItemName : "",
				script.mItem.mQuantity);
			TCHAR tipText[MAX_PATH] = {0};
			_stprintf(
				tipText,
				CHATMGR->GetChatMsg(1176),
				itemText);
			tipList.push_back(tipText);
		}
	}
}

void CLimitDungeonManager::SetTipDescription(const LimitDungeonScript& script, ToolTipList& tipList)
{	
	typedef LimitDungeonScript::DescriptionList DescriptionList;
	const DescriptionList descriptionList = script.mDescriptionList;

	for(DescriptionList::const_iterator iter = descriptionList.begin();
		descriptionList.end() != iter;
		++iter)
	{
		const DWORD messageIndex = *iter;
		LPCTSTR message = CHATMGR->GetChatMsg(messageIndex);

		do
		{
			const size_t maxLength = 35;
			TCHAR buffer[MAX_PATH] = {0};
			_mbsnbcpy(
				PUCHAR(buffer),
				PUCHAR(message),
				maxLength);
			buffer[maxLength] = 0;
			message += _tcslen(buffer);
			tipList.push_back(buffer);
		} while (*message);
	}
}

void CLimitDungeonManager::SetTipTime(const LimitDungeonScript& script, ToolTipList& tipList)
{
	__time64_t todayTime = 0;
	_time64(&todayTime);

	// 091106 LUJ, open은 이미 지났을 수도 있지만, close는 항상 예정되어 있다
	//			따라서 이 이벤트를 추적하면 된다
	for(ScheduleMap::const_iterator iter = mScheduleMap.begin();
		mScheduleMap.end() != iter;
		++iter)
	{
		const Event& event = iter->second;

		if(Event::TypeOpenAlarm == event.mType)
		{
			continue;
		}
		else if(Event::TypeCloseAlarm == event.mType)
		{
			continue;
		}
		else if(script.mMapType != event.mMapType)
		{
			continue;
		}
		else if(script.mChannel != event.mChannelIndex)
		{
			continue;
		}

		const __time64_t eventTime = iter->first;
		const __time64_t startTime = eventTime - event.mDate.mPlayTime;

		// 091106 LUJ, 다음 일정을 표시해준다
		if(todayTime < startTime)
		{
			const BOOL isEvening(12 < event.mDate.mStartHour);
			TCHAR text[MAX_PATH] = {0};
			_stprintf(
				text,
				CHATMGR->GetChatMsg(1766),
				GetDayString(todayTime, startTime),
				CHATMGR->GetChatMsg(isEvening ? 1768 : 1767),
				event.mDate.mStartHour - (isEvening ? 12 : 0),
				event.mDate.mStartmMinute,
				event.mDate.mPlayTime / 60);
			tipList.push_back(text);
			// 091106 LUJ, 대기중인 이벤트인 경우 루프를 탈출한다
			//			요구사항이 다음과 같다. 진행 중일 경우 시간 표시.
			//			더불어 다음 일정은 반드시 표시해야한다.
			break;
		}

		const struct tm closeDate = *_localtime64(&eventTime);
		const BOOL isEvening(12 < closeDate.tm_hour);
		// 091106 LUJ, 현재 일정을 표시해준다
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			CHATMGR->GetChatMsg(1765),
			CHATMGR->GetChatMsg(isEvening ? 1768 : 1767),
			closeDate.tm_hour - (isEvening ? 12 : 0),
			closeDate.tm_min);
		tipList.push_back(text);
	}
}

void CLimitDungeonManager::AskJoin(MAPTYPE mapType, DWORD channelIndex)
{
	const LimitDungeonScript& script = GAMERESRCMNGR->GetLimitDungeonScript(
		mapType,
		channelIndex);
	CHero* const hero = OBJECTMGR->GetHero();

	if(0 == hero)
	{
		return;
	}
    else if(script.mLevel.mMin > hero->GetLevel())
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1741));
		return;
	}
	else if(script.mLevel.mMax < hero->GetLevel())
	{
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1741));
		return;
	}

	CInventoryExDialog* const inventoryDialog = GAMEIN->GetInventoryDialog();

	if(0 == inventoryDialog)
	{
		return;
	}

	const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(script.mItem.mItemIndex);
	POSTYPE itemPosition = 0;

	if(itemInfo)
	{
		CItem* const item = inventoryDialog->GetItemForIdx(
			script.mItem.mItemIndex,
			WORD(-1),
			script.mItem.mQuantity);

		if(0 == item)
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(74),
				itemInfo ? itemInfo->ItemName : "?");
			return;
		}

		itemPosition = item->GetPosition();
	}

	MSG_DWORD3 message;
	ZeroMemory(&message, sizeof(message));
	message.Category = MP_LIMITDUNGEON;
	message.Protocol = MP_LIMITDUNGEON_JOIN_SYN;
	message.dwObjectID = gHeroID;
	message.dwData1 = mapType;
	message.dwData2 = channelIndex;
	message.dwData3 = itemPosition;
	NETWORK->Send(
		&message,
		sizeof(message));
}

CLimitDungeonManager::~CLimitDungeonManager(void)
{}

CLimitDungeonManager* CLimitDungeonManager::GetInstance()
{
	static CLimitDungeonManager instance;
	return &instance;
}

void CLimitDungeonManager::NetworkMsgParser(BYTE Protocol, LPVOID packet)
{
	switch(Protocol)
	{
	case MP_LIMITDUNGEON_JOIN_ACK:
		{
			const MSG_DWORDEX2* const message = (MSG_DWORDEX2*)packet;
			const DWORD monsterCount = DWORD(message->dweData1);
			const __time64_t playTime = message->dweData2;
			SetMonsterCount(monsterCount);
			SetPlayTime(playTime);
			UpdateWindow();
			break;
		}
	case MP_LIMITDUNGEON_SET_STATUS_ACK:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)packet;
			const DWORD monsterCount = message->dwData;
			SetMonsterCount(monsterCount);
			break;
		}
	case MP_LIMITDUNGEON_JOIN_NACK:
		{
			const MSG_LIMITDUNGEON_ERROR* const message = (MSG_LIMITDUNGEON_ERROR*)packet;
			PutMessage(message->mError);
			break;
		}
	}
}

void CLimitDungeonManager::PutMessage()
{
	const ScheduleMap::const_iterator scheduleIterator = mScheduleMap.begin();
	const Event& event = scheduleIterator->second;
	__time64_t second = -1;

	switch(event.mType)
	{
	case Event::TypeOpenAlarm:
		{
			if(IsPlayMap())
			{
				break;
			}

			second = abs(event.mAlarmTime);
			break;
		}
	case Event::TypeCloseAlarm:
		{
			if(event.mMapType != MAP->GetMapNum())
			{
				break;
			}
			else if(int(event.mChannelIndex) != gChannelNum + 1)
			{
				break;
			}

			CLimitDungeonDialog* const dialog = (CLimitDungeonDialog*)g_cWindowManager.GetWindowForID(LD_LIMIT_INFO_DLG);

			if(0 == dialog)
			{
				break;
			}

			second = dialog->GetPlayTime();
			break;
		}
	}

	// 100104 LUJ, 지난 이벤트는 알려주지 않도록 한다
	if(0 >= second)
	{
		return;
	}

	// 091106 LUJ, 초 단위를 정수 나눗셈할 경우 1분씩 오차가 발생하므로 반올림한다.
	const double minute = floor(double(second) / 60.0f + 0.5f);
	TCHAR text[MAX_PATH] = {0};
	_stprintf(
		text,
		CHATMGR->GetChatMsg(event.mMessage),
		GetMapName(event.mMapType),
		int(minute));
	NOTIFYMGR->AddMsg(
		text);
	CHATMGR->AddMsg(
		CTC_SYSMSG,
		text);
}

void CLimitDungeonManager::PutMessage(MSG_LIMITDUNGEON_ERROR::Error error)
{
	switch(error)
	{
	case MSG_LIMITDUNGEON_ERROR::ErrorMapMove:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1711));
			break;
		}
	case MSG_LIMITDUNGEON_ERROR::ErrorRoom:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1742));
			break;
		}
	case MSG_LIMITDUNGEON_ERROR::ErrorLevel:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1741));
			break;
		}
	case MSG_LIMITDUNGEON_ERROR::ErrorItem:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1678));
			break;
		}
	}
}

void CLimitDungeonManager::SetPlayTime(__time64_t playTime)
{
	CLimitDungeonDialog* const dialog = (CLimitDungeonDialog*)g_cWindowManager.GetWindowForID(LD_LIMIT_INFO_DLG);

	if(0 == dialog)
	{
		return;
	}

	dialog->SetPlayTime(playTime);
}

void CLimitDungeonManager::SetMonsterCount(DWORD monsterCount)
{
	CLimitDungeonDialog* const dialog = (CLimitDungeonDialog*)g_cWindowManager.GetWindowForID(LD_LIMIT_INFO_DLG);

	if(0 == dialog)
	{
		return;
	}

	dialog->SetMonsterCount(monsterCount);
}

void CLimitDungeonManager::RefreshScript()
{
	LPCTSTR filePath = "system\\resource\\LimitDungeon.bin";
	struct stat fileStatus = {0};

	if(stat(filePath, &fileStatus))
	{
		return;
	}
	else if(0 == memcmp(&fileStatus, &mFileStatus, sizeof(fileStatus)))
	{
		return;
	}

	mFileStatus	= fileStatus;
	g_CGameResourceManager.LoadLimitDungeonScript();
	UpdateSchedule();
}

void CLimitDungeonManager::Process()
{
	if(IsReady())
	{
		PutMessage();
		PostponeSchedule();
		return;
	}

#ifdef _GMTOOL_
		RefreshScript();
#endif
}

BOOL CLimitDungeonManager::IsReady()
{
	__time64_t todayTime = 0;
	_time64(&todayTime);
	const ScheduleMap::const_iterator iter = mScheduleMap.begin();
	const __time64_t startSecond = iter->first;

	return todayTime > startSecond;
}

void CLimitDungeonManager::PostponeSchedule()
{
	const ScheduleMap::iterator iterator = mScheduleMap.begin();
	const __time64_t second = iterator->first;
	const Event event = iterator->second;
	mScheduleMap.erase(iterator);
	mScheduleMap.insert(std::make_pair(second + GetSecondForOneWeek(), event));
}

LPCTSTR CLimitDungeonManager::GetDayString(__time64_t todayTime, __time64_t time)
{
	const struct tm todayDate = *_localtime64(&todayTime);
	const struct tm date = *_localtime64(&time);
	const BOOL isSameYear = (todayDate.tm_year == date.tm_year);
	const BOOL isSameDay = (todayDate.tm_yday == date.tm_yday);

	if(isSameYear &&
		isSameDay)
	{
		return CHATMGR->GetChatMsg(1749);
	}

	switch(date.tm_wday)
	{
	case 0:
		return CHATMGR->GetChatMsg(1335);
	case 1:
		return CHATMGR->GetChatMsg(1336);
	case 2:
		return CHATMGR->GetChatMsg(1337);
	case 3:
		return CHATMGR->GetChatMsg(1338);
	case 4:
		return CHATMGR->GetChatMsg(1339);
	case 5:
		return CHATMGR->GetChatMsg(1340);
	case 6:
		return CHATMGR->GetChatMsg(1341);
	}

	return "?";
}

__time64_t CLimitDungeonManager::GetSecondForOneWeek() const
{
	const __time64_t oneHour(60 * 60);
	const __time64_t oneDay(oneHour * 24);
	return oneDay * 7;
}

BOOL CLimitDungeonManager::IsPlayMap()
{
	const MAPTYPE mapType = MAP->GetMapNum();
	const LimitDungeonScript& script = g_CGameResourceManager.GetLimitDungeonScript(
		mapType,
		gChannelNum + 1);
	return script.mMapType == mapType;
}

void CLimitDungeonManager::UpdateSchedule()
{
	mScheduleMap.clear();

	__time64_t todayTime = 0;
	_time64(&todayTime);
	const struct tm todayDate = *_localtime64(&todayTime);
	mScheduleMap.insert(std::make_pair(todayTime, Event()));

	typedef CGameResourceManager::LimitDungeonScriptMap ScriptMap;
	const ScriptMap& scriptMap = g_CGameResourceManager.GetLimitDungeonScriptMap();

	// 091106 LUJ, 일정을 구성한다
	for(ScriptMap::const_iterator scriptIter = scriptMap.begin();
		scriptMap.end() != scriptIter;
		++scriptIter)
	{
		const LimitDungeonScript& script = scriptIter->second;

		// 091106 LUJ, 일정을 등록한다
		for(LimitDungeonScript::DateList::const_iterator dateIter = script.mDateList.begin();
			script.mDateList.end() != dateIter;
			++dateIter)
		{
			const LimitDungeonScript::Date& date = *dateIter;
			const WORD dayOnWeek = 7;
			// 091106 LUJ: 시작 요일과 끝 요일 사이에 며칠 간격인지 나타내는 상수 테이블
			//			행: 시작 요일(일~토), 열: 끝 요일(일~토)
			const int howManyDayBetweenDayOfWeek[][dayOnWeek] =
			{
				{0, 1, 2, 3, 4, 5, 6},
				{6, 0, 1, 2, 3, 4, 5},
				{5, 6, 0, 1, 2, 3, 4},
				{4, 5, 6, 0, 1, 2, 3},
				{3, 4, 5, 6, 0, 1, 2},
				{2, 3, 4, 5, 6, 0, 1},
				{1, 2, 3, 4, 5, 6, 0},
			};

			// 091106 LUJ, 컨텐츠가 시작될 시간을 얻는다
			struct tm futureDate(todayDate);
			const int howManyDay = howManyDayBetweenDayOfWeek[todayDate.tm_wday][date.mDayOfWeek];
			futureDate.tm_mday = futureDate.tm_mday + howManyDay;
			mktime(&futureDate);
			futureDate.tm_hour = date.mStartHour;
			futureDate.tm_min = date.mStartmMinute;
			futureDate.tm_sec = 0;

			const __time64_t openTime = _mktime64(&futureDate);
			const LimitDungeonScript::AlarmList& alarmList = script.mAlarmList;

			// 091106 LUJ, 메시지를 표시한 일정을 설정한다
			for(LimitDungeonScript::AlarmList::const_iterator alarmIter = alarmList.begin();
				alarmList.end() != alarmIter;
				++alarmIter)
			{
				typedef LimitDungeonScript::Alarm Alarm;
				const Alarm& alarm = *alarmIter;

				// 091106 LUJ, 게임 종료 후에는 알람이 표시되지 않도록 한다
				if(date.mPlayTime < alarm.mTime)
				{
					continue;
				}

				__time64_t alarmTime = openTime + alarm.mTime;

				if(todayTime > alarmTime)
				{
					alarmTime = alarmTime + GetSecondForOneWeek();
				}

				Event event;
				event.mType = (0 < alarm.mTime ? Event::TypeCloseAlarm : Event::TypeOpenAlarm);
				event.mAlarmTime = alarm.mTime;
				event.mMessage = alarm.mMessage;
				event.mMapType = script.mMapType;
				event.mChannelIndex = script.mChannel;
				event.mDate = date;
				mScheduleMap.insert(
					std::make_pair(alarmTime, event));
			}

			// 091106 LUJ, 일정을 올바로 표시하기 위해 닫힘 이벤트도 넣는다
			const __time64_t closeTime = openTime + date.mPlayTime;
			Event event;
			event.mType = Event::TypeClose;
			event.mMapType = script.mMapType;
			event.mChannelIndex = script.mChannel;
			event.mDate = date;
			mScheduleMap.insert(
				std::make_pair(closeTime + (todayTime > closeTime ? GetSecondForOneWeek() : 0), event));
		}
	}
}

void CLimitDungeonManager::UpdateWindow()
{
	struct Configuration
	{
		LONG mId;
		BOOL mIsActive;
	};
	const Configuration configurationArray[] = {
		{ TUTORIALBTN_DLG, FALSE },
		{ BM_DIALOG, FALSE },
		{ MNM_DIALOG, FALSE },
		{ MONSTERMETER_DLG, FALSE },
		{ LD_LIMIT_INFO_DLG, TRUE },
	};

	for(int i = sizeof(configurationArray) / sizeof(*configurationArray);
		i-- > 0;)
	{
		const Configuration& configuration = configurationArray[i];
		cWindow* const window = g_cWindowManager.GetWindowForID(configuration.mId);

		if(0 == window)
		{
			continue;
		}

		window->SetActive(configuration.mIsActive);

		if(FALSE == configuration.mIsActive)
		{
			window->SetDisable(TRUE);
		}
	}
}