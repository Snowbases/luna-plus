// ChannelDialog.cpp: implementation of the CChannelDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChannelDialog.h"
#include "WindowIDEnum.h"
#include "CharSelect.h"
#include "cRItemEx.h"
#include "ChatManager.h"
#include "MainGame.h"
#include "cResourceManager.h"
#include "ObjectManager.h"

#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif


extern int g_nServerSetNum;

int gChannelNum;
int g_MapChannelNum[MAX_MAP_NUM];
BOOL gCheatMove;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChannelDialog::CChannelDialog()
{
	m_bInit = FALSE;
	m_SelectRowIdx = 0;
	m_BaseChannelIndex = 0;
	m_eState = eState_CharSelect;
}

CChannelDialog::~CChannelDialog()
{
	// 070421 LYW --- ChannelDialog : Release m_pChannelLCtrl.
	ReleaseChannelLCtrl() ;
}

// 070421 LYW --- ChannelDialog : Add function to release m_pChannelLCtrl.
void CChannelDialog::ReleaseChannelLCtrl()
{
	m_pChannelLCtrl->DeleteAllItems() ;
}

void CChannelDialog::Linking()
{
	m_pChannelLCtrl = (cListCtrl*)GetWindowForID(CHA_CHANNELLIST);
}

DWORD CChannelDialog::ActionEvent(CMouse* mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);
	
	int rowidx = m_pChannelLCtrl->GetSelectedRowIdx();
	if(we & WE_ROWCLICK)
	{
		SelectChannel(rowidx);
	}
	else if( we & WE_ROWDBLCLICK )
	{
		OnConnect();
	}
	return we;
}


void CChannelDialog::SetChannelList(MSG_CHANNEL_INFO* pInfo)
{
	char filename[64];
	CMHFile file;

	// 각 맵의 채널 읽기.
	int nMapNum, nChannelNum;
	sprintf(filename,"System/MapChannel.bin");
	ZeroMemory(g_MapChannelNum, sizeof(g_MapChannelNum));

	if( file.Init( filename,"rb") )
	{
		char buf[256];

		while( ! file.IsEOF() )
		{
			file.GetString( buf );

			if( strcmp( buf, "$MAP_CHANNEL" ) == 0)
			{
				nMapNum = file.GetWord();
				nChannelNum = file.GetWord();

				g_MapChannelNum[nMapNum] = nChannelNum;
			}
		}
		file.Release();
	}

	m_pChannelLCtrl->DeleteAllItems();
	char temp[MAX_CHANNEL_NAME+4] = {0, };
	BYTE Count = 0;
	WORD LowCrowd = 1000; 
	int rowidx = 0;
	m_BaseChannelIndex = 0;

	int nChannelCount = pInfo->Count;

	if( nChannelCount > CHARSELECT->GetMaxChannel( g_nServerSetNum ) )
		nChannelCount = CHARSELECT->GetMaxChannel( g_nServerSetNum );

	for(int i=0; i<nChannelCount; ++i)
	{
		cRITEMEx *ritem = new cRITEMEx(2);
		sprintf(temp, "%s %d", pInfo->ChannelName, i+1);
		strcpy(ritem->pString[0], temp);

		// 070122 LYW --- Setting color.
		ritem->rgb[0] = RGBA_MAKE( 10, 10, 10, 255 ) ;
		ritem->rgb[1] = RGBA_MAKE( 10, 10, 10, 255 ) ;
		
		//Crowd Level
		if(pInfo->PlayerNum[i] < 30)
			// 070122 LYW --- Modified this line.
			//strcpy(ritem->pString[1], CHATMGR->GetChatMsg(211) );
			strcpy(ritem->pString[1], RESRCMGR->GetMsg(286) );
		else if(pInfo->PlayerNum[i] < 100)	//MAX USER PER CHANNEL
			// 070122 LYW --- Modified this line.
			//strcpy(ritem->pString[1], CHATMGR->GetChatMsg(212) );
			strcpy(ritem->pString[1], RESRCMGR->GetMsg(287) );
		else 
		{
			// 070122 LYW --- Modified this line.
			//strcpy(ritem->pString[1], CHATMGR->GetChatMsg(213) );
			strcpy(ritem->pString[1], RESRCMGR->GetMsg(288) );
			ritem->rgb[1] = RGBA_MAKE(255,0,0,255);			
		}
		
		ritem->dwID = pInfo->PlayerNum[i];
		m_pChannelLCtrl->InsertItem(i, ritem);
		
		Count++;

		if(LowCrowd == 0)
			continue;
		if(LowCrowd > pInfo->PlayerNum[i])
		{
			LowCrowd = pInfo->PlayerNum[i];
			rowidx = i;
		}		
	}

	rowidx = 0;

	cRITEMEx* pRItem = (cRITEMEx*)m_pChannelLCtrl->GetRItem(rowidx);
	// 070122 LYW --- Modified this line.
	//pRItem->rgb[0] = RGBA_MAKE(255,234,0,255);
	if (pRItem)
	{
		pRItem->rgb[0] = RGBA_MAKE(255, 255, 255, 255);
		pRItem->rgb[1] = RGBA_MAKE(255, 255, 255, 255);
	}
	gChannelNum = rowidx+m_BaseChannelIndex;
	m_SelectRowIdx = rowidx;
	SetActive(TRUE);
#ifdef _GMTOOL_
	GMTOOLMGR->SetChannelCount( Count );
#endif
}

