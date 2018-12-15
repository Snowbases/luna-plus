#pragma once


#define		PURPLE_OUTBOUND_INFORM					9
#define		PURPLE_OUTBOUND_DOWNLOAD_PROGRESS		18
#define		PURPLE_OUTBOUND_DOWNLOAD_SPEED			27
#define		PURPLE_OUTBOUND_DOWNLOAD_RESTTIME		31
#define		PURPLE_OUTBOUND_FILENAME				36
#define		PURPLE_OUTBOUND_PATCH_PROGRESS			45
#define		PURPLE_OUTBOUND_PATCH_FILE_NUMBER		54
#define		PURPLE_OUTBOUND_ENDING					63


typedef struct PurpleCDS
{
	TCHAR szString[160];
	DWORD dwIndex;
} PurpleCDS;

/**
 * 1. 외부모듈 기동할 때
 *		퍼플런쳐는 다음 내용을 시작인자로 전달한다.
 *		(1) HWND : SendMessage를 받을 윈도우 핸들. 10진수로 표시된다.
 *		(2) argument : 기타 외부모듈이 기동할 때 필요한 내용을 전달한다.
 *
 * 2. 퍼플런쳐/외부모듈 간의 연동
 *		(1) 퍼플런쳐 => 외부모듈로 통신하는 경우는 없으며 (최초에 인자를 전달하기만 한다), 
 *			이후에는 외부모듈 => 퍼플런쳐의 통신만을 한다. 
 *		(2) 통신방식을 WM_COPYDATA를 사용하며 메시지를 전달할 때에는 다음의 형태를 취한다. 
 *			::SendMessage(HWND, WM_COPYDATA, HWND, PCOPYDATASTRUCT);
 *				a. HWND : 퍼플런쳐 윈도우 핸들이다. 이 값은 외부모듈이 최초 기동할 때 인자로 전달된다.
 *				b. WM_COPYDATA : 윈도우 메시지
 *				c. HWND : 메시지를 리턴받을 윈도우 핸들을 기입한다. (이 값은 미래를 위해서 세팅한다.)
 *				d. PCOPYDATASTRUCT : WM_COPYDATA를 쓸 때 전송하는 구조체이다. 사용법은 MSDN 참고.
 *		
 * 						typedef struct tagCOPYDATASTRUCT {
 * 							ULONG_PTR dwData;
 *							DWORD cbData;
 *							PVOID lpData;
 *						} COPYDATASTRUCT, *PCOPYDATASTRUCT; 
 *
 * 3. 인덱스(dwData)에 따른 의미
 *		(1) PURPLE_OUTBOUND_INFORM : Inform 스태틱에 스트링을 표시한다.
 *			- szString : 스태틱에 표시할 스트링
 *			- dwIndex : N/A
 *		
 *		(2) PURPLE_OUTBOUND_DOWNLOAD_PROGRESS : 다운로드 프로그래스를 조정한다.
 *			- szString : N/A
 *			- dwIndex : 프로그래스 (0부터 100까지)
 *
 *		(3) PURPLE_OUTBOUND_DOWNLOAD_SPEED : 다운로드 속도를 표시한다.
 *			- szString : N/A
 *			- dwIndex : 다운로드 스피드 (byte / millisecond)
 *
 *		(3.1) PURPLE_OUTBOUND_DOWNLOAD_RESTTIME : 남은시간을 표시한다. 
 *			- szString : N/A
 *			- dwIndex : 남은시간 (millisecond)
 *
 *		(4) PURPLE_OUTBOUND_FILENAME : 현재 처리하는 파일의 이름을 표시한다. 
 *			- szString : 처리중인 파일명칭
 *			- dwIndex : N/A
 *
 *		(5) PURPLE_OUTBOUND_PATCH_PROGRESS : 패치 프로그래스를 조정한다. 패치 프로그래스는 전체설치공정을 표시한다.
 *			- szString : N/A
 *			- dwIndex : 프로그래스 (0부터 100까지)
 *
 *		(6) PURPLE_OUTBOUND_PATCH_FILE_NUMBER : 패치할 전체 파일 개수 및 현재까지 처리한 파일의 개수를 표시한다.
 *			- szString : N/A
 *			- dwIndex : HIWORD => 전체파일개수, LOWORD => 현재까지 처리한 파일 개수
 *
 *		(7) PURPLE_OUTBOUND_ENDING : 외부모듈이 종료됨을 전달한다. 
 *			- szString : 오류스트링
 *			- dwIndex : 오류코드
 *
 *			a. 오류코드 0 : 패치완료 후 정상종료됨. 퍼플런쳐는 이 메시지를 받으면 게임시작 stand by 상태가 된다.
 *			b. 오류코드 1 : 인스톨쉴드를 다시 받아야 함을 뜻한다. szString에 다운받아야 하는 파일 URL을 기록한다.
 *			c. 기타 오류코드 : 하나씩 지정한다. (예를 들면, dll 로드에 실패하였다. 패치파일 다운로드에 실패하였다등..)
**/














