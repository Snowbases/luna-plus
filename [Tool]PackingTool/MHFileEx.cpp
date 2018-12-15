#define __MHFILEEX_C__

#include "stdafx.h"
#include "MHFileEx.h"



CMHFileEx::CMHFileEx()
{
	m_fp = NULL;
	memset( m_szFullFileName, 0, 256 );
	memset( m_szFileName, 0, 256 );
	memset( m_szExt, 0, 256 );
	memset( &m_Header, 0, sizeof(MHFILE_HEADER) );
	m_crc1 = m_crc2 = 0;
	m_pData = NULL;	
	m_pBinData = NULL;

	memset( &m_DOFHeader, 0, sizeof(DOF_HEADER) );
	memset( &m_DOFTail, 0, sizeof(DOF_TAIL) );

	strcpy( m_szDOF, "dof" );
}

CMHFileEx::~CMHFileEx()
{
	Clear();
}

void CMHFileEx::Clear()
{
	if( m_pData )
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	if( m_pBinData )
	{
		delete [] m_pBinData;
		m_pBinData = NULL;
	}
}

BOOL CMHFileEx::New( const char* fullfilename )
{
	Clear();
	m_pData = new char[MEGA];
	memset( m_pData, 0, MEGA );

	InitFileName( fullfilename );

	return TRUE;
}

BOOL CMHFileEx::OpenTxt( const char* fullfilename )
{
	InitFileName( fullfilename );

	//__asm int 3;
	m_fp = fopen( fullfilename, "rb" );
	if( m_fp )
	{
		Clear();
		m_pData = new char[MEGA];
		memset( m_pData, 0, MEGA );

		m_Header.dwDataSize = 0;
		while( !feof(m_fp) )
		{
			m_pData[m_Header.dwDataSize] = fgetc( m_fp );
			m_Header.dwDataSize++;
			if( m_Header.dwDataSize >= MEGA )
				break;
		}
		m_Header.dwDataSize--;
		m_pData[m_Header.dwDataSize] = 0;		
		
		m_DOFHeader.dwDataSize = m_Header.dwDataSize;

		fclose( m_fp );
	}

	return TRUE;
}

