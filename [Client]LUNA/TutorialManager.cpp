#include "stdafx.h"
#include "WindowIDEnum.h"
#include "GameIn.h"
#include "Audio/MHAudioManager.h"
#include "ObjectManager.h"
#include "Tutorialmanager.h"
#include "interface/cWindowManager.h"
#include "Interface/cStatic.h"
#include "InventoryExDialog.h"
#include "Item.h"
#include "Tutorial.h"
#include "Tutorial_Msg.h"
#include "TutorialDlg.h"
#include "TutorialBtnDlg.h"
#include "MHFile.h"

GLOBALTON(cTutorialManager)

#define CERR_MSG2		"Over angle bracket } : %d - %d"					// } 괄호 닫기가 잘못 되었다는 에러 메시지.
#define CERR_MSG3		"Over angle bracket { : %d - %d"					// { 괄호 열기가 잘못 되었다는 에러 메시지.	
#define CERR_MSG4		"Invalid parameter"									// 잘못된 인자가 넘어왔다는 에러 메시지.

// TUTORIAL PART.
#define TERR_MSG1		"Invalid file name~!!"								// 튜토리얼 스크립트 파일 명이 잘못 되었다는 에러 메시지.
#define TERR_MSG2		"Failed to open file : %s"							// 파일 열기에 실패 했다는 에러 메시지.
#define TERR_MSG3		"Invalid Tutorial Info : %d"						// 튜토리얼 정보가 잘못되었다는 에러 메시지.
#define TERR_MSG4		"Invalid Tutorial Index : %d"						// 튜토리얼 인덱스가 잘못되었다는 에러 메시지.
#define TERR_MSG5		"Invalid start condition 1 : %d"					// 제 1 발동 조건이 잘못되었다는 에러 메시지.
#define TERR_MSG6		"Failed to create tutorial : %d"					// 튜토리얼 생성에 실패 했다는 에러 메시지.
#define TERR_MSG7		"Invalid tutorial pointer"							// 인자로 넘어온 튜토리얼 포인터가 잘못되었다는 에러 메시지.
#define TERR_MSG8		"Invalid speech index : %d - %d"					// 대화 인덱스가 잘못되었다는 에러 메시지.
#define TERR_MSG10		"Invalid npc index : %d - %d"						// NPC 인덱스가 잘못되었다는 에러 메시지.
#define TERR_MSG11		"Invalid face index : %d - %d"						// 얼굴 인덱스가 잘못 되었다는 에러 메시지.
#define TERR_MSG12		"Invalid first condition index : %d - %d"			// 첫번째 완료 조건이 잘못되었다는 에러 메시지.
#define TERR_MSG13		"Invalid Sub Tutorial Info : %d - %d"				// 서브 튜토리얼 정보가 잘못 되었다는 에러 메시지.
#define TERR_MSG14		"Failed to create sub tutorial : %d - %d"			// 서트 튜토리얼 생성에 실패 했다는 에러 메시지.
#define TERR_MSG15		"Failed to receive completed tutorial : %d"			// 완료한 튜토리얼 정보를 받는데 실패 했다는 에러.
#define TERR_MSG16		"Failed to receive current sub tutorial"			// 현재 진행중인 서브 튜토리얼 받는데 실패 했다는 에러.
#define TERR_MSG17		"Failed to receive window id : %s"					// 지정된 문자열로 윈도우 아이디 받기 실패.

// TUTORIAL MESSAGE PART.
#define MERR_MSG1		"Over string - Msg %d"								// 메시지 길이를 넘었다는 에러 메시지.
#define MERR_MSG2		"Failed to create message - Msg %d"					// 메시지 길이를 넘었다는 에러 메시지.
#define MERR_MSG3		"Invalid message Info : %d"							// 잘못된 메시지 정보라는 에러 메시지.
#define MERR_MSG4		"Invalid message index : %d"						// 잘못된 메시지 인덱스라는 에러 메시지.

// PROCESS PART.
#define PERR_MSG1		"Invalid tutorial Info. Can't start tutorial! : %d"	// 잘못된 튜토리얼 정보로 인해 튜토리얼 진행을 하지 못한다는 에러.
#define PERR_MSG2		"Failed to receive npc info : %d npc"				// 몇 번 npc 정보를 받는데 실패 했다는 에러 메시지.
#define PERR_MSG3		"Invalid pointer pItem~!!"							// 인자로 넘어온 pItem이 잘못되었다는 에러.
#define PERR_MSG4		"Failed to receive inventory dialog."				// 인벤토리 다이얼로그를 얻지 못했다는 에러 메시지.
#define PERR_MSG5		"Failed to receive list dialog of help dialog."		// 도움말 창의 리스트 다이얼로그를 받지 못했다는 에러.

// NETWORK PART.
#define NERR_MSG1		"Failed to casting tutorial data in networpaser"	// 네트워크 파서에서 튜토리얼 데이터 형 변환에 실패 했다는 에러.
#define NERR_MSG2		"Invalid tutorial index"							// 잘못 된 튜토리얼 인덱스 에러.
#define NERR_MSG3		"Failed to receive tutorial position : %d"			// 몇 번 인덱스로 튜토리얼 정보를 받는데 실패 했다는 에러.
#define NERR_MSG4		"Failed to receive tutorial info : %d"				// 인덱스에 해당하는 튜토리얼 정보를 받는데 실패 했다는 에러.

cTutorialManager::cTutorialManager(void)
{
	m_bActiveTutorial = TRUE ;												// 튜토리얼을 활성화 한다.

	m_bUpdated = FALSE ;													// 업데이트 여부를 false로 세팅한다.

	m_nCurTutorialIdx	= 0 ;												// 현재 튜토리얼 인덱스를 0로 세팅한다.

	m_nShow_BtnSndIdx = 0 ;													// 버튼 출연 사운드 인덱스를 0으로 세팅한다.

	m_nEndTutorial_SndIdx = 0 ;												// 튜토리얼 완료 사운드 인덱스를 0으로 세팅한다.
}





//=========================================================================
//		튜토리얼 매니져 소멸자 함수.
//=========================================================================
cTutorialManager::~cTutorialManager(void)									// 소멸자 함수.
{
	Release() ;																// 해제 함수를 호출한다.
}





//=========================================================================
//		튜토리얼 매니져 초기화 함수.
//=========================================================================
void cTutorialManager::Init() 												// 초기화 함수.
{
	if( !LoadMsg("Data/Script/Tutorial/Tutorial_Msg.bin") )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Failed to load tutorial message", __FUNCTION__, MB_OK);
#endif
	}

	if( !LoadTutorial("Data/Script/Tutorial/Tutorial_Script.bin") )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Failed to load tutorial info", __FUNCTION__, MB_OK);
#endif
	}

	if( !LoadHelperInfo("Data/Script/Tutorial/Help_Link_List.bin") )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, "Failed to load helper info", __FUNCTION__, MB_OK);
#endif
	}
}





//=========================================================================
//		튜토리얼 매니져 해제 함수.
//=========================================================================
void cTutorialManager::Release() 											// 해제 함수.
{
	ReleaseMsg() ;															// 튜토리얼 메시지 해제 함수를 호출한다.
	ReleaseTutorial() ;														// 튜토리얼 정보 해제 함수를 호출한다.
	ReleaseHelperInfo() ;													// 도움말 정보 해제 함수를 호출한다.
}





