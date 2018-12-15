#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: CNpcRecallMgr.h
//	DESC		: NPC를 소환하는 관리를 하는 매니져 클래스.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 18, 2008
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define NPCRECALLMGR	CNpcRecallMgr::GetInstance()

//#define _USE_NPCRECALL_ERRBOX_					// 이 매크로가 활성화 되어 있으면, 에러 발생시 메시지 박스를 띄우고,
												// 비활성화 시키면, Sworking 폴더 내, 에러 로그를 남긴다.
//-------------------------------------------------------------------------------------------------
//		The class CNpcRecallMgr.
//-------------------------------------------------------------------------------------------------
class CNpcRecallMgr
{
public :
	GETINSTANCE(CNpcRecallMgr) ;

	CNpcRecallMgr(void) ;														// 생성자 함수.
	~CNpcRecallMgr(void) ;														// 소멸자 함수.

	void MTOA_ChangeMap_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void CTOA_MapOut(DWORD dwIndex, char* pMsg, DWORD dwLength) ;

	void Throw_Error(char* szErr, char* szCaption) ;							// 에러 메시지 처리를 하는 함수.
	void WriteLog(char* pMsg) ;													// 로그를 남기는 처리를 하는 함수.

};