BOOL CMHFileEx::OpenBin( const char* fullfilename )
{
	if( !InitFileName( fullfilename ) )
		return FALSE;

	m_fp = fopen( fullfilename, "rb" );
// 20090515 ONS AES알고리즘을 사용하여 bin파일을 연다(START)
#ifdef _AES_FILE_
	if( m_fp )
	{
		/*w
			현재 버젼에 해당하는 키값이 있을경우 AES복호화를 하고, 없을 경우 기존의 방식으로 복호화한다.
		*/
		Clear();
		fread( &m_Header, sizeof(m_Header), 1, m_fp );	// header

		/*
			버젼을 검색하여 해당하는 키를 설정한다.
		*/
		if(!m_AESFile.SetBinVersion(m_Header.dwVersion - (m_Header.dwType+m_Header.dwDataSize)))
		{
			ASSERT(!"Bin-File Version Error!");
			MessageBox( NULL, "Bin-File Version Error!", NULL, MB_OK );
			return FALSE;
		}

		if(m_AESFile.IsExistKey())
		{
			/*
				AES복호화 알고리즘을 이용하여 파일을 연다.
			*/
			m_pData = new char[MEGA];
			if(m_pData == NULL)
			{
				ASSERT(!"memory allocation failed - m_pData");
				return FALSE;
			}
			memset( m_pData, 0, MEGA );

			if(!m_AESFile.AESDecryptData(m_fp, m_pData, m_Header.dwDataSize, m_Header.dwType))
			{
				char str[512];
				sprintf(str, "File Open Error : %s",fullfilename);
				MessageBox( NULL, str, NULL, MB_OK );
				return FALSE;
			}
			fclose( m_fp );
			m_fp = NULL;

		}
		else
		{
			/*
				기존의 디코딩 알고리즘을 이용하여 파일을 연다.
			*/
			fread( &m_crc1, sizeof(char), 1, m_fp );			// crc1
			if( m_Header.dwDataSize )						// data
			{
				m_pData = new char[MEGA];
				memset( m_pData, 0, MEGA );
				fread( m_pData, sizeof(char), m_Header.dwDataSize, m_fp );
				m_pData[m_Header.dwDataSize] = 0;
			}
			fread( &m_crc2, sizeof(char), 1, m_fp );			// crc2
			
			fclose( m_fp );
			m_fp = NULL;

			if( !CheckHeader() )	return FALSE;
			if( !CheckCRC() )		return FALSE;

			m_DOFHeader.dwDataSize = m_Header.dwDataSize;
		}
	}
	else
	{
		return FALSE;
	}
#else
	if( m_fp )
	{
		Clear();
		fread( &m_Header, sizeof(m_Header), 1, m_fp );	// header
		fread( &m_crc1, sizeof(char), 1, m_fp );		// crc1
		if( m_Header.dwDataSize )						// data
		{
			m_pData = new char[MEGA];
			memset( m_pData, 0, MEGA );
			fread( m_pData, sizeof(char), m_Header.dwDataSize, m_fp );
			m_pData[m_Header.dwDataSize] = 0;
		}
		fread( &m_crc2, sizeof(char), 1, m_fp );		// crc2
	}	
	fclose( m_fp );

	if( !CheckHeader() )	return FALSE;
	if( !CheckCRC() )		return FALSE;

	m_DOFHeader.dwDataSize = m_Header.dwDataSize;
#endif
// 20090515 ONS AES알고리즘을 사용하여 bin파일을 연다(END)

	// desc_hseos_주석기능_01
	// S 주석기능 추가 added by hseos 2007.05.09
	// ..bin 파일의 경우 bin.txt 파일이 있을 경우에는 bin.txt 파일을 열도록 한다.
	// ..없을 경우엔 그냥 bin 을 연다.
	// ..bin.txt 파일은 주석이 포함된 텍스트 파일이다. 주석 기능을 사용하기 위해 추가된 코드임.
	/* 보류
	char szTxt[2048];

	sprintf(szTxt, "%s.txt", fullfilename);
	FILE* fp = fopen(szTxt, "rt");
	if (fp)
	{
		// bin에서 읽어온 데이터
		char* pszBinData = new char[MEGA];
 		ZeroMemory(pszBinData, MEGA);
		memcpy(pszBinData, m_pData, strlen(m_pData));

		// txt에서 데이터를 읽는다.
		memset( m_pData, 0, MEGA );
		int nCnt = 0;
		int nSize = 0;
		while(TRUE)
		{
			nSize = fread( &m_pData[nCnt], sizeof(char), 1, fp );
			if (nSize == 0) break;
			nCnt++;
		}

		// txt에서 읽어온 데이터에서 주석을 삭제할 데이터
		char* pszTxtData = new char[MEGA];
 		ZeroMemory(pszTxtData, MEGA);
		memcpy(pszTxtData, m_pData, strlen(m_pData));

		// 주석 제거
		char* pForTxt = pszTxtData;
		int nLen = strlen(pszTxtData);
		while(*pszTxtData)
		{
			if (*pszTxtData == '/' && *(pszTxtData+1) == '/')
			{
				// 삭제해야 할(처리하지 않아야 할) 문자의 개수를 구한다.
				int nCnt = 0;
 				while(*(pszTxtData+nCnt) != '\n')
				{
					nCnt++;
				}
				nCnt++;

				// 구한 개수만큼 건너뛰어서 덮어씌운다.
				for(int i=0; i<nLen; i++)
				{
					*(pszTxtData+i) = *(pszTxtData+i+nCnt);
				}
			}
			else
			{
				// 주석이 있었으면 뒤의 문자열을 땡겨 덮어씌었기 때문에 포인터를 증가하면 안 되고,
				// 주석이 없었을 때만 포인터를 증가해야 한다.
				pszTxtData++;
			}
		}
		pszTxtData = pForTxt;

		// bin 과 txt 의 순수 데이터 비교
		int nBinLen = strlen(pszBinData);
		int nTxtLen = strlen(pszTxtData);
		BOOL bWrong = FALSE;

		if (nBinLen == nTxtLen)
		{
			for(int i=0; i<nBinLen; i++)
			{
				if (pszBinData[i] != pszTxtData[i])
				{
					bWrong = TRUE;
					break;
				}
			}
		}
		else
		{
			bWrong = TRUE;
		}
		
		if (bWrong)
		{
			AfxMessageBox("bin 파일과 bin.txt 파일의 내용에 차이가 있습니다. bin 파일에 주석이 포함되어 있거나 잘못된 텍스트가 포함되어 있지 않은지 확인바랍니다.");
		}

		delete [] pszBinData;
		delete [] pszTxtData;
		fclose(fp);
	}
	*/
	// E 주석기능 추가 added by hseos 2007.05.09

	return TRUE;
}

