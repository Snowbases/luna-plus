// MHFile.h: interface for the CMHFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHFILE_H__CF584ED8_B10C_4E1B_A1CF_4E098D34E802__INCLUDED_)
#define AFX_MHFILE_H__CF584ED8_B10C_4E1B_A1CF_4E098D34E802__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

interface I4DyuchiFileStorage;

#ifdef _AES_FILE_
#include "AESFile.h"
#endif

#define MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR		0x00000001
#define MHFILE_FLAG_DONOTCHANGE_DIRECTORY_TOROOT	0x00000010

/*
struct MHFILE_HEADER
{
	DWORD	Version;	// version
	DWORD	Type;		// file Á¾·ù
	DWORD	FileSize;	// file size
	DWORD	Etc1;		// ¿¹ºñ data
	DWORD	Etc2;
};
*/
struct MHFILE_HEADER
{
	DWORD	dwVersion;	// version
	DWORD	dwType;		// file Á¾·ù
	DWORD	FileSize;	// data size
};

enum{
	MHFILE_NORMALMODE,
	MHFILE_PACKEDFILE,
	MHFILE_ENGINEPACKEDFILE,
};
class line_node;

class CMHFile  
{
	FILE* fp;
public:
	CMHFile();
	virtual ~CMHFile();

	BOOL Init( LPCTSTR filename, LPCTSTR mode,DWORD dwFlag = 0 );
	void Release();

	char* GetStringInQuotation();
	void GetStringInQuotation(char* pBuf);
	char* GetString();
	int GetString(char* pBuf);
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.10.08
	// void GetLine(char * pBuf, int len);
	void GetLine(char * pBuf, int len, BOOL bParseKewordString = TRUE);
	// E 농장시스템 추가 added by hseos 2007.10.08
	void GetLineX(char * pBuf, int len);	// ÁÖ¼®Ã³¸® µÈ °÷ ÀÐÀ»¶§ »ç¿ë
	int GetInt();
	LONG GetLong();
	float GetFloat();
	DWORD GetDword();
	WORD GetWord();
	BYTE GetByte();
	BOOL GetBool();
	BOOL GetHex(DWORD* pOut);
	// 080603 LYW --- MHFile : 함수 return 타입 수정.
	//BOOL GetExpPoint();
	EXPTYPE GetExpPoint();
	DWORDEX GetDwordEx() ;
	BOOL GetLevel();
	
	void GetFloat(float* pArray,int count);
	void GetWord(WORD* pArray,int count);
	void GetDword(DWORD* pArray,int count);
		
	int GetWord(WORD* pOut);

	BOOL Seek(int n);
	

	BOOL IsEOF();

	BOOL IsInited();

// jsd - binary file ÀÐ±â À§ÇØ
protected:
	// yh
	DWORD			m_bReadMode;
	
	// ¿£ÁøÆÑÅ·¿ë
	I4DyuchiFileStorage* m_pFileStorage;
	void* m_pFilePointer;

	int				m_Dfp;
	MHFILE_HEADER	m_Header;				// ÆÄÀÏ Çì´õ
	char			m_crc1;					// ¾ÏÈ£È­ °Ë»ç µ¥ÀÌÅÍ
	char			m_crc2;					// ¾ÏÈ£È­ °Ë»ç µ¥ÀÌÅÍ
	char*			m_pData;				// ½ÇÁ¦ µ¥ÀÌÅÍ
	BOOL			m_bInit;
	char			m_sFileName[MAX_PATH];

public:
	BOOL	OpenBin( LPCTSTR filename );
	void	Clear();
	BOOL	CheckHeader();
	BOOL	CheckCRC();
	char*	GetFileName()	{ return m_sFileName; }
	char*	GetData()		{ return m_pData; }
	DWORD	GetDataSize()	{ return m_Header.FileSize; }
	// 090109 LUJ, CRC값을 반환한다
	char	GetCrc1() const	{ return m_crc1; }
	char	GetCrc2() const	{ return m_crc2; }

#ifdef _AES_FILE_
private:
	cAESFile m_AESFile;
#endif
	char mBuffer[ MAX_PATH ];
};

void _parsingKeywordString( const char* in, char* out);
//void _parsingKeywordStringEx( const char* inString, line_node ** Top);

#endif // !defined(AFX_MHFILE_H__CF584ED8_B10C_4E1B_A1CF_4E098D34E802__INCLUDED_)