//=========================================================================
//		튜토리얼 정보를 로딩하는 함수.
//=========================================================================
BOOL cTutorialManager::LoadTutorial(char* strFileName) 						// 튜토리얼 정보를 로딩하는 함수.
{
	CMHFile file ;															// 묵향 파일을 선언한다.

	if( !strFileName )														// 인자로 넘어온 파일명이 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, TERR_MSG1, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	if( !file.Init( strFileName, "rb") )
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG2, strFileName);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	int nCount = 0 ;														// 카운트 변수를 선언하고 0으로 초기화 한다.

	char Token[1024];														// 토큰 용 임시 버퍼를 선언한다.

	if( !file.IsEOF() )
	{
		m_nShow_BtnSndIdx = file.GetWord();
		m_nEndTutorial_SndIdx = file.GetWord();
	}

	while( !file.IsEOF() )													// 파일의 끝이 아닌 동안 while문을 돌린다.
	{
		memset(Token, 0, sizeof(Token)) ;									// 임시 버퍼를 초기화 한다.
		file.GetString( Token );											// 스트링을 읽어 임시 버퍼에 담는다.

		char commant ;
		memcpy(&commant, Token, sizeof(char)) ;

		if( strcmp(Token,"$TUTORIAL") == 0 )								// 읽어들인 스트링이 $TUTORIAL과 같다면,
		{
			cTutorial* pTutorial = NULL ;									// 튜토리얼 정보를 담을 포인터를 선언하고 null 처리 한다.
			pTutorial = LoadMainTutorial(&file) ;							// 메인 튜토리얼 정보를 로딩한다.

			if( !pTutorial )	return FALSE ;								// false return 처리를 한다.

			m_Tutorial_List.AddTail(pTutorial) ;							// 튜토리얼 리스트에 튜토리얼을 추가한다.

			++nCount ;														// 카운트 변수를 증가한다.
		}
		else if( strcmp(Token,"") == 0 )									// 읽어들인 스트링이 공란 이라면,
		{
			continue ;														// continue 처리를 한다.
		}
		else if( commant == '@' )
		{
			char commantBuf[1024] ;
			file.GetLine(commantBuf, sizeof(commantBuf)) ;
		}
		else																// TUTORIAL도 아니고, 공란도 아니면, 오타다~!!
		{	
#ifdef _GMTOOL_
			char tempBuf[1024] = {0};
			sprintf(tempBuf, TERR_MSG3, nCount);
			MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
			return FALSE;
		}
	}

	return TRUE ;															// true return 처리를 한다.
}





//=========================================================================
//		메인 튜토리얼 정보를 로딩하는 함수.
//=========================================================================
cTutorial* cTutorialManager::LoadMainTutorial(CMHFile* pFile) 				// 메인 튜토리얼 정보를 로딩하는 함수.
{
	if( !pFile )															// 인자로 넘어온 파일 포인터 정보가 유효하지 않으면,
	{
		return NULL ;														// null return 처리를 한다.
	}

	int nTutorialCount = 0 ;												// 튜토리얼의 카운트를 담을 변수를 선언하고 0으로 세팅한다.
	nTutorialCount = m_Tutorial_List.GetCount() ;							// 현재 튜토리얼 리스트의 카운트를 받는다.

	int nIndex ;															// 튜토리얼 인덱스를 담을 변수를 선언한다.

	int nStartCondition1 ;													// 제 1 발동 조건을 담을 변수를 선언한다.
	int nStartCondition2 ;													// 제 2 발동 조건을 담을 변수를 선언한다.

	// Setting index of tutorial.
	nIndex = -1 ;															// 튜토리얼 인덱스 변수를 -1로 세팅한다.
	nIndex = pFile->GetInt() ;												// 파일로 부터 튜토리얼 인덱스를 읽어들인다.

	if( nIndex < 0 || nIndex != nTutorialCount )							// 읽어들인 튜토리얼 인덱스가 0이하건, 튜토리얼 카운트와 같지 않으면,
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG4, nTutorialCount);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return 0;
	}

	// Setting first start condition of tutorial.
	nStartCondition1 = -1 ;													// 제 1 발동 조건 변수를 -1로 세팅한다.
	nStartCondition1 = pFile->GetInt() ;									// 파일로 부터 제 1 발동 조건을 읽어들인다.

	if( nStartCondition1 < 0 || nStartCondition1 >= e_MAX_SCONDITION)		// 제 1 발동 조건이 0이하거나 발동 조건 최대수를 넘으면,
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG5, nTutorialCount);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return 0;
	}

	// Setting second start condition of tutorial.
	nStartCondition2 = -1 ;													// 제 2 발동 조건 변수를 -1로 세팅한다.
	nStartCondition2 = pFile->GetInt() ;									// 파일로 부터 제 2 발동 조건을 읽어들인다.

	// Create tutorial.
	cTutorial* pTutorial = NULL ;											// 튜토리얼 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pTutorial = new cTutorial ;												// 튜토리얼을 생성하고, 튜토리얼 정보를 포인터로 받는다.

	if( !pTutorial )														// 튜토리얼 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG6, nTutorialCount);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return 0;
	}

	// Setting tutorial info.
	pTutorial->Set_Index(nIndex) ;											// 튜토리얼의 인덱스를 세팅한다.
	pTutorial->Set_StartCondition1(nStartCondition1) ;						// 제 1 발동 조건을 세팅한다.
	pTutorial->Set_StartCondition2(nStartCondition2) ;						// 제 2 발동 조건을 세팅한다.
    
	if( !LoadSubTutorial(pFile, pTutorial) ) return NULL ;					// 서브 튜토리얼 로딩에 실패하면, null return 처리를 한다.

	return pTutorial ;														// 튜토리얼 정보를 return 처리 한다.
}





