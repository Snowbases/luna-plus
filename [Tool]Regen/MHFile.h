#pragma once

interface I4DyuchiFileStorage;

#define MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR		0x00000001
#define MHFILE_FLAG_DONOTCHANGE_DIRECTORY_TOROOT	0x00000010

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
	void GetLine(char * pBuf, int len, BOOL bParseKewordString = FALSE);
	int GetInt();
	LONG GetLong();
	float GetFloat();
	DWORD GetDword();
	WORD GetWord();
	BYTE GetByte();
	BOOL GetBool();
	void GetFloat(float* pArray,int count);
	void GetWord(WORD* pArray,int count);
	void GetDword(DWORD* pArray,int count);
	int GetWord(WORD* pOut);
	BOOL Seek(int n);
	BOOL IsEOF();
	BOOL IsInited();

protected:
	DWORD			m_bReadMode;
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
	static void Save(LPCTSTR fileName, LPCTSTR text);

private:
	cAESFile m_AESFile;
	char mBuffer[ MAX_PATH ];
};

void _parsingKeywordString( const char* in, char* out);