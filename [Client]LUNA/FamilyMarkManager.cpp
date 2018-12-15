// FamilyMarkManager.cpp: implementation of the CFamilyMarkManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FamilyMarkManager.h"

#include "FamilyMarkImg.h"
#include "FamilyMark.h"
#include "cImageSelf.h"
#include "FreeImage/FreeImage.h"
#include "MHFile.h"
#include "ObjectManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//06. 06. 서버셋별 구분 추가 - 이영준
//파일이름 앞에 서버셋 번호 두자리 부여
extern int g_nServerSetNum;
//////////////////////////////////////////////////////////////////////////////

CFamilyMarkManager::CFamilyMarkManager()
{
	m_pLoadingImg = NULL;
	m_MarkTable.Initialize(32);
}

CFamilyMarkManager::~CFamilyMarkManager()
{
//	Release();
}

BOOL CFamilyMarkManager::Init()
{
	if(m_pLoadingImg == NULL)
		m_pLoadingImg = LoadMarkImg(0);

	if(m_pLoadingImg == NULL)
		return FALSE;

	return TRUE;
}

void CFamilyMarkManager::Release()
{
	m_MarkTable.SetPositionHead();
	while(CFamilyMark* pMark = m_MarkTable.GetData())
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
}


CFamilyMarkImg* CFamilyMarkManager::LoadMarkImg(MARKNAMETYPE markname)
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
 	sprintf(filename, "Data/Interface/2DImage/FamilyMark/%02d_%d.tga", g_nServerSetNum, markname);
	//////////////////////////////////////////////////////////////////////////////

	cImageSelf* pImg = new cImageSelf;
	if(pImg->LoadSprite(filename) == FALSE)
	{
		delete pImg;
		pImg = NULL;
		return NULL;
	}

	CFamilyMarkImg* pMarkImg = new CFamilyMarkImg(markname,pImg);
	return pMarkImg;
}

BOOL CFamilyMarkManager::LoadMarkInfo()
{
	//이미지 읽어옴
	return TRUE;
}

void CFamilyMarkManager::SendFamilyMarkImgRequest(MARKNAMETYPE MarkName)
{
	// 070801 웅주, 기존 패밀리 제거 작업으로 인한 주석 처리

	//MSG_DWORD msg;
	//SetProtocol(&msg,MP_FAMILY,MP_FAMILYMARK_REQUEST_SYN);
	//msg.dwObjectID = HEROID;
	//msg.dwData = MarkName;
	//NETWORK->Send(&msg,sizeof(msg));
}
void CFamilyMarkManager::SendFamilyMarkImgUpdate(DWORD FamilyIdx,char* pImgData)
{
	// 070801 웅주, 기존 패밀리 제거 작업으로 인한 주석 처리

	//MSG_FAMILYMARK_IMG msg;
	//SetProtocol(&msg,MP_FAMILY,MP_FAMILYMARK_UPDATE_SYN);
	//msg.dwObjectID = HEROID;
	//msg.FamilyIdx = FamilyIdx;
	//memcpy(msg.imgData,pImgData,FAMILYMARK_BUFSIZE);
	//NETWORK->Send(&msg,sizeof(msg));
}
void CFamilyMarkManager::UpdateFamilyMarkImg(DWORD FamilyIdx,MARKNAMETYPE markname,char* pImgData)
{
	/*
	// 그림 파일로 저장
	char filename[256];
	sprintf(filename, "Data/Interface/2DImage/MunpaMark/%d.tga", markname);
	*/

	//////////////////////////////////////////////////////////////////////////////
	//06. 06. 서버셋별 구분 추가 - 이영준
	//파일이름 앞에 서버셋 번호 두자리 부여
	char filename[256];
 	sprintf(filename, "Data/Interface/2DImage/FamilyMark/%02d_%d.tga", g_nServerSetNum, markname);
	//////////////////////////////////////////////////////////////////////////////

	char tempBuf[FAMILYMARKBPP*FAMILYMARKWIDTH*FAMILYMARKWIDTH];
	memset(tempBuf,0,FAMILYMARKBPP*FAMILYMARKWIDTH*FAMILYMARKWIDTH);
	memcpy(tempBuf,pImgData,FAMILYMARK_BUFSIZE);
	WriteTGA(filename,tempBuf,
		FAMILYMARKWIDTH,	FAMILYMARKWIDTH, 
		FAMILYMARKBPP*FAMILYMARKWIDTH,
		FAMILYMARKBPP*8);

	// 마크 정보 저장 
	
	// 기존 문파 마크가 있으면 갱신
	CFamilyMark* pMark = m_MarkTable.GetData(markname);
	if(pMark)
	{
		CFamilyMarkImg* pMarkImg = LoadMarkImg(markname);
		if(pMarkImg)
			pMark->SetFamilyMarkImg(pMarkImg);		
	}
}


