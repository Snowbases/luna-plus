//--------------------------------------------------
//	Tool.h
//	[5/4/2009 LeHide]
//	
//	맵 툴의 모든 데이터를 담고, 핸들링도 함.
//
//	Version		:	1.0.200954
//--------------------------------------------------

#pragma once

typedef LPVOID VBHASH_HANDLE;

class CDlgBarTool;
class CHistoryManager;
class CDialogTilePalette;
class CDialogHFieldProperty;
class C3DBrush;
class CLoadFailedTextureDialog;
class CDialogEditTileSet;
class CDialogEditEntry;
class CDialogEditZone;
class CSectionList;
class CDialogTileSet;
class MToolMouseStatus;

struct SectionSplitter
{
	bool		bSectionInputMode;	
	WORD		wSectionMany;	
	POINT		mousePos;
	POINT		clickPos;
	RECT		rcCurSection;			// 현재 설정할려고 하는 세션의 범위.
	RECT		rcOldSection;
	WORD		wCurSectionTileMany;	// 현재 설정한 세션의 타일 개수.
	LPWORD		pwCurTNum;
	CSectionList*	pSectionList;

	SectionSplitter() :
	pSectionList(new CSectionList)
	{
		bSectionInputMode	= false;
		wSectionMany		= 0;
		rcCurSection.top	= 0;
		rcCurSection.bottom	= 0;
		rcCurSection.left	= 0;
		rcCurSection.bottom	= 0;

		rcOldSection.top	= 0;
		rcOldSection.bottom	= 0;
		rcOldSection.left	= 0;
		rcOldSection.bottom	= 0;
		wCurSectionTileMany	= 0;
		pwCurTNum			= NULL;
	}

	~SectionSplitter()
	{
		if( pwCurTNum ) delete pwCurTNum;

		delete pSectionList;
		pSectionList = 0;
	}
};

class CTool  
{
public:
	CTool();
	virtual ~CTool();

public:
	void OnClearAlphaMapWithTileIndex(POINT* ptCursor,UINT nFlags);
	void OnClearAlphaMapWithObjectIndex(POINT* ptCursor,UINT nFlags);
	void OnClearAlphaMapAll(POINT* ptCursor,UINT nFlags);

	void EnableHFieldBlend(BOOL bEnable);
	void SetHFieldVertexColorBrushSize(float fRadius);
	void ReplaceTile(LPCTSTR szFileName,DWORD dwTileIndex);
	void GetDirectionalLightDesc(DIRECTIONAL_LIGHT_DESC* pLightDesc);
	void EnableDirectionalLight(DIRECTIONAL_LIGHT_DESC* pLightDesc);
	void DisableDirectionalLight();
	void ChangeHFieldIndexBufferNum(DWORD dwNum);
	void UpdateTilePalette();
	void SetHFieldDetail(DWORD dwDetail) {m_dwHFieldDetail = dwDetail;}
	DWORD GetSelectedTileList(HFIELD_POS** ppTileList);
	void SetSelectMode(BOOL bSwitch) {m_bSelectMode = bSwitch;}
	void SetPickColorMode(BOOL bSwitch) {m_bPickColorMode = bSwitch;}


	void SetAlphaStaticModel(DWORD dwAlpha);
	void SetAlphaHField(DWORD dwAlpha);
	void SetAlphaViewOnlyHField(DWORD dwAlpha);

	DWORD GetAlphaStaticModel() {return m_dwStaticModelAlpha;}
	DWORD GetAlphaHField() {return m_dwHFieldAlpha;}
	DWORD GetAlphaViewOnlyHField() {return m_dwViewHFieldAlpha;}

	void OnDeleteAllGXLights();
	void OnDeleteAllGXObjects();
	void OnNewMap();
	void ResetHeight(float h);

	void SetHFieldCreateDesc(HFIELD_CREATE_DESC*	phfCreate);
	void BeginCreateStaticModel();
	void EndCreateStaticModel();

	void ResetDefaultCamera(DWORD dwViewportIndex);
	void ResetCamera(DWORD dwViewportIndex);
	void CleanHeightField();

