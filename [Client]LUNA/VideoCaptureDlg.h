#pragma once
#include "./interface/cDialog.h"

class cComboBox;
class cCheckBox;

class CVideoCaptureDlg : public cDialog
{
	cComboBox* m_pSizeCombo;		// 사이즈 조절
	cComboBox* m_pOptCombo;			// 화질 조절
	cCheckBox* m_pNoCursor;			// 커서 표시 여부
public:
	CVideoCaptureDlg();
	virtual ~CVideoCaptureDlg();

	// 해상도
	enum eVideoCaptureSize
	{
		eVideoCaptureSize_Default,
		eVideoCaptureSize_500x375,
		eVideoCaptureSize_Max,
	};

	// 화질
	enum eVideoCaptureOpt
	{
		eVideoCaptureOpt_High,
		eVideoCaptureOpt_Low,
		eVideoCaptureOpt_Max,
	};

	void Linking();

	const char* GetTextVideoCaptureSize( eVideoCaptureSize eSize );			// 해상도에 해당하는 InterfaceMsg 반환
	const char* GetTextVideoCaptureOpt( eVideoCaptureOpt eOpt );			// 화질에 해당하는 InterfaceMsg 반환

	int GetSelecedSize();													// 선택된 해상도 반환
	int GetSelecedOpt();													// 선택된 화질 반환

	BOOL IsIncludeCursor();													// 녹화시 커서 포함 여부 반환
};