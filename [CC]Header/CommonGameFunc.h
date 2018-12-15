#ifndef __COMMONGAMEFUNC_H__
#define __COMMONGAMEFUNC_H__

// ----------------------------------------------------------------------------------
// 게임에서 사용하는 공통 Global Function (both client and server)
// ----------------------------------------------------------------------------------


class CObject;
struct _PACKET_GAME;

extern BOOL g_bAssertMsgBox;
extern char g_szHeroIDName[];

char* GetCurTimeToString();
DWORD GetCurTimeValue();
DWORD GetCurTime();
DWORD GetCurTimeToSecond();
void StringtimeToSTTIME(stTIME* Time, char* strTime);
void SetProtocol(MSGBASE* pMsg,BYTE bCategory,BYTE bProtocol);
void WriteAssertMsg(char* pStrFileName,int Line,char* pMsg);
void CriticalAssertMsg(char* pStrFileName,int Line,char* pMsg);
eITEMTABLE GetTableIdxPosition(WORD abs_pos);
// 090109 LUJ, 스크립트 검증 값을 설정한다
void SetScriptCheckValue( ScriptCheckValue& );
DWORD GetMainTargetID(MAINTARGET* pMainTarget);
BOOL GetMainTargetPos(MAINTARGET* pMainTarget,VECTOR3* pRtPos, CObject ** ppObject);

BOOL IsSameDay(char* strDay,WORD Year,WORD Month,WORD Day);

void SetVector3(VECTOR3* vec,float x,float y,float z);
float CalcDistanceXZ(VECTOR3* v1,VECTOR3* v2);
void VRand(VECTOR3* pResult,VECTOR3* pOriginPos,VECTOR3* pRandom);
void TransToRelatedCoordinate(VECTOR3* pResult,VECTOR3* pOriginPos,float fAngleRadY);
void RotateVectorAxisX(VECTOR3* pResult,VECTOR3* pOriginVector,float fAngleRadX);
void RotateVectorAxisY(VECTOR3* pResult,VECTOR3* pOriginVector,float fAngleRadY);
void RotateVectorAxisZ(VECTOR3* pResult,VECTOR3* pOriginVector,float fAngleRadZ);
void RotateVector( VECTOR3* pResult, VECTOR3* pOriginVector, float x, float y, float z );
void RotateVector( VECTOR3* pResult, VECTOR3* pOriginVector, VECTOR3 vRot );

void BeelinePoint(VECTOR3 * origin, VECTOR3 * dir, float dist, VECTOR3 * result);
void AdjacentPointForOriginPoint(VECTOR3 * origin, VECTOR3 * target, float dist, VECTOR3 * result);
// 090603 LUJ, 맵 이름 반환
LPTSTR GetMapName( MAPTYPE );
#ifdef _CLIENT_
void AddComma( char* pBuf );
char* AddComma( DWORDEX dwMoney );
char* RemoveComma( char* str );
// 090603 LUJ, 맵 번호 반환
MAPTYPE GetMapNumForName( LPCTSTR );
#else
enum GameRoomType
{
	GameRoomIsInvalid = 0,
	GameRoomIsFree,
	GameRoomIsPremium,
	GameRoomTypeMaxCount,
};
GameRoomType GetGameRoomType(const _PACKET_GAME&);
#endif

BOOL IsVillage();
void SafeStrCpy( char* pDest, const char* pSrc, int nDestBufSize );
void SafeStrCpyEmptyChange( char* pDest, const char* pSrc, int nDestBufSize );
BOOL CheckValidPosition(VECTOR3&);
float roughGetLength(float fDistX, float fDistY);
void LoadEffectFileTable(LPTSTR);
DWORD FindEffectNum(LPCTSTR);
void ERRORBSGBOX(LPTSTR, ...);
BOOL IsEmptyLine(LPTSTR);
int GetPastTime(DWORD);
int	GetPastTime(char* pszDate);
BOOL IsCharInString(char* pStr, char* pNoChar);
// 090319 NYJ - 하우징 시스템에서 사용하는 공용함수
DWORD Get_HighCategory(DWORD FunitureIndex);
DWORD Get_LowCategory(DWORD FunitureIndex);
DWORD Get_Work(DWORD FunitureIndex);
DWORD Get_Index(DWORD FunitureIndex);
DWORD Get_Function(DWORD FunitureIndex);

BOOL Get_SplitedFurnitureIndexFromFileName(char* pFileName, DWORD& HighCategory, DWORD& LowCategory, DWORD& Work, DWORD& Index, DWORD& Function);
char* Get_HighCategorySeparator(DWORD FunitureIndex);
char* Get_LowCategorySeparator(DWORD FunitureIndex);
char* Get_WorkSeparator(DWORD FunitureIndex);
char* Get_IndexSeparator(DWORD FunitureIndex);
char* Get_FuntionSeparator(DWORD FunitureIndex);
char* ConvertFunitureIndex2FileName(DWORD FurnitureIndex);
DWORD ConvertFileName2FunitureIndex(char* pFileName);

BOOL IsExteriorWall(DWORD dwFurnitureIndex);
BOOL IsInteriorWall(DWORD dwFurnitureIndex);
BOOL IsFloor(DWORD dwFurnitureIndex);
BOOL IsCeiling(DWORD dwFurnitureIndex);
BOOL IsDoor(DWORD dwFurnitureIndex);
BOOL IsStart(DWORD dwFurnitureIndex);
BOOL IsActionable(DWORD dwFurnitureIndex);

void OutputDebug( char* pMsg, ... );

DWORD DecodeHidePartsFlag(const char flagArray[eWearedItem_Max]);
void EncodeHidePartsFlag(DWORD flag, char flagArray[eWearedItem_Max]);

#ifdef _DEBUG
#define HTRACE					OutputDebug
#define HTRACE_FUNC()			OutputDebug( "%s\n", __FUNCTION__ );
#define HTRACE_FUNCX(d)			OutputDebug("%s-%d\n", __FUNCTION__, d)
#else
#define HTRACE					__noop
#define HTRACE_FUNC()			__noop
#define HTRACE_FUNCX(d)			__noop
#endif

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

// 090909 ONS 스킬트리 정보를 가져오는 함수
SkillData* GetSkillTreeList();

// 091112 ONS 금액표시 색상 반환 함수 
DWORD GetMoneyColor(DWORD);
DWORD GetHashCodeFromTxt(LPCTSTR);
VECTOR3 GetRadToVector( const float fRad );

#endif //__COMMONGAMEFUNC_H__