BOOL CMHFileEx::SaveToTxt( const char* filename )
{
	m_fp = fopen( filename, "wb" );
	if( m_fp )
	if( m_pData )
		fwrite( m_pData, sizeof(char), m_Header.dwDataSize, m_fp );
	fclose( m_fp );

	return TRUE;
}

BOOL CMHFileEx::SaveToTxt()
{
	MakeFullFileName();
	m_fp = fopen( m_szFullFileName, "wb" );
	if( m_fp )
	if( m_pData )
		fwrite( m_pData, sizeof(char), m_Header.dwDataSize, m_fp );
	fclose( m_fp );

	return TRUE;
}

BOOL CMHFileEx::SaveToBin( const char* filename )
{
	if( m_pData )
	{
		m_fp = fopen( filename, "wb" );
		if( m_fp )
		{
// 20090515 ONS AES알고리즘을 사용하여 bin파일을 암호화한다(START)
#ifdef _AES_FILE_
			char fname[512];
			m_pBinData = new char[m_Header.dwDataSize+1];
			memcpy( m_pBinData, m_pData, m_Header.dwDataSize+1 );
			m_pBinData[m_Header.dwDataSize] = 0;
			
			strcpy(fname, filename);
			// AES 알고리즘을 이용하여 bin데이터를 암호화 한후, 파일로 저장한다.
			if(!m_AESFile.AESEncryptData(m_fp, fname, m_pBinData, m_Header.dwDataSize))
			{
				MessageBox( NULL, "Failed Save To Bin", NULL, MB_OK );
				return FALSE;
			}
#else
			ConvertBin();
			fwrite( &m_Header, sizeof(m_Header), 1, m_fp );
			fwrite( &m_crc1, sizeof( char ), 1, m_fp );
			fwrite( m_pBinData, sizeof( char ), m_Header.dwDataSize, m_fp );
			fwrite( &m_crc1, sizeof( char ), 1, m_fp );
#endif
// 20090515 ONS AES알고리즘을 사용하여 bin파일을 암호화/복호화한다(END)
		}
		fclose( m_fp );
	}

	return TRUE;
}

BOOL CMHFileEx::SaveToBin()
{
	MakeFullFileName();
	return SaveToBin( m_szFullFileName );
}

BOOL CMHFileEx::CheckHeader()
{
	if( m_Header.dwVersion != (20040308+m_Header.dwType+m_Header.dwDataSize) )
	{
		Clear();
		MessageBox( NULL, "Header Version Error!!", "Error!!", MB_OK );
		return FALSE;
	}
	if( m_Header.dwType == 0 )
	{
		Clear();
		MessageBox( NULL, "Header Type Error!!", "Error!!", MB_OK );
		return FALSE;
	}
	if( m_Header.dwDataSize > MEGA )
	{
		Clear();
		MessageBox( NULL, "Header DataSize Error!!", "Error!!", MB_OK );
		return FALSE;
	}

	return TRUE;
}

