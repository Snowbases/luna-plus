#include "stdafx.h"
#include "GuildMarkManager.h"

#include "GuildMarkImg.h"
#include "GuildMark.h"
#include "cImageSelf.h"
#include "FreeImage/FreeImage.h"
#include "MHFile.h"
#include "ObjectManager.h"



extern int g_nServerSetNum;


CGuildMarkManager::CGuildMarkManager() :
mDirectory( "data\\interface\\2dimage\\guildmark" )
{
	m_pLoadingImg = NULL;
	m_MarkTable.Initialize(32);
}

CGuildMarkManager::~CGuildMarkManager()
{
	Release();
}

BOOL CGuildMarkManager::Init()
{
	if(m_pLoadingImg == NULL)
		m_pLoadingImg = LoadMarkImg(0);

	if(m_pLoadingImg == NULL)
		return FALSE;

	return TRUE;
}

void CGuildMarkManager::Release()
{
	m_MarkTable.SetPositionHead();
	while(CGuildMark* pMark = m_MarkTable.GetData())
	{
		pMark->Clear();
		delete pMark;		
	}

	m_MarkTable.RemoveAll();

	if( m_pLoadingImg )
	{
		m_pLoadingImg->Release();
		delete m_pLoadingImg;
		m_pLoadingImg = NULL;
	}

	/*for(	ImageList::iterator it = mImageList.begin();
			mImageList.end() != it;
			++it )
	{
		cImageSelf* image = *it;

		SAFE_DELETE( image );
	}

	mImageList.clear();*/
}


CGuildMarkImg* CGuildMarkManager::LoadMarkImg(MARKNAMETYPE markname)
{
	/*
	//서버별 구분 추가
	char filename[256];
 	sprintf(filename,"Data/Interface/2DImage/MunpaMark/%d.tga", markname);
	*/

	//////////////////////////////////////////////////////////////////////////////
	//06. 06. 서버셋별 구분 추가 - 이영준
	//파일이름 앞에 서버셋 번호 두자리 부여
	char filename[256];
 	//sprintf(filename, "Data/Interface/2DImage/GuildMark/%02d_%d.tga", g_nServerSetNum, markname);

	sprintf( filename, "%s\\%02d_%d.tga", mDirectory, g_nServerSetNum, markname );
	
	cImageSelf* pImg = new cImageSelf;
	if(pImg->LoadSprite(filename) == FALSE)
	{
		SAFE_DELETE( pImg );
		return NULL;
	}

	//mImageList.push_back( pImg );

	//CGuildMarkImg* pMarkImg = new CGuildMarkImg(markname,pImg);
	//return pMarkImg;

	return new CGuildMarkImg(markname,pImg);
}

BOOL CGuildMarkManager::LoadMarkInfo()
{
	//이미지 읽어옴
	return TRUE;
}

//void CGuildMarkManager::SendGuildMarkImgRequest(MARKNAMETYPE MarkName)
//{
//	MSG_DWORD msg;
//	SetProtocol(&msg,MP_GUILD,MP_GUILDMARK_REQUEST_SYN);
//	msg.dwObjectID = HEROID;
//	msg.dwData = MarkName;
//	NETWORK->Send(&msg,sizeof(msg));
//}
//void CGuildMarkManager::SendGuildMarkImgUpdate(DWORD GuildIdx,char* pImgData)
//{
//	MSG_GUILDMARK_IMG msg;
//	SetProtocol(&msg,MP_GUILD,MP_GUILDMARK_UPDATE_SYN);
//	msg.dwObjectID = HEROID;
//	msg.GuildIdx = GuildIdx;
//	memcpy(msg.imgData,pImgData,GUILDMARK_BUFSIZE);
//	NETWORK->Send(&msg,sizeof(msg));
//}
void CGuildMarkManager::UpdateGuildMarkImg(DWORD GuildIdx,MARKNAMETYPE markname, const char* pImgData)
{
	//const char* directory = "Data/Interface/2DImage/GuildMark";
	
	if( ERROR_PATH_NOT_FOUND == CreateDirectory( mDirectory, 0 ) )
	{
		ASSERT( 0 );
		return;
	}
	
	// 06. 06. 서버셋별 구분 추가 - 이영준
	// 파일이름 앞에 서버셋 번호 두자리 부여
	char filename[256];
 	sprintf(filename, "%s\\%02d_%d.tga", mDirectory, g_nServerSetNum, markname );
	
	char tempBuf[GUILDMARKBPP*GUILDMARKWIDTH*GUILDMARKWIDTH] = { 0, };
	memcpy(tempBuf,pImgData,GUILDMARK_BUFSIZE);

	WriteTGA(
		filename,
		tempBuf,
		GUILDMARKWIDTH,
		GUILDMARKHEIGHT, 
		GUILDMARKBPP * GUILDMARKWIDTH,
		GUILDMARKBPP * 8 );

	// 기존 문파 마크가 있으면 갱신
	CGuildMark* pMark = m_MarkTable.GetData(markname);
	if( pMark )
	{
		CGuildMarkImg* pMarkImg = LoadMarkImg(markname);

		if( pMarkImg )
		{
			pMark->SetGuildMarkImg(pMarkImg);		
		}
	}
}


