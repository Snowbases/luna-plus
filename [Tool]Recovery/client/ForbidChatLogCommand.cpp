#include "StdAfx.h"
#include "client.h"
#include "ForbidChatLogCommand.h"

CForbidChatLogCommand::CForbidChatLogCommand(CclientApp& application, const TCHAR* title, const CForbidChatLogCommand::Configuration& configuration) :
CCommand(application, title),
mConfiguration(configuration)
{}

void CForbidChatLogCommand::SaveToExcel(DWORD serverIndex, const CListCtrl& listCtrl) const
{
	CString textHead;
	textHead.LoadString(IDS_STRING528);

	CString textLog;
	textLog.LoadString(IDS_STRING10);

	CclientApp::SaveToExcel(
		textHead + _T(" ") + textLog + _T("-") + mApplication.GetServerName(serverIndex),
		textLog,
		listCtrl);
}

void CForbidChatLogCommand::Initialize(CListCtrl& listCtrl) const
{
	CRect rect;
	int	step = -1;
	listCtrl.GetClientRect( rect );

	listCtrl.InsertColumn( ++step, L"PlayerIndex", LVCFMT_LEFT, 0 );
	CString textGMName;
	textGMName.LoadString( IDS_STRING299 );
	listCtrl.InsertColumn( ++step, textGMName, LVCFMT_LEFT, int( rect.Width() * 0.15f ) );
	CString textStartTime;
	textStartTime.LoadString( IDS_STRING537 );
	listCtrl.InsertColumn( ++step, textStartTime, LVCFMT_LEFT, int( rect.Width() * 0.23f ) );
	CString textEndTime;
	textEndTime.LoadString( IDS_STRING538 );
	listCtrl.InsertColumn( ++step, textEndTime, LVCFMT_LEFT, int( rect.Width() * 0.23f ) );
	CString textReason;
	textReason.LoadString( IDS_STRING539 );
	listCtrl.InsertColumn( ++step, textReason, LVCFMT_LEFT, int( rect.Width() * 0.39f ) );
}

void CForbidChatLogCommand::Find(DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime)
{
	MSG_RM_LOG_REQUEST message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_FORBID_CHAT_LOG;
	message.Protocol = MP_RM_FORBID_CHAT_LOG_SIZE_SYN;
	message.mRequestTick = mTickCount = GetTickCount();
	message.dwObjectID = mConfiguration.mPlayerIndex;
	strncpy( message.mBeginDate, CW2AEX< MAX_PATH >( beginTime ), sizeof( message.mBeginDate ) );
	strncpy( message.mEndDate, CW2AEX< MAX_PATH >( endTime ), sizeof( message.mEndDate ) );
	mApplication.Send( serverIndex, message, sizeof( message ) );
}

void CForbidChatLogCommand::Stop(DWORD serverIndex) const
{
	MSGROOT message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_RM_FORBID_CHAT_LOG;
	message.Protocol = MP_RM_FORBID_CHAT_LOG_STOP_SYN;
	mApplication.Send( serverIndex, message, sizeof( message ) );
}

void CForbidChatLogCommand::Parse(const MSGROOT* message, CListCtrl& listCtrl, CProgressCtrl& progressCtrl, CStatic& resultStatic, CButton& findButton, CButton& stopButton) const
{
	switch(message->Protocol)
	{
	case MP_RM_FORBID_CHAT_LOG_ACK:
		{
			const DWORD previousLogIndex = _ttoi(listCtrl.GetItemText(
				listCtrl.GetItemCount() - 1,
				0));

			const MSG_RM_FORBID_CHAT_LOG* m = (MSG_RM_FORBID_CHAT_LOG*)message;

			for(size_t i = 0; i < m->mSize; ++i)
			{
				const MSG_RM_FORBID_CHAT_LOG::Log& log = m->mLog[i];

				int field = -1;
				const int row = listCtrl.GetItemCount();
				TCHAR buffer[MAX_PATH] = {0};

				listCtrl.InsertItem(
					row,
					_itot(log.mPlayerIndex, buffer, 10),
					++field);
				listCtrl.SetItemText(
					row,
					++field,
					CString(log.m_OperatorName));
				listCtrl.SetItemText(
					row,
					++field,
					CString(log.mRegisterDate));
				listCtrl.SetItemText(
					row,
					++field,
					CString(log.mReleaseDate));
				listCtrl.SetItemText(
					row,
					++field,
					CString(log.mReason));
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
	case MP_RM_FORBID_CHAT_LOG_SIZE_ACK:
		{
			const MSG_DWORD* const m = ( MSG_DWORD* )message;
			const DWORD size = m->dwData;

			CString text;
			text.Format( _T( "0/%d" ), size );

			resultStatic.SetWindowText( text );
			progressCtrl.SetRange32( 0, size );
			progressCtrl.SetPos( 0 );
			findButton.EnableWindow( FALSE );
			stopButton.EnableWindow( TRUE );

			listCtrl.DeleteAllItems();
			break;
		}
	case MP_RM_FORBID_CHAT_LOG_SIZE_NACK:
		{
			CString textThereIsNoResult;
			textThereIsNoResult.LoadString(IDS_STRING1);
			MessageBox(0, textThereIsNoResult, _T(""), MB_ICONERROR | MB_OK);
			break;
		}
	case MP_RM_FORBID_CHAT_LOG_STOP_ACK:
		{
			findButton.EnableWindow(TRUE);
			stopButton.EnableWindow(FALSE);

			CString textSearchWasStopped;
			textSearchWasStopped.LoadString(IDS_STRING2);

			MessageBox(0, textSearchWasStopped, _T(""), MB_ICONERROR | MB_OK);
			break;
		}
	case MP_RM_FORBID_CHAT_LOG_NACK_BY_AUTH:
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