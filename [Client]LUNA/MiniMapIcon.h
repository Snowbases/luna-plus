#pragma once


enum eQUEST_MARK
{
	eQM_NONE,
	eQM_EXCLAMATION,
	eQM_QUESTION,
	eQM_EXCLAMATION_HAS_TIMEQUEST,
	eQM_QUESTION_HAS_TIMEQUEST,
	eQM_MAX,
};


enum
{
	eMINIMAPMODE_SMALL,
	eMINIMAPMODE_FULL,
	eMINIMAPMODE_MAX,
};


enum eMINIMAP_ICON
{
	eMNMICON_SELF,
	eMNMICON_PARTYMEMBER,
	eMNMICON_FAMILYMEMBER,
	eMNMICON_NORMALNPC,
	eMNMICON_LOADINGPOINT,
	eMNMICON_CAMERA,
	eMNMICON_QUEST1,
	eMNMICON_QUEST2,
	eMNMICON_QUEST3,
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.10.31
	// ..없는 것들이 eBIGMAP_ICON 에 있는데 수정하기 뭐해서 NULL 로 추가. 추가 안 하면 큰 미니맵에서 툴팁이 표시 안 됨.
	eMNMICON_NULL01,
	eMNMICON_NULL02,
	eMNMICON_NULL03,
	eMNMICON_NULL04,
	eMNMICON_FARM,
	// E 농장시스템 추가 added by hseos 2007.10.31
	// 090909 ShinJS --- 길찾기시 Move Point 추가
	eMNMICON_MOVEPOINT1,
	eMNMICON_MOVEPOINT2,
	eMNMICON_MOVEPOINT3,
	eMNMICON_MOVEPOINT4,
	eMNMICON_MOVEPOINT5,
	eMNMICON_MOVEPOINT6,

	eMNMICON_MAX,
};


enum eBIGMAP_ICON
{
	eBMICON_SELF,
	eBMICON_PARTYMEMBER,
	eBMICON_FAMILYMEMBER,
	eBMICON_STORE,
	eBMICON_TRAINER,
	eBMICON_BANK,
	eBMICON_NOMALNPC,
	eBMICON_LOADINGPOINT,
	eBMICON_LOGIN,
	eBMICON_CAMERA,
	eBMICON_QUEST1,	// 퀘스트 NPC 표시 프레임 1~3
	eBMICON_QUEST2,
	eBMICON_QUEST3,
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.10.31
	eBMICON_FARM,
	// E 농장시스템 추가 added by hseos 2007.10.31
	// 090909 ShinJS --- 길찾기시 Move Point 추가
	eBMICON_MOVEPOINT1,
	eBMICON_MOVEPOINT2,
	eBMICON_MOVEPOINT3,
	eBMICON_MOVEPOINT4,
	eBMICON_MOVEPOINT5,
	eBMICON_MOVEPOINT6,

	eBMICON_MAX,
};


class cImage;
// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.10.31
class CSHFarm;
// E 농장시스템 추가 added by hseos 2007.10.31

class CMiniMapIconImage
{
public:
	cImage* m_pImage;
	VECTOR2 m_ImageWH;

public:
	CMiniMapIconImage() :
	m_pImage( 0 ) 
	{};

	~CMiniMapIconImage();
	
	void InitIconImage(cImage* pImage,DWORD w,DWORD h)
	{
		m_pImage = pImage;
		m_ImageWH.x = (float)w;
		m_ImageWH.y = (float)h;
	}
};

class CMiniMapIcon
{
protected:
	CMiniMapIconImage* m_pIconImage;

	//CMiniMapIconImage* m_pMarkExclamation[3];
	//CMiniMapIconImage* m_pMarkQuestion[3];
	typedef std::vector< CMiniMapIconImage* >	MiniMapIconVector;
	MiniMapIconVector							m_pMarkExclamation;
	MiniMapIconVector							m_pMarkQuestion;

	int		m_ShowQuestMark;
	
	DWORD	m_dwObjectID;
	VECTOR3 m_ObjectPos;

	VECTOR2 m_ScreenPos;

	int		m_Kind;
	VECTOR3 m_TargetPos;
	int		m_Interpolation;
	char	m_MemberName[MAX_NAME_LENGTH+1];
	
	DWORD	m_dwColor;
	BOOL	m_bAlwaysTooltip;

	// 091005 ShinJS --- 출력 위치에서 이미지의 위치를 정하기 위한 비율 ( 기본 : 0.5, 0.5로 그림의 정중앙 )
	VECTOR2	m_fImagePosRatio;

	// 091005 ShinJS --- 프레임 변환을 위한 시간 설정
	DWORD	m_dwFrameTick;

	// 091005 ShinJS --- 같은 Frame을 사용하지 않고 독립적인 Frame을 사용하기 위해 추가
	BOOL	m_bUsePublicFrame;
	DWORD	m_dwLastCheckTime;
	SIZE_T	m_FrameIdx;
	
public:
	CMiniMapIcon();
	virtual ~CMiniMapIcon();

	void Init(CMiniMapIconImage* pIconImage, int Kind );
	void SetData( DWORD dwObjectID, VECTOR3* pObjectPos, PARTY_MEMBER* pMember, CSHFarm* pFarm );

	virtual void Update(DWORD MinimapImageWidth,DWORD MinimapImageHeight);
	virtual void Render(VECTOR2* pMapStartPixel,RECT* pCullRect);

	void RenderQuestMark(VECTOR2* pMapStartPixel,RECT* pCullRect);
	
	int GetToolTip(int x,int y,VECTOR2* pMapStartPixel,char* pOutText, float* distOut = NULL );

	const DWORD GetObjectID() const { return m_dwObjectID; }
	VECTOR3 GetPosition();
	VECTOR2* GetScreenPos()			{	 return &m_ScreenPos;	}

	void SetRGBColor( DWORD dwColor ) { m_dwColor = dwColor; }
	void SetAlwaysTooltip( BOOL bAlways ) { m_bAlwaysTooltip = bAlways;	}

	void SetObjectPos(DWORD PosX, DWORD PosZ)		{ m_ObjectPos.x = (float)PosX;	m_ObjectPos.z = (float)PosZ;	}
	void SetTargetPos(DWORD PosX, DWORD PosZ)		{ m_TargetPos.x = (float)PosX;	m_TargetPos.z = (float)PosZ;	}

	void SetMarkImage(CMiniMapIconImage* pMarkExclamation, CMiniMapIconImage* pMarkQuestion, SIZE_T size );
	void ShowQuestMark( int nKind ) { m_ShowQuestMark = nKind; }
	void SetImagePosRatio( float x, float y )		{ m_fImagePosRatio.x = x;	m_fImagePosRatio.y = y; }
	void SetUsePublicFrame( BOOL bUsePublicFrame )	{ m_bUsePublicFrame = bUsePublicFrame; }
	void SetFrameTick( DWORD dwFrameTick )			{ m_dwFrameTick = dwFrameTick; }
};