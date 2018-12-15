/*
080515 LUJ, libcurl을 이용하여 HTTP 프로토콜로 파일을 다운로드받는다
			관련된 함수, 변수가 클래스 안에 패키징되어 있다.
			
			하나의 스레드에서 참조하므로, 별도의 잠금은 설정하지 않는다
*/
#pragma once
//090819 ONS 퍼플런처UI에 전송속도와 남은시간을 표시.
typedef void ( *PursuitFunction )( double percent, double remaintime, double down_speed );
typedef void CURL;

class CZipMemFile;

class CNetwork
{
public:
	static CNetwork& GetInstance();
	virtual ~CNetwork(void);

	static int WriteToMemory( char* data, size_t size, size_t nmemb, void* );
	static int WriteToDisk( char* data, size_t size, size_t nmemb, void* );
	static int Pursuit( LPVOID p, double doubleTotal, double doubleNow, double unsignedLongTotal, double unsignedLongNow );
	// 081201 LUJ, 다운로드 결과를 반환하도록 한다
	// 081202 LUJ, 다운로드를 메모리에 받을 수 있도록 한다
	static BOOL Download( const TCHAR* path, const TCHAR* fileName, PursuitFunction callBackFunctionPursuit, bool isMemoryDownload );
	// 080625 LUJ, DLL 초기화
	bool Initialize();
	void Release();

	CZipMemFile&	GetMemoryFile() const;
	CFile&			GetDiskFile() const;

private:
	CNetwork(void);

private:
	CURL*	mCurl;
	// 080625 LUJ, libcurl.dll이 패치 파일에 포함되어 있음. 프로그램이 실행 중일 때 dll이 바인딩되어 있어
	//				압축해제할 때 오류가 발생함. 이를 피하기 위해 dll을 수동으로 제어하여, 압축 풀기 전에
	//				dll 사용을 해제하기위해 아래 변수 추가
	HMODULE	mLibraryModule;

	PursuitFunction mCallBackFunctionPursuit;

};
