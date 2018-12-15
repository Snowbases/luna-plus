// client.h : client 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호


// CclientApp:
// 이 클래스의 구현에 대해서는 client.cpp을 참조하십시오.
//
#include "grid_control/GridCtrl.h"


// 스킬 스크립트에서 사용하는 자료구조가 MFC의 자료구조와 충돌하여 SkillInfo.h를 참조할 수 없다
// 따라서 기본적인 정보만 파싱해서 담기위한 구조체를 재정의했다.
struct ActiveSkillScript
{
	DWORD	mIndex;
	CString	mName;
	BOOL	mIsBuff;
	DWORD	mMoney;	// 학습에 필요한 돈
	DWORD	mPoint;	// 학습에 필요한 포인트
};

struct QuestScript
{
	DWORD	mMonsterIndex;
	DWORD	mMonsterHuntCount;

	// 키: 아이템 인덱스, 값: 필요 개수
	typedef std::map< DWORD, DURTYPE >	ItemMap;
	ItemMap								mItemMap;
};


class CLoginDialog;
class CNewUserDialog;
class CPlayerDialog;
class CItemDialog;
class CWaitDialog;
class CSkillDialog;
class CItemFindDialog;
class CFamilyDialog;
class CQuestDialog;
class CGuildDialog;
class CUserDialog;
class CPermissionDialog;
class COperatorDialog;
class CLogDialog;
// 081021 LUJ, 권한 관리 창 추가
class CAuthorityDialog;
class CMemoDialog;
class CTournamentDialog;

class CclientApp : public CWinApp
{
public:
	CclientApp();
	~CclientApp();


// 재정의
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage( MSG* );
	void Parse( DWORD serverIndex, const MSGROOT* );
	// 081021 LUJ, 잠금 변수가 거짓일 경우, 대기 창이 표시되지 않도록 함
	void Send( const TCHAR* serverName, const MSGROOT&, size_t, bool isLock = true );
	void Send( DWORD serverIndex, const MSGROOT&, size_t, bool isLock = true );
	void SendAll( const MSGROOT&, size_t, bool isLock = true );
	CString	GetRankName( DWORD rank ) const;

	const TCHAR*	GetServerName( DWORD serverIndex ) const;
	DWORD			GetServerIndex( const TCHAR* serverName ) const;

	// 가능한 접속 개수
	size_t	GetConnectionSize() const;
	
protected:
	void SetWait( BOOL );


// 구현
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;


protected:
	// 동적 할당된 대화창을 담아두고, 일시에 제거한다
	typedef std::list< CDialog* >	DialogList;
	DialogList						mDialogList;

	CLoginDialog* 				mLoginDialog;
	CNewUserDialog* 			mNewUserDialog;
	CPlayerDialog* 				mPlayerDialog;
	CItemDialog* 				mItemDialog;
	CWaitDialog* 				mWaitDialog;
	CSkillDialog*				mSkillDialog;
	CItemFindDialog*			mItemFindDialog;
	CFamilyDialog*				mFamilyDialog;
	CQuestDialog*				mQuestDialog;
	CGuildDialog*				mGuildDialog;
	CUserDialog*				mUserDialog;
	CPermissionDialog*			mPermissionDialog;
	COperatorDialog*			mOperatorDialog;
	// 080401 LUJ, 농장 로그 창 추가
	CLogDialog*					mLogDialog;
	// 081021 LUJ, 권한 관리 창 추가
	CAuthorityDialog*			mAuthorityDialog;
	CMemoDialog*				mMemoDialog;
	CTournamentDialog*			mTournamentDialog;

public:
	inline CDialog*				GetWaitDialog()			const	{ return ( CDialog* )mWaitDialog; }
	inline CNewUserDialog*		GetNewUserDialog()		const	{ return mNewUserDialog; }
	inline CItemDialog*			GetItemDialog()			const	{ return mItemDialog; }	
	inline CSkillDialog*		GetSkillDialog()		const	{ return mSkillDialog; }
	inline CPlayerDialog*		GetPlayerDialog()		const	{ return mPlayerDialog; }
	inline CQuestDialog*		GetQuestDialog()		const	{ return mQuestDialog; }
	inline CFamilyDialog*		GetFamilyDialog()		const	{ return mFamilyDialog; }
	inline CUserDialog*			GetUserDialog()			const	{ return mUserDialog; }
	inline CPermissionDialog*	GetPermissionDialog()	const	{ return mPermissionDialog; }
	inline CLogDialog*			GetLogDialog()			const	{ return mLogDialog; }
	// 081021 LUJ, 권한 관리 창 추가
	inline CAuthorityDialog*	GetAuthorityDialog()	const	{ return mAuthorityDialog; }	
	inline CMemoDialog*			GetMemoDialog()			const	{ return mMemoDialog; }
	inline CTournamentDialog*	GetTournamentDialog()	const	{ return mTournamentDialog; }
	
protected:
	BOOL			mIsWaiting;

