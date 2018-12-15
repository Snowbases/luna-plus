#include "StdAfx.h"
#include "MHFile.h"
#include "Engine/GraphicEngine.h"

CMHFile::CMHFile()
{
	fp = NULL;
	memset( &m_Header, 0, sizeof(m_Header) );
	m_crc1 = m_crc2 = 0;
	m_pData = NULL;
	m_Dfp = 0;
	m_bInit = FALSE;
	memset( m_sFileName, 0, MAX_PATH );

	m_pFileStorage = 0;
	m_pFilePointer = 0;
}

CMHFile::~CMHFile()
{
	Release();
}

BOOL CMHFile::OpenBin( LPCTSTR filename )
{
	if( filename == NULL )
		return FALSE;

	int totalsize;
	fp = fopen( filename, "rb" );
	strcpy(m_sFileName,filename);

	if( fp )
	{
		fseek(fp,0,SEEK_END);
		totalsize = ftell(fp);
		fseek(fp,0,SEEK_SET);
		/*
		현재 버젼에 해당하는 키값이 있을경우 AES복호화를 하고, 없을 경우 기존의 방식으로 복호화한다.
		*/
		Clear();
		fread( &m_Header, sizeof(m_Header), 1, fp );	// header

		/*
		버젼을 검색하여 해당하는 키를 설정한다.
		*/
		if(!m_AESFile.SetBinVersion(m_Header.dwVersion - (m_Header.dwType+m_Header.FileSize)))
		{
			ASSERT(!"Bin-File Version Error!");
			char str[256];
			sprintf(str,"Bin-File Version Error! - %s", filename);
			MessageBox( NULL, str, NULL, MB_OK );
			return FALSE;
		}

		if(m_AESFile.IsExistKey())
		{
			/*
			AES복호화 알고리즘을 이용하여 파일을 연다.
			*/
			int datasize = totalsize - sizeof(m_Header)-2;

			m_pData = new char[datasize];
			if(m_pData == NULL)
			{
				ASSERT(!"memory allocation failed - m_pData");
				return FALSE;
			}
			memset( m_pData, 0, datasize);

			if(!m_AESFile.AESDecryptData(fp, m_pData, datasize, m_Header.dwType))
			{
				char str[512];
				sprintf(str, "File Open Error : %s",filename);
				MessageBox( NULL, str, NULL, MB_OK );
				return FALSE;
			}
			fclose( fp );
			fp = NULL;
		}
		else
		{
			/*
			기존의 디코딩 알고리즘을 이용하여 파일을 연다.
			*/
			fread( &m_crc1, sizeof(char), 1, fp );			// crc1
			if( m_Header.FileSize )						// data
			{
				m_pData = new char[m_Header.FileSize];
				fread( m_pData, sizeof(char), m_Header.FileSize, fp );
			}
			fread( &m_crc2, sizeof(char), 1, fp );			// crc2

			fclose( fp );
			fp = NULL;

			if( !CheckHeader() )	return FALSE;
			if( !CheckCRC() )		return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	m_bInit = TRUE;

	return TRUE;
}

BOOL CMHFile::CheckHeader()
{
	if( ( m_Header.dwVersion != (DWORD)(20040308+m_Header.dwType+m_Header.FileSize) ))
	{
		Clear();
		MessageBox( NULL, "Header Version Error!!", m_sFileName, MB_OK );
		return FALSE;
	}
	if( m_Header.dwType == 0 )
	{
		Clear();
		MessageBox( NULL, "Header Type Error!!", m_sFileName, MB_OK );
		return FALSE;
	}	

	return TRUE;
}

BOOL CMHFile::CheckCRC()
{
	if( m_crc1 != m_crc2 )
	{
		Clear();
		MessageBox( NULL, "CheckCrc Error!!", "Error!!", MB_OK );
		return FALSE;
	}

	// decode
	char crc = (char)m_Header.dwType;
	for( DWORD i = 0; i < m_Header.FileSize; ++i )
	{
		crc += m_pData[i];
		m_pData[i] -= (char)i;
		if( i%m_Header.dwType == 0 )
			m_pData[i] -= (char)m_Header.dwType;
	}

	if( m_crc1 != crc )
	{
		Clear();
		MessageBox( NULL, "CheckCrc Error!!", "Error!!", MB_OK );
		return FALSE;
	}

	return TRUE;
}

void CMHFile::Clear()
{
	if( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	m_Dfp = 0;
	m_bInit = FALSE;
}

BOOL CMHFile::Init( LPCTSTR filename, LPCTSTR mode, DWORD dwFlag )
{
	if(!(dwFlag & MHFILE_FLAG_DONOTCHANGE_DIRECTORY_TOROOT))
		DIRECTORYMGR->SetLoadMode(eLM_Root);

	//	if( strcmp( filename, "./Data/Interface/2DImage/image_msg.bin" ) == 0 )
	//		int a = 1;
	//	printf( "%s\n", filename );	//KES DEBUG

	if(g_bUsingEnginePack)
	{
		if( strcmp( "rb", mode ) == 0 )		m_bReadMode = MHFILE_PACKEDFILE;
		else if( strcmp( "ET", mode ) == 0 )m_bReadMode = MHFILE_ENGINEPACKEDFILE;
		else if( strcmp( "EB", mode ) == 0 )m_bReadMode = MHFILE_ENGINEPACKEDFILE;
		else								m_bReadMode = MHFILE_NORMALMODE;
	}
	else
	{
		if( strcmp( "rb", mode ) == 0 )		m_bReadMode = MHFILE_PACKEDFILE;
		else								m_bReadMode = MHFILE_NORMALMODE;
	}


	ySWITCH(m_bReadMode)
		yCASE(MHFILE_PACKEDFILE)
		return OpenBin( filename );
	yCASE(MHFILE_NORMALMODE)
		fp = fopen(filename,"r");
	if(fp)
		return TRUE;
	else
	{
		if(!(dwFlag & MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR))
		{
			char buffer[255];
			sprintf(buffer, "[%s] file is not found",filename);
			LOGEX(buffer,PT_MESSAGEBOX);
		}

		return FALSE;
	}
	yCASE(MHFILE_ENGINEPACKEDFILE)
		g_pExecutive->GetFileStorage(&m_pFileStorage);
	m_pFilePointer = m_pFileStorage->FSOpenFile(
		LPTSTR( filename ),
		FSFILE_ACCESSMODE_TEXT );
	if(m_pFilePointer && m_pFileStorage)
		return TRUE;
	else
		return FALSE;
	yENDSWITCH

		return FALSE;
}

void CMHFile::Release()
{
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	Clear();

	if(m_pFilePointer)
	{
		m_pFileStorage->FSCloseFile(m_pFilePointer);
		m_pFileStorage->Release();
		m_pFilePointer = NULL;
		m_pFileStorage = NULL;
	}
}

char* CMHFile::GetString()
{
	ZeroMemory( mBuffer, sizeof( mBuffer ) );
	GetString( mBuffer );
	return mBuffer;
}

int CMHFile::GetString( char* pBuf )
{
	char buf[512] = {0,};
	DWORD j = 0;
	BOOL bStart, bEnd;
	bStart = bEnd = FALSE;
	// yh;
	int rt = 0;

	ySWITCH(m_bReadMode)
		yCASE(MHFILE_PACKEDFILE)
		if(m_Dfp == (int)m_Header.FileSize) return (rt=EOF);

	for( DWORD i = m_Dfp; i < m_Header.FileSize; ++i )
	{
		if( m_pData[i] == 0x0d && m_pData[i+1] == 0x0a )	// return
		{
			if( bStart )
				break;
			else
			{
				m_Dfp += 2;
				++i;
			}
		}
		else if( m_pData[i] == 0x20 || m_pData[i] == 0x09 )	// spacebar, tab
		{
			++m_Dfp;
			if( bStart )
				bEnd = TRUE;
		}
		else
		{
			if( bEnd ) break;

			buf[j] = m_pData[i];
			++j;
			++m_Dfp;
			bStart = TRUE;
		}
	}
	buf[j] = 0;
	yCASE(MHFILE_NORMALMODE)
		rt = fscanf( fp, "%s", buf );
	yCASE(MHFILE_ENGINEPACKEDFILE)
		rt = m_pFileStorage->FSScanf(m_pFilePointer,"%s",buf);

	yENDSWITCH

		//	( buf, (char**)(&pBuf) );
		_parsingKeywordString( buf, pBuf );

	return rt;
}

char* CMHFile::GetStringInQuotation()
{
	char buf[ MAX_PATH ] = { 0 };
	int c = 0;

	ySWITCH(m_bReadMode)
		yCASE(MHFILE_PACKEDFILE)
		//			m_Dfp--;
		while( (c=m_pData[m_Dfp]) != '"' )	// 첫따옴표 나올때까지 스킵
		{
			++m_Dfp;
			if( c == 10 )					//첫따옴표 나오기전에 엔터가 있으면 중지
			{
				mBuffer[ 0 ] = 0;
				return mBuffer;
			}
		}
		int n = 0;
		while( 1 )
		{
			++m_Dfp;
			buf[n] = m_pData[m_Dfp];
			if( buf[n] == '"' )
			{
				buf[n] = 0;
				++m_Dfp;
				break;
			}
			if( buf[n] == 10 )				// 엔터
			{
				__asm int 3;
				buf[n] = 0;
				break;
			}
			++n;
		}

		memcpy( mBuffer, buf, n+1 );	
		yCASE(MHFILE_NORMALMODE)
			while( (c=fgetc(fp)) != '"')	// 첫따옴표 나올때까지 스킵
			{
				if( c == 10 )//첫따옴표 나오기전에 엔터가 있으면 중지
				{
					Seek(-1);
					mBuffer[ 0 ] = 0;
					return mBuffer;
				}
			}
			int n=0;
			while(1)
			{
				ASSERT(!feof(fp));
				buf[n] = fgetc(fp);
				if(buf[n] == '"' )
				{
					buf[n] = 0;
					break;
				}
				if(buf[n] == 10) // 엔터
				{
					Seek(-1);
					buf[n] = 0;
					break;
				}
				++n;
			}

			memcpy( mBuffer, buf, n+1 );
			yCASE(MHFILE_ENGINEPACKEDFILE)
				ASSERT(0);
			// 삑!~ 지원안함

			yENDSWITCH

				return mBuffer;
}

void CMHFile::GetStringInQuotation(char* pBuf)
{
	strcpy(pBuf,GetStringInQuotation());
}

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.10.08
// void CMHFile::GetLine(char * pBuf, int len)
void CMHFile::GetLine(char * pBuf, int len, BOOL bParseKewordString)
// E 농장시스템 추가 added by hseos 2007.10.08
{
	char buf[2048] = {0,};
	DWORD j = 0;

	ySWITCH(m_bReadMode)
		yCASE(MHFILE_PACKEDFILE)
		for( DWORD i = m_Dfp; i < m_Header.FileSize; ++i )
		{
			if( m_pData[i] == 0x0d && m_pData[i+1] == 0x0a )	// return
			{
				m_Dfp += 2;
				break;
			}
			else
			{
				buf[j] = m_pData[i];
				j++;
				m_Dfp++;
			}
		}
		buf[j] = 0;
		yCASE(MHFILE_NORMALMODE)
			fgets(buf, 512, fp);
		if( buf[strlen(buf)-1] == 10 )
			buf[strlen(buf)-1] = 0;
		yCASE(MHFILE_ENGINEPACKEDFILE)
			ASSERT(0);
		// 삑!~ 지원안함

		yENDSWITCH

			//_parsingKeywordString(buf, &pBuf);

			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.10.08
			// ..sscanf 로 읽을 경우에는 읽기 전에 ^s 를 ' ' 로 변경하면 안 됨. 그래서 매개변수 추가
			// _parsingKeywordString(buf, pBuf);
			if (bParseKewordString)
				_parsingKeywordString(buf, pBuf);
			else
				// 080108 LUJ, 버퍼 크기만큼만 복사하도록 함
				_tcsncpy( pBuf, buf, len );
		//lstrcpy(pBuf, buf);
		// E 농장시스템 추가 added by hseos 2007.10.08
}

int CMHFile::GetInt()
{
	return atoi(GetString());
}

LONG CMHFile::GetLong()
{
	return (LONG)atoi(GetString());
}

float CMHFile::GetFloat()
{
	return (float)atof(GetString());
}

DWORD CMHFile::GetDword()
{
	return (DWORD)atoi(GetString());
}

WORD CMHFile::GetWord()
{
	return (WORD)atoi(GetString());
}

BYTE CMHFile::GetByte()
{
	return (BYTE)atoi(GetString());
}

BOOL CMHFile::GetBool()
{
	return (BOOL)atoi(GetString());
}

void CMHFile::GetFloat(float* pArray,int count)
{
	for(int n=0;n<count;++n)
	{
		pArray[n] = GetFloat();
	}
}

void CMHFile::GetWord(WORD* pArray,int count)
{
	for(int n=0;n<count;++n)
	{
		pArray[n] = GetWord();
	}
}

void CMHFile::GetDword(DWORD* pArray,int count)
{
	for(int n=0;n<count;++n)
	{
		pArray[n] = GetDword();
	}
}

int CMHFile::GetWord(WORD* pOut)
{
	ASSERT(m_bReadMode == MHFILE_ENGINEPACKEDFILE);

	return m_pFileStorage->FSScanf(m_pFilePointer,"%d",pOut);
}

BOOL CMHFile::IsEOF()
{
	ySWITCH(m_bReadMode)
		yCASE(MHFILE_PACKEDFILE)
		if( m_Header.FileSize == (DWORD)m_Dfp )
			return TRUE;
		else
			return FALSE;
	yCASE(MHFILE_NORMALMODE)
		return feof(fp) ? TRUE : FALSE;
	yCASE(MHFILE_ENGINEPACKEDFILE)
		ASSERT(0);
	// 삑!~ 지원안함

	yENDSWITCH

		return TRUE;
}

BOOL CMHFile::IsInited()
{
	ySWITCH(m_bReadMode)
		yCASE(MHFILE_PACKEDFILE)
		return m_bInit ? TRUE : FALSE;
	yCASE(MHFILE_NORMALMODE)
		return fp ? TRUE : FALSE;
	yCASE(MHFILE_ENGINEPACKEDFILE)
		ASSERT(0);
	// 삑!~ 지원안함

	yENDSWITCH

		return TRUE;
}
BOOL CMHFile::Seek(int n)
{
	ySWITCH(m_bReadMode)
		yCASE(MHFILE_PACKEDFILE)
		m_Dfp += n;
	return TRUE;
	yCASE(MHFILE_NORMALMODE)
		return fseek( fp, n, SEEK_CUR );
	yCASE(MHFILE_ENGINEPACKEDFILE)
		ASSERT(0);
	// 삑!~ 지원안함

	yENDSWITCH

		return TRUE;
}

void _parsingKeywordString( const char* in, char* out )
{
	char* head = out;

	while( *in )
	{
		//if(*in & 0x80)	//국외에서 문제다!
		if( IsDBCSLeadByte(*in) )
		{
			*out++ = *in++;
			*out++ = *in++;
		}
		else
		{
			switch( *in )
			{
			case TEXT_DELIMITER:
				{
					switch( *( ++in ) )
					{
					case TEXT_SPACECHAR:
						{	
							*out++ = ' ';
						}
						break;
					case TEXT_EMPTYCHAR:
						{
							*head = 0;
						}
						break;
					case TEXT_DELIMITER:	//KES 추가
						{
							*out++ = '^';
						}
						break;
					}

				}
				break;
			default:
				{
					*out++ = *in;
				}
			}

			++in;
		}
	}

	*out = 0;
}

void CMHFile::Save(LPCTSTR fileName, LPCTSTR text)
{
	FILE* const file = fopen(
		fileName,
		"wb");

	if(0 == file)
	{
		return;
	}

	char buffer[MAX_BUFFER_SIZE] = {0};
	_tcsncpy(
		buffer,
		text,
		_tcslen(text));

	cAESFile aesFile;
	if(FALSE == aesFile.AESEncryptData(
		file,
		"",
		buffer,
		_tcslen(text)))
	{
		OutputDebugString("There is error in CMHFile::Save()\n");
	}

	fclose(file);
}