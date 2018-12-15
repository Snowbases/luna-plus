// cResourceManager.cpp: implementation of the cResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cResourceManager.h"
#include "../Engine/GraphicEngine.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(cResourceManager);
cResourceManager::cResourceManager()
{
	m_pImageInfoArray = NULL;
	m_nMaxImageInfoNum = 0;

	// 080320 LYW --- cResourceManager : Initialize class icon part.
 	m_ClassIconList.RemoveAll() ;
 	m_byIconCount = 0 ;
}

cResourceManager::~cResourceManager()
{
	Release();

	// 080320 LYW --- cResourceManager : Release class icon part.
 	ReleaseIconList() ;	
}
void cResourceManager::Init(char * szImagePath, char * szMsgPath)
{
	strcpy( m_szImageListPath,	szImagePath );
	strcpy( m_szMsgListPath,	szMsgPath );


	// ImageInfo List
	CMHFile fp;
	fp.Init(m_szImageListPath, "rb");
	m_nMaxImageInfoNum = fp.GetInt();
	m_pImageInfoArray = new IMAGE_NODE[ m_nMaxImageInfoNum ];
	for( int i = 0 ; i < m_nMaxImageInfoNum ; ++i )
	{
		fp.GetInt();
		strcpy( m_pImageInfoArray[i].szFileName, fp.GetString() );
		m_pImageInfoArray[i].size.x			= fp.GetFloat();
		m_pImageInfoArray[i].size.y			= fp.GetFloat();
		m_pImageInfoArray[i].layer			= fp.GetInt();
		m_pImageInfoArray[i].pSpriteObject	= NULL;
	}
	fp.Release();

	// 080320 LYW --- cResourceManager : 클래스 아이콘을 로딩한다.
 	LoadClassIcon() ;
	LoadInterfaceMessage();
}
IMAGE_NODE * cResourceManager::GetInfo( int idx )
{	
	if( idx < 0 )
		return NULL;
	if( idx >= m_nMaxImageInfoNum ) 
		return NULL;
	return &m_pImageInfoArray[idx];
}
IDISpriteObject * cResourceManager::GetImageInfo( int idx )
{
	if( idx >= m_nMaxImageInfoNum ) 
		return NULL;

	if( !m_pImageInfoArray[idx].pSpriteObject )
	{
		m_pImageInfoArray[idx].pSpriteObject = g_pExecutive->GetRenderer()->CreateSpriteObject(m_pImageInfoArray[idx].szFileName,0);
	}

	return ( m_pImageInfoArray[idx].pSpriteObject );
}
IDISpriteObject * cResourceManager::GetImageInfo( char * fileName, cImageSize size, int layer )
{
	for( int i = 0 ; i < m_nMaxImageInfoNum ; ++i )
	{
		if( m_pImageInfoArray[i].pSpriteObject && 0 == strcmp( m_pImageInfoArray[i].szFileName, fileName ) )
		{
			return m_pImageInfoArray[i].pSpriteObject;
		}
	}

	PTRLISTPOS pos = m_pImageList.GetHeadPosition();
	while(pos)
	{
		IMAGE_NODE * node = (IMAGE_NODE *)m_pImageList.GetNext(pos);
		if(0 == strcmp( node->szFileName, fileName ) )
		{	
			return node->pSpriteObject;
		}
	}

	IMAGE_NODE * pNewNode	= new IMAGE_NODE;
	strcpy( pNewNode->szFileName, fileName );
	pNewNode->pSpriteObject = g_pExecutive->GetRenderer()->CreateSpriteObject(fileName,0);
	pNewNode->layer			= layer;
	pNewNode->size			= size;
	
	if( !pNewNode->pSpriteObject )
	{
		char buffer[255];
		GetCurrentDirectory(255, buffer);
		strcat(buffer, fileName);
		ASSERTMSG(0, buffer);
		if( pNewNode ) delete pNewNode;
	}

	m_pImageList.AddTail( pNewNode );
	return pNewNode->pSpriteObject;
}


void cResourceManager::Release()
{
	ReleaseResourceAll();
	if(m_pImageInfoArray)
	{
		delete [] m_pImageInfoArray;
		m_pImageInfoArray = NULL;
	}
}

void cResourceManager::ReleaseResourceAll()
{
	if(m_pImageInfoArray)
	{
		for( int i = 0 ; i < m_nMaxImageInfoNum ; ++i )
		{
			if( m_pImageInfoArray[i].pSpriteObject )
			{
				m_pImageInfoArray[i].pSpriteObject->Release();
				m_pImageInfoArray[i].pSpriteObject = NULL;
			}
		}
	}

	PTRLISTPOS pos = m_pImageList.GetHeadPosition();
	while(pos)
	{
		IMAGE_NODE * node = (IMAGE_NODE *)m_pImageList.GetNext(pos);
		node->pSpriteObject->Release();
		node->pSpriteObject = NULL;
		SAFE_DELETE(node);
	}
	m_pImageList.RemoveAll();
}

LPTSTR cResourceManager::GetMsg(int idx) const
{
	const StringContainer::const_iterator iterator = mInterfaceMessageContainer.find(idx);

	if(mInterfaceMessageContainer.end() == iterator)
	{
		return "";
	}

	return LPTSTR(iterator->second.c_str());
}

void cResourceManager::LoadInterfaceMessage()
{
	CMHFile file;
	file.Init(
		"Data\\Interface\\Windows\\InterfaceMsg.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH * 2] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = _T("\t");
		LPCTSTR token = _tcstok(
			buffer,
			seperator);

		if(0 == token)
		{
			continue;
		}

		std::string& text = mInterfaceMessageContainer[_ttoi(token)];
		text.clear();

		for(token = _tcstok(0, seperator);
			token;
			token = _tcstok(0, seperator))
		{
			text += token;
			text += " ";
		}
	}
}

