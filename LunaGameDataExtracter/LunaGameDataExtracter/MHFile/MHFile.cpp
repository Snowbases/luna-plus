// MHFile.cpp: implementation of the CMHFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MHFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMHFile::CMHFile()
{
	fp = NULL;
	m_bReadMode = MHFILE_NORMALMODE;
	memset( &m_Header, 0, sizeof(m_Header) );
	m_crc1 = m_crc2 = 0;
	m_pData = NULL;
	m_Dfp = 0;
	m_bInit = FALSE;
}

CMHFile::~CMHFile()
{
	Release();
}


BOOL CMHFile::Init(char* filename,char* mode, DWORD dwFlag)
{
	if( strcmp( "rb", mode ) == 0 )
		m_bReadMode = MHFILE_PACKEDFILE;
	else
		m_bReadMode = MHFILE_NORMALMODE;

	if( m_bReadMode == MHFILE_PACKEDFILE )
		return OpenBin( filename );
	else if( m_bReadMode == MHFILE_NORMALMODE )
	{		
		fp = fopen(filename,mode);
		if(fp)
			return TRUE;
		else
		{
			if(!(dwFlag & MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR))
			{			
				/*char buffer[255];
				sprintf(buffer, "[%s] file is not found",filename);
				MessageBox(NULL, buffer, 0,0);*/
			}		
			return FALSE;
		}
	}

	return TRUE;
}

void CMHFile::Release()
{
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	Clear();
}

char* CMHFile::GetString()
{
	static char buf[256];
	GetString( buf );
	return buf;
}

void CMHFile::GetString(char* pBuf)
{
	if( m_bReadMode == MHFILE_PACKEDFILE )
	{
		DWORD j = 0;
		BOOL bStart, bEnd;
		bStart = bEnd = FALSE;
		
		if( m_Dfp == (int)m_Header.dwFileSize ) return;
		
		for( DWORD i = m_Dfp; i < m_Header.dwFileSize; ++i )
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
				m_Dfp++;
				if( bStart )
					bEnd = TRUE;
			}
			else
			{
				if( bEnd ) break;
				pBuf[j] = m_pData[i];
				j++;
				m_Dfp++;
				bStart = TRUE;
			}
		}
		pBuf[j] = 0;	
	}
	else if( m_bReadMode == MHFILE_NORMALMODE )
		fscanf(fp,"%s",pBuf);
}

void CMHFile::GetLine(char * pBuf, int len)
{
	if( m_bReadMode == MHFILE_PACKEDFILE )
	{		
		DWORD j = 0;
		
		for( DWORD i = m_Dfp; i < m_Header.dwFileSize; ++i )
		{
			if( m_pData[i] == 0x0d && m_pData[i+1] == 0x0a )	// return
			{
				m_Dfp += 2;
				break;
			}
			else
			{
				pBuf[j] = m_pData[i];
				j++;
				m_Dfp++;
			}
		}
		pBuf[j] = 0;
	}
	else if( m_bReadMode == MHFILE_NORMALMODE )
		fgets(pBuf, len, fp);
}