//=========================================================================
//		서브 튜토리얼 정보를 로딩하는 함수.
//=========================================================================
BOOL cTutorialManager::LoadSubTutorial(CMHFile* pFile, cTutorial* pTutorial)// 서브 튜토리얼 정보를 로딩하는 함수.
{
	if( !pFile )															// 인자로 넘어온 파일 포인터가 유효하지 않으면,
	{
		return FALSE;
	}

	if( !pTutorial )														// 인자로 넘어온 튜토리얼 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, TERR_MSG7, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	int nMT_Idx = pTutorial->Get_Index();
	int nMsg_Count = m_Tutorial_Msg.GetCount();
	int nCount2 = 0;
	int nOpenCount = 0;
	int nCloseCount= 0;
	char Token[1024] = {0};

	while( !pFile->IsEOF() )												// 파일 포인터가 파일을 끝에 도달하지 않을 동안	while문을 돌린다.
	{
		memset(Token, 0, sizeof(Token)) ;									// 임시 버퍼를 초기화 한다.
		pFile->GetString( Token );											// 스트링을 읽어 임시 버퍼에 담는다.

		if( Token[0] == '}' )												// }와 같으면,
		{
			if( nCloseCount > 0 )											// 닫기 괄호가 0보다 크면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, CERR_MSG2, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			++nCloseCount ;													// 닫히는 괄호 카운트 수를 증가한다.
			break;															// while문을 탈출한다.
		}
		else if( Token[0] == '{' )											// {와 같으면,
		{
			if( nOpenCount > 0 )											// 열기 괄호가 0보다 크면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, CERR_MSG3, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			++nOpenCount ;													// 열리는 괄호 카운트 수를 증가한다.
			continue ;														// continue 처리를 한다.
		}
		else if( strcmp( Token, "$SUBTUTORIAL" ) == 0 )						// $SUBTUTORIAL과 같으면,
		{
			SUB_TUTORIAL stST ;												// 서브 튜토리얼 정보를 담을 구조체를 선언한다.
			memset(&stST, 0, sizeof(SUB_TUTORIAL)) ;						// 서브 튜토리얼 구조체를 초기화 한다.

			stST.nIndex = pTutorial->Get_SubTutorial_Count() ;				// 서브 튜토리얼의 인덱스를 세팅한다.

			stST.nSpeech_Idx = pFile->GetInt() ;							// 파일로 부터 튜토리얼 대사 인덱스를 받는다.
			if(stST.nSpeech_Idx < 0 || stST.nSpeech_Idx >= nMsg_Count)		// 대사 인덱스가 0보다 작거나, 최대 메시지 카운트 이상이면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, TERR_MSG8, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			stST.nSound_Idx = pFile->GetWord();
			stST.nNpc_Idx = pFile->GetInt();

			if(stST.nNpc_Idx < 1 )											// npc 인덱스가 1보다 작거나, 최대 메시지 카운트 이상이면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, TERR_MSG10, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			stST.nFace_Idx = pFile->GetInt();

			const int npcEmotionMax = 5;

			if(stST.nFace_Idx < 0 || stST.nFace_Idx >= npcEmotionMax)
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, TERR_MSG11, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			stST.nComplete_Condition1 = pFile->GetInt() ;					// 파일로 부터 제 1 완료 조건을 받는다.
			if(stST.nComplete_Condition1 < 0 || 
				stST.nComplete_Condition1 >= e_MAX_CCONDITION)				// npc 인덱스가 0보다 작거나, 최대 메시지 카운트 이상이면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, TERR_MSG12, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			switch(stST.nComplete_Condition1)								// 첫번째 발동 조건을 확인한다.
			{
			case e_CLICK_OKBTN :											// 확인 버튼을 누르는 것이면,
				{
					stST.nComplete_Condition2 = pFile->GetInt() ;			// 파일로 부터 제 2 완료 조건을 받는다.
				}
				break ;

			case e_NPC_TALK :												// 특정 npc와 대화하는 것이면,
				{
					stST.nComplete_Condition2 = pFile->GetInt() ;			// 파일로 부터 제 2 완료 조건을 받는다.
				}
				break ;

			case e_OPEN_DIALOG :											// 특정 창을 열어야 하는 것이면,
				{
					strcpy(stST.szCompleteCondition2, pFile->GetString()) ;	// 파일로 부터 두번째 완료 조건을 받는다.		
				}
				break ;
			}

			SUB_TUTORIAL* pSubTutorial = NULL ;								// 서브 튜토리얼 정보를 받을 포인터를 선언하고 null 처리를 한다.
			pSubTutorial = new SUB_TUTORIAL ;								// 서브 튜토리얼을 생성한다.

			if( !pSubTutorial )												// 서브 튜토리얼 정보가 유효하지 않으면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, TERR_MSG14, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}
			
			pSubTutorial->nIndex				= stST.nIndex ;				// 서브 튜토리얼의 인덱스를 세팅한다.
			pSubTutorial->nSpeech_Idx			= stST.nSpeech_Idx ;		// 대화 인덱스를 세팅한다.
			pSubTutorial->nSound_Idx			= stST.nSound_Idx ;			// 사운드 인덱스를 세팅한다.
			pSubTutorial->nNpc_Idx				= stST.nNpc_Idx ;			// NPC 인덱스를 세팅한다.
			pSubTutorial->nFace_Idx				= stST.nFace_Idx ;			// 얼굴 표정 인덱스를 세팅한다.
			pSubTutorial->nComplete_Condition1	= stST.nComplete_Condition1 ;	// 첫번째 완료 조건을 세팅한다.

			switch(pSubTutorial->nComplete_Condition1)						// 첫번재 완료 조건을 확인한다.
			{
			case e_CLICK_OKBTN :											// 확인 버튼을 누르는 것이면,
				{
					pSubTutorial->nComplete_Condition2	= stST.nComplete_Condition2 ;	// 두번째 완료 조건을 세팅한다.
				}
				break ;

			case e_NPC_TALK :												// 특정 npc와 대화하는 것이면,
				{
					pSubTutorial->nComplete_Condition2	= stST.nComplete_Condition2 ;	// 두번째 완료 조건을 세팅한다.
				}
				break ;

			case e_OPEN_DIALOG :											// 특정 창을 열어야 하는 것이면,
				{
					strcpy(pSubTutorial->szCompleteCondition2, stST.szCompleteCondition2) ;	// 두번째 완료 조건을 세팅한다.
				}
				break ;
			}

			++nCount2 ;														// 서브 카운트 변수를 증가한다.

			pTutorial->Add_SubTutorial( pSubTutorial ) ;					// 서브 튜토리얼을 추가한다.
		}
		else																// SUBTUTORIAL, {, }, 가 아니면, 오타다.
		{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, TERR_MSG13, nMT_Idx, nCount2);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
		}
	}

	return TRUE ;															// true return 처리를 한다.
}





//=========================================================================
//		튜토리얼 정보를 해제하는 함수.
//=========================================================================
void cTutorialManager::ReleaseTutorial() 									// 튜토리얼 정보를 해제하는 함수.
{
	cTutorial* pTutorial ;													// 튜토리얼 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pDeletePos = NULL ;											// 리스트 상의 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pDeletePos = m_Tutorial_List.GetHeadPosition() ;						// 튜토리얼 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pDeletePos)														// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pTutorial = NULL ;													// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
		pTutorial = (cTutorial*)m_Tutorial_List.GetNext(pDeletePos) ;		// 현재 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pTutorial ) continue ;											// 튜토리얼 정보가 유효하지 않으면 continue 처리를 한다.

		delete pTutorial ;													// 튜토리얼 정보를 삭제한다.
	}

	m_Tutorial_List.RemoveAll() ;											// 튜토리얼 리스트를 모두 비운다.
}





//=========================================================================
//		지정된 인덱스의 튜토리얼 정보를 받환하는 함수.
//=========================================================================
cTutorial* cTutorialManager::Get_Tutorial(int nIndex) 						// 인덱스에 해당하는 튜토리얼 정보를 리턴하는 함수.
{
	int nTutorial_Count = 0 ;												// 튜토리얼 수를 받을 변수를 선언하고 0으로 초기화 한다.
	nTutorial_Count = m_Tutorial_List.GetCount() ;							// 튜토리얼 카운트를 받는다.

	if( nIndex < 0 || nIndex >= nTutorial_Count ) return NULL ;				// 인자로 넘어온 인덱스가, 0보다 작거나, 현재 튜토리얼 수보다 크면, null return 처리를 한다.

	PTRLISTPOS returnPos = NULL ;											// 인덱스에 해당하는 튜토리얼 위치를 담을 포인터를 선언하고 null 처리를 한다.
	returnPos = m_Tutorial_List.FindIndex(nIndex) ;							// 인덱스에 해당하는 튜토리얼의 위치정보를 받는다.

	if( !returnPos ) return NULL ;											// 위치 정보가 유효하지 않으면, null return 처리를 한다.

	return (cTutorial*)m_Tutorial_List.GetAt(returnPos) ;					// 위치에 해당하는, 튜토리얼 정보를 return 처리를 한다.
}






//=========================================================================
//		튜토리얼 메시지를 로딩하는 함수.
//=========================================================================
BOOL cTutorialManager::LoadMsg(char* strFileName) 							// 튜토리얼 메시지를 로딩하는 함수.
{
	CMHFile file ;															// 묵향 파일을 선언한다.

	if( !strFileName )														// 인자로 넘어온 파일명이 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, TERR_MSG1, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	if( !file.Init( strFileName, "rb") )
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG2, strFileName);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	int nMsgCount = 0 ;														// 메시지 카운트를 담을 변수를 선언하고 0으로 세팅한다.

	int nMsgIndex = 0 ;														// 메시지 인덱스를 담을 변수를 선언하고 0으로 세팅한다.

	char buff[1024] ;														// 임시 버퍼를 선언한다.

	while(!file.IsEOF())													// while문을 돌린다.
	{
		nMsgCount = m_Tutorial_Msg.GetCount() ;								// 메시지 카운트를 받는다.

		memset(buff, 0, sizeof(buff)) ;										// 임시 버퍼를 초기화 한다.
		file.GetString(buff);

		if( strcmp(buff, "#Msg") == 0 )										// 버퍼가 #Msg와 같다면,
		{
			cTutorial_Msg* pMsg = NULL ;									// 튜토리얼 메시지 정보를 받을 포인터를 선언하고 null 처리를 한다.
			pMsg = new cTutorial_Msg ;										// 튜토리얼 메시지를 생성한다.

			if( !pMsg )														// 튜토리얼 메시지 생성에 실패하면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0, };
				sprintf(tempBuf, MERR_MSG2, nMsgCount);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			nMsgIndex = file.GetInt() ;
			if( nMsgIndex < 0 || nMsgIndex != nMsgCount )					// 튜토리얼 메시지 생성에 실패하면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0, };
				sprintf(tempBuf, MERR_MSG4, nMsgCount);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}


			pMsg->Set_MsgIndex(nMsgIndex) ;									// 생성 된 메시지의 인덱스를 세팅한다.

			if( !ReadMsgLine(&file, pMsg) ) return FALSE ;					// 튜토리얼 메시지의 라인 읽기가 실패하면, false return 처리를 한다.

			m_Tutorial_Msg.AddTail(pMsg) ;									// 튜토리얼 메시지 리스트에, 메시지를 추가한다.

			continue ;														// continue 처리를 한다.
		}
		else if( strcmp(buff, "") == 0 )									// 버퍼가 #Msg가 공란과 같다면,
		{
			continue ;														// continue 처리를 한다.
		}
		else																// #Msg도 아니고, 공란도 아니면, 에러다~!!
		{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0, };
				sprintf(tempBuf, MERR_MSG3, nMsgCount);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
		}
	}

	return TRUE ;															// true return 처리를 한다.
}