void CChannelDialog::SetActive(BOOL val)
{
	cDialog::SetActive(val);
}

void CChannelDialog::OnConnect()
{	
	cRITEMEx * pRItem = (cRITEMEx*)m_pChannelLCtrl->GetRItem(m_SelectRowIdx);
	if(pRItem)
	{
		switch( m_eState )
		{
		case eState_CharSelect:
			{
				if(gChannelNum == -1)
				{
					// ShinJS 090430 --- 메세지 번호 수정
					CHARSELECT->DisplayNotice(322);
				}
				else if( CHARSELECT->EnterGame() == FALSE )
				{
					CHARSELECT->DisplayNotice(18);
				}
			}
			break;
		case eState_GameIn:
			{
				if( HERO->GetState() != eObjectState_None )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2222 ) );
					break;
				}

				MSG_WORD msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_USERCONN;
				msg.Protocol = MP_USERCONN_CHANGE_CHANNEL_IN_GAME_SYN;
				msg.dwObjectID = HEROID;
				msg.wData = (WORD)gChannelNum;
				NETWORK->Send( &msg, sizeof(msg) );
			}
			break;
		}
	}
}

void CChannelDialog::SelectChannel(int rowidx)
{	
	cRITEMEx * pRItem = (cRITEMEx *)m_pChannelLCtrl->GetRItem(rowidx);
	
	if(pRItem != NULL)
	{
		if(m_SelectRowIdx != rowidx)
		{
			// 070122 LYW --- Modified this line.
			//pRItem->rgb[0] = RGBA_MAKE(255,234,0,255);
			pRItem->rgb[0] = RGBA_MAKE(255, 255, 255, 255);
			pRItem->rgb[1] = RGBA_MAKE(255, 255, 255, 255);
			
			pRItem = (cRITEMEx*)m_pChannelLCtrl->GetRItem(m_SelectRowIdx);
			//pRItem->rgb[0] = RGBA_MAKE( 255, 255 ,255 ,255);
			pRItem->rgb[0] = RGBA_MAKE(10, 10, 10, 255);
			pRItem->rgb[1] = RGBA_MAKE(10, 10, 10, 255);
			
			cRITEMEx* pRItem = (cRITEMEx*)m_pChannelLCtrl->GetRItem(rowidx);
			if(pRItem->dwID >= 300)
				gChannelNum = -1;
			else
				gChannelNum = rowidx + m_BaseChannelIndex;
			
			m_SelectRowIdx = rowidx;
		}
	}
}

