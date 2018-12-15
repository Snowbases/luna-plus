/*********************************************************************

	 파일		: SHDebug.h
	 작성자		: hseos
	 작성일		: 2007/04/13

	 파일설명	: 디버그 클래스의 헤더

 *********************************************************************/

#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//
class CSHDebug
{
public:
	//----------------------------------------------------------------------------------------------------------------
	typedef enum							// 디버그 정보 종류
	{
		DEBUG_INFO_KIND_FARM_OWN = 0,		// ..농장 소유
		DEBUG_INFO_KIND_FARM_GARDEN,		// ..텃밭 상태
		DEBUG_INFO_KIND_RESIDENT_REGIST,	// ..주민등록 상태
		DEBUG_INFO_KIND_FAMILY,				// ..패밀리 상태
		DEBUG_INFO_KIND_MAX_NUM,
	} DEBUG_INFO_KIND;

private:
	//----------------------------------------------------------------------------------------------------------------
	static BOOL					m_bShowDebugInfo[DEBUG_INFO_KIND_MAX_NUM];
	static int					m_nValue[5][DEBUG_INFO_KIND_MAX_NUM];

	static int					m_nExceptionProcGrade;
	static UINT					m_nErrorCode;					

public:
	CSHDebug();
	virtual ~CSHDebug();
	static VOID					Init();
	//							메인
	static VOID					MainLoop();
	//							그리기
	static VOID					Render();

	//----------------------------------------------------------------------------------------------------------------
	//							디버그 정보 표시 토글
	static VOID					ToggleShowDebugInfo(DEBUG_INFO_KIND nKind, int nValue1 = NULL, int nValue2 = NULL, int nValue3 = NULL)
								{
									m_bShowDebugInfo[nKind] = !m_bShowDebugInfo[nKind];
									if (m_bShowDebugInfo[nKind] == FALSE && (nValue1 != NULL || nValue2 != NULL))
										m_bShowDebugInfo[nKind] = TRUE;
									m_nValue[nKind][0] = nValue1;
									m_nValue[nKind][1] = nValue2;
									m_nValue[nKind][2] = nValue3;
								}
	//							디버그 정보 표시 얻기
	static BOOL					GetShowDebugInfo(DEBUG_INFO_KIND nKind)
								{
									return m_bShowDebugInfo[nKind];
								}
	//							텍스트 입력 분석/처리
	static VOID					ParseTextInput(char* pszText, char* pCmd);
	//							농장 디버그 정보 표시
	static VOID					RenderDebugInfoFarm();


	static void SetExceptionProcGrade(int nGrade)		{ m_nExceptionProcGrade = nGrade; }
	static int  GetExceptionProcGrade()					{ return m_nExceptionProcGrade; }
	static void LogExceptionError(char* pszTxt);

	static void SetErrorCode(int nCode)					{ m_nErrorCode = nCode; }
	static void RenderErrorCode();
};