//=========================================================================
//		튜토리얼 메시지를 해제하는 함수.
//=========================================================================
void cTutorialManager::ReleaseMsg() 										// 튜토리얼 메시지를 해제하는 함수.
{
	cTutorial_Msg* pMsg ;													// 삭제할 메시지 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pDeletePos = NULL ;											// 삭제할 메시지 위치 정보를 담을 포인터를 선언하고 null 처리를 한다.
	pDeletePos = m_Tutorial_Msg.GetHeadPosition() ;							// 튜토리얼 메시지 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while( pDeletePos )														// 삭제 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pMsg = NULL ;														// 메시지 포인터를 null 처리를 한다.
		pMsg = (cTutorial_Msg*)m_Tutorial_Msg.GetNext(pDeletePos) ;			// 위치에 해당하는 메시지 정보를 받고, 위치정보를 다음으로 세팅한다.

		if( !pMsg ) continue ;												// 메시지 정보가 유효하지 않으면, continue 처리를 한다.

		//---KES 메모리 릭 수정
		pMsg->Release();
		//--------------------
		delete pMsg ;														// 메시지 정보를 삭제한다.
	}

	m_Tutorial_Msg.RemoveAll() ;											// 튜토리얼 메시지 리스트를 모두 비운다.
}





//=========================================================================
//		튜토리얼 메시지 라인을 읽는 함수.
//=========================================================================
BOOL cTutorialManager::ReadMsgLine(CMHFile* pFile, cTutorial_Msg* pMsg) 	// 튜토리얼 메시지의 라인을 읽는 함수.
{
	if( !pFile )															// 인자로 넘어온 파일 포인터가 유효하지 않으면,
	{
		return FALSE ;														// false return 처리를 한다.
	}

	if( !pMsg )																// 인자로 넘어온 튜토리얼 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, MERR_MSG3, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	int nMsgIdx = pMsg->Get_MsgIndex() ;									// 현재 메시지의 인덱스를 받는다.

	int nLineCount = 0 ;													// 메시지의 라인 수를 담을 변수를 선언하고 0으로 세팅한다.

	int nOpenCount = 0 ;													// 열리는 괄호 카운트 수를 0으로 세팅한다.
	int nCloseCount= 0 ;													// 닫히는 괄호 카운트 수를 0으로 세팅한다.

	char buff[1024] ;														// 임시 버퍼를 선언한다.

	memset(buff, 0, sizeof(buff)) ;											// 임시 버퍼를 초기화 한다.
	pFile->GetLine(buff, sizeof(buff)) ;												// 임시 버퍼에 라인을 읽어들인다.

	while( !pFile->IsEOF() )												// 파일의 끝이 아니라면 계속 while문을 돌린다.
	{
		nLineCount = pMsg->Get_LineCount() ;								// 메시지의 라인 카운트를 받는다.

		memset(buff, 0, sizeof(buff)) ;										// 임시 버퍼를 초기화 한다.
		pFile->GetLine(buff, sizeof(buff)) ;											// 임시 버퍼에 라인을 읽어들인다.

		if( strcmp( buff, "{") == 0 )
		{
			if( nOpenCount > 0 )											// 열기 괄호가 0보다 크면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, CERR_MSG3, nMsgIdx, nLineCount);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);;
#endif
				return FALSE;
			}

			++nOpenCount ;													// 열리는 괄호 카운트 수를 증가한다.
			continue ;
		}
		else if( strcmp( buff, "}") == 0 )
		{
			if( nCloseCount > 0 )											// 닫기 괄호가 0보다 크면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, CERR_MSG2, nMsgIdx, nLineCount);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);;
#endif
				return FALSE;
			}

			++nCloseCount ;													// 닫히는 괄호 카운트 수를 증가한다.
			break ;
		}
		else
		{
			pMsg->Add_MsgLine(buff);
		}
	}

	return TRUE ;															// true return 처리를 한다.
}





//=========================================================================
//		지정된 인덱스의 튜토리얼 메시지를 반환하는 함수.
//=========================================================================
cTutorial_Msg* cTutorialManager::Get_TutorialMsg(int nIndex) 				// 인덱스에 해당하는 튜토리얼 메시지 정보를 리턴하는 함수.
{
	int nMst_Count = 0 ;													// 튜토리얼 메시지 수를 받을 변수를 선언하고 0으로 초기화 한다.
	nMst_Count = m_Tutorial_Msg.GetCount() ;								// 튜토리얼 메시지 카운트를 받는다.

	if( nIndex < 0 || nIndex >= nMst_Count ) return NULL ;					// 인자로 넘어온 인덱스가, 0보다 작거나, 현재 메시지 수보다 크면, null return 처리를 한다.

	PTRLISTPOS returnPos = NULL ;											// 인덱스에 해당하는 메시지 위치를 담을 포인터를 선언하고 null 처리를 한다.
	returnPos = m_Tutorial_Msg.FindIndex(nIndex) ;							// 인덱스에 해당하는 메시지의 위치정보를 받는다.

	if( !returnPos ) return NULL ;											// 위치 정보가 유효하지 않으면, null return 처리를 한다.

	return (cTutorial_Msg*)m_Tutorial_Msg.GetAt(returnPos) ;				// 위치에 해당하는, 메시지 정보를 return 처리를 한다.
}





//=========================================================================
//		도움말 정보를 로드하는 함수.
//=========================================================================
BOOL cTutorialManager::LoadHelperInfo(char* strFileName) 					// 도움말 정보를 로딩하는 함수.
{
	CMHFile file ;															// 묵향 파일을 선언한다.

	if( !strFileName )														// 인자로 넘어온 파일명이 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, TERR_MSG1, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	if( !file.Init( strFileName, "rb") )
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG2, strFileName);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	int nLinkCount = 0 ;													// 도움말 링크 카운트를 담을 변수를 선언하고 0으로 초기화 한다.

	int nIndex ;															// 링크 인덱스를 담을 변수를 선언한다.

	char LinkTitle[1024] ;													// 링크 제목을 담을 임시 버퍼를 선언한다.

	int nTutorialIndex ;													// 튜토리얼 인덱스를 담을 변수를 선언한다.

	while(!file.IsEOF())													// while문을 돌린다.
	{
		nLinkCount = m_Helper_List.GetCount();
		nIndex = file.GetInt();
		nTutorialIndex = file.GetInt();

		if( nTutorialIndex < 0 || nTutorialIndex > m_Tutorial_List.GetCount() ) // 튜토리얼 인덱스가 0보다 작거나, 튜토리얼 수 보다 크면,
		{
#ifdef _GMTOOL_
			char tempBuf[1024] = {0};
			sprintf(tempBuf, "Invalid linked tutorial index : %d", nLinkCount);
			MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
            return FALSE;
		}

		memset(LinkTitle, 0, sizeof(LinkTitle)) ;							// 임시 버퍼를 초기화 한다.
		file.GetLine(LinkTitle, sizeof(LinkTitle));										// 파일로 부터 링크 제목을 읽어들인다.

		if( strlen(LinkTitle) < 2 )											// 링크 제목이 한글자
		{
#ifdef _GMTOOL_
			char tempBuf[1024] = {0};
			sprintf(tempBuf, "Invalid link title : %d", nLinkCount);
			MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
            return FALSE;
		}

		HELPLINK* pHelpLink = NULL ;
		pHelpLink = new HELPLINK ;

		if( !pHelpLink )
		{
#ifdef _GMTOOL_
			char tempBuf[1024] = {0};
			sprintf(tempBuf, "Failed to create help link : %d", nLinkCount);
			MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
            return FALSE;
		}

		pHelpLink->nIndex = nIndex ;										// 도움말 인덱스를 세팅한다.
		strcpy(pHelpLink->Title, LinkTitle) ;								// 도움말 링크 제목을 세팅한다.
		pHelpLink->nTutorial_Idx = nTutorialIndex ;							// 도움말 링크 튜토리얼 인덱스를 세팅한다.

		m_Helper_List.AddTail( pHelpLink ) ;								// 도움말 리스트에 링크를 추가한다.
	}

	return TRUE ;															// true return 처리를 한다.
}