BOOL CMHFileEx::CheckCRC()
{
	if( m_crc1 != m_crc2 )
	{
		Clear();
		MessageBox( NULL, "CheckCrc Error!!", "Error!!", MB_OK );
		return FALSE;
	}

	// decode
	char crc = m_Header.dwType;
	for( DWORD i = 0; i < m_Header.dwDataSize; ++i )
	{
		crc += m_pData[i];
		m_pData[i] -= (char)i;
		if( i%m_Header.dwType == 0 )
			m_pData[i] -= m_Header.dwType;
	}

	if( m_crc1 != crc )
	{
		Clear();
		MessageBox( NULL, "CheckCrc Error!!", "Error!!", MB_OK );
		return FALSE;
	}

#ifdef _CHINA
	char temp[64] = {0, };
	strncpy( temp, m_pData, 11 );
	if( strcmp( strupr(temp), "*TESTSERVER" ) != 0 &&
		strcmp( strupr(temp), "*USERSERVER" ) != 0 )
		return FALSE;
#endif

	return TRUE;
}

BOOL CMHFileEx::ConvertBin()
{
	// encode
	m_Header.dwVersion = 20040308;
	if( m_Header.dwDataSize == 0 )
		m_Header.dwType = 1;
	else
		m_Header.dwType = rand()%m_Header.dwDataSize + 1;
	m_Header.dwVersion += (m_Header.dwType+m_Header.dwDataSize);

	m_pBinData = new char[m_Header.dwDataSize+1];
	memcpy( m_pBinData, m_pData, m_Header.dwDataSize+1 );
	m_pBinData[m_Header.dwDataSize] = 0;

	char crc = m_Header.dwType;
	for( DWORD i = 0; i < m_Header.dwDataSize; ++i )
	{
		m_pBinData[i] += (char)i;		
		if( i%m_Header.dwType == 0 )
			m_pBinData[i] += m_Header.dwType;
		crc += m_pBinData[i];
	}

	m_crc1 = m_crc2 = crc;

	return TRUE;
}

BOOL CMHFileEx::InitFileName( const char* fullfilename )
{
	strcpy( m_szFullFileName, fullfilename );

	int len = strlen(m_szFullFileName);
	int i, j, k;

	for( i = len-1; i >= 0; --i )
	{
		if( m_szFullFileName[i] == '.' )
			j = i;
		if( m_szFullFileName[i] == '\\' )
		{
			k = i;
			break;
		}
	}

	for( i = 0; i < len-j; ++i )
		m_szExt[i] = m_szFullFileName[j+1+i];
	for( i = 0; i < len-k; ++i )
		m_szFileName[i] = m_szFullFileName[k+1+i];	

#ifdef _CHINA
	if( strcmp( strupr(m_szFileName), "SERVERLIST.BIN" ) != 0 )
		return FALSE;
#endif

	return TRUE;
}

void CMHFileEx::MakeFullFileName()
{
	int len = strlen(m_szFullFileName);
	int lenext = strlen(m_szExt);
	int i, j;

	for( i = len-1; i >= 0; --i )
	{
		if( m_szFullFileName[i] == '.' )
		{
			j = i;
			break;
		}
	}
	for( i = 0; i < lenext; ++i )
		m_szFullFileName[j+1+i] = m_szExt[i];

	m_szFullFileName[j+1+i] = 0;
}

void CMHFileEx::SetData( char* str )
{
	if( m_pData )
	{
		memcpy( m_pData, str, MEGA );
		m_Header.dwDataSize = strlen( str );
	}
}

BOOL CMHFileEx::MakeDOFName()
{
	int len = strlen(m_szFullFileName);
	int lenext = strlen(m_szDOF);
	int i, j;

	for( i = len-1; i >= 0; --i )
	{
		if( m_szFullFileName[i] == '.' )
		{
			j = i;
			break;
		}
	}
	for( i = 0; i < lenext; ++i )
		m_szFullFileName[j+1+i] = m_szDOF[i];

	m_szFullFileName[j+1+i] = 0;

	return TRUE;
}

BOOL CMHFileEx::SaveToDOF()
{
	MakeDOFName();
	return SaveToDOF( m_szFullFileName );
}

