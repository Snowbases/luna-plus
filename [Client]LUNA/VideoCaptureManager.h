#pragma once

#include "bandicap.h"

#define VIDEOCAPTUREMGR		CVideoCaptureManager::GetInstance()

class cDialog;

// 091201 ShinJS --- 비디오 녹화 Class 추가
class CVideoCaptureManager
{
	// 반디소프트 Library Class
	CBandiCaptureLibrary	 m_bandiCaptureLibrary;

public:
	CVideoCaptureManager();
	~CVideoCaptureManager();

	GETINSTANCE( CVideoCaptureManager );

	BOOL IsCapturing();

	BOOL CaptureStart();
	void CaptureStop();

	void Process();

	void SetCaptureOpt();
};