//=========================================================================
//		도움말 정보를 해제하는 함수.
//=========================================================================
void cTutorialManager::ReleaseHelperInfo() 									// 도움말 정보를 해제하는 함수.
{
	HELPLINK* pHelpLink ;													// 도움말 링크 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pDeletePos = NULL ;											// 삭제 위치 정보를 받을 위치 포인터를 선언하고 null 처리를 한다.
	pDeletePos = m_Helper_List.GetHeadPosition() ;							// 도움말 링크를 담고 있는 리스트를 헤드로 세팅하고 위치정보를 받는다.

	while( pDeletePos )														// 삭제 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pHelpLink = NULL ;													// 도움말 링크 정보 포인터를 null 처리를 한다.
		pHelpLink = (HELPLINK*)m_Helper_List.GetNext(pDeletePos) ;			// 위치에 해당하는 도움말 링크 정보를 받고, 다음 위치로 세팅한다.

		if( !pHelpLink ) continue ;											// 도움말 링크 정보가 유효하지 않으면, continue 처리를 한다.

		delete pHelpLink ;													// 도움말 링크 정보를 삭제한다.
	}

	m_Helper_List.RemoveAll() ;												// 도움말 링크 리스트를 모두 비운다.
}





//=========================================================================
//		지정된 인덱스의 도움말 링크를 반환하는 함수.
//=========================================================================
HELPLINK* cTutorialManager::Get_HelpLink(int nIndex) 						// 인덱스에 해당하는 도움말 링크를 리턴하는 함수.
{
	int nLink_Count = 0 ;													// 도움말 링크 수를 받을 변수를 선언하고 0으로 초기화 한다.
	nLink_Count = m_Helper_List.GetCount() ;								// 도움말 링크 수를 받는다.

	if( nIndex < 0 || nIndex >= nLink_Count ) return NULL ;					// 인자로 넘어온 인덱스가, 0보다 작거나, 현재 링크 수보다 크면, null return 처리를 한다.

	PTRLISTPOS returnPos = NULL ;											// 인덱스에 해당하는 링크 위치를 담을 포인터를 선언하고 null 처리를 한다.
	returnPos = m_Helper_List.FindIndex(nIndex) ;							// 인덱스에 해당하는 링크의 위치정보를 받는다.

	if( !returnPos ) return NULL ;											// 위치 정보가 유효하지 않으면, null return 처리를 한다.

	return (HELPLINK*)m_Helper_List.GetAt(returnPos) ;						// 위치에 해당하는, 링크 정보를 return 처리를 한다.
}





//=========================================================================
//		도움말 링크에서 선택 된 튜토리얼을 실행하는 함수.
//=========================================================================
void cTutorialManager::Start_HelpTutorial(cTutorial* pTutorial) 			// 도움말 링크에서 선택 된 튜토리얼을 실행하는 함수.
{
	if( !pTutorial )														// 인자로 넘어온 튜토리얼 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, TERR_MSG7, __FUNCTION__, MB_OK);
#endif
        return;
	}

	m_nCurTutorialIdx = pTutorial->Get_Index() ;							// 현재 튜토리얼 인덱스를 세팅한다.

	m_pTutorialDlg->Set_TutorialInfo(pTutorial) ;							// 튜토리얼 다이얼로그에서 튜토리얼을 시작한다.
	m_pTutorialDlg->Start_Tutorial() ;										// 튜토리얼을 실행한다.
	// 090417 ONS 튜토리얼 다이얼로그 상태 ON
	SetOnOffTutorial(TRUE);
}





//=========================================================================
//		네트워크 메시지 파싱 처리를 하는 함수.
//=========================================================================
void cTutorialManager::NetworkMsgParse(BYTE Protocol,void* pMsg) 			// 네트워크 메시지 파싱 함수.
{
	if(!pMsg) return ;														// 인자로 넘어온 메시지 정보가 유효하지 않으면, return 처리를 한다.

	switch(Protocol)														// 프로토콜 정보를 확인한다.
	{
	case MP_TUTORIAL_MAINDATALOAD :											// 프로토콜이 튜토리얼 데이터를 로딩하는 것이면,
		{
			SEND_TUTORIAL_DATA* msg = (SEND_TUTORIAL_DATA*)pMsg ;			// 튜토리얼 데이터 형으로 메시지를 받는다.

			if(!msg)														// 메시지 정보가 유효하지 않으면,
			{
#ifdef _GMTOOL_
				MessageBox(NULL, TERR_MSG1, __FUNCTION__, MB_OK);
#endif
        		return;
			}

			int		nIndex ;												// 인덱스를 담을 변수를 선언한다.
			BOOL	isComplete ;											// 완료 여부를 담을 변수를 선언한다.

			cTutorial* pTutorial ;											// 튜토리얼 정보를 담을 포인터를 선언한다.

			PTRLISTPOS pos ;												// 위치 정보를 담을 포인터를 선언한다.

			pos = NULL ;													// pos를 null로 세팅한다.
			pos = m_Tutorial_List.GetHeadPosition() ;						// 튜토리얼 리스트를 헤드로 세팅한다.

			while( pos )													// 위치 정보가 유효할동안 while문을 돌린다.
			{
				pTutorial = NULL ;											// 튜토리얼 포인터를 null로 세팅한다.
				pTutorial = (cTutorial*)m_Tutorial_List.GetNext(pos) ;		// 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

				if( !pTutorial ) continue ;									// 튜토리얼 정보가 없으면 continue 처리를 한다.

				pTutorial->Set_Complete(FALSE) ;							// 튜토리얼 완료 여부를 false로 세팅한다.
			}

			for( WORD count = 0 ; count < msg->wCount ; ++count )			// 메시지 카운트 만큼 for문을 돌린다.
			{
				nIndex = -1 ;												// 메시지 튜토리얼의 인덱스를 받을 변수를 -1로 세팅한다.
				nIndex = msg->TutorialList[count].nTutorialIdx ;			// 메시지 튜토리얼의 인덱스를 받는다.

				if( nIndex < 0 )											// 인덱스가 0보다 작으면,
				{
#ifdef _GMTOOL_
					MessageBox(NULL, TERR_MSG2, __FUNCTION__, MB_OK);
#endif
        			continue ;												// continue 처리를 한다.
				}

				pos = NULL ;												// 위치 정보를 받을 포인터를 null 처리를 한다.
				pos = m_Tutorial_List.FindIndex( nIndex ) ;					// 메시지 튜토리얼 인덱스와 같은 튜토리얼 위치를 받는다.

				if( !pos )													// 위치 정보가 유효하지 않으면,
				{
#ifdef _GMTOOL_
					char tempBuf[1024] = {0};
					sprintf(tempBuf, NERR_MSG3, nIndex);
					MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
					continue;
				}

				pTutorial = NULL ;											// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
				pTutorial = (cTutorial*)m_Tutorial_List.GetAt(pos) ;		// 위치에 해당하는 튜토리얼 정보를 받는다.

				if( !pTutorial ) 
				{
#ifdef _GMTOOL_
					char tempBuf[1024] = {0};
					sprintf(tempBuf, NERR_MSG4, nIndex);
					MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
					continue;
				}

				isComplete = FALSE ;										// 완료 여부를 false로 세팅한다.
				isComplete = msg->TutorialList[count].IsComplete ;			// 메시지 튜토리얼의 완료 여부를 받는다.

				pTutorial->Set_Complete(isComplete) ;						// 현재 튜토리얼에 완료 여부를 세팅한다.
			}
		}
		break ;
	}
}

