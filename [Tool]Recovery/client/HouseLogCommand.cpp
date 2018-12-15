#include "StdAfx.h"
#include "client.h"
#include "HouseLogCommand.h"

CHouseLogCommand::CHouseLogCommand(CclientApp& application, const TCHAR* title, const CHouseLogCommand::Configuration& configuration) :
CCommand(application, title),
mConfiguration(configuration)
{}

void CHouseLogCommand::SaveToExcel(DWORD serverIndex, const CListCtrl& listCtrl) const
{
	CString textHead;
	textHead.LoadString(IDS_STRING165);

	CString textPoint;
	textPoint.LoadString(IDS_STRING34);

	CString textLog;
	textLog.LoadString(IDS_STRING10);

	CclientApp::SaveToExcel(
		textHead + _T(" ") + textPoint + _T(" ") + textLog + _T("-") + mApplication.GetServerName(serverIndex),
		textLog,
		listCtrl);
}


void CHouseLogCommand::Initialize(CListCtrl& listCtrl) const
{
	struct Field
	{
		int mText;
		int mWidth;
	};
	const Field fieldArray[] = {
		{IDS_STRING22, 0}, // 로그 번호
		{IDS_STRING3, 110}, // 일자
		{IDS_STRING4, 70}, // 로그 종류
		{IDS_STRING301, 50}, // 유저 번호
		{IDS_STRING54, 100}, // 이름
		{IDS_STRING482, 100}, // 아이템 DB 번호
		{IDS_STRING189, 0}, // 아이템 번호
		{IDS_STRING189, 100}, // 아이템 이름
		{IDS_STRING478, 0}, // 가구 번호
		{IDS_STRING478, 100}, // 가구 이름
		{IDS_STRING479, 40}, // 슬롯
		{IDS_STRING476, 80}, // 집 이름
		{IDS_STRING480, 80}, // 꾸미기 점수
		{IDS_STRING481, 60}, // 집 점수
	};

	CRect rect;
	listCtrl.GetClientRect(
		rect);
	
	for(int i = 0; i < _countof(fieldArray); ++i)
	{
		const Field& field = fieldArray[i];

		CString text;
		text.LoadString(
			field.mText);
		listCtrl.InsertColumn(
			i,
			text,
			LVCFMT_LEFT,
			field.mWidth);
	}
}


void CHouseLogCommand::Find(DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime)
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_HOUSE_LOG;
	message.Protocol = MP_RM_HOUSE_LOG_SIZE_SYN;
	message.mRequestTick = mTickCount = GetTickCount();
	message.dwObjectID	= mConfiguration.mUserIndex;
	strncpy(
		message.mBeginDate,
		CW2AEX< MAX_PATH >(beginTime),
		sizeof(message.mBeginDate));
	strncpy(
		message.mEndDate,
		CW2AEX< MAX_PATH >(endTime),
		sizeof(message.mEndDate));
	mApplication.Send(
		serverIndex,
		message,
		sizeof(message));
}


void CHouseLogCommand::Stop(DWORD serverIndex) const
{
	MSGROOT message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_HOUSE_LOG;
	message.Protocol = MP_RM_HOUSE_LOG_STOP_SYN;
	mApplication.Send(
		serverIndex,
		message,
		sizeof(message));
}


void CHouseLogCommand::Parse(const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton) const
{
	switch(message->Protocol)
	{
	case MP_RM_HOUSE_LOG_ACK:
		{
			const DWORD previousLogIndex = _ttoi(listCtrl.GetItemText(
				listCtrl.GetItemCount() - 1,
				0));

			const MSG_RM_HOUSE_LOG* m = (MSG_RM_HOUSE_LOG*)message;

			for(size_t i = 0; i < m->mSize; ++i)
			{
				const MSG_RM_HOUSE_LOG::Log& log = m->mLog[i];

				if(previousLogIndex > log.mIndex)
				{
					continue;
				}

				int field = -1;
				const int row = listCtrl.GetItemCount();
				TCHAR buffer[MAX_PATH] = {0};

				listCtrl.InsertItem(
					row,
					_itot(log.mIndex, buffer, 10),
					++field);
				listCtrl.SetItemText(
					row,
					++field,
					CA2WEX< sizeof(log.mDate) >(log.mDate));
				listCtrl.SetItemText(
					row,
					++field,
					mApplication.GetText(log.mType));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mUser, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					CA2WEX< sizeof(log.mPlayer) >(log.mPlayer));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mItem.mDbIndex, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mItem.mIndex, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					mApplication.GetItemName(log.mItem.mIndex));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mFurniture, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					mApplication.GetFurnitureName(log.mFurniture));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mSlot, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					CA2WEX< sizeof(log.mHouse) >(log.mHouse));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mDecorationPoint, buffer, 10));
				listCtrl.SetItemText(
					row,
					++field,
					_itot(log.mHousePoint, buffer, 10));
			}

			{
				int minRange = 0;
				int maxRange = 0;
				progressCtrl.GetRange(minRange, maxRange);
				progressCtrl.SetPos(progressCtrl.GetPos() + int(m->mSize));

				CString text;
				text.Format(_T("%d/%d"), progressCtrl.GetPos(), maxRange);
				resultStatic.SetWindowText(text);

				// 080523 LUJ, 버튼 활성화 체크가 정상적으로 되도록 수정함
				if(progressCtrl.GetPos() == maxRange)
				{
					findButton.EnableWindow(TRUE);
					stopButton.EnableWindow(FALSE);
				}
			}

			break;
		}
	case MP_RM_HOUSE_LOG_SIZE_ACK:
		{
			const MSG_DWORD* m = (MSG_DWORD*)message;
			const DWORD size = m->dwData;
			CString text;
			text.Format(_T("0/%d"), size);

			resultStatic.SetWindowText(text);
			progressCtrl.SetRange32(0, size);
			progressCtrl.SetPos(0);
			findButton.EnableWindow(FALSE);
			stopButton.EnableWindow(TRUE);
			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_HOUSE_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString(IDS_STRING1);
			MessageBox(0, textThereIsNoResult, _T(""), MB_ICONERROR | MB_OK);
			break;
		}
	case MP_RM_HOUSE_LOG_STOP_ACK:
		{
			findButton.EnableWindow(TRUE);
			stopButton.EnableWindow(FALSE);

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString(IDS_STRING2);

			MessageBox(0, textSearchWasStopped, _T(""), MB_ICONERROR | MB_OK);
			break;
		}
	case MP_RM_HOUSE_LOG_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString(IDS_STRING18);

			MessageBox(0, textYouHaveNoAuthority, _T(""), MB_OK | MB_ICONERROR);
			break;
		}
	default:
		{
			ASSERT(0);
			break;
		}
	}
}
