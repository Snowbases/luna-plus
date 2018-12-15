//=================================================================================================
//	FILE		: CProfileDlg.cpp
//	PURPOSE		: Profile for character.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 11, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include "stdafx.h"
#include ".\profiledlg.h"
#include "WindowIDEnum.h"
#include "./interface/cStatic.h"
// 061226 LYW --- Include resource manager's header file.
#include "./interface/cResourceManager.h"

// 071004 LYW --- ProfileDlg : Include chatting manager.
#include "ChatManager.h"


//=================================================================================================
// NAME			: CProfileDlg()
// PURPOSE		: The Function Constructor.
// ATTENTION	:
//=================================================================================================
CProfileDlg::CProfileDlg(void)
{
	memset(m_szClass, 0, sizeof(char)*5*12*24 ) ;
	// 061226 LYW --- Change load string part.
	/*
	strcpy(m_szClass[0][0],  "워리어") ;
	strcpy(m_szClass[0][1],  "로그") ;
	strcpy(m_szClass[0][2],  "메이지") ;
	strcpy(m_szClass[0][3],  "파이터") ;
	strcpy(m_szClass[0][4],  "로그") ;
	strcpy(m_szClass[0][5],  "메이지") ;
	strcpy(m_szClass[1][0],  "워리어") ;
	strcpy(m_szClass[1][1],  "시프") ;
	strcpy(m_szClass[1][2],  "카발리스트") ;
	strcpy(m_szClass[1][3],  "하이나이트") ;
	strcpy(m_szClass[1][4],  "아처") ;
	strcpy(m_szClass[1][5],  "어콜라이트") ;
	strcpy(m_szClass[2][0],  "다크나이트") ;
	strcpy(m_szClass[2][1],  "버서커") ;
	strcpy(m_szClass[2][2],  "호크") ;
	strcpy(m_szClass[2][3],  "헌터") ;
	strcpy(m_szClass[2][4],  "소서러") ;
	strcpy(m_szClass[2][5],  "메지션") ;
	strcpy(m_szClass[2][6],  "팔라딘") ;
	strcpy(m_szClass[2][7],  "가드") ;
	strcpy(m_szClass[2][8],  "스카우트") ;
	strcpy(m_szClass[2][9],  "써그") ;
	strcpy(m_szClass[2][10], "클레릭") ;
	strcpy(m_szClass[2][11], "몽크") ;
	strcpy(m_szClass[3][0],  "소드마스터") ;
	strcpy(m_szClass[3][1],  "팬덤") ;
	strcpy(m_szClass[3][2],  "위저드") ;
	strcpy(m_szClass[3][3],  "스킨 마스터") ;
	strcpy(m_szClass[3][4],  "슈터") ;
	strcpy(m_szClass[3][5],  "프리스트") ;
	strcpy(m_szClass[4][0],  "그랜드 소드 마스터") ;
	strcpy(m_szClass[4][1],  "데스나이트") ;
	strcpy(m_szClass[4][2],  "대거 마스터") ;
	strcpy(m_szClass[4][3],  "어쎄신") ;
	strcpy(m_szClass[4][4],  "엘리멘탈 메이지") ;
	strcpy(m_szClass[4][5],  "네츄럴 메이지") ;
	strcpy(m_szClass[4][6],  "그랜드 스킨 마스터") ;
	strcpy(m_szClass[4][7],  "엔젤 나이트") ;
	strcpy(m_szClass[4][8],  "보우 마스트") ;
	strcpy(m_szClass[4][9],  "레인저") ;
	strcpy(m_szClass[4][10], "세인트") ;
	strcpy(m_szClass[4][11], "디바인") ;
	*/
	// 070122 LYW --- Modified this part.
	/*
	strcpy(m_szClass[0][0],  RESRCMGR->GetMsg( 982)) ;
	strcpy(m_szClass[0][1],  RESRCMGR->GetMsg( 980)) ;
	strcpy(m_szClass[0][2],  RESRCMGR->GetMsg( 981)) ;
	strcpy(m_szClass[0][3],  RESRCMGR->GetMsg( 982)) ;
	strcpy(m_szClass[0][4],  RESRCMGR->GetMsg( 980)) ;
	strcpy(m_szClass[0][5],  RESRCMGR->GetMsg( 981)) ;
	*/
	strcpy(m_szClass[0][0],  RESRCMGR->GetMsg( 33)) ;
	strcpy(m_szClass[0][1],  RESRCMGR->GetMsg( 34)) ;
	strcpy(m_szClass[0][2],  RESRCMGR->GetMsg( 35)) ;
	strcpy(m_szClass[0][3],  RESRCMGR->GetMsg( 33)) ;
	strcpy(m_szClass[0][4],  RESRCMGR->GetMsg( 34)) ;
	strcpy(m_szClass[0][5],  RESRCMGR->GetMsg( 35)) ;

	strcpy(m_szClass[1][0],  RESRCMGR->GetMsg( 979)) ;
	strcpy(m_szClass[1][1],  RESRCMGR->GetMsg( 983)) ;
	strcpy(m_szClass[1][2],  RESRCMGR->GetMsg( 984)) ;
	strcpy(m_szClass[1][3],  RESRCMGR->GetMsg( 985)) ;
	strcpy(m_szClass[1][4],  RESRCMGR->GetMsg( 986)) ;
	strcpy(m_szClass[1][5],  RESRCMGR->GetMsg( 987)) ;
	strcpy(m_szClass[2][0],  RESRCMGR->GetMsg( 988)) ;
	strcpy(m_szClass[2][1],  RESRCMGR->GetMsg( 989)) ;
	strcpy(m_szClass[2][2],  RESRCMGR->GetMsg( 990)) ;
	strcpy(m_szClass[2][3],  RESRCMGR->GetMsg( 991)) ;
	strcpy(m_szClass[2][4],  RESRCMGR->GetMsg( 992)) ;
	strcpy(m_szClass[2][5],  RESRCMGR->GetMsg( 993)) ;
	strcpy(m_szClass[2][6],  RESRCMGR->GetMsg( 994)) ;
	strcpy(m_szClass[2][7],  RESRCMGR->GetMsg( 995)) ;
	strcpy(m_szClass[2][8],  RESRCMGR->GetMsg( 996)) ;
	strcpy(m_szClass[2][9],  RESRCMGR->GetMsg( 997)) ;
	strcpy(m_szClass[2][10], RESRCMGR->GetMsg( 998)) ;
	strcpy(m_szClass[2][11], RESRCMGR->GetMsg( 999)) ;
	strcpy(m_szClass[3][0],  RESRCMGR->GetMsg(1000)) ;
	strcpy(m_szClass[3][1],  RESRCMGR->GetMsg(1001)) ;
	strcpy(m_szClass[3][2],  RESRCMGR->GetMsg(1002)) ;
	strcpy(m_szClass[3][3],  RESRCMGR->GetMsg(1003)) ;
	strcpy(m_szClass[3][4],  RESRCMGR->GetMsg(1004)) ;
	strcpy(m_szClass[3][5],  RESRCMGR->GetMsg(1005)) ;
	strcpy(m_szClass[4][0],  RESRCMGR->GetMsg(1006)) ;
	strcpy(m_szClass[4][1],  RESRCMGR->GetMsg(1007)) ;
	strcpy(m_szClass[4][2],  RESRCMGR->GetMsg(1008)) ;
	strcpy(m_szClass[4][3],  RESRCMGR->GetMsg(1009)) ;
	strcpy(m_szClass[4][4],  RESRCMGR->GetMsg(1010)) ;
	strcpy(m_szClass[4][5],  RESRCMGR->GetMsg(1011)) ;
	strcpy(m_szClass[4][6],  RESRCMGR->GetMsg(1012)) ;
	strcpy(m_szClass[4][7],  RESRCMGR->GetMsg(1013)) ;
	strcpy(m_szClass[4][8],  RESRCMGR->GetMsg(1014)) ;
	strcpy(m_szClass[4][9],  RESRCMGR->GetMsg(1015)) ;
	strcpy(m_szClass[4][10], RESRCMGR->GetMsg(1016)) ;
	strcpy(m_szClass[4][11], RESRCMGR->GetMsg(1017)) ;

	memset( m_szRacial, 0, sizeof(char)*2*16 ) ;
	// 061226 LYW --- Change load string part.
	/*
	strcpy( m_szRacial[0], "인간") ;
	strcpy( m_szRacial[1], "엘프") ;
	*/
	// 070122 LYW --- Modified this part.
	/*
	strcpy( m_szRacial[0], RESRCMGR->GetMsg(918)) ;
	strcpy( m_szRacial[1], RESRCMGR->GetMsg(919)) ;
	*/

	strcpy( m_szRacial[0], RESRCMGR->GetMsg(247)) ;
	strcpy( m_szRacial[1], RESRCMGR->GetMsg(248)) ;

	// 090504 ONS 신규종족 추가에 따른 처리
	strcpy( m_szRacial[2], RESRCMGR->GetMsg(1129)) ;

	memset( m_szMapName, 0, sizeof(char)*24 ) ;

	int count = 0 ;
	for( count = 0 ; count < MAX_PRINTCOUNT ; ++count )
	{
		m_pStatic[ count ] = NULL ;
	}
}


