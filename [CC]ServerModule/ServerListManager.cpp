#include "StdAfx.h"
#include "ServerListManager.h"
#include "..\[CC]ServerModule\MHFile.h"

GLOBALTON(CServerListManager)

CSimpleCodec::CSimpleCodec(char* filename,BOOL bEncoding,char* CodecKey)
{
	m_fp = fopen(filename,bEncoding ? "wb" : "rb");
	m_CurCodecKeyPos = 0;
	m_CodecKeyLen = strlen(CodecKey);
	m_CodecKeybuf = new char[m_CodecKeyLen+1];
	strcpy(m_CodecKeybuf,CodecKey);
}
CSimpleCodec::~CSimpleCodec()
{
	fclose(m_fp);
	delete [] m_CodecKeybuf;
}
BOOL CSimpleCodec::Encode(void* pBufIn,int length)
{
#define MAX_ENCODEBUF_LENGTH	1024
	char tempBuf[MAX_ENCODEBUF_LENGTH];
	ASSERT(length<=MAX_ENCODEBUF_LENGTH);

	memcpy(tempBuf,pBufIn,length);
	for(int n=0;n<length;++n,++m_CurCodecKeyPos)
	{
		if(m_CurCodecKeyPos==m_CodecKeyLen)
			m_CurCodecKeyPos = 0;
		tempBuf[n] = tempBuf[n] + m_CodecKeybuf[m_CurCodecKeyPos];
	}

	int wrotelen = fwrite(tempBuf,length,1,m_fp);
	ASSERT(wrotelen == 1);
	return (wrotelen == 1);
}
BOOL CSimpleCodec::Decode(void* pBufOut,int length)
{
	int readlen = fread(pBufOut,length,1,m_fp);
	if(readlen != 1)
		return FALSE;
	
	char* pchBufOut = (char*)pBufOut;
	for(int n=0;n<length;++n,++m_CurCodecKeyPos)
	{
		if(m_CurCodecKeyPos==m_CodecKeyLen)
			m_CurCodecKeyPos = 0;
		pchBufOut[n] = pchBufOut[n] - m_CodecKeybuf[m_CurCodecKeyPos];
	}

	return TRUE;
}

CServerListManager::CServerListManager()
{
	ZeroMemory(
		&mBillingInfo,
		sizeof(mBillingInfo));
	// LUJ, 웹런처 로그인 시 게임키로 사용한다
	mBillingInfo.dwConnectionIndex = 5;
}

CServerListManager::~CServerListManager()
{
	Clear();
}

void CServerListManager::Clear()
{
	PTRLISTSEARCHSTART(m_ServerInfoList,SERVERINFO*,pInfo)
		delete pInfo;
	PTRLISTSEARCHEND

	m_ServerInfoList.RemoveAll();
}

BOOL CServerListManager::Add(SERVERINFO* pInfo)
{
	SERVERINFO* pNewServerInfo = new SERVERINFO;
	memcpy(pNewServerInfo,pInfo,sizeof(SERVERINFO));
	m_ServerInfoList.AddTail(pNewServerInfo);

	return TRUE;
}

BOOL CServerListManager::Save(char* pFileName)
{
	BOOL rt;
	CSimpleCodec codec(pFileName,TRUE,"yunhozzang!#&");
	codec.Encode(&m_ServerSetNum,sizeof(m_ServerSetNum));
	PTRLISTSEARCHSTART(m_ServerInfoList,SERVERINFO*,pInfo)
		rt = codec.Encode(pInfo,sizeof(SERVERINFO));
		ASSERT(rt);
	PTRLISTSEARCHEND

	return TRUE;
}

BOOL CServerListManager::Load(char* pFileName)
{
	Clear();
	CSimpleCodec codec(pFileName,FALSE,"yunhozzang!#&");
	codec.Decode(&m_ServerSetNum,sizeof(m_ServerSetNum));
	SERVERINFO info;
	while(codec.Decode(&info,sizeof(SERVERINFO)))
	{
		if(info.wServerKind >= MAX_SERVER_KIND)
		{
			MessageBox( 0, _TEXT( "" ), 0, 0 );
		}
		
		Add(&info);
	}

	return TRUE;
}

