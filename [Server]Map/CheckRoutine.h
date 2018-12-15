// CheckRoutine.h: interface for the CCheckRoutine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKROUTINE_H__1F0CA9EB_BC00_41F9_B11E_E8E021026AB2__INCLUDED_)
#define AFX_CHECKROUTINE_H__1F0CA9EB_BC00_41F9_B11E_E8E021026AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayer;
enum eCheckBits{ CB_EXIST = 1, CB_ICONIDX=2, CB_QABSPOS=4, CB_DURA=8, CB_ALL=15, CB_ENOUGHDURA=16, };
#define CHKRT CCheckRoutine::GetInstance()
class CCheckRoutine  
{
public:
	CCheckRoutine();
	virtual ~CCheckRoutine();
	GETINSTANCE(CCheckRoutine)
	BOOL StateOf(CPlayer * pPlayer, EObjectState);
	BOOL MoneyOf(CPlayer*, eITEMTABLE, MONEYTYPE);
	BOOL LevelOf(CPlayer*, LEVELTYPE);
	BOOL DistanceOf(CPlayer*, VECTOR3 *vTPos, DWORD dist);
	BOOL ItemOf(CPlayer*, POSTYPE, DWORD wItemIdx, DURTYPE, POSTYPE QuickPos, WORD flag = CB_ALL);
	BOOL DearlerItemOf(WORD wDealerIdx, DWORD wBuyItemIdx);
};

#endif // !defined(AFX_CHECKROUTINE_H__1F0CA9EB_BC00_41F9_B11E_E8E021026AB2__INCLUDED_)