	// 아이템 정보
public:
	const TCHAR*		GetItemName( DWORD itemIndex ) const;
	DWORD				GetItemIndex( const TCHAR* ) const;
	const ItemScript&	GetItemScript( DWORD itemIndex ) const;
	BOOL				IsDuplicateItem( DWORD itemIndex ) const;
	LPCTSTR GetPetGradeText( DWORD grade ) const;
	LPCTSTR GetPetAritificialIntelligenceText( DWORD artificialIntelligence ) const;
	CString ConvertToText(MONEYTYPE) const;

	// 아이템 위치에 따라 어떤 종류의 인벤토리인지 반환한다
	CString	GetItemPositionType( Item::Area, POSTYPE ) const;

	CString GetText( eFamilyLog )		const;
	CString GetText( eGuildLog )		const;
	CString GetText( eLogSkill )		const;
	CString GetText( eLogitemmoney )	const;
	CString GetText( eQuestLog )		const;
	CString GetText( eGM_POWER )		const;
	CString GetText( RecoveryLog )		const;
	// 080328 LUJ, 드롭 옵션 키에 대한 문자열 반환
	CString GetText( ITEM_OPTION::Drop::Key ) const;
	// 080402 LUJ, 농장에 대한 로그 반환. 농장 작업자가 로그 작성 시 임의로 해서 불가피하게 두 가지 종류가 생겼다...
	CString GetText( FARM_LOG_KIND )	const;
	// 080403 LUJ, 유료 아이템 구입에 대한 로그 반환
	CString GetText( ItemShopLog )		const;
	// 080416 LUJ, 경험치 로그 반환
	// 080716 LUJ, 잘못된 타입 수정
	CString GetText( eLogExppoint )		const;
	// 080430 LUJ, 오브젝트 종류 문자열 반환
	CString GetText( EObjectKind )		const;
	// 080523 LUJ, 낚시 로그 문자열 반환
	CString GetText( eFishingLog )		const;
	// 080716 LUJ, 펫 상태 문자열 반환
	CString GetText( ePetState )		const;
	// 080716 LUJ, 펫 종류 문자열 반환
	CString GetText( ePetType )			const;
	// 080716 LUJ, 펫 로그 문자열 반환
	CString GetText( ePetLog )			const;
	// 081027 LUJ, 공성 로그 문자열 반환
	CString GetText( eSiegeLog )		const;
	CString GetText(eAutoNoteLogKind) const;
	CString GetText(eMoneyLogtype) const;
	CString GetText(eHouseLog) const;
	CString GetText(eCookingLog) const;
	CString GetText(eCOOKRECIPE) const;
	CString GetText(HOUSE_HIGHCATEGORY) const;
	CString GetText(eHOUSEFURNITURE_STATE) const;
	CString GetText(eConsignmentLog) const;
	void CreateGrid( CWnd* parent, CGridCtrl*, UINT id, const CRect&, const CString*, BOOL autoHeight ) const;
	
	// 셀에 값을 세팅하는 유틸리티 함수
	static void SetCellValue( DWORD value,	CGridCtrl&, GV_ITEM&, int state );
	static void SetCellValue( float value,	CGridCtrl&, GV_ITEM&, int state );
	static void SetCellValue( int value,	CGridCtrl&, GV_ITEM&, int state );
	static void SetCellValue( const TCHAR*,	CGridCtrl&, GV_ITEM&, int state );