//=================================================================================================
// NAME			: CProfileDlg()
// PURPOSE		: The Function Destructor.
// ATTENTION	:
//=================================================================================================
CProfileDlg::~CProfileDlg(void)
{
}


//=================================================================================================
// NAME			: Linking()
// PURPOSE		: Link member statics to registed statics.
// ATTENTION	:
//=================================================================================================
void CProfileDlg::Linking()
{
	int count = 0 ;
	for( count = 0 ; count < MAX_PRINTCOUNT ; ++count )
	{
		m_pStatic[ count ] = ( cStatic* )GetWindowForID( CP_CHARNAME + count ) ;
	}
}


//=================================================================================================
// NAME			: UpdateProfile()
// PURPOSE		: Update information to profile dialog.
// ATTENTION	:
//=================================================================================================
void CProfileDlg::UpdateProfile( char* strName, CHARACTER_TOTALINFO* charInfo )
{
	char tempStr[24] ;
	
	m_pStatic[0]->SetStaticText( strName ) ;

	memset( tempStr, 0, sizeof(char)*24 ) ;
	sprintf( tempStr, "%d", charInfo->Level ) ;
	m_pStatic[1]->SetStaticText( tempStr ) ;

	memset( tempStr, 0, sizeof(char)*24 ) ;

	WORD idx = 1;
	if( charInfo->JobGrade > 1 )
	{
		idx = charInfo->Job[charInfo->JobGrade - 1];
	}
	WORD job = ( charInfo->Job[0] * 1000 ) + ( ( charInfo->Race + 1 ) * 100 ) + ( charInfo->JobGrade * 10 ) + idx;
	int msg = 0;
	switch( job )
	{
	case 1111:	msg = 364;	break;
	case 1121:	msg = 365;	break;
	case 1122:	msg = 366;	break;
	case 1131:	msg = 367;	break;
	case 1132:	msg = 368;	break;
	case 1133:	msg = 369;	break;
	case 1141:	msg = 370;	break;
	case 1142:	msg = 371;	break;
	case 1143:	msg = 372;	break;
	case 1144:	msg = 373;	break;
	case 1151:	msg = 374;	break;
	case 1152:	msg = 375;	break;
	case 1153:	msg = 376;	break;
	case 1154:	msg = 377;	break;
	case 1155:	msg = 378;	break;
	case 1156:	msg = 379;	break;
	case 1161:	msg = 380;	break;
	case 1162:	msg = 381;	break;
	case 1163:	msg = 382;	break;

	case 1211:	msg = 364;	break;
	case 1221:	msg = 365;	break;
	case 1222:	msg = 366;	break;
	case 1231:	msg = 367;	break;
	case 1232:	msg = 368;	break;
	case 1233:	msg = 369;	break;
	case 1241:	msg = 370;	break;
	case 1242:	msg = 371;	break;
	case 1243:	msg = 372;	break;
	case 1244:	msg = 373;	break;
	case 1251:	msg = 374;	break;
	case 1252:	msg = 375;	break;
	case 1253:	msg = 376;	break;
	case 1254:	msg = 377;	break;
	case 1255:	msg = 378;	break;
	case 1256:	msg = 379;	break;
	case 1261:	msg = 380;	break;
	case 1262:	msg = 381;	break;
	case 1263:	msg = 382;	break;

	case 2111:	msg = 383;	break;
	case 2121:	msg = 384;	break;
	case 2122:	msg = 385;	break;
	case 2131:	msg = 386;	break;
	case 2132:	msg = 387;	break;
	case 2133:	msg = 388;	break;
	case 2141:	msg = 389;	break;
	case 2142:	msg = 390;	break;
	case 2143:	msg = 391;	break;
	case 2144:	msg = 392;	break;
	case 2151:	msg = 393;	break;
	case 2152:	msg = 394;	break;
	case 2153:	msg = 395;	break;
	case 2154:	msg = 396;	break;

	// 080709 LYW --- ProfileDlg : 아크레인저 직업을 추가한다.
	case 2155:  msg = 1004;	break;

	case 2161:	msg = 397;	break;
	case 2162:	msg = 398;	break;
	case 2163:	msg = 399;	break;

	case 2211:	msg = 383;	break;
	case 2221:	msg = 384;	break;
	case 2222:	msg = 385;	break;
	case 2231:	msg = 386;	break;
	case 2232:	msg = 387;	break;
	case 2233:	msg = 388;	break;
	case 2241:	msg = 389;	break;
	case 2242:	msg = 390;	break;
	case 2243:	msg = 391;	break;
	case 2244:	msg = 392;	break;
	case 2251:	msg = 393;	break;
	case 2252:	msg = 394;	break;
	case 2253:	msg = 395;	break;
	case 2254:	msg = 396;	break;

	// 080709 LYW --- ProfileDlg : 아크레인저 직업을 추가한다.
	case 2255:	msg = 1004;	break;

	case 2261:	msg = 397;	break;
	case 2262:	msg = 398;	break;
	case 2263:	msg = 399;	break;

	case 3111:	msg = 400;	break;
	case 3121:	msg = 401;	break;
	case 3122:	msg = 402;	break;
	case 3131:	msg = 403;	break;
	case 3132:	msg = 404;	break;
	case 3133:	msg = 405;	break;
	case 3141:	msg = 406;	break;
	case 3142:	msg = 407;	break;
	case 3143:	msg = 408;	break;
	case 3144:	msg = 409;	break;
	case 3151:	msg = 410;	break;
	case 3152:	msg = 411;	break;
	case 3153:	msg = 412;	break;
	case 3154:	msg = 413;	break;
	case 3155:	msg = 414;	break;
	case 3161:	msg = 415;	break;
	case 3162:	msg = 416;	break;
	case 3163:	msg = 417;	break;

	case 3211:	msg = 400;	break;
	case 3221:	msg = 401;	break;
	case 3222:	msg = 402;	break;
	case 3231:	msg = 403;	break;
	case 3232:	msg = 404;	break;
	case 3233:	msg = 405;	break;
	case 3241:	msg = 406;	break;
	case 3242:	msg = 407;	break;
	case 3243:	msg = 408;	break;
	case 3244:	msg = 409;	break;
	case 3251:	msg = 410;	break;
	case 3252:	msg = 411;	break;
	case 3253:	msg = 412;	break;
	case 3254:	msg = 413;	break;
	case 3255:	msg = 414;	break;
	case 3261:	msg = 415;	break;
	case 3262:	msg = 416;	break;
	case 3263:	msg = 417;	break;
	// 090504 ONS 신규종족 추가에 따른 직업(전직단계)추가
	case 4311:	msg = 1132; break;
	case 4321:	msg = 1133; break;
	case 4331:	msg = 1134; break;
	case 4341:	msg = 1135; break;
	case 4351:	msg = 1136; break;
	}

	m_pStatic[2]->SetStaticText( RESRCMGR->GetMsg( msg ) );
	m_pStatic[3]->SetStaticText( m_szRacial[ charInfo->Race ] );
	m_pStatic[4]->SetStaticText( GetMapName( charInfo->LoginMapNum ) );
	m_pStatic[5]->SetStaticText( charInfo->GuildName );
}


//=================================================================================================
// NAME			: UpdateProfile()
// PURPOSE		: Add function to refresh pofile.
// ATTENTION	:
//=================================================================================================
void CProfileDlg::RefreshProfile()
{
	m_pStatic[0]->SetStaticText( "" ) ;
	m_pStatic[1]->SetStaticText( "" ) ;
	m_pStatic[2]->SetStaticText( "" ) ;
	m_pStatic[3]->SetStaticText( "" ) ;
	m_pStatic[4]->SetStaticText( "" ) ;
	m_pStatic[5]->SetStaticText( "" ) ;
}