BOOL cTutorialManager::Check_StartCondition(cTutorial* pTutorial) 			// 튜토리얼 발동 조건을 체크하는 함수.
{
	if( !pTutorial )														// 인자로 넘어온 튜토리얼 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, TERR_MSG7, __FUNCTION__, MB_OK);
#endif
        return FALSE;
	}

	int nCondition = 0 ;													// 조건 인덱스를 담을 변수를 선언하고 0으로 세팅한다.
	nCondition = pTutorial->Get_StartCondition1() ;							// 현재 튜토리얼의 시작 조건을 받는다.

	if( nCondition < 0 || nCondition >= e_MAX_SCONDITION )					// 조건이 0보다 작거나, 최대 조건을 넘어서면,
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG5, pTutorial->Get_Index());
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
        return FALSE;
	}

	switch(nCondition)														// 시작 조건을 확인한다.
	{
	case e_FIRST_GAMEIN :													// 처음 게임에 로그인 했을 때 시작하는 조건이면,
		{
			if( pTutorial->Is_Completed() ) return FALSE ;					// false return 처리를 한다.

			m_bUpdated = TRUE ;												// 업데이트 여부를 true로 세팅한다.

			Start_CurTutorial(pTutorial) ;									// 현재 튜토리얼을 시작한다.
		}
		break ;

	case e_COMPLETE_TUTORIAL :												// 시작 조건이 다른 튜토리얼 완료라면,
		{
			int nCompleteCondition = pTutorial->Get_StartCondition2() ;		// 현재 튜토리얼의 시작 조건을 받는다.

			cTutorial* pCheckTutorial = NULL ;								// 체크 할 튜토리얼 포인터를 선언하고 null 처리를 한다.
			pCheckTutorial = Get_Tutorial(nCompleteCondition) ;				// 체크 할 튜토리얼 정보를 받는다.

			if( !pCheckTutorial )											// 체크 할 튜토리얼 정보를 받지 못했다면,
			{
#ifdef _GMTOOL_
				char tempBuf[1024] = {0};
				sprintf(tempBuf, TERR_MSG15, nCompleteCondition);
				MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
				return FALSE;
			}

			if( !pCheckTutorial->Is_Completed() ) return FALSE ;			// false return 처리를 한다.

			m_bUpdated = TRUE ;												// 업데이트 여부를 true로 세팅한다.
				
			Start_CurTutorial(pTutorial) ;									// 현재 튜토리얼을 시작한다.

			return TRUE ;													// true return 처리를 한다.
		}
		break ;

	case e_LEVEL :															// 시작 조건이 일정 레벨 도달이라면,
		{
			int nLevelCondition = pTutorial->Get_StartCondition2() ;		// 현재 튜토리얼의 시작 조건을 받는다.

			if( nLevelCondition <= HERO->GetLevel() )						// HERO의 레벨이 실행 레벨 이상이면,
			{
				m_bUpdated = TRUE ;											// 업데이트 여부를 true로 세팅한다.
				
				Start_CurTutorial(pTutorial) ;								// 현재 튜토리얼을 시작한다.

				return TRUE ;												// true return 처리를 한다.
			}
		}
		break ;

	default : break ;														// 지금까지 외의 경우는...
	}

	return FALSE ;															// false return 처리를 한다.
}





//=========================================================================
//		특정 npc와의 대화 여부를 체크하는 함수.
//=========================================================================
BOOL cTutorialManager::Check_NpcTalk(void* pMsg) 							// npc와 대화를 하는 조건을 체크하는 함수.
{
	if( !pMsg )
	{
#ifdef _GMTOOL_
		MessageBox(NULL, CERR_MSG4, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;									// 인자로 넘어온 포인터를 MSG_DWORD2* 형으로 받는다.

	CNpc* pNpc = NULL ;														// npc 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pNpc = (CNpc*)OBJECTMGR->GetObject(pmsg->dwData1);						// npc 정보를 받는다.

	if( !pNpc )																// npc 정보가 유효하지 않다면,
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, PERR_MSG2, pmsg->dwData1);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return FALSE;
	}

	if( Check_NpcTalk_EndCondition(pmsg->dwData2) )		return FALSE ;		// 현재 npc 인덱스로 완료 조건을 체크한다.

	if( Check_NpcTalk_StartCondition(pmsg->dwData2) )						// 시작 조건이 성공하면, 
	{
		return TRUE ;														// true return 처리를 한다.
	}
	else																	// 시작 조건이 실패하면,
	{
		return FALSE ;														// false return 처리를 한다.
	}
}





//=========================================================================
//		npc와 대화 조건 중, 시작 조건을 체크하는 함수.
//=========================================================================
BOOL cTutorialManager::Check_NpcTalk_StartCondition(DWORD dwNpcIdx) 		// npc와 대화 조건 중, 시작 조건을 체크하는 함수.
{
	if( !m_bActiveTutorial ) return FALSE ;									// 튜토리얼이 비활성화 상태면, return 처리를 한다.

	if(m_bUpdated) return FALSE ;											// 튜토리얼 이벤트가 업데이트 되었으면, return 처리를 한다.
	
	cTutorial* pCurTutorial ;												// 현재 튜토리얼 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;													// 튜토리얼 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_Tutorial_List.GetHeadPosition() ;								// 튜토리얼 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																// 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pCurTutorial = NULL ;												// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
		pCurTutorial = (cTutorial*)m_Tutorial_List.GetNext(pos) ;			// 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pCurTutorial ) continue ;										// 튜토리얼 정보가 유효하지 않으면 continue 처리를 한다.

		if( pCurTutorial->Is_Completed() ) continue ;						// 완료한 튜토리얼이면, 건너뛴다.

		if( pCurTutorial->Get_StartCondition1() != e_TALK_NPC ) continue ;	// npc와 대화하는 시작 조건이 아니면 건너뛴다.

		if(DWORD(pCurTutorial->Get_StartCondition2()) != dwNpcIdx ) continue ;

		m_bUpdated = TRUE ;													// 업데이트 여부를 true로 세팅한다.
		
		Start_CurTutorial(pCurTutorial) ;									// 현재 튜토리얼을 시작한다.

		return TRUE ;														// 리턴 처리를 한다.
	}

	return FALSE ;															// false return 처리를 한다.
}





//=========================================================================
//		npc와 대화 조건 중, 완료 조건을 체크하는 함수.
//=========================================================================
BOOL cTutorialManager::Check_NpcTalk_EndCondition(DWORD dwNpcIdx) 			// npc와 대화 조건 중, 완료 조건을 체크하는 함수.
{
	if( m_pTutorialDlg->IsSameEndNpc(dwNpcIdx) )							// 종료 조건과 같은 npc 라면,
	{
		if(m_pTutorialDlg->IsHaveNextSubTutorial())							// 다음 서브 튜토리얼 정보가 있다면,
		{
			m_pTutorialDlg->StartNextSubTutorial() ;						// 다음 서브 튜토리얼을 진행한다.
		}
		else																// 다음 서브 투토리얼 정보가 없으면,
		{
			m_pTutorialDlg->UpdateTutorial() ;								// 튜토리얼을 업데이트 한다.
		}
	}

	return FALSE ;															// false return 처리를 한다.
}





//=========================================================================
//		레벨업 시, 진행 가능한 tutorial이 있는지 확인 후, 있으면 진행.
//=========================================================================
void cTutorialManager::Check_LevelUp()
{
	if( !m_bActiveTutorial ) return ;										// 튜토리얼이 비활성화 상태면, return 처리를 한다.

	if(m_bUpdated) return ;													// 튜토리얼 이벤트가 업데이트 되었으면, return 처리를 한다.
	
	cTutorial* pCurTutorial ;												// 현재 튜토리얼 정보를 담을 포인터를 선언한다.

	int nCondition1 ;														// 튜토리얼의 발동 조건을 담을 변수를 선언한다.
	int nCondition2 ;														// 두번째 발동 조건을 담을 변수를 선언한다.

	PTRLISTPOS pos = NULL ;													// 튜토리얼 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_Tutorial_List.GetHeadPosition() ;								// 튜토리얼 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																// 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pCurTutorial = NULL ;												// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
		pCurTutorial = (cTutorial*)m_Tutorial_List.GetNext(pos) ;			// 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pCurTutorial ) continue ;										// 튜토리얼 정보가 유효하지 않으면 continue 처리를 한다.

		if( pCurTutorial->Is_Completed() ) continue ;						// 완료한 튜토리얼이면, 건너뛴다.

		nCondition1 = -1 ;													// 발동 조건 변수를 -1로 세팅한다.
		nCondition1 = pCurTutorial->Get_StartCondition1() ;					// 현재 튜토리얼의 발동 조건을 받는다.

		if( nCondition1 != e_LEVEL ) continue ;								// 레벨 업 시 발동하는 튜토리얼이 아니면 continue 처리를 한다.

		nCondition2 = -1 ;													// 두번째 발동 조건을 -1로 세팅한다.
		nCondition2 = pCurTutorial->Get_StartCondition2() ;					// 두번째 발동 조건을 받는다.

		if( nCondition2 > HERO->GetLevel() ) continue ;					// 레벨 미달이면 continue 한다.
		
		Start_CurTutorial(pCurTutorial) ;									// 현재 튜토리얼을 시작한다.

		return ;
	}
}





