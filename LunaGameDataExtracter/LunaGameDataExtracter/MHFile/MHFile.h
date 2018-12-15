// MHFile.h: interface for the CMHFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHFILE_H__CF584ED8_B10C_4E1B_A1CF_4E098D34E802__INCLUDED_)
#define AFX_MHFILE_H__CF584ED8_B10C_4E1B_A1CF_4E098D34E802__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _AES_FILE_
#include "AESFile.h"
#endif

#define MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR 0x00000001

struct MHFILE_HEADER
{
	DWORD	dwVersion;	// version
	DWORD	dwType;		// file Á¾·ù
	DWORD	dwFileSize;	// data size
};

enum
{
	MHFILE_NORMALMODE,
	MHFILE_PACKEDFILE,
	MHFILE_ENGINEPACKEDFILE,
};

class CMHFile  
{
	FILE* fp;
public:
	CMHFile();
	virtual ~CMHFile();

	BOOL Init(char* filename,char* mode, DWORD dwFlag = 0);
	void Release();

	char* GetStringInQuotation();
	void GetStringInQuotation(char* pBuf);
	char* GetString();
	void GetString(char* pBuf);
	void GetLine(char * pBuf, int len);
	void GetLineX(char * pBuf, int len);	// ÁÖ¼®Ã³¸® µÈ °÷ ÀÐÀ»¶§ »ç¿ë
	int GetInt();
	LONG GetLong();
	float GetFloat();
	DWORD GetDword();
	WORD GetWord();
	BYTE GetByte();
	BOOL GetBool();
	// 080603 LYW --- MHFile : 함수 return 타입 수정.
	//BOOL GetExpPoint();
	EXPTYPE GetExpPoint();
	// 080603 LYW --- MHFile : __int64 길이의 숫자를 읽어들이는 함수 추가.
	DWORDEX GetDwordEx() ;
	BOOL GetLevel();
	BOOL Seek(int n);	
	
	void GetFloat(float* pArray,int count);
	void GetWord(WORD* pArray,int count);
	void GetDword(DWORD* pArray,int count);

	BOOL IsEOF();

	
	BOOL IsInited();

	void SetString(char * str);
	
protected:	// JSD - binary file
	DWORD			m_bReadMode;
	int				m_Dfp;
	MHFILE_HEADER	m_Header;				// ÆÄÀÏ Çì´õ
	char			m_crc1;					// ¾ÏÈ£È­ °Ë»ç µ¥ÀÌÅÍ
	char			m_crc2;					// ¾ÏÈ£È­ °Ë»ç µ¥ÀÌÅÍ
	char*			m_pData;				// ½ÇÁ¦ µ¥ÀÌÅÍ
	BOOL			m_bInit;

public:
	BOOL	OpenBin( char* filename );
	void	Clear();
	BOOL	CheckHeader();
	BOOL	CheckCRC();
	char*	GetData()		{ return m_pData; }
	DWORD	GetDataSize()	{ return m_Header.dwFileSize; }
	// 090109 LUJ, CRC값을 반환한다
	char	GetCrc1() const	{ return m_crc1; }
	char	GetCrc2() const	{ return m_crc2; }
	
#ifdef _AES_FILE_
private:
	cAESFile m_AESFile;
#endif
};

//void _parsingKeywordString( const char* in, char* out);

#endif // !defined(AFX_MHFILE_H__CF584ED8_B10C_4E1B_A1CF_4E098D34E802__INCLUDED_)