	// 날짜 콤보에 값을 세팅하는 유틸리티 함수
	static void SetDateCombo( CComboBox& yearCombo, CComboBox& monthCombo, CComboBox& dayCombo, BOOL isToday );

	// 와일드카드 문자를 DB에서 사용가능하게 바꿔준다
	static void ConvertWildCardForDb( char* text );

	// 리스트 컨트롤의 내용을 엑셀로 저장한다
	static void SaveToExcel( const TCHAR* fileName, const TCHAR* tableName, const CListCtrl& );

	// 선택된 항목을 클립 보드에 저장한다
	// 081020 LUJ, 전체 항목을 복사할지 결정하는 플래그 추가
	static void SaveToClipBoard( const CListCtrl&, BOOL isWholeCopy = FALSE );
	// 081020 LUJ, Comma Separated Values 파일로 저장
	static void SaveToCSV( const TCHAR* fileName, const CListCtrl& listCtrl );

	// 리스트 컨트롤에 서버 이름을 세팅한다
	static void SetServerListCtrl( CListCtrl& );

	// 아이템 목록을 세팅한다
	void SetItemList( CListCtrl& ) const;
	void SetItemList( CListCtrl&, eItemCategory ) const;
	void SetItemList(CListCtrl&, LPCTSTR) const;
	// 문자열에 해당하는 단어가 포함된 모든 아이템을 콤보에 세팅한다
	void SetItemCombo( CComboBox&, const TCHAR* keyword ) const;

	// 유효한 서버 목록을 세팅한다
	void SetServerComboBox( CComboBox& ) const;

	// 직업 목록을 세팅한다
	void SetJobCombo( CComboBox&, DWORD jobType, DWORD race, DWORD jobLevel, DWORD jobIndex );

	// 길드 직위 목록을 세팅한다
	void SetRankCombo( CComboBox& ) const;

	// 사용자 레벨 목록을 세팅한다
	void SetUserLevelCombo( CComboBox& ) const;
 	
	// 아이템 스크립트
protected:
	void LoadItemScript();	

	// 키: 아이템 인덱스
	typedef std::map< DWORD, ItemScript >	ItemMap;
	ItemMap									mItemMap;

	// 인챈트 스크립트
public:
	// 인챈트에 소요되는 재료 인덱스로 스크립트를 가져온다
	const EnchantScript* GetEnchantScript( DWORD itemIndex ) const;

protected:
	void LoadEnchantScript();

	typedef stdext::hash_map< DWORD, EnchantScript >	EnchantScriptMap;
	EnchantScriptMap									mEnchantScriptMap;


	// 스킬 스크립트
public:
	typedef std::map< DWORD, DWORD >	SkillSizeMap;

	const ActiveSkillScript* GetActiveSkillScript( DWORD skillIndex ) const;
	// 081202 LUJ, 문자열에 해당하는 스킬 스크립트를 반환한다
	const ActiveSkillScript* GetActiveSkillScript( CString skillName ) const;
	DWORD GetSkillSize( DWORD skillIndex ) const;
	const SkillSizeMap& GetSkillSizeMap() const;	
	const TCHAR* GetTip( DWORD index ) const;
	// 081202 LUJ, 키워드에 해당하는 스킬 이름을 콤보에 넣는다
	size_t SetSkillCombo( CComboBox&, CString keyword ) const;

protected:
	void LoadActiveSkillScript();
	void LoadTip();
	
	// 키: 스킬 인덱스
	typedef stdext::hash_map< DWORD, ActiveSkillScript >	ActiveSkillScriptMap;
	ActiveSkillScriptMap									mActiveSkillScriptMap;

	// 키: 스킬 인덱스, 값: 스킬 최대 레벨	
	SkillSizeMap						mSkillSizeMap;

	// 키: 스킬 인덱스
	typedef stdext::hash_map< DWORD, CString >	SkillTipMap;
	SkillTipMap									mSkillTipMap;


