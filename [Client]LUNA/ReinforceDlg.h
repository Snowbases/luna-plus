#pragma once


#include "interface/cDialog.h"


class CItem;
class CVirtualItem;
class cIconDialog;
class cIconGridDialog;
class cStatic;


class CReinforceDlg : public cDialog  
{
public:
	CReinforceDlg();
	virtual ~CReinforceDlg();

	void Linking();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );

	void OnActionEvent(LONG lId, void * p, DWORD we);
	virtual void SetActive( BOOL );
	
	void Succeed( const MSG_ITEM_REINFORCE_OPTION_ACK& );
	void Fail();
	void Restore();
	void Send();


	// 080228 LUJ, 보호 아이템 관리
public:	
	void SetProtectedActive( CItem& );

private:
	ITEMBASE mProtectionItem;


private:
	// 현재 강화할 아이템이 있으면 새로운 재료를 등록시키고 그렇지 않으면 등록하지 않는다.
	// 080228 LUJ, 널포인터를 막기 위해 참조형으로 변경
	void AddMaterial( CItem& );

	// 재료를 소거한다.
	void RemoveMaterial();

	void Submit( BOOL isCheck );	
	void Release();
 

private:
	CVirtualItem*		mItemIcon;

	typedef std::vector< CItem* >	MaterialIcon;
	MaterialIcon					mMaterialIcon;
	
	cIconDialog*		mBaseDialog;
	cIconGridDialog*	mIconGridDialog;
	cStatic*			mTextQuantity;

	int					mQuantity;
	int					mMaxQuantity;

	BOOL				mIsReinforcing;

	MSG_ITEM_REINFORCE_SYN	mMessage;

	// 080228 LUJ, 보호되는 옵션. 보호 상태일 때 수치를 낮게 받은 경우 별도의 메시지를 표시하기 위해 저장한다
	ITEM_OPTION		mProtectedOption;

	// 080228 LUJ, 일반/보호 모드 표시용 컨트롤
	cWindow*	mTitle;
	cWindow*	mProtectTitle;
	cWindow*	mProtectSymbol;

	// 080929 LUJ, 보조 아이템 처리
public:
	// 080929 LUJ, 보조 아이템을 등록한다
	void SetSupportItem( const ITEMBASE& );
private:
	ICONBASE mSupportItem;
};