	I4DyuchiGXExecutive* GetExecutive() {return m_pExecutive;}
	I4DyuchiGXRenderer* GetRenderer() {return m_pExecutive->GetRenderer();}
	I4DyuchiGXGeometry*	GetGeometry() {return m_pExecutive->GetGeometry();}
	I4DyuchiFileStorage* GetFileStorage() {return m_pFileStorage;}

	void SetMousePos(CPoint* pPoint) {m_pointMousePos = *pPoint;}
	CPoint* GetMousePos() {return &m_pointMousePos;}
	BOOL Initialize(CWnd* pWnd);
	BOOL CreateNewHeightField();
	BOOL CreateNewStaticModel();

	BOOL ExportTileTable(char* szFileName); 	// 타일 테이블 Export
	BOOL ExportTileTableForCorum(char* szFileName);					// 코룸용? 코룸만드는데 쓰이던 맵툴인가보다..
	BOOL ExportHeightField(char* szFileName); // 하이트필드 Export
	BOOL ImportHeightField(char* szFileName,DWORD dwIndexBufferNum);
	BOOL ImportViewOnlyHeightField(char* szFileName);
	BOOL ExportStaticModel(char* szFileName); // 스태틱 모델 Export
	BOOL ImportStaticModel(char* szFileName);

	BOOL CreateStaticModelFromMod(char* szFileName);
	BOOL CreateLightFromMod(char* szFileName);
	BOOL SampleFromMod(char* szFileName);


	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void Render();	

	BOOL BuildMap( MAABB* pWorldBox);
	BOOL ShadeLightMap(DWORD dwAmbientColor,DWORD dwFlag);
	BOOL ToggleLightmap();
	BOOL ToggleViewOnlyLightTexture();
	BOOL ToggleViewOnlyHField();
	BOOL ToggleViewColSTM();
	BOOL ToggleBoundingBox();

	BOOL ViewTilePalette();
	BOOL ViewEditZone();

	//yh
	BOOL ViewTileSet();
	void SetPickTileSetMode();

	void SetLightMode();
	void SetPickTileMode();
	void SetPickVertexMode();
	void SetBrushMode();
	void SetGXObjectMode();
	void SetTileInfoMode();
	void SetHFieldObjectMode();
	void SetTriggerMode();
	void SetEditType(DWORD dwFlag);
	void SetTileBrushMode(BOOL bSwitch);
	void SetDrawLightMapMode();
	void SetDrawHFieldVertexColor();
	void SetDrawHFieldaAlphaMap();

	void OnLButtonDown(UINT nFlags, CPoint* point);	/// 이것을 언두의 기준으로 삼자.
	void OnLButtonUp(UINT nFlags, CPoint* point);
	void OnRButtonDown(UINT nFlags, CPoint* point);
	void OnMButtonDown(UINT nFlags, CPoint* point);


	void SetRenderMode(DWORD dwFlag);

	//trustpak
	void EnableRenderBothMode(void);
	void DisableRenderBothMode(void);


	void LockXAxis();
	void LockYAxis();
	void LockZAxis();
	void SetFreeAxis() {m_dwMoveAxis = 0;}
	void CleanStaticModel();
	
	void SetCamera(DWORD dwFlag,DWORD dwViewportIndex);
	void OnMouseMove(UINT nFlags, CPoint point);

	void ResetTransformBrush();
	void OnSize(UINT nType, int cx, int cy);
	void SaveMap(char* szFileName);					/// 맵 세이브
	void LoadMap(char* szFileName);

	void LoadGXObjects(char* szFileName);
	void LoadGXObjects(FILE* fp);

	void LoadLights(char* szFileName);
	void LoadLights(FILE* fp);


	void SaveGXObjects(char* szFileName);			/// GXObjects 세이브
	void SaveGXObjects(FILE* fp);
	void SaveLights(char* szFileName);				/// Lights 세이브
	void SaveLights(FILE* fp);
	void DisableRenderHFObj();
	void EnableRenderHFObj();
	void FreezeHFObjDetail(DWORD dwDetailLevel);
	void UnfreezeHFObjDetail();

	void RenderTileText();	//Added by KBS 030224
	// 모든 라이트의 포지션에서 인자만큼 더해준다.
	void SetLightPositionAll(VECTOR3&);	
	void FillSelecteTile();
	void RotateSelecteTile();

	void CleanZoneData();