//=========================================================================
//		아이템을 습득했을 때 튜토리얼 진행 여부를 체크하는 함수.
//=========================================================================
void cTutorialManager::Check_GetItem(CItem* pItem)							// 아이템을 습득했을 때 튜토리얼 진행 여부를 체크하는 함수.
{
	if( !pItem )															// 인자로 넘어온 아이템 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, PERR_MSG3, __FUNCTION__, MB_OK);
#endif
		return;
	}

	if( !m_bActiveTutorial ) return ;										// 튜토리얼이 비활성화 상태면, return 처리를 한다.

	if(m_bUpdated) return ;													// 튜토리얼 이벤트가 업데이트 되었으면, return 처리를 한다.
	
	cTutorial* pCurTutorial ;												// 현재 튜토리얼 정보를 담을 포인터를 선언한다.

	int nCondition1 ;														// 튜토리얼의 발동 조건을 담을 변수를 선언한다.
	int nCondition2 ;														// 두번째 발동 조건을 담을 변수를 선언한다.

	PTRLISTPOS pos = NULL ;													// 튜토리얼 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_Tutorial_List.GetHeadPosition() ;								// 튜토리얼 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																// 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pCurTutorial = NULL ;												// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
		pCurTutorial = (cTutorial*)m_Tutorial_List.GetNext(pos) ;			// 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pCurTutorial ) continue ;										// 튜토리얼 정보가 유효하지 않으면 continue 처리를 한다.

		if( pCurTutorial->Is_Completed() ) continue ;						// 완료한 튜토리얼이면, 건너뛴다.

		nCondition1 = -1 ;													// 발동 조건 변수를 -1로 세팅한다.
		nCondition1 = pCurTutorial->Get_StartCondition1() ;					// 현재 튜토리얼의 발동 조건을 받는다.

		if( nCondition1 != e_GET_ITEM ) continue ;							// 아이템 습득시 발동하는 튜토리얼이 아니면 continue 처리를 한다.

		nCondition2 = -1 ;													// 두번째 발동 조건을 -1로 세팅한다.
		nCondition2 = pCurTutorial->Get_StartCondition2() ;					// 두번째 발동 조건을 받는다.

		if( nCondition2 == e_GET_STACK_ITEM )								// 두번째 발동 조건이 스택 아이템일 경우,
		{
			CInventoryExDialog* pInven = NULL ;								// 인벤토리 정보를 받을 포인터를 선언하고 null 처리를 한다.
			pInven = GAMEIN->GetInventoryDialog() ;							// 인벤토리 정보를 받는다.

			if( !pInven )
			{
#ifdef _GMTOOL_
				MessageBox(NULL, PERR_MSG4, __FUNCTION__, MB_OK);
#endif
				continue;
			}

			CItem* pCheckItem = NULL ;										// 체크 아이템을 담을 포인터를 선언하고 null 처리를 한다.
			pCheckItem = pInven->GetItemForIdx( pItem->GetItemIdx() ) ;		// 인벤으로 들어온 아이템의 인덱스로 체크 아이템을 받는다.

			if( !pCheckItem ) continue ;									// 인벤토리에서 체크 아이템을 찾지 못하면, continue 처리를 한다.

			if(pCheckItem->GetItemBaseInfo().Durability > 1)				// 체크 아이템의 개수가 1개 이상이면,
			{
				Start_CurTutorial(pCurTutorial) ;							// 현재 튜토리얼을 시작한다.

				return  ;													// return 처리를 한다.
			}
		}
		else																// 두번째 발동 조건이 스택 아이템이 아닐 경우,
		{
			Start_CurTutorial(pCurTutorial) ;								// 현재 튜토리얼을 시작한다.

			return ;														// return 처리를 한다.
		}
	}
}





//=========================================================================
//		튜토리얼 정보를 업데이트 하는 함수.
//=========================================================================
void cTutorialManager::Check_OpenDialog(DWORD dwWindowID, BOOL bVal) 		// 창을 여는 완료 조건을 체크하는 함수.
{
	if( !m_pTutorialDlg )													// 튜토리얼 다이얼로그 정보가 없으면,
	{
		return ;															// return 처리를 한다.
	}

	if( !m_pTutorialDlg )													// 튜토리얼 다이얼로그 정보가 없으면,
	{
		return ;															// return 처리를 한다.
	}

	if( m_nCurTutorialIdx < 0 )												// 튜토리얼 인덱스가 0보다 작으면,
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG4, m_nCurTutorialIdx);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return;
	}

	cTutorial* pTutorial = NULL ;											// 튜토리얼 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pTutorial = Get_Tutorial(m_nCurTutorialIdx) ;							// 현재 진행중인 튜토리얼 정보를 받는다.

	if( !pTutorial )														// 튜토리얼 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG4, m_nCurTutorialIdx);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
	}

	//if( pTutorial->Is_Completed() ) return ;								// 완료한 튜토리얼 이면, return 처리를 한다.

	SUB_TUTORIAL* pSub = NULL ;												// 서브 튜토리얼을 받을 포인터를 선언하고 null 처리를 한다.
	pSub = m_pTutorialDlg->GetCurSubTutorial() ;							// 현재 튜토리얼의 현재 서브 튜토리얼 정보를 받는다.

	if( !pSub )
	{
		return;
	}

	if( pSub->nComplete_Condition1 != e_OPEN_DIALOG ) return ;				// 완료 조건이 창을 여는 것이 아니면, return 처리를 한다.

	DWORD dwCheckID = 0 ;													// 체트용 윈도우 아이디를 선언하고 0으로 세팅한다.

	LONG lWindwID = 0 ;														// 윈도우 아이디를 0으로 세팅한다.
	lWindwID = IDSEARCH(pSub->szCompleteCondition2) ;						// 열어야 하는 창의 아이디를 받는다.
	cWindow* pWindow = WINDOWMGR->GetWindowForID(lWindwID) ;				// 윈도우 정보를 받는다.

	if( !pWindow )
	{
#ifdef _GMTOOL_
		char tempBuf[1024] = {0};
		sprintf(tempBuf, TERR_MSG17, pSub->szCompleteCondition2);
		MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
		return;
	}

	dwCheckID = pWindow->GetID() ;											// 윈도우의 아이디를 받는다.

	if( dwCheckID == dwWindowID )											// 인자로 넘어온 아이디와 체크 아이디가 같으면,
	{
		if( bVal )															// 활성화 값이 true 이면,
		{
			if( m_pTutorialDlg->IsHaveNextSubTutorial() )					// 다음 튜토리얼이 있으면,
			{
				m_pTutorialDlg->StartNextSubTutorial() ;					// 다음 튜토리얼을 진행한다.
			}
			else															// 다음 튜토리얼이 없으면,
			{
				m_pTutorialDlg->UpdateTutorial() ;							// 튜토리얼을 완료 한다.
			}
		}
	}
}





