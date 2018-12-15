#pragma once
#include ".\Interface\cDialog.h"

class cStatic;
class cButton;
class CBigMapDlg;

class CWorldMapDlg : public cDialog
{
	struct stMapBtnInfo{
		MAPTYPE mapnum;
		LONG buttonID;
		LONG imageButtonID;
		cButton* pButton;
		cButton* pImageButton;
		LONG pointerX;
		LONG pointerY;

		stMapBtnInfo(){
			mapnum = 0;
			buttonID = 0;
			imageButtonID = 0;
			pButton = NULL;
			pImageButton = NULL;
			pointerX = 0;
			pointerY = 0;
		}
	};

	typedef std::map< DWORD, stMapBtnInfo > MAPBTNINFO;
	MAPBTNINFO m_mapMapBtnList;
	typedef std::map< MAPTYPE, DWORD > MAPNUM_MAPBTNID;
	MAPNUM_MAPBTNID m_mapMapnumMapbtnid;

	int m_nCurPosFontSize;
	int m_nCurPosFontShadowDist;
	POINT m_CurPosTextPoint;
	LONG m_CurPosTextWidth;

	CBigMapDlg* m_pBigMapDlg;

	LONG m_MapNameStaticID;
	cStatic* m_pMapNameStatic;

	LONG m_PointerBtnID;
	cButton* m_pPointerBtn;

	MAPTYPE m_ActivatedMapnum;

	stMapBtnInfo& GetMapBtnInfo( MAPTYPE mapnum );

public:
	CWorldMapDlg(void);
	virtual ~CWorldMapDlg(void);

	void LoadWorldMapScript();
	void Linking();

	CBigMapDlg* GetBigMapDlg() { return m_pBigMapDlg; }

	void SetActive( BOOL bActive );
	DWORD ActionEvent( CMouse * mouseInfo );
	void OnActionEvent( LONG id, void* p, DWORD event );
	void Render();

	void OpenMap( MAPTYPE mapnum );
	void ActiveMap( MAPTYPE mapnum, BOOL bActive );
};
