#pragma once


#include "interface/cDialog.h"


class CItem;
class CVirtualItem;
class cIconDialog;
class cStatic;

struct EnchantScript;


class CEnchantDialog : public  cDialog
{
public:
	CEnchantDialog();
	virtual ~CEnchantDialog();

	void Linking();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );
	virtual DWORD ActionEvent(CMouse*);
	virtual void OnActionEvent(LONG lId, void* p, DWORD we);
	virtual void SetActive( BOOL );

	void Restore();
	void Succeed( const MSG_ITEM_ENCHANT_ACK& );
	void Fail();

	void Send();
	void SetProtectedActive(CItem&);

private:
	ITEMBASE mProtectionItem;

	// 080228 LUJ, 일반/보호 모드 표시용 컨트롤
	cWindow*	mTitle;
	cWindow*	mProtectTitle;
	cWindow*	mProtectSymbol;

	// 090714 ShinJS --- 인챈트시 소비자금 출력용 Static
	cStatic*	m_pEnchantCost;


private:
	BOOL IsEnchantable( const ITEMBASE*, const EnchantScript* ) const;
	void RemoveMaterial();

	cIconDialog*	mIconDialog;
	CVirtualItem*	mSourceItem;
	CItem*			mMaterialItem;
	cStatic*		mResult;

	MSG_ITEM_ENCHANT_SYN	mMessage;

	// 090917 ShinJS --- 인챈트 시도 이전의 옵션정보 저장
	ITEM_OPTION		m_PrevOption;
};