	// 퀘스트 스크립트
public:
	const TCHAR*	GetQuestTitle( DWORD mainQuestIndex, DWORD subQuestIndex ) const;
	const TCHAR*	GetQuestDescription( DWORD mainQuestIndex, DWORD subQuestIndex ) const;
	const TCHAR*	GetQuestItemName( DWORD questItemIndex ) const;
	const TCHAR*	GetQuestItemDescription( DWORD questItemIndex ) const;
	DWORD			GetMainQuestIndex( DWORD questItemIndex ) const;
	DWORD			GetSubQuestIndex( DWORD questItemIndex ) const;

protected:
	// 퀘스트 아이템을 읽는다
	void LoadQuestItemScript();

public:
	struct QuestItem
	{
		DWORD	mMainQuestIndex;
		DWORD	mSubQuestIndex;
		CString mName;
		CString mDescription;
	};

	// 키: 퀘스트 아이템 인덱스
	typedef stdext::hash_map< DWORD, QuestItem >	QuestItemMap;
	const QuestItemMap& GetQuestItemMap() const;

protected:
	QuestItemMap mQuestItemMap;

// 퀘스트 문자열을 읽는다
	void LoadQuestStringScript();

public:
	struct QuestString
	{
		DWORD	mMainQuestIndex;
		DWORD	mSubQuestIndex;

		CString	mTitle;
		CString	mDescription;
	};

	// 키: 메인퀘스트 인덱스와 서브퀘스트 인덱스가 MAKELPPARAM으로 조합되어 있다
	typedef stdext::hash_map< DWORD, QuestString >	QuestStringMap;	
	const QuestStringMap& GetQuestStringMap() const;

protected:	
	QuestStringMap mQuestStringMap;


	// 퀘스트 스크립트를 읽는다
public:
	const QuestScript* GetQuestScript( DWORD mainQuestIndex, DWORD subQuestIndex ) const;
	
protected:
	void LoadQuestScript();

	// 키: 메인퀘스트 인덱스와 서브퀘스트 인덱스가 MAKELPPARAM으로 조합되어 있다
	typedef stdext::hash_map< DWORD, QuestScript >	QuestScriptMap;
	QuestScriptMap									mQuestScriptMap;


	// 인터페이스 메시지
public:
	LPCTSTR GetJobName( DWORD code ) const;
	LPCTSTR GetInterfaceMessage( DWORD Index ) const;

protected:
	void LoadInterfaceMessage();

	typedef stdext::hash_map< DWORD, CString >	InterfaceMessage;
	InterfaceMessage							mInterfaceMessage;

	
	// 주민등록증 정보를 읽는다
public:
	typedef std::map< DWORD, CString >	LicenseFavoriteMap;
	typedef std::map< DWORD, CString >	LicenseAreaMap;
	typedef std::map< DWORD, CString >	LicenseAgeMap;

	const LicenseFavoriteMap&	GetLicenseFavoriteMap() const;
	const LicenseAreaMap&		GetLicenseAreaMap() const;
	const LicenseAgeMap&		GetLicenseAgeMap() const;

protected:
	void LoadLicenseScript();

	LicenseFavoriteMap							mLicenseFavoriteMap;
	LicenseAreaMap								mLicenseAreaMap;
	LicenseAgeMap								mLicenseAgeMap;


