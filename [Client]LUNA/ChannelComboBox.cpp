#include "stdafx.h"
#include "ChannelComboBox.h"
#include "cWindowManager.h"
#include "cPushupButton.h"
#include "cFont.h"
#include "../Input/Mouse.h"
#include "MHFile.h"
#include "WindowIDEnum.h"


cChannelComboBox::cChannelComboBox() : cComboBox()
{
	m_type = WT_CHANNELCOMBOBOX;
}

cChannelComboBox::~cChannelComboBox()
{
	m_ChannelCongestion.RemoveAll();
}

void	cChannelComboBox::Render()
{
	cWindow::Render();
	m_pComboBtn->Render();

	if(m_pComboBtn->IsPushed())
	{
		// 리스트 보임
		int listnum = GetItemCount();
		switch(listnum)
		{
		case 0:
			break;
		case 1:
			{
				VECTOR2 start_pos;
				start_pos.x = m_absPos.x;
				start_pos.y = m_absPos.y+m_height;
				m_DownImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
			}
			break;
		default:
			{
				VECTOR2 start_pos;
				start_pos.x = m_absPos.x;
				start_pos.y = m_absPos.y+m_height;
				m_TopImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
				for(int i = 0 ; i < listnum - 2 ; i++)
				{
					start_pos.y += m_middleHeight;
					m_MiddleImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
				}
				start_pos.y += m_middleHeight;
				m_DownImage.RenderSprite(NULL,NULL,0,&start_pos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));

			}
			break;

		}
		
		VECTOR2 m_tmpPos;
		// Over Image Display 
		if( m_nOverIdx != -1 )
		{
			m_tmpPos.x = m_absPos.x ;
			m_tmpPos.y = m_absPos.y + (((float)m_nOverIdx+1)*m_middleHeight) +3;
			m_OverImage.RenderSprite(&m_OverImageScale,NULL,0,&m_tmpPos,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
		}
		//


		PTRLISTPOS pos = m_ListItem.GetHeadPosition();
		ITEM* item = NULL;
		LONG i = 0;

		PTRLISTPOS CongestionPos = m_ChannelCongestion.GetListItem()->GetHeadPosition();
		ITEM* pItem = NULL;

		while(pos)
		{
			item = (ITEM*)m_ListItem.GetNext(pos);
			RECT rect={(LONG)m_absPos.x+m_textClippingRect.left, (LONG)m_absPos.y+m_height+m_textClippingRect.top+(m_middleHeight*i), 1,1};
	
			CFONT_OBJ->RenderFont(m_wFontIdx,item->string,lstrlen(item->string),&rect,RGBA_MERGE(item->rgb, m_alpha * m_dwOptionAlpha / 100));

			pItem = (ITEM*)m_ChannelCongestion.GetListItem()->GetNext( CongestionPos );

			RECT rectCongestion = rect;

			rectCongestion.left		=	rectCongestion.left + CFONT_OBJ->GetTextExtentWidth( m_wFontIdx, item->string, strlen( item->string ) ) + m_dwTextDistance;
			rectCongestion.right	=	rectCongestion.right + CFONT_OBJ->GetTextExtentWidth( m_wFontIdx, pItem->string, strlen( pItem->string ) ) + m_dwTextDistance;

			if( pItem )
				CFONT_OBJ->RenderFont(m_wFontIdx,pItem->string,lstrlen(pItem->string),&rectCongestion,RGBA_MERGE(pItem->rgb, m_alpha * m_dwOptionAlpha / 100));

			i++;
		}
	}



	RECT rect={(LONG)m_absPos.x+5, (LONG)m_absPos.y+6, 1,1};

	CFONT_OBJ->RenderFont(m_wFontIdx,m_szComboText,lstrlen(m_szComboText),&rect,RGBA_MERGE(m_ComboTextColor, m_alpha * m_dwOptionAlpha / 100));
}

void cChannelComboBox::InsertChannelInfo( char* pItemText, DWORD dwColor )
{
	WORD	nMaxLine	=	m_ChannelCongestion.GetMaxLine() + 1;
	
	m_ChannelCongestion.SetMaxLine( nMaxLine );

	ITEM* pItem = new ITEM;
	strcpy(pItem->string, pItemText );
	pItem->rgb = dwColor;
	m_ChannelCongestion.AddItem(pItem);
}

void cChannelComboBox::RemoveAllChannelComboItem()
{
	RemoveAll();
	m_ChannelCongestion.RemoveAll();
	
	SetMaxLine( 0 );
	m_ChannelCongestion.SetMaxLine( 0 );
}

void cChannelComboBox::LoadChannelInfo()
{
	char fileName[128]	=	{ 0, };
	char tempString[MAX_PATH]	=	{ 0, };
	char strID[128]	=	{ 0, };


	LONG	lID				=	0;
	DWORD	dwTextDistance	=	0;
	DWORD	dwFreeCnt		=	0;
	DWORD	dwFreeColor		=	0;
	DWORD	dwBusyCnt		=	0;
	DWORD	dwBusyColor		=	0;
	DWORD	dwExceedColor	=	0;

	DWORD	dwR;
	DWORD	dwG;
	DWORD	dwB;

	CMHFile	file;
	sprintf( fileName, "Data/Interface/Windows/ChannelInfo.bin");

	BOOL bResult	=	file.Init( fileName, "rb" );

	if( !bResult )
	{
		MessageBox( NULL, "Loading failed!! Data/Interface/Windows/ChannelInfo.bin", 0, 0);
	}

	if( file.IsEOF() )
		return;

	while( !file.IsEOF() )
	{
		strcpy( tempString, file.GetString() );
		if( strcmp( tempString, "ID" ) == 0 )
		{
			file.GetString( strID );
			lID	=	IDSEARCH( strID );
			if( lID != WINDOW_ID( MNM_CHANNEL_COMBOBOX ) )
				return;
		}
		else if( strcmp( tempString, "TEXTDISTANCE" ) == 0 )
		{
			dwTextDistance	=	file.GetDword();
		}
		else if( strcmp( tempString, "FREECOUNT" ) == 0 )
		{
			dwFreeCnt	=	file.GetDword();
		}
		else if( strcmp( tempString, "FREECOLOR" ) == 0 )
		{
			dwR	=	file.GetDword();
			dwG	=	file.GetDword();
			dwB	=	file.GetDword();

			dwFreeColor	=	RGB_HALF( dwR, dwG, dwB );
		}
		else if( strcmp( tempString, "BUSYCOUNT" ) == 0 )
		{
			dwBusyCnt	=	file.GetDword();
		}
		else if( strcmp( tempString, "BUSYCOLOR" ) == 0 )
		{
			dwR	=	file.GetDword();
			dwG	=	file.GetDword();
			dwB	=	file.GetDword();

			dwBusyColor	=	RGB_HALF( dwR, dwG, dwB );
		}
		else if( strcmp( tempString, "EXCEEDCOLOR" ) == 0 ) 
		{
			dwR	=	file.GetDword();
			dwG	=	file.GetDword();
			dwB	=	file.GetDword();

			dwExceedColor	=	RGB_HALF( dwR, dwG, dwB );
		}
	}

	m_dwTextDistance	=	dwTextDistance;

	m_dwFreeCnt		=	dwFreeCnt;
	m_dwBusyCnt		=	dwBusyCnt;

	m_dwFreeColor	=	dwFreeColor;
	m_dwBusyColor	=	dwBusyColor;
	m_dwExceedColor	=	dwExceedColor;

	file.Release();
}
