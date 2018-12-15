/*
	클래스명 : class AESFile

	설    명 : 기존의 Bin파일의 Decoding방식을 AES복호화 방식으로 변경하기 위한 클래스
*/
#pragma once
#ifdef _AES_FILE_

#define MAX_KEY_LENGTH		512


struct AESFILE_HEADER
{
	DWORD	dwVersion;	// version
	DWORD	dwType;		// fileType 
	DWORD	dwDataSize;	// data size
};


// Bin파일 헤더버젼정보를 저장하는 배열
const DWORD dwVersion[] = 
{
	20040308,
	20090506,		
};

// 헤더버젼에 대응하는 키값을 저장하는 배열
const unsigned char aKey[][MAX_KEY_LENGTH] = 
{
	"",
	"E76B2413958B00E193A1",
};



class cAESFile
{
	/* 누적된 버젼(키) 카운터 */ 
	int	m_nVersionCount;

	DWORD m_dwCurVersion;
	DWORD m_dwLastVersion;

	unsigned char m_aCurKey[MAX_KEY_LENGTH];
	unsigned char m_aLastKey[MAX_KEY_LENGTH];

public:
	/*
		Bin파일헤더의 버젼정보를 이용하여 해당하는 키가 있는지 검색한다.
		키가 존재하면 버젼과 키값을 저장한다.
	*/
	BOOL SetBinVersion( DWORD BinVersion );

	/*
		키값의 길이를 반환한다.
	*/
	BOOL IsExistKey();

public:
	cAESFile();
	virtual ~cAESFile();

	void Init();
	BOOL AESEncryptData(FILE* fp, char* pFileName, char* pData, DWORD dwDataSize);
	BOOL AESDecryptData(FILE* fp, char* pData, DWORD dwDataSize, DWORD dwType);

};

#endif