	//yh
	void OnClickTileSet(POINT* pt,UINT nFlags);
	//

	void OnClickVertex(POINT* pt,UINT nFlags);
	void OnClickGXObject(POINT* pt,UINT nFlags);
	void OnClickHFieldObject(POINT* ptCursor,UINT nFlags);
	void OnClickBrush(POINT *ptCursor,UINT nFlags);
	void OnClickTrigger(POINT* ptCursor,UINT nFlags);
	void OnClickLightMap(POINT* pt,UINT nFlags);
	BOOL OnClickHFieldVertexColor(POINT* pt,VECTOR3* pv3IntersectPoint,UINT nFlags);
	BOOL OnClickHFieldAlphaTexel(POINT* pt,VECTOR3* pv3IntersectPoint,UINT nFlags);


	void OnRClickLight(POINT* ptCursor,UINT nFlags);
	void OnRClickGXObject(POINT* ptCursor,UINT nFlags);
	void OnRClickGXTrigger(POINT* ptCursor,UINT nFlags);
	void OnRClickLightMap(POINT* pt,UINT nFlags);
	BOOL OnRClickHFieldVertexColor(POINT* pt,VECTOR3* pv3IntersectPoint,UINT nFlags);

	void DrawBrushArea(VECTOR3* pv3IntersectPoint);
	BOOL GetPosIntersect(VECTOR3* pv3Pos,POINT* pPoint);


	BOOL GetMovePos(VECTOR3* pv3Pos,VECTOR3* pv3BasePoint,POINT* pPoint);
	BOOL GetRot(float* pfRad,VECTOR3* pv3Rot,POINT* pPoint);
	BOOL GetScale(VECTOR3* pv3Scale,VECTOR3* pv3BasePoint,POINT* pPoint);

	void GetTileInfo(TILE_INFO* pTileInfo,POINT* pt);
	void UpdateStatus();
	void ClearSelectedItem();
	BOOL PutBrush();
	void CreateBrushArea();
	IHeightField*				GetHField() { return m_pHField; }
	CNpcInfoList*				GetNpcInfoList() { return m_pNpcInfoList; }
	void OnSectionSplitterDown(UINT nFlags, POINT *pt);
	void OnSectionSplitterMove(UINT nFlags, POINT *pt);
	void OnSectionSplitterUp(UINT nFlags, POINT *pt);

	//Added by KBS
	void OnSpotSplitterDown( UINT nFlags, POINT *pt );
	void OnSpotSplitterMove(UINT nFlags, POINT *pt);
	void OnSpotSplitterUp(UINT nFlags, POINT *pt);
	void OnStartSpotSplitterDown( UINT nFlags, POINT *pt );	//0219
	void OnStartSpotSplitterMove(UINT nFlags, POINT *pt);	//0219	
	void OnStartSpotSplitterUp(UINT nFlags, POINT *pt);		//0219
	//--


	void EnableSectionSplitter();
	void DisableSectionSplitter();

	//Added by KBS
	void EnableSpotSplitter();
	void DisableSpotSplitter();
	void EnableStartSpotSplitter();	//0219
	void DisableStartSpotSplitter();	//0219
	//--

	Section_Info*				GetSectionInfo();
	void DeleteSection( Section_Info* pSection_Info );

	//Added by KBS
	Section_Info*				GetSpotInfo();			
	void DeleteSpot( Section_Info* pSection_Info );
	Section_Info*				GetStartSpotInfo(); 			//0219
	void DeleteStartSpot( Section_Info* pSection_Info );		//0219
	//--

	BOOL CreateNPC( Npc_Info* pNpcInfo );
	BOOL Loadttb( const char* szFileName );
	//

	// 2002/11/04
	void GetWorldBox( MAABB* pWorldBox){ *pWorldBox = m_WorldBox;}
	void SetWorldBox( MAABB* pWorldBox){ m_WorldBox = *pWorldBox;}

	// Back Ground color modify.
	void SetBackGroundColor(BYTE byRed, BYTE byGreen, BYTE byBlue);

	void SetCurrentMapFileName(LPCTSTR);
	LPCTSTR	GetCurrentMapFileName() const;
	void SetCurrentTilePalleteFileName(LPCTSTR);
	LPCTSTR GetCurrentTilePalleteFileName() const;
	void SetCurrentTileSetFileName(LPCTSTR);
	LPCTSTR GetCurrentTileSetFileName() const;

