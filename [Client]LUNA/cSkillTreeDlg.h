#pragma once
#include "./Interface/cTabDialog.h"

class cWindow;
class cStatic;

class cSkillTreeDlg :
	public cTabDialog
{
	cStatic* mpSkillPoint;

	// 080203 LYW --- 행동과 관련된 아이콘 인덱스 생성기 추가.
	CIndexGenerator m_IdxGenerator ;

public:
	cSkillTreeDlg(void);
	virtual ~cSkillTreeDlg(void);

	void Linking();

	void Render();
	void Add( cWindow *window );

	void SetSkillPoint();
	virtual void SetActive(BOOL val);

	// 080203 LYW --- 인덱스 생성기 반환 함수 추가.
	CIndexGenerator* GetIndexGenerator() { return &m_IdxGenerator ; }
};
