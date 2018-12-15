/*********************************************************************

	 파일		: SHMain.h
	 작성자		: hseos
	 작성일		: 2007/04/10

	 파일설명	: 메인 클래스의 헤더

 *********************************************************************/

#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHMain
{
public:
	typedef enum											// 공용으로 사용할 결과 리턴값 정의
	{
		RESULT_OK = 0,										// 성공
		RESULT_OK_02,
		RESULT_OK_03,
		RESULT_OK_04,
		RESULT_OK_05,
		RESULT_OK_06,

		RESULT_FAIL_INVALID,								// 실패(시스템 범위를 벗어나는 오류)

		RESULT_FAIL_01,										// 실패01
		RESULT_FAIL_02,										// 실패02
		RESULT_FAIL_03,										// 실패03
		RESULT_FAIL_04,										// 실패04
		RESULT_FAIL_05,										// 실패05
		RESULT_FAIL_06,										// 실패06
		RESULT_FAIL_07,										// 실패07
		RESULT_FAIL_08,										// 실패08
		RESULT_FAIL_09,										// 실패09
	} RESULT;

private:
	
public:
	CSHMain();
	virtual ~CSHMain();
	static VOID			Init();
	//					메인
	static VOID			MainLoop();
	//					그리기
	static VOID			Render();
};