BOOL CMHFileEx::SaveToDOF( const char* filename )
{
	if( m_pData )
	{
		m_fp = fopen( filename, "wb" );
		if( m_fp )
		{
			ConvertDOF();
			fwrite( &m_DOFHeader, sizeof(m_DOFHeader), 1, m_fp );
			fwrite( &m_crc1, sizeof( char ), 1, m_fp );
			fwrite( m_pBinData, sizeof( char ), m_DOFHeader.dwDataSize, m_fp );
			fwrite( &m_DOFTail, sizeof( m_DOFTail ), 1, m_fp );
		}
		fclose( m_fp );
	}

	return TRUE;
}

BOOL CMHFileEx::ConvertDOF()
{
	// encode
	m_DOFHeader.dwSeed = rand()%m_DOFHeader.dwDataSize;
	m_DOFTail.dwSeed = rand()%m_DOFHeader.dwDataSize;
	m_DOFHeader.dwVersion = (DWORD)DOFHEADER + m_DOFHeader.dwSeed + m_DOFHeader.dwDataSize;
	m_DOFTail.dwVersion = (DWORD)DOFTAIL + m_DOFTail.dwSeed;

	m_pBinData = new char[m_DOFHeader.dwDataSize];
	memcpy( m_pBinData, m_pData, m_DOFHeader.dwDataSize );
//	m_pBinData[m_DSOFHeader.dwDataSize] = 0;

	m_crc1 = 0;
	for( DWORD i = 0; i < m_DOFHeader.dwDataSize; ++i )
	{
		m_pBinData[i] += ((char)i + m_DOFHeader.dwSeed + m_DOFTail.dwSeed);
		m_crc1 += m_pBinData[i];
	}

	return TRUE;
}

BOOL CMHFileEx::OpenDOF( const char* fullfilename )
{
	if( !InitFileName( fullfilename ) )
		return FALSE;

	m_fp = fopen( fullfilename, "rb" );
	if( m_fp )
	{
		Clear();
		fread( &m_DOFHeader, sizeof(m_DOFHeader), 1, m_fp );	// header
		fread( &m_crc1, sizeof(char), 1, m_fp );				// crc1
		if( m_DOFHeader.dwDataSize )							// data
		{
			m_pData = new char[MEGA];
			memset( m_pData, 0, MEGA );
			fread( m_pData, sizeof(char), m_DOFHeader.dwDataSize, m_fp );
			m_pData[m_DOFHeader.dwDataSize] = 0;
		}
		fread( &m_DOFTail, sizeof(m_DOFTail), 1, m_fp );		// tail
	}	
	fclose( m_fp );

	if( !CheckDOF() )	return FALSE;

	return TRUE;
}

BOOL CMHFileEx::CheckDOF()
{
	if( m_DOFHeader.dwVersion != ((DWORD)DOFHEADER + m_DOFHeader.dwSeed + m_DOFHeader.dwDataSize) )
	{
		Clear();
		MessageBox( NULL, "Header Version Error!!", "Error!!", MB_OK );
		return FALSE;
	}
	if( m_DOFHeader.dwSeed == 0 )
	{
		Clear();
		MessageBox( NULL, "Header Seed Error!!", "Error!!", MB_OK );
		return FALSE;
	}
	if( m_DOFHeader.dwDataSize > MEGA )
	{
		Clear();
		MessageBox( NULL, "Header DataSize Error!!", "Error!!", MB_OK );
		return FALSE;
	}
	if( m_DOFTail.dwVersion != ((DWORD)DOFTAIL + m_DOFTail.dwSeed) )
	{
		Clear();
		MessageBox( NULL, "Header Version Error!!", "Error!!", MB_OK );
		return FALSE;
	}

	// decode
	char crc = 0;
	for( DWORD i = 0; i < m_DOFHeader.dwDataSize; ++i )
	{
		crc += m_pData[i];
		m_pData[i] -= ((char)i + m_DOFHeader.dwSeed + m_DOFTail.dwSeed);
	}
	if( m_crc1 != crc )
	{
		Clear();
		MessageBox( NULL, "CheckCrc Error!!", "Error!!", MB_OK );
		return FALSE;
	}

	return TRUE;
}