//=========================================================================
//		튜토리얼 정보를 업데이트 하는 함수.
//=========================================================================
void cTutorialManager::UpdateTutorial(int nCompleteIndex) 					// 튜토리얼 정보를 업데이트 하는 함수.
{
	m_bUpdated = FALSE ;													// 업데이트 여부 변수를 false로 세팅한다.

	int		nIndex ;														// 인덱스를 담을 변수를 선언한다.

	cTutorial* pTutorial ;													// 튜토리얼 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;													// 위치 정보를 담을 포인터를 선언하고 null 처리를 한다.
	pos = m_Tutorial_List.GetHeadPosition() ;								// 튜토리얼 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	int nCompleteIdx = -1 ;
	while(pos)																// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pTutorial = NULL ;													// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
		pTutorial = (cTutorial*)m_Tutorial_List.GetNext(pos) ;				// 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pTutorial ) continue ;											// 튜토리얼 정보가 유효하지 않으면, continue 처리를 한다.

		if(pTutorial->Get_StartCondition1() == e_COMPLETE_TUTORIAL)
		{
			if(nCompleteIdx == pTutorial->Get_StartCondition2() && !pTutorial->Is_Completed()) 
			{
				Start_CurTutorial(pTutorial) ;
				return ;
			}
		}
		else if(pTutorial->Get_StartCondition1() == e_LEVEL)
		{
			if(HERO->GetLevel() >= pTutorial->Get_StartCondition2() && !pTutorial->Is_Completed())
			{
				Start_CurTutorial(pTutorial) ;
				return ;
			}
		}

		nIndex = -1 ;														// 인덱스를 -1로 세팅한다.
		nIndex = pTutorial->Get_Index() ;									// 현재 튜토리얼의 인덱스를 받는다.

		if( nIndex < 0 )
		{
#ifdef _GMTOOL_
			char tempBuf[1024] = {0};
			sprintf(tempBuf, TERR_MSG4, nIndex);
			MessageBox(NULL, tempBuf, __FUNCTION__, MB_OK);
#endif
			continue;
		}

		if( nIndex != nCompleteIndex ) continue ;							// 인자로 넘어온 인덱스와 현재 튜토리얼 인덱스가 다르면 continue 처리를 한다.

		pTutorial->Set_Complete( TRUE ) ;									// 튜토리얼의 완료 여부를 true 로 세팅한다.

		MSG_INT Msg;
		Msg.Category = MP_TUTORIAL;
		Msg.Protocol = MP_TUTORIAL_UPDATE_SYN;
		Msg.dwObjectID = gHeroID;
		Msg.nData = pTutorial->Get_Index();

		NETWORK->Send( &Msg, sizeof(Msg) );
		AUDIOMGR->Play(
			Get_EndTutorialSndIdx(),
			gHeroID);

		nCompleteIdx = pTutorial->Get_Index() ;
	}
}





//=========================================================================
//		현재 튜토리얼을 진행하라는 함수.
//=========================================================================
void cTutorialManager::Start_CurTutorial(cTutorial* pTutorial)				// 현재 튜토리얼을 진행하라는 함수.
{
	if( !m_bActiveTutorial ) return ;										// 튜토리얼이 활성화 상태가 아니면, return 처리를 한다.

	if( !pTutorial )														// 인자로 넘어온 튜토리얼 정보가 유효하지 않으면,
	{
#ifdef _GMTOOL_
		MessageBox(NULL, TERR_MSG7, __FUNCTION__, MB_OK);
#endif
        return;
	}

	m_nCurTutorialIdx = pTutorial->Get_Index() ;							// 현재 튜토리얼 인덱스를 세팅한다.

	if( m_pTutorialDlg->IsActive() ) return ;

	m_pTutorialDlg->Set_TutorialInfo(pTutorial) ;							// 튜토리얼 다이얼로그에서 튜토리얼을 시작한다.

	if( pTutorial->Get_StartCondition1() == 0 )								// 발동 조건이 게임에 처음 접속했을 때 이면, 
	{
		m_pTutorialDlg->Start_Tutorial() ;									// 튜토리얼을 바로 시작한다.
		return ;															// return 처리를 한다.
	}

	if( m_pTutorialDlg->IsActive() ) return ;				// 현재 튜토리얼 창이 열린 상태면, 버튼을 활성화 하지 않는다.

	m_pBtnDlg->Active_NoticeBtn() ;											// 튜토리얼 알림 버튼을 활성화 한다.
}





//=========================================================================
//		멤버 다이얼로그들을 리셋팅 하는 함수.
//=========================================================================
void cTutorialManager::Reset_MemberDlg() 									// 멤버 다이얼로그들을 리셋팅 하는 함수.
{
	m_bUpdated = FALSE ;													// 업데이트 여부를 false로 세팅한다.
	m_nCurTutorialIdx	= 0 ;												// 현재 튜토리얼 인덱스를 0로 세팅한다.

	m_pTutorialDlg	= NULL ;												// 튜토리얼 진행을 위한 다이얼로그 포인터.
	m_pHelperDlg	= NULL ;												// 도움말 모드 진행을 위한 다이얼로그 포인터.
	m_pBtnDlg		= NULL ;												// 튜토리얼 알림 버튼 다이얼로그 포인터.
}





//=========================================================================
// 080502 LYW : 지정 된 타입의 튜토리얼이 완료 되었는지 반환하는 함수 추가.
//=========================================================================
BYTE cTutorialManager::IsCompleteTutorial(BYTE byTutorialType)
{
	if( !m_bActiveTutorial ) return FALSE ;									// 튜토리얼이 비활성화 상태면, return 처리를 한다.리를 한다.
	
	cTutorial* pCurTutorial ;												// 현재 튜토리얼 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;													// 튜토리얼 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_Tutorial_List.GetHeadPosition() ;								// 튜토리얼 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																// 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pCurTutorial = NULL ;												// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
		pCurTutorial = (cTutorial*)m_Tutorial_List.GetNext(pos) ;			// 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pCurTutorial ) continue ;										// 튜토리얼 정보가 유효하지 않으면 continue 처리를 한다.

		if( pCurTutorial->Get_StartCondition1() != byTutorialType ) continue ;	// 튜토리얼 타입 체크.

		if( pCurTutorial->Is_Completed() ) return TRUE ;					// 결과 return 처리.
		else return FALSE ;
	}

	ASSERT(TRUE) ;

#ifdef _GMTOOL_
	MessageBox(NULL, "Invalid tutorial type!!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
	return FALSE ;
}





//=========================================================================
// 080502 LYW : 지정 된 타입의 튜토리얼을 시작하는 함수 추가.
//=========================================================================
void cTutorialManager::Start_Specification_Tutorial(BYTE byTutorialType)
{
	if( !m_bActiveTutorial ) return ;										// 튜토리얼이 비활성화 상태면, return 처리를 한다.리를 한다.
	
	cTutorial* pCurTutorial ;												// 현재 튜토리얼 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;													// 튜토리얼 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_Tutorial_List.GetHeadPosition() ;								// 튜토리얼 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																// 위치 정보가 유효할 동안 whlie문을 돌린다.
	{
		pCurTutorial = NULL ;												// 튜토리얼 정보를 받을 포인터를 null 처리를 한다.
		pCurTutorial = (cTutorial*)m_Tutorial_List.GetNext(pos) ;			// 위치에 해당하는 튜토리얼 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pCurTutorial ) continue ;										// 튜토리얼 정보가 유효하지 않으면 continue 처리를 한다.

		if( pCurTutorial->Get_StartCondition1() != byTutorialType ) continue ;	// 튜토리얼 타입 체크.

		if( pCurTutorial->Is_Completed() ) return ;

		Start_CurTutorial(pCurTutorial) ;

		return ;
	}

	ASSERT(TRUE) ;

#ifdef _GMTOOL_
	MessageBox(NULL, "Invalid tutorial type!!", __FUNCTION__, MB_OK) ;
#endif //_GMTOOL_
}

// 090417 ONS 전직 가이드 개선 튜토리얼 
BOOL cTutorialManager::CheckTutorialByIndex(int TutorialIndex)
{
	if( !m_bOnOff_Tutorial ) return FALSE ;									// 튜토리얼이 비활성화 상태면, return 처리를 한다.리를 한다.
	
	cTutorial* pCurTutorial ;												// 현재 튜토리얼 정보를 담을 포인터를 선언한다.
	pCurTutorial = Get_Tutorial(TutorialIndex);

	if( !pCurTutorial )	return FALSE;

	return TRUE;
}

