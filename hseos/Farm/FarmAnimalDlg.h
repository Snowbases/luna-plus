#pragma once

#include ".\Interface\cDialog.h"
#include ".\interface\cButton.h"
#include ".\interface\cGuagen.h"
#include ".\interface\cStatic.h"
#include ".\interface\cPushupButton.h"
#include "VirtualItem.h"
#include "cImageSelf.h"
#include "Item.h"
#include "../hseos/Farm/Animal.h"

class cButton;
class cIconDialog;
class CObjectGuagen;

class CAnimalDialog :
	public cDialog
{
	struct ImageData
	{
		cImageSelf*	pImage;
		VECTOR2*	pImageRect;
		VECTOR3		vDir;
		DWORD		dwCreateTime;
		float		fAlpha;

		void Clear() {pImage=NULL; pImageRect=NULL; dwCreateTime=0; fAlpha=0.0f;}
		void SetImage(cImageSelf* image, VECTOR2* imagerect, VECTOR3 dir) {
			pImage=image; pImageRect=imagerect; vDir=dir; dwCreateTime=gCurTime; fAlpha=1.0f;
		}
	};

	CAnimal*			m_pAnimal;
	// 가축 슬롯 넘버
	cStatic*			m_pSlotNum;
	// 가축 이름
	cStatic*			m_pName;
	// 가축 만족도 게이지
	CObjectGuagen*		m_pContentmentGuage;
	// 가축 만족도 수치
	cStatic*			m_pContentment;
	// 가축 HP 게이지
	CObjectGuagen*		m_pHPGuage;
	// 가축 HP 수치
	cStatic*			m_pHP;
	// 가축 관심도 게이지
	CObjectGuagen*		m_pInterestGuage;
	// 가축 관심도 수치
	cStatic*			m_pInterest;
	// 가축 이미지
	cStatic*			m_pImage;
	// 수확하기 버튼
	cPushupButton*		m_pGetItemBtn;

	BOOL				m_bLockGetItemBtn;

	ImageData			m_CurImage;

public:

	CAnimalDialog(void);
	virtual ~CAnimalDialog(void);

	void Linking();
	virtual void SetActive(BOOL val);
	VOID SetAnimal(CAnimal* pAnimal);
	VOID SetSlotNum(char* pText);
	VOID SetName(char* pText);
	VOID SetContentment(char* pText);
	VOID SetInterest(char* pText);
	VOID SetHP(char* pText);

	VOID SetContentmentValue(GUAGEVAL val, DWORD estTime);
	VOID SetInterestValue(GUAGEVAL val, DWORD estTime);
	VOID SetHPValue(GUAGEVAL val, DWORD estTime);

	VOID RefreshAnimalInfo();
	int GetImgNum( int itemKind );
	VOID SetAnimalImgInfo( ITEM_KIND, int imageIndex );
	virtual void OnActionEvent(LONG id, void* p, DWORD event) ;

	cPushupButton*	GetItemBtn()	{	return m_pGetItemBtn;	}
};
