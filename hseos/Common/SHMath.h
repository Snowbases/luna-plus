/*********************************************************************

	 파일		: SHMath.h
	 작성자		: hseos
	 작성일		: 2005/07/14

	 파일설명	: 수학 관련된 클래스의 헤더

 *********************************************************************/

#pragma once

#define SHMath_SECOND(x)		(x##000)
#define SHMath_MINUTE(x)		(SHMath_SECOND(60)*x)

#define SHMath_BIN8_00000001	0x0001
#define SHMath_BIN8_00000010	0x0002
#define SHMath_BIN8_00000100	0x0004
#define SHMath_BIN8_00001000	0x0008
#define SHMath_BIN8_00001000	0x0008
#define SHMath_BIN8_00010000	0x0010
#define SHMath_BIN8_00100000	0x0020
#define SHMath_BIN8_01000000	0x0040
#define SHMath_BIN8_10000000	0x0080

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
class CSHMath
{
private:

public:
	//-------------------------------------------------------------------------------------
	CSHMath();
	~CSHMath();

	//-------------------------------------------------------------------------------------
	// nStart ~ nEnd 사이의 난수 nNum개를 발생시켜 pBuf에 저장. bDuplicate 는 중복 가능 여부
	//
	static BOOL		GetRandomNum(int nNum, int nStart, int nEnd, int bDuplicate, int* pBuf);
	// 난수 얻기
	static int		GetRandomNum(int nStart, int nEnd)	
					{ 
						if (nStart > nEnd) 
						{
							int nTmp = nStart;
							nStart = nEnd;
							nEnd = nTmp;
						}
						else if (nStart == nEnd) return nStart;
						return (rand()%(nEnd-nStart+1))+nStart; 
					}

	//-------------------------------------------------------------------------------------
	// 비트 체크
	static BOOL		BitAndOp(int nTarget, int nBit)		{ return (nTarget & (1<<nBit)) ? TRUE : FALSE; }		
};