	// 농작물 이름을 읽는다
public:
	const TCHAR* GetCropName( DWORD cropIndex, DWORD cropLevel ) const;
	// 080630 LUJ, 가축 이름을 반환한다
	const TCHAR* GetLivestockName( DWORD kind ) const;

protected:
	void LoadFarmScript();
	// 키: MAKELONG( 농작물 종류, ) 
	typedef std::map< DWORD, CString >	FarmCropMap;
	FarmCropMap							mFarmCropMap;
	// 080630 LUJ, 가축 이름
	typedef std::map< DWORD, CString >	LivestockMap;
	LivestockMap						mLivestockMap;

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMenuNewUser();
	afx_msg void OnMenuItemSearch();
	afx_msg void OnMenuGuild();
	afx_msg void OnMenuItemLog();
	afx_msg void OnMenuFamliy();
	afx_msg void OnMenuPermission();
	afx_msg void OnMenuOperator();
	afx_msg void OnMenuPassword();
	afx_msg void OnMenuOperatorLog();
	afx_msg void OnMenuItemOptionLog();
	// 080320 LUJ, 드롭 옵션 로그 메뉴
	afx_msg void OnMenuItemDropOptionLog();
	// 080401 LUJ, 농장 로그
	afx_msg void OnMenuFarmLog();
	// 080403 LUJ, 유료 아이템 구입 로그
	afx_msg void OnMenuItemShopLog();
	// 080523 LUJ, 낚시 로그
	afx_msg void OnMenuFishLog();
	// 080630 LUJ, 가축 로그
	afx_msg void OnMenuLivestockLog();
	// 080716 LUJ, 아이템 전역 로그
	afx_msg void OnMenuItemGlobalLog();
	// 081021 LUJ, 권한 관리
	afx_msg void OnMenuAuthority();
	// 081027 LUJ, 토너먼트
	afx_msg void OnMenuTournament();
	// 081027 LUJ, 공성 로그
	afx_msg void OnMenuSiegeLog();
	// 081205 LUJ, 채팅 로그
	afx_msg void OnMenuChatLog();
	// 090122 LUJ, 스크립트 해킹 로그
	afx_msg void OnMenuScriptHackLog();
	// 090122 LUJ, 이름 변경 로그
	afx_msg void OnMenuRenameLog();
	// 090304 LUJ, 아이템 변환 시뮬레이션
	afx_msg void OnMenuSimulationItemChange();
	// 090406 LUJ, 돈 로그
	afx_msg void OnMenuMoneyLog();
	afx_msg void OnHouseLog();
	afx_msg void OnCookLog();
	afx_msg void OnRecipeLog();
	afx_msg void OnConsignLog();
	afx_msg void OnGameRoomLog();
	
public:
	// 080304 LUJ, 로그를 표시
	void Log( const TCHAR*, ... );

	// 080328 LUJ, 로그 저장하지 않는 화면 출력
	void Put( const TCHAR*, ... );

	// 080401 LUJ, 멀티바이트 문자열에 포함된 특수한 문자열을 제거한다
	CString RemoveString( const char* source, const char* extractedString ) const;
	// 090304 LUJ, 초를 시간 문자열로 변환한다
	CString GetDateString( DWORD second ) const;
	// 090304 LUJ, 0~1사이의 실수를 얻는다
	float GetRandomFloat() const;
	// 080630 LUJ, 스크립트를 로딩한다. 스레드로 처리되어, 로그인 창에 로딩 상태를 표시해준다
	static UINT LoadScript( LPVOID );


public:
	// 080702 LUJ, 주 프레임 창을 반환한다
	inline virtual CWnd* GetMainWnd()	{ return m_pMainWnd; }


public:
	// 080716 LUJ, 펫 이름을 읽는다
	void	LoadPetScript();
	LPCTSTR	GetPetName( DWORD index ) const;
private:
	typedef stdext::hash_map< DWORD, CString >	PetNameMap;
	PetNameMap									mPetNameMap;

	// 080731 LUJ, 펫 상태
public:
	// 080731 LUJ, 펫 상태 정보를 읽는다
	void	LoadPetStatusScript();
	// 080731 LUJ, 펫 타입 정보가 담기는 셋
	typedef std::set< ePetType > PetTypeSet;
	// 080731 LUJ, 레벨에 따라 가질 수 있는 타입을 반환
	const PetTypeSet&	GetPetType( DWORD level ) const;

	struct PetStatusScript
	{
		int	mStrength;
		int	mDexterity;
		int	mVitality;
		int	mIntelligence;
		int	mWisdom;
	};
	// 080731 LUJ, 펫 상태 스크립트를 반환한다
	const PetStatusScript& GetPetStatusScript( ePetType, DWORD level ) const;
private:
	// 080731 LUJ, 펫 레벨에 따라 가질 수 있는 타입
	typedef stdext::hash_map< DWORD, PetTypeSet >	PetStatusMap;
	PetStatusMap									mPetStatusMap;