void cResourceManager::ReleaseResource(int layer)
{
	for( int i = 0 ; i < m_nMaxImageInfoNum ; ++i )
	{
		if( m_pImageInfoArray[i].pSpriteObject && m_pImageInfoArray[i].layer == layer)
		{
			m_pImageInfoArray[i].pSpriteObject->Release();
			m_pImageInfoArray[i].pSpriteObject = NULL;
			return;
		}
	}

	PTRLISTPOS pos = m_pImageList.GetHeadPosition();
	while(pos)
	{
		IMAGE_NODE * node = (IMAGE_NODE *)m_pImageList.GetAt(pos);
		if(node->layer == layer)
		{
			node->pSpriteObject->Release();
			node->pSpriteObject = NULL;
			SAFE_DELETE(node);
			m_pImageList.RemoveAt(pos);
			break;
		}
		m_pImageList.GetNext(pos);
	}
}

// 070723 LYW --- cResourceManager : Add function to return msg number of class name.
// 캐릭터의 클래스명을 참조하는 곳이 많아서, 리소스 매니져에 클래스 명을 리턴하는 펑션을 추가함.
int cResourceManager::GetClassNameNum(int classIdx)
{
	int msg = 0;
	switch( classIdx )
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

	// 080709 LYW --- cResourceManager : 아크레인저 직업을 추가한다.
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

	// 080709 LYW --- cResourceManager : 아크레인저 직업을 추가한다.
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

	return msg ;
}

// 080320 LYW --- cResourceManager : Add function to return class icon index.
 int cResourceManager::GetClassIconNum(int nClass)
 {
 	IDINDEX* pIdIdx = NULL ;														// 인덱스와 클래스 인덱스 정보를 받을 포인터를 선언한다.
 
 	PTRLISTPOS pos	= NULL ;														// 리스트상의 위치 정보를 받을 포인터를 선언한다.
 
 	pos = m_ClassIconList.GetHeadPosition() ;										// 클래스 아이콘 리스트를 해더로 세팅한다.
 
 	while(pos)																		// 위치정보가 있는동안, while을 돌린다.
 	{
 		pIdIdx = (IDINDEX*)m_ClassIconList.GetNext(pos) ;							// pos에 해당하는 인덱스, 클래스 인덱스 정보를 받는다.
 
 		if( pIdIdx )																// 받은 정보가 유효한지 체크한다.
 		{
 			if( pIdIdx->nClassIdx == nClass )										// 인자로 넘어온 클래스 인덱스와, 리스트의 클래스 인덱스가 같다면,
 			{
 				return pIdIdx->nIdx ;												// 인덱스를 리턴한다.
 			}
 		}
 	}
 
 	return -1 ;	
 }
 
 // 080320 LYW --- cResourceManager : Add function to load class icon.
 void cResourceManager::LoadClassIcon()
 {
 	CMHFile fp;																		// 묵향 파일을 선언한다.
 
 	fp.Init( FILE_IMAGE_CLASS_ICON_PATH, "rb" ) ;									// 클래스 아이콘 파일을 읽어들인다.
 
 	int nNull = 0 ;																	// 의미없는 수를 담을 변수를 선언한다.
 
 	while(FALSE == fp.IsEOF())
 	{
 		IDINDEX* pIdIdx = new IDINDEX ;												// 인덱스와 클래스 인덱스를 담을 메모리를 할당한다.
 
 		pIdIdx->nIdx = fp.GetInt() ;												// 파일로 부터 인덱스를 받는다.
 
 		pIdIdx->nClassIdx = fp.GetInt() ;											// 파일로 부터 클래스 인덱스를 받는다.
 
 		m_ClassIconList.AddTail(pIdIdx) ;											// 클래스 아이콘 리스트에 추가한다.
 
 		for( int count = 0 ; count < 5 ; ++count )									// 나머지 뒤의 숫자 수 만큼 for문을 돌린다.
 		{
 			nNull = fp.GetInt() ;													// 여기서 사용하지 않는 수는 그냥 읽기만 한다.
 		}
 	}
 
 	m_byIconCount = BYTE(m_ClassIconList.GetCount());
 }
 
 // 080320 LYW --- cResourceManager : Add function to release icon list.
 void cResourceManager::ReleaseIconList()
 {
 	IDINDEX* pIdIdx = NULL ;														// 인덱스와 클래스 인덱스 정보를 받을 포인터를 선언한다.
 
 	PTRLISTPOS pos	= NULL ;														// 리스트상의 위치 정보를 받을 포인터를 선언한다.
 
 	pos = m_ClassIconList.GetHeadPosition() ;										// 클래스 아이콘 리스트를 해더로 세팅한다.
 
 	while(pos)																		// 위치정보가 있는동안, while을 돌린다.
 	{
 		pIdIdx = (IDINDEX*)m_ClassIconList.GetNext(pos) ;							// pos에 해당하는 인덱스, 클래스 인덱스 정보를 받는다.
 
 		if( pIdIdx )																// 받은 정보가 유효한지 체크한다.
 		{
 			delete pIdIdx ;															// 받은 주소의 메모리를 삭제한다.
 
 			pIdIdx = NULL ;															// 주소를 가리키는 포인터를 NULL 처리 한다.
 		}
 	}
 
 	m_ClassIconList.RemoveAll() ;													// 리스트를 모두 비운다.
 }