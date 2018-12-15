/*********************************************************************

	 파일		: SHMath.cpp
	 작성자		: hseos
	 작성일		: 2005/07/14

	 파일설명	: 수학 관련된 처리를 하는 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "SHMath.h"

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHMath Method																												  생성자
//
CSHMath::CSHMath()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHMath Method																											  	  파괴자
//
CSHMath::~CSHMath()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------
//-- GetRandomNum Method						   nStart ~ nEnd 사이의 난수 nNum개를 발생시켜 pBuf에 저장. bDuplicate 는 중복 가능 여부
// 
BOOL CSHMath::GetRandomNum(int nNum, int nStart, int nEnd, int bDuplicate, int* pBuf)
{
//	srand((unsigned int)time(NULL));

	BOOL* bAlready = new BOOL[nEnd-nStart+1];
	int nCnt = 0;

	ZeroMemory(bAlready, sizeof(bAlready[0])*(nEnd-nStart+1));

	DWORD nStartTime = GetTickCount();

	if (bDuplicate)
	{
		for(int i=0; i<nNum; i++)
		{
			pBuf[i] = (rand()%(nEnd-nStart+1))+nStart;
		}
	}
	else
	{
		while(nCnt < nNum)
		{
			int nValue = (rand()%(nEnd-nStart+1));

			if (bAlready[nValue] == FALSE)
			{
				pBuf[nCnt] = nValue+nStart;
				bAlready[nValue] = TRUE;
				nCnt++;
			}

			DWORD nCurTime = GetTickCount();
			if (nCurTime - nStartTime > 2000)
			{
				delete[] bAlready;
				return FALSE;
			}
		}
	}

	delete[] bAlready;

	return TRUE;
}