	void UpdateLoadFailedTextureDialog(void);
	void ShowFailedTextureDialog(void);

	BOOL ToggleDisplayCameraInfo(void);	
	BOOL ToggleTileEditMode(VOID);
	BOOL ToggleTileSetEditMode(VOID);

	VOID SetRenderToolMode(DWORD dwRenderToolMode);
	DWORD GetRenderToolMode(VOID)	{ return m_dwRenderToolMode; }


	BOOL ToggleRenderStaticModel(VOID);

	VOID DisableRenderStaticModel(VOID);
	VOID EnableRenderStaticMode(VOID);

	void ZoomInOut( float p_Value );
	void Undo();
	void Redo();
	CHistoryManager& GetHistoryManager() const;
	void UpdateTile(LPCTSTR, DWORD index);

private:
	BOOL AttachDevice();
	BOOL CreateHeightField(HFIELD_DESC* pDesc);
	void OnClickLight(POINT* ptCursor,UINT nFlags);
	void OnClickTile(POINT* pt,UINT nFlags);

public:
	SectionSplitter*			m_pSectionSplitter;
	CNpcInfoList*				m_pNpcInfoList;
	SectionSplitter*			m_pSpotSplitter;		//Added by KBS
	SectionSplitter*			m_pStartSplitter;		//0219

private:
	float m_fHFieldVertexColorBrushSize;
	CPoint m_ptMousePos;
	DWORD m_dwMouseStatus;
	HMODULE m_hModGFunc;
	BOOL m_bRenderPerMouseMove;
	BOOL m_TileBrushMode;
	BOOL m_bEnableViewColSTM;
	BOOL m_bSelectMode;
	BOOL m_bPickColorMode;
	VECTOR2 m_v2PointSelectRect[4];
	BOOL m_bDrawSelectedRect;
	DWORD m_dwSelectedVertexNum;
	HFIELD_POS*					m_pSelectedVertexIndex;
	DWORD m_dwMaxSelectedVertexNum;
	DWORD m_dwSelectedTileNum;
	HFIELD_POS*					m_pSelectedTileIndex;
	DWORD m_dwMaxSelectedTileNum;
	DWORD m_dwCurrentViewportIndex;
	CPoint m_ptMousePrvDown;
	CPoint m_ptMousePrvDrg;
	DWORD m_dwArrowRefIndex;
	VECTOR3 m_v3ArrowPos;
	DWORD m_dwMoveAxis;
	C3DBrush*					m_pBrush;
	VBHASH_HANDLE				m_pHash;	
	GXOBJECT_HANDLE				m_gxoSelected;
	GXLIGHT_HANDLE				m_gxlSelected;
	VECTOR3 m_v3PickedPos;
	PLANE m_planePicked;
	GXTRIGGER_HANDLE			m_gxtSelected;
	MToolMouseStatus*			m_Mouse;
	CWnd* m_pEditWindow;
	I4DyuchiGXExecutive*		m_pExecutive;
	I4DyuchiGXRenderer*			m_pRendererForPalette;
	I4DyuchiFileStorage*		m_pFileStorage;
	IDIFontObject*				m_pFont;
	HFIELD_DESC					m_hfDesc;
	HFIELD_CREATE_DESC			m_hfcreate;
	DWORD m_dwHFieldDetail;
	TILE_INFO					m_tileInfo;
	DWORD m_dwEditMode;
	DWORD m_dwRenderMode;
	IHeightField*				m_pHFieldViewOnly;
	IHeightField*				m_pHField;
	DWORD m_dwSelectedHFieldObjectIndex;
	I3DStaticModel*				m_pStaticModel;
	GXMAP_HANDLE				m_ghMap;
	CDialogTilePalette*			m_pPalette;
	CDialogTileSet*				m_pTileSetDialog;
	CDialogEditTileSet*			m_pEditTileSetDialog;
	CDialogEditEntry*			m_pEditEntryDialog;
	CLoadFailedTextureDialog*	m_pLoadFailedTextureDialog;	
	CDialogEditZone*			m_pEditZoneDialog;
	CString m_szCurrentMapFileName;
	CString m_szCurrentTilePaletteFileName;
	CString m_szCurrentTileSetFileName;
	BOOL m_bDisplayCameraInfo;	
	TEXTURE_TABLE				m_aLoadFailedTextureTable[LOAD_FAILED_TEXTURE_TABLE_SIZE];	
	DWORD m_dwRenderToolMode;
	BOOL m_bRenderStaticModel;
	MAABB m_WorldBox;
	DWORD m_dwStaticModelAlpha;
	DWORD m_dwHFieldAlpha;
	DWORD m_dwViewHFieldAlpha;
	CPoint m_pointMousePos;
	BOOL m_bIsHFieldCreateMode;
	BOOL m_bEnableLightMap;
	BOOL m_bEnableViewOnlyHField;
	BOOL m_bEnableBoundingBox;
	BOOL m_bEnableViewOnlyLightTexture;
	BOOL m_bViewTilePalette;
	BOOL m_bViewTileSet;
	BOOL m_bViewEditZone;
	VECTOR3 m_v3Clicked;
	BOOL m_bMapBuild;
	LIGHT_DESC					m_lightDesc[MAX_LIGHT_NUM];
	DWORD m_dwLightNum;
	CAMERA_DESC					m_cameraDesc[MAX_CAMERA_NUM];
	DWORD m_dwCameraNum;
	GXOBJECT_HANDLE				m_gxhList[MAX_OBJECTS_NUM];
	DWORD m_dwGXObjectNum;
	GXLIGHT_HANDLE				m_gxhLightPrj;
	DWORD m_dwZoneNum;
	GXZONE_DESC					m_gxZoneData[MAX_ZONE_NUM];
	float m_fLightPrjHeight;
	DWORD m_dwBackColor;
	HWND m_hWndStatus;
	BOOL m_bLightCopy;
	BOOL m_bObjectCopy;
	VIEW_TYPE					m_CurrentViewMode;
	TCHAR m_szDefaultPath[MAX_PATH];
	CString m_szHFieldName;
	CHistoryManager* mHistoryManager;
};