	// 080731 LUJ, 펫 종류와 레벨을 키로 묶는다.
	typedef std::pair< ePetType, DWORD >				PetStatusKey;
	typedef std::map< PetStatusKey, PetStatusScript >	PetStatusScriptMap;
	PetStatusScriptMap									mPetStatusScriptMap;

	// 080731 LUJ, 펫 HP/MP 정보
public:
	// 080731 LUJ, 펫 HP/MP 정보를 읽는다
	void	LoadPetBodyScript();
	// 080731 LUJ, 펫 종류/등급에 따라 가지는 정보
	struct PetBodyScript
	{
		DWORD	mHealth;
		DWORD	mMana;
	};
	// 080731 LUJ, 펫 종류/등급에 따라 정보를 반환한다
	const PetBodyScript& GetPetBodyScript( ePetType, DWORD grade ) const;
	// 080731 LUJ, 도움말을 위해 유효한 펫 정보 조건을 문자열로 반환한다.
	void GetPetBodyConditionText( CString& ) const;
private:
	// 080731 LUJ, 키: 펫 타입 * 10 + 펫 등급
	typedef std::map< DWORD, PetBodyScript >	PetBodyMap;
	PetBodyMap									mPetBodyMap;

	// 080731 LUJ, 펫 설정 정보
public:
	struct PetSystemScript
	{
		struct SkillSlot
		{
			DWORD	mMinSize;
			DWORD	mMaxSize;
		}
		mSkillSlot;

        DWORD	mMaxGrade;
		DWORD	mMaxLevel;
		
	};
	// 080731 LUJ, 펫 설정 정보를 반환한다
	const PetSystemScript& GetPetSystemScript() const;
	// 080731 LUJ, 펫 설정 정보를 읽는다
	void	LoadPetSystemScript();
private:
	PetSystemScript	mPetSystemScript;

public:
	void LoadMonsterScript();
	LPCTSTR	GetMonsterName( DWORD index ) const;

private:
	typedef stdext::hash_map< DWORD, CString >	MonsterNameMap;
	MonsterNameMap								mMonsterNameMap;

private:
	typedef stdext::hash_map< DWORD, CString > FurnitureNameMap;
	FurnitureNameMap mFurnitureNameMap;

public:
	void LoadFurnitureScript();
	LPCTSTR	GetFurnitureName(DWORD index) const;

	struct CookScript
	{
		typedef std::map< LEVELTYPE, EXPTYPE > ExperienceContainer;
		ExperienceContainer mCookCountContainer;
		ExperienceContainer mEatCountContainer;
		ExperienceContainer mFireCountContainer;
	};
	struct RecipeScript
	{
		typedef DWORD ItemIndex;
		typedef std::map< ItemIndex, size_t > MaterialContainer;
		MaterialContainer mMaterialContainer;

		ItemIndex mResultItemIndex;
		DWORD mRemainTime;
		EXPTYPE mMinimumExperience;
		EXPTYPE mMaximumExperience;
		LEVELTYPE mLevel;
	};

	typedef DWORD RecipeIndex;
	typedef std::map< RecipeIndex, RecipeScript > RecipeContainer;
	RecipeContainer mRecipeContainer;

	void LoadCookScript();
	EXPTYPE GetMaxCookCount(LEVELTYPE) const;
	EXPTYPE GetMaxEatCount(LEVELTYPE) const;
	EXPTYPE GetMaxFireCount(LEVELTYPE) const;
	LEVELTYPE GetMaxCookLevel() const { return LEVELTYPE(mCookScript.mCookCountContainer.size()); }
	LPCTSTR GetRecipeName(RecipeIndex) const;
	LEVELTYPE GetRecipeLevel(RecipeIndex) const;
	void UpdateRecipeListCtrl(const MSG_RM_COOK_RECIPE&, CListCtrl& learned, CListCtrl& forgotten);

private:
	CookScript mCookScript;
};

extern CclientApp theApp;