CFamilyMark* CFamilyMarkManager::GetFamilyMark(MARKNAMETYPE markname)
{
	if(markname == 0)
		return NULL;
	// 기존 마크 있으면 그냥 리턴한다.
	CFamilyMark* pMark = m_MarkTable.GetData(markname);
	if(pMark)
		return pMark;

	// 마크 이미지를 로딩한다
	CFamilyMarkImg* pMarkImg = LoadMarkImg(markname);

	// 마크 이미지가 없다면 로딩이미지로 바꾸고 마크를 요청한다.
	if(pMarkImg == NULL)
	{
		pMarkImg = m_pLoadingImg;
		SendFamilyMarkImgRequest(markname);
	}

	// 마크이미지를 마크에 셋팅하고 리턴한다.
	pMark = new CFamilyMark;
	pMark->SetFamilyMarkImg(pMarkImg);
	m_MarkTable.Add(pMark,markname);
	return pMark;
}

void CFamilyMarkManager::OnRecvFamilyMarkImg(MSG_FAMILYMARK_IMG* pmsg)
{
	UpdateFamilyMarkImg(pmsg->FamilyIdx,pmsg->MarkName,pmsg->imgData);
}

BOOL CFamilyMarkManager::ChangeFamilyMarkSyn(DWORD FamilyIdx,char* filename)
{
	char markf[256];
	sprintf( markf, "Data/Interface/2DImage/FamilyMark/%s", filename );
	char imgData[FAMILYMARK_BUFSIZE];
	if(ReadBitInfoFromFile(markf,imgData) == FALSE)
	{
		//그림 파일을 읽는데 실패했습니다.
		//return FALSE;
		strcat(markf, ".bmp");
		if(ReadBitInfoFromFile(markf,imgData) == FALSE)
			return FALSE;
	}

	SendFamilyMarkImgUpdate(FamilyIdx,imgData);
	return TRUE;
}

BOOL CFamilyMarkManager::ReadBitInfoFromFile(char* filename,char* imgData)
{
//	FIBITMAP* pBitMap = FreeImage_LoadBMP(filename);
	FIBITMAP* pBitMap = FreeImage_Load(FIF_BMP, filename, BMP_DEFAULT);

	BITMAPINFO* pBitInfo = FreeImage_GetInfo(pBitMap);

	if( (pBitMap == 0) || (pBitInfo == 0) )
		return FALSE;
	//사이즈가 틀림
	if( pBitInfo->bmiHeader.biHeight != FAMILYMARKHEIGHT ||
		pBitInfo->bmiHeader.biWidth != FAMILYMARKWIDTH )
		return FALSE;

	FIBITMAP* pConvertedBitmap = FreeImage_ConvertTo16Bits565(pBitMap);

	int bytesPerLine = FAMILYMARKWIDTH*FAMILYMARKBPP;
	BYTE* pData = FreeImage_GetBits(pConvertedBitmap);
	for(int y=0;y<FAMILYMARKHEIGHT;++y)
	{
		memcpy(
			&imgData[(FAMILYMARKHEIGHT-y-1)*bytesPerLine],
			&pData[y*bytesPerLine],
			bytesPerLine);
	}
	
	FreeImage_Unload(pBitMap);
	FreeImage_Unload(pConvertedBitmap);

	return TRUE;
}


CFamilyMark* CFamilyMarkManager::LoadFamilyMarkImg( MARKNAMETYPE markname )
{
	CFamilyMarkImg* pMarkImg = LoadMarkImg( markname );
	if( pMarkImg == NULL )
		return NULL;

	CFamilyMark* pMark = new CFamilyMark;
	pMark->SetFamilyMarkImg(pMarkImg);
	m_MarkTable.Add(pMark,markname);
	return pMark;
}

