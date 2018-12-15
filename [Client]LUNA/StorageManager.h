// StorageManager.h: interface for the StorageManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STORAGEMANAGER_H__542D9E11_C448_4CE6_BB1A_238B8A6BE1E9__INCLUDED_)
#define AFX_STORAGEMANAGER_H__542D9E11_C448_4CE6_BB1A_238B8A6BE1E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define STORAGEMGR USINGTON(CStorageManager)

class CStorageManager
{
public:
	CStorageManager();
	virtual ~CStorageManager();

	void NetworkMsgParse(BYTE Protocol,void* pMsg);

	void BuyStorageSyn();
	void DelStorageSyn(BYTE num);
	
	void PutInOutMoney(int vals);
	void PutInOutMoneyResult(MSG_DWORD3 * msg);
	
	static void OnPutInMoneyStorageOk( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	static void OnPutInMoneyStorageCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	static void OnPutOutMoneyStorageOk( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );	
};
EXTERNGLOBALTON(CStorageManager);
#endif // !defined(AFX_STORAGEMANAGER_H__542D9E11_C448_4CE6_BB1A_238B8A6BE1E9__INCLUDED_)