extern CTool* g_pTool;
extern CDlgBarTool* g_pDlgBarTool;

DWORD AddIndex(HFIELD_POS* pPosListDest,DWORD dwPosSrcNum,HFIELD_POS* pPosListSrc,DWORD dwPosDestNum);
/*
Release Note

Ver 2.0.2009.05.06 By LeHide

이번 버전에서 구현한 사항:


1. Undo
	일단 타일 편집중에만, 1단계 언두가 되도록 해 봤습니다
	언두의 기준은 한번 마우스를 눌렀다가 뗄 때를 1개의 턴으로 보고, 그것만 기억합니다.

	(횟수를 늘리는 것/ 다른 작업에도 적용등은 금방 추가됩니다.
	이번 버전은 버그가 있나, 사용하기에 어떤가를 알기 위해서 내 놓은 테스트 버전입니다)
	현재 언두의 구현 방법은 모든 데이터의 세이브이므로,
	무거운 맵을 다룰시 '첫 마우스 누르기'가 엄청 무거워질 수 도 있습니다
	일단 사용 데이터가 모이면 백단계가 됐든 어쨌든 리두도 구현가능하고 하니,
	일단 이 버전으로 테스트 좀 해주세요!!!


2. 5분 세이브
	5분마다 Autosave0.map ~ Autosave9.map 까지 10개의 카피본이 만들어지고,
	9번을 넘어갈 경우 덮어씌웁니다
	탐색기의 '보기'옵션을 '자세히'로 놓고, 수정한 날짜로 정렬을 하시면 보기 편합니다.

	새로 프로그램을 실행시켰을 경우 0번부터 다시 저장합니다
	-> 최근 9개를 자동으로 리네임 하도록 고칠 생각입니다


3. 카메라 이동 추가 - wasdxc키를 통한 이동
	W-카메라 전방이동, S-카메라 후방이동(뒷걸음), AD-사이드스텝, XC-절대상하이동
	휠 돌릴시 줌인/줌아웃(카메라 전방/후방이동)


4.. 버그 수정
	타일 팔레트 윈도를 불렀을 때 바로 내용이 보이지 않는데
	(스크롤 바를 움직이거나 하면 나옴) 이것이 바로 갱신되도록 함
*/