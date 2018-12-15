#pragma once
#include ".\interface\cdialog.h"									// 다이얼로그 인터페이스 클래스 헤더 불러오기.
#include "cImageSelf.h"												// 이미지 셀프 클래스 헤더 불러오기.

#include "cStatic.h"												// 스태틱 클래스 헤더 불러오기.

enum NPC_EMOTION													// NPC 모션 이넘 코드.
{
	EMOTION_NORMAL,													// 기본 모션.
	EMOTION_HAPPY,													// 행복 모션.
	EMOTION_SURPRISE,												// 놀람 모션.
	EMOTION_UNHAPPY,												// 불행 모션.
	EMOTION_ANGRY,													// 화남 모션.

	EMOTION_MAX,													// 모션 최대수치.
};

struct NPC_IMAGE													// NPC 이미지 구조체.
{
	WORD		idx;												// 인덱스.
	cImageSelf	image[5];											// 이미지 배열.
};

class cNpcImageDlg : public cDialog									// 기본 다이얼로그를 상속받은 NPC 이미지 다이얼로그 클래스.
{
	WORD	mNpcIdx;												// NPC 인덱스.
	BYTE	mEmotion;												// 모션번호.
	DWORD	COLOR;													// 색상 변수.
	DWORD	COLOR2;													// 색상 변수.

	VECTOR2	SCALE;													// 스케일 벡터.
	VECTOR2	POS;													// 위치 벡터.
	VECTOR2 BACK_SCALE;												// 배경 스케일 벡터.
	VECTOR2 BACK_POS;												// 배경 위치 벡터.
	cImage	mImageBack;
	NPC_IMAGE mpCurImage;
	cStatic* m_pTopImage;
	cStatic* m_pBottomImage;
	SHORT_RECT mScreenRect;

public:
	cNpcImageDlg(void);												// 생성자 함수.
	virtual ~cNpcImageDlg(void);
	void Linking();
	void LoadingImage(WORD indx);
	void SetActive(BOOL val);
	void SetNpc(WORD NpcUniqueIdx);
	void SetEmotion(BYTE emotion);
	virtual void Render();
};