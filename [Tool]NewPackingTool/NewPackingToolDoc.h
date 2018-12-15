// NewPackingToolDoc.h : CNewPackingToolDoc 클래스의 인터페이스
//


#pragma once

#include "stdafx.h"

class CNewPackingToolDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CNewPackingToolDoc();
	DECLARE_DYNCREATE(CNewPackingToolDoc)

// 특성
public:

// 작업
public:

// 재정의
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현
public:
	virtual ~CNewPackingToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL	OnOpenDocument(LPCTSTR lpszPathName);

	afx_msg void	OnFileNew();
	afx_msg void	OnFileSave();
	afx_msg void	OnFileSaveAs();

	//void			AddOutStr(LPTSTR str);
	char*			GetData(int index);
	void			SaveData(int index, LPTSTR str);
	void			SelChange(int index, BOOL modified);

public:
	CMHFileMng		m_FileMng;
	int				m_nFileNum;

	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};


