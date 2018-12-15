//////////////////////////////////////////////////////////////////////
// class name : cResourceManager 
// Lastest update : 2003.3.10. by taiyo
//////////////////////////////////////////////////////////////////////


#ifndef _cRESOURCEMANAGER_H_
#define _cRESOURCEMANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cResourceDef.h"
#include "cWindowDef.h"
#include "../4DyuchiGRX_common/IGeometry.h"


class cPtrList;

/**********************************************************************
 * cResourceManager class
 * - ÀÎµ¦½º·Î FileInfo¸¦ °¡Á®¿È
 * - ÆÄÀÏÀÌ¸§À¸·Î FileInfo¸¦ °¡Á®¿È
 * - 
 **********************************************************************/
#define MAX_FILENAME_SIZE			64
struct IMAGE_NODE 
{
	IMAGE_NODE():		pSpriteObject(NULL){}
	IDISpriteObject *	pSpriteObject;
	char				szFileName[ MAX_FILENAME_SIZE ];
	cImageSize			size;
	int					layer;
};

#define RESRCMGR	USINGTON(cResourceManager)

// 080320 LYW --- cResourceManager : ?대옒???꾩씠肄?愿由ъ뿉 ?꾩슂??援ъ“泥?異붽?.
 struct IDINDEX
 {
 	int nIdx ;
 	int nClassIdx ;
 } ;

class cResourceManager
{

	struct MSG_NODE 
	{
		char *			msg;
		int				len;
	};

	char			m_szImageListPath	[	MAX_FILENAME_SIZE	];
	char			m_szMsgListPath		[	MAX_FILENAME_SIZE	];
	IMAGE_NODE *	m_pImageInfoArray;
	int				m_nMaxImageInfoNum;
	cPtrList		m_pImageList;
	// 091228 LUJ, 키: 메시지 번호
	typedef stdext::hash_map< int, std::string > StringContainer;
	StringContainer mInterfaceMessageContainer;

public:	

//	MAKESINGLETON(cResourceManager);
	cResourceManager();
	virtual ~cResourceManager();

	void Init(char * szImageListPath, char * szMsgListPath);

	// get node info
	IMAGE_NODE * GetInfo( int idx );

	// Get IDISpriteObject
	IDISpriteObject * GetImageInfo( int idx );
	IDISpriteObject * GetImageInfo( char * fileName, cImageSize size, int layer );
	LPTSTR GetMsg(int idx) const;
	void Release();
	void ReleaseResourceAll();
	void ReleaseResource(int layer);

	public :
 	cPtrList		m_ClassIconList ;
 	BYTE			m_byIconCount ;
 
 	void LoadClassIcon();
 	void ReleaseIconList(); 
   	int GetClassNameNum(int classIdx);
 	int GetClassIconNum(int nClass);   
 	BYTE GetClassIconCount() const { return m_byIconCount ; }
	void LoadInterfaceMessage();
};
EXTERNGLOBALTON(cResourceManager);
#endif // _cRESOURCEMANAGER_H_