void CMHFile::GetLineX(char * pBuf, int len)
{
	if( m_bReadMode == MHFILE_PACKEDFILE )
	{
		DWORD j = 0;
		
		if( m_Dfp > 2 ) m_Dfp -= 2;
		for( DWORD i = m_Dfp; i < m_Header.dwFileSize; ++i )
		{
			if( m_pData[i] == 0x0d && m_pData[i+1] == 0x0a )	// return
			{
				m_Dfp += 2;
				break;
			}
			else
			{
				pBuf[j] = m_pData[i];
				j++;
				m_Dfp++;
			}
		}
		pBuf[j] = 0;
	}
	else if( m_bReadMode == MHFILE_NORMALMODE )
		fgets(pBuf, len, fp);
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
// 080603 LYW --- MHFile : 함수 return 타입 수정.
//BOOL CMHFile::GetExpPoint()
EXPTYPE CMHFile::GetExpPoint()
{
	return (EXPTYPE)atoi(GetString());
}
// 080603 LYW --- MHFile : __int64 길이의 숫자를 읽어들이는 함수 추가.
DWORDEX CMHFile::GetDwordEx()
{
	return (DWORDEX)_atoi64(GetString()) ;
}
BOOL CMHFile::GetLevel()
{
	return (LEVELTYPE)atoi(GetString());
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

BOOL CMHFile::IsEOF()
{
	if( m_bReadMode == MHFILE_PACKEDFILE )
		return (m_Header.dwFileSize == (DWORD)m_Dfp) ? TRUE : FALSE;
	else if( m_bReadMode == MHFILE_NORMALMODE )
		return feof(fp) ? TRUE : FALSE;

	return FALSE;
}

BOOL CMHFile::Seek(int n)
{
	if( m_bReadMode == MHFILE_PACKEDFILE )
		return TRUE;
	else if( m_bReadMode == MHFILE_NORMALMODE )
		return fseek(fp,n,SEEK_CUR);

	return TRUE;
}

char* CMHFile::GetStringInQuotation()
{
	char buf[256]={0,};
	static char buf2[256];
	int c;
	
	if( m_bReadMode == MHFILE_PACKEDFILE )
	{
		while( (c=m_pData[m_Dfp]) != '"' )	// 첫따옴표 나올때까지 스킵
		{
			++m_Dfp;
			if( c == 10 )					//첫따옴표 나오기전에 엔터가 있으면 중지
			{
				buf2[0] = 0;
				return buf2;
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
				buf[n] = 0;
				break;
			}
			++n;
		}

		memcpy( buf2, buf, n+1 );
	}
	else if( m_bReadMode == MHFILE_NORMALMODE )
	{
		while( (c=fgetc(fp)) != '"')	// 첫따옴표 나올때까지 스킵
		{
			if( c == 10 )//첫따옴표 나오기전에 엔터가 있으면 중지
			{
				Seek(-1);
				buf2[0] = 0;
				return buf2;
			}
		}
		int n=0;
		while(1)
		{
			ASSERT(!feof(fp));
			buf[n] = (char)fgetc(fp);
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

		memcpy( buf2, buf, n+1 );	
	}

	return buf2;
}

void CMHFile::GetStringInQuotation(char* pBuf)
{
	strcpy(pBuf,GetStringInQuotation());
}

void CMHFile::SetString(char * str)
{
	fprintf(fp, "%s", str);
}

// Binary File
BOOL CMHFile::OpenBin( char* filename )
{
	if( filename == NULL )
		return FALSE;

	int totalsize;
	fp = fopen( filename, "rb" );

#ifdef _AES_FILE_
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
		if(!m_AESFile.SetBinVersion(m_Header.dwVersion - (m_Header.dwType+m_Header.dwFileSize)))
		{
			MessageBox(
				NULL,
				_T("Bin-File Version Error!"),
				NULL,
				MB_OK | MB_ICONERROR);
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
			memset( m_pData, 0, datasize );

			if(!m_AESFile.AESDecryptData(fp, m_pData, datasize, m_Header.dwType))
			{
				Clear();
				TCHAR str[MAX_PATH] = {0};
				_sntprintf(
					str,
					_countof(str),
					_T("File Open Error : %s"),
					filename);
				MessageBox(
					NULL,
					str,
					NULL,
					MB_OK | MB_ICONERROR);
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
			if( m_Header.dwFileSize )						// data
			{
				m_pData = new char[m_Header.dwFileSize];
				fread( m_pData, sizeof(char), m_Header.dwFileSize, fp );
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
#else
	if( fp )
	{
		Clear();
		fread( &m_Header, sizeof(m_Header), 1, fp );	// header
		fread( &m_crc1, sizeof(char), 1, fp );			// crc1
		if( m_Header.dwFileSize )						// data
		{
			m_pData = new char[m_Header.dwFileSize];
			fread( m_pData, sizeof(char), m_Header.dwFileSize, fp );
		}
		fread( &m_crc2, sizeof(char), 1, fp );			// crc2
	}	
	else
	{
		return FALSE;
	}
	fclose( fp );
	fp = NULL;

	if( !CheckHeader() )	return FALSE;
	if( !CheckCRC() )		return FALSE;
#endif

	m_bInit = TRUE;

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

BOOL CMHFile::CheckHeader()
{
	if( m_Header.dwVersion != (DWORD)(20040308+m_Header.dwType+m_Header.dwFileSize) )
	{
		Clear();
		MessageBox(
			NULL,
			_T("Header Version Error!"),
			_T("Error!!"),
			MB_OK | MB_ICONERROR);
		return FALSE;
	}
	if( m_Header.dwType == 0 )
	{
		Clear();
		MessageBox(
			NULL,
			_T("Header Type Error!"),
			_T("Error!!"),
			MB_OK | MB_ICONERROR);
		return FALSE;
	}	

	return TRUE;
}

BOOL CMHFile::CheckCRC()
{
	if( m_crc1 != m_crc2 )
	{
		Clear();
		MessageBox(
			NULL,
			_T("CheckCrc Error!"),
			_T("Alert"),
			MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// decode
	char crc = (char)m_Header.dwType;
	for( DWORD i = 0; i < m_Header.dwFileSize; ++i )
	{
		crc = char( crc + m_pData[i] );
		m_pData[i] = char( m_pData[i] - (char)i);
		if( i%m_Header.dwType == 0 )
			m_pData[i] = char(m_pData[i] - (char)m_Header.dwType);
	}

	if( m_crc1 != crc )
	{
		Clear();
		MessageBox(
			NULL,
			_T("CheckCrc Error!"),
			_T("Alert"),
			MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

BOOL CMHFile::IsInited()
{
	if( m_bReadMode == MHFILE_PACKEDFILE )
		return m_bInit ? TRUE : FALSE;
	else if( m_bReadMode == MHFILE_NORMALMODE )
		return fp ? TRUE : FALSE;

	return FALSE;
}
/*
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
}*/