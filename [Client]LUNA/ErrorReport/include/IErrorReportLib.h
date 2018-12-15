/*
	에러 리포트 라이브러리.
	만든자 : 이진영
	대충 릴리즈 날짜 : 2010-03-08


	ftp는 패시브모드로만 연결한다.
*/
#include <dbghelp.h>

struct INIT_DUMP_HANDLER
{
	int				iBuildNum;				// 덤프파일의 이름을 결정할 때 쓰일 빌드넘버.
	MINIDUMP_TYPE	dumpType;				// MiniDumpNormal : 가장 기본적인 덤프. 파일 크기가 가장작다. 기본 스택만 출력.
											// MiniDumpWithFullMemory : 가장 큰 덤프. 태스크메니저에 나오는 메모리사용량만큼의 크기를 가진다. 모든 메모리정보를 포함.
	char			szFtpServerIP[16];		// ftp 서버의 주소. 지정하지 않을 경우 dump만 생성.
	int				iFtpServerPort;			// ftp 서버의 포트.
	char			szUserName[16];			// ftp 유저 계정.		15자 제한.
	char			szPass[16];				// ftp 유저 암호.		15자 제한.

	HINSTANCE		hApplicationInstance;
	BOOL			bUseDescriptionDialog;		// 상황 묘사 창을 띄운다. 게임이 풀스크린으로 돌거나 할 땐 소용 없으므로 FALSE로 두길 추천.

};

// 초기화 함수.
void InitErrorDumpHandler( INIT_DUMP_HANDLER* pInitHandler);

// 익셉션 에러 핸들러.
// SetUnhandledExceptionFilter( ErrorDumpHandler); // 이렇게 설정하면 된다.
LONG WINAPI ErrorDumpHandler( EXCEPTION_POINTERS *pE);

// 임의로 보내기.
//void SendErrorDumpToServer(void);			// 코드 중간에 임의로 덤프를 보낼 경우 쓴다.

//
//// 아래는 예제 함수.
//// 요렇게 걸어놓으시면 됩니다.
//void inline InitExceptionFilter( HINSTANCE hInstance)
//{
//	INIT_DUMP_HANDLER init;
//	memset( &init, 0, sizeof(init));
//	init.iBuildNum	=	1;
//	init.dumpType	=	MiniDumpNormal;
//	strcpy_s( init.szFtpServerIP, 16, "211.232.63.107");
////	strcpy_s( init.szFtpServerIP, 16, "127.0.0.1");
//	init.iFtpServerPort	=	4321;
//	strcpy_s( init.szUserName, 16, "dumpdemo");
//	strcpy_s( init.szPass, 16, "dumpdemo");
//	init.hApplicationInstance = hInstance;
//	init.bUseDescriptionDialog = TRUE;
//
//	InitErrorDumpHandler( &init);		// szFtpServerIP == NULL 일 경우 dump 만 생성.
//	SetUnhandledExceptionFilter( ErrorDumpHandler);
//}