CGuildMark* CGuildMarkManager::GetGuildMark(MARKNAMETYPE markname)
{
	if(markname == 0)
		return NULL;
	// 기존 마크 있으면 그냥 리턴한다.
	CGuildMark* pMark = m_MarkTable.GetData(markname);
	if( pMark )
		return pMark;

	// 마크 이미지를 로딩한다
	CGuildMarkImg* pMarkImg = LoadMarkImg(markname);

	// 마크 이미지가 없다면 로딩이미지로 바꾸고 마크를 요청한다.
	if( ! pMarkImg )
	{
		pMarkImg = m_pLoadingImg;
		
		MSG_DWORD msg;
		msg.Category	= MP_GUILD;
		msg.Protocol	= MP_GUILDMARK_REQUEST_SYN;		
		msg.dwObjectID	= HEROID;
		msg.dwData		= markname;
		NETWORK->Send(&msg,sizeof(msg));
	}

	// 마크이미지를 마크에 셋팅하고 리턴한다.
	pMark = new CGuildMark;
	pMark->SetGuildMarkImg(pMarkImg);
	m_MarkTable.Add(pMark,markname);
	return pMark;
}

//void CGuildMarkManager::OnRecvGuildMarkImg(MSG_GUILDMARK_IMG* pmsg)
//{
//	UpdateGuildMarkImg(pmsg->GuildIdx,pmsg->MarkName,pmsg->imgData);
//}

BOOL CGuildMarkManager::ChangeGuildMarkSyn(DWORD GuildIdx, const char* filename)
{
	char name[ MAX_PATH ] = { 0 };
	sprintf( name, filename );
	
	MSG_GUILDMARK_IMG message;
	{
		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILDMARK_UPDATE_SYN;
		message.dwObjectID	= HEROID;
		message.GuildIdx	= GuildIdx;
	}

	if( ! ReadBitInfoFromFile( name, message.imgData) )
	{
		strcat( name, ".bmp" );

		if( ! ReadBitInfoFromFile( name, message.imgData) )
		{
			return FALSE;
		}
	}

	NETWORK->Send( &message, sizeof( message ) );
	return TRUE;
}

BOOL CGuildMarkManager::ReadBitInfoFromFile(char* filename,char* imgData)
{
//	FIBITMAP* pBitMap = FreeImage_LoadBMP(filename);
	FIBITMAP* pBitMap = FreeImage_Load(FIF_BMP, filename, BMP_DEFAULT);

	const BITMAPINFO* pBitInfo = FreeImage_GetInfo(pBitMap);

	if( !	pBitInfo											||
		!	pBitInfo											||
			pBitInfo->bmiHeader.biHeight	!= GUILDMARKHEIGHT	||
			pBitInfo->bmiHeader.biWidth		!= GUILDMARKWIDTH )
	{
		return FALSE;
	}

	FIBITMAP*	pConvertedBitmap	= FreeImage_ConvertTo16Bits565( pBitMap );
	const int	bytesPerLine		= GUILDMARKWIDTH * GUILDMARKBPP;
	BYTE*		pData				= FreeImage_GetBits( pConvertedBitmap );

	for(int row=0;row< GUILDMARKHEIGHT; ++row)
	{
		memcpy(
			&imgData[ ( GUILDMARKHEIGHT - row - 1 ) * bytesPerLine ],
			&pData[ row * bytesPerLine ],
			bytesPerLine );
	}
	
	FreeImage_Unload(pBitMap);
	FreeImage_Unload(pConvertedBitmap);

	return TRUE;
}


CGuildMark* CGuildMarkManager::LoadGuildMarkImg( MARKNAMETYPE markname )
{
	CGuildMarkImg* pMarkImg = LoadMarkImg( markname );
	if( pMarkImg == NULL )
		return NULL;

	CGuildMark* pMark = new CGuildMark;
	pMark->SetGuildMarkImg(pMarkImg);
	m_MarkTable.Add(pMark,markname);
	return pMark;
}


CGuildMarkManager* CGuildMarkManager::GetInstance()
{
	static CGuildMarkManager instance;

	return &instance;
}