BOOL CServerListManager::LoadFromText(const char* pFileName)
{
	Clear();

	CMHFile file;

	if(FALSE == file.Init((char*)pFileName, "r"))
	{
		return FALSE;
	}

	// 서버셋 번호를 디렉토리 이름에서 얻어낸다
	{
		TCHAR serverListFile[MAX_PATH] = {0};
		ConvertText(
			serverListFile,
			pFileName,
			_countof(serverListFile));

		TCHAR textPath[_MAX_PATH] = {0};
		_tfullpath(
			textPath,
			serverListFile,
			sizeof(textPath) / sizeof(*textPath));

		TCHAR textDrive[_MAX_DRIVE] = {0};
		TCHAR textDirectory[_MAX_DIR] = {0};
		TCHAR textFileName[_MAX_FNAME] = {0};
		TCHAR textExtension[_MAX_EXT] = {0};
		_tsplitpath(
			textPath,
			textDrive,
			textDirectory,
			textFileName,
			textExtension);

		LPCTSTR seperator = _T("\\/");

		for(LPCTSTR token = _tcstok(textDirectory, seperator);
			0 < token;
			token = _tcstok(0, seperator))
		{
			m_ServerSetNum = _ttoi(
				token);
		}
	}

	BOOL isComment = FALSE;

	while(FALSE == file.IsEOF())
	{
		char text[MAX_PATH] = {0};
		file.GetLine(
			text,
			_countof(text));
		const int length = strlen(
			text);

		// 중간의 주석 제거
		for(int i = 0; i < length - 1; ++i)
		{
			char& first = text[i];
			char& second = text[i+1];

			if('/' == first && '/' == second)
			{
				first = 0;
				break;
			}
			else if('/' == first && '*' == second)
			{
				first = 0;
				isComment = TRUE;
				break;
			}
			else if('*' == first && '/' == second)
			{
				first = ' ';
				second = ' ';
				isComment = FALSE;
				break;
			}
		}

		if(isComment)
		{
			continue;
		}

		const char* const textSeperator = " \t=:\n\r";
		const char* const token = strtok(
			text,
			textSeperator);

		if(0 == token)
		{
			continue;
		}
		else if(0 == stricmp("billing", token))
		{
			const char* const textAddress = strtok(
				0,
				textSeperator);
			const char* const textPort = strtok(
				0,
				textSeperator);

			ConvertText(
				mBillingInfo.szIPForServer,
				textAddress ? textAddress : "",
				_countof(mBillingInfo.szIPForServer));
			mBillingInfo.wPortForServer = WORD(atoi(
				textPort ? textPort : ""));
			continue;
		}
		else if(0 == stricmp("gameKey", token))
		{
			const char* const textKey = strtok(
				0,
				textSeperator);

			mBillingInfo.dwConnectionIndex = atoi(
				textKey ? textKey : "");
		}
		
		const char* const textServerKind = token;
		const char* const textMapIndex = strtok(
			0,
			textSeperator);
		const char* const textServerAddress = strtok(
			0,
			textSeperator);
		const char* const textServerPort = strtok(
			0,
			textSeperator);
		const char* const textUserAddress = strtok(
			0,
			textSeperator);
		const char* const textUserPort = strtok(
			0,
			textSeperator);

		if(0 == textServerAddress)
		{
			continue;
		}

		SERVERINFO info;
		ZeroMemory(
			&info,
			sizeof(info));
		info.wServerKind = WORD(
			atoi(textServerKind ? textServerKind : ""));
		info.wServerNum = WORD(
			atoi(textMapIndex ? textMapIndex : ""));

		ConvertText(
			info.szIPForServer,
			textServerAddress ? textServerAddress : "",
			_countof(info.szIPForServer));
		info.wPortForServer = WORD(
			atoi(textServerPort ? textServerPort : ""));

		ConvertText(
			info.szIPForUser,
			textUserAddress ? textUserAddress : "",
			_countof(info.szIPForUser));
		info.wPortForUser = WORD(
			atoi(textUserPort ? textUserPort : ""));

		if(FALSE == CheckIp(info.szIPForServer) ||
			FALSE == CheckIp(info.szIPForUser))
		{
			continue;
		}

		Add(
			&info);
	}

	return TRUE;
}

// 090303 nsoh 서버리스트 IP 유효범위 체크
BOOL CServerListManager::CheckIp(LPCTSTR pIp)
{
	int nCount = 0;
	TCHAR tempIp[16] = {0};
	_tcsncpy(
		tempIp,
		pIp,
		_countof(tempIp));
	int tempNum = _ttoi(_tcstok(tempIp, _T(".")));

	if(tempNum < 0 || tempNum > 255) return FALSE;

	while(nCount++ < 3)
	{
		tempNum = _ttoi(_tcstok(0, _T(".")));

		if(tempNum < 0 || tempNum > 255) return FALSE;
	}
	return TRUE;
}

SERVERINFO* CServerListManager::GetServerInfoByNum(DWORD ServerKind,WORD ServerNum)
{
	PTRLISTSEARCHSTART(m_ServerInfoList,SERVERINFO*,pInfo)
		if(pInfo->wServerKind != ServerKind)
			continue;
		if(pInfo->wServerNum == ServerNum)
			return pInfo;
	PTRLISTSEARCHEND

	return NULL;
}
SERVERINFO* CServerListManager::GetServerInfoByPort(DWORD ServerKind,WORD Port)
{
	PTRLISTSEARCHSTART(m_ServerInfoList,SERVERINFO*,pInfo)
		if(pInfo->wServerKind != ServerKind)
			continue;
		if(pInfo->wPortForServer == Port)
			return pInfo;
	PTRLISTSEARCHEND

	return NULL;
}
SERVERINFO* CServerListManager::GetServerInfoByIp(DWORD ServerKind, LPCTSTR ip)
{
	PTRLISTSEARCHSTART(m_ServerInfoList,SERVERINFO*,pInfo)
		if(pInfo->wServerKind != ServerKind)
			continue;
		if(_tcscmp(pInfo->szIPForServer,ip) == 0)
			return pInfo;
	PTRLISTSEARCHEND

	return NULL;
}

void CServerListManager::SetPositionHead()
{
	m_Pos = m_ServerInfoList.GetHeadPosition();
}

SERVERINFO* CServerListManager::GetNextServerInfo()
{
	if(m_Pos == NULL)
		return NULL;
	
	return (SERVERINFO*)m_ServerInfoList.GetNext(m_Pos);
}

void CServerListManager::ConvertText(TCHAR* lhs, const char* rhs, size_t size) const
{
#ifdef _UNICODE
	mbstowcs(
		lhs,
		rhs,
		size);
#else
	_tcsncpy(
		lhs,
		rhs,
		size);
#endif
}