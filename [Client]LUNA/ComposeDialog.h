/*
080916 LUJ, 중복되지 않는 두 아이템을 합성하여 새로운 아이템을 결과로 얻는 인터페이스
*/
#pragma once

#include "interface/cDialog.h"

class CItem;
class cListDialog;
class cIconDialog;
class cIconGridDialog;
class cButton;
class cStatic;

class CComposeDialog : public cDialog
{
public:
	CComposeDialog();
	virtual ~CComposeDialog();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );
	virtual void SetActive( BOOL );
	void Linking();
	void OnActionEvent( LONG id, void* p, DWORD event );
	void SetKeyItem( const ITEMBASE& );
	// 080916 LUJ, 합성 성공 메시지 처리
	void Proceed( const MSG_ITEM_COMPOSE_ACK& );
	// 080916 LUJ, 합성 실패 메시지 처리
	void Proceed( const MSG_ITEM_COMPOSE_NACK& );
	// 080916 LUJ, 프로그레스 창에서 지정 시간에 호출한다.
	void Send();

private:
	// 080916 LUJ, 등록된 메모리를 해제한다
	void Release();
	// 080916 LUJ, 컨트롤을 초기화한다
	void Clear( cIconDialog& ) const;
	void Clear( cIconGridDialog& ) const;
	void Clear( cIcon* ) const;
	// 080916 LUJ, 결과 아이콘을 반환한다
	CItem* GetResultItem( POSTYPE );
	// 080916 LUJ, 주어진 ITEMBASE 정보로 아이콘을 갱신한다
	void UpdateItem( CItem&, const ITEMBASE& ) const;
	// 080916 LUJ, 결과를 페이지 단위로 갱신한다
	void UpdateResult( const ComposeScript&, int pageIndex );

private:
	typedef std::list< CItem* >	ItemList;
	ItemList					mItemList;

	ICONBASE				mKeyIconBase;
	cListDialog*			mOptionListDialog;
	cIconDialog*			mSourceIconDialog;
	cIconGridDialog*		mResultIconGridDialog;
	cButton*				mSubmitButton;
	cButton*				mPagePreviousButton;
	cButton*				mPageNextButton;
	cStatic*				mPageStatic;
	MSG_ITEM_COMPOSE_SYN	mComposeMessage;
};