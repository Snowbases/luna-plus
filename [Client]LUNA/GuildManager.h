#pragma once


#define GUILDMGR CGuildManager::GetInstance()


class CGuildManager  
{
	void MarkChange(MSG_DWORD3 * pInfo);

	void SetPlayerNickName(CObject* pObject, char* NickName);
	void SetPlayerGuildName(CObject* pObject, DWORD dwGuildIdx, char* GuildName);
public:	
	static CGuildManager* GetInstance();

	CGuildManager();
	virtual ~CGuildManager();
	
	void Init();
	void Release();
	void NetworkMsgParse(BYTE Protocol,void* pMsg);	
	
	const char* GetMasterName() const;
	DWORD GetMemberSize() const;
	DWORD GetStudentSize() const;
	const char* GetMemberName(DWORD MemberID) const;
	// 081031 LUJ, 플레이어 정보를 반환한다
	const GUILDMEMBERINFO& GetMemberInfo( DWORD playerIndex ) const;
	const GUILDMEMBERINFO& GetMemberInfo( const TCHAR* playeName ) const;
	void SetScore( DWORD );
	DWORD GetScore() const;
	DWORD GetLevel() const;
	const char* GetGuildName() const;
	const GUILDMEMBERINFO* GetSelectedMember() const;
	
	const char* GetRankName(BYTE rank);


	// 070801 웅주, 길드 정보를 읽어들인다.
private:
	// 081031 LUJ, 소환 스크립트 파싱 추가
	void LoadScript();
	void ParseScoreScript( const std::list< std::string >& );
	void ParseLevelScript( const std::list< std::string >& );
	void ParseSkillScript( const std::list< std::string >& );

public:
	struct ScoreSetting
	{
		DWORD	mUnitTime;		// 접속 단위 시간
		float	mUnitTimeScore;	// 접속 단위 시간 당 얻는 점수
		float	mHuntScore;		// 몬스터 사냥시마다 얻는 점수
		DWORD	mUpdateGap;		// DB에 갱신하고 길드원에게 브로드캐스팅하는 주기. 밀리초 단위. 너무 잦으면 부하발생...
		float	mKillerScore;	// 다른 길드원을 살해앴을 때 얻는 점수
		float	mCorpseScore;	// 다른 길드원에게 살해되었을 때 잃는 점수

		// 길드전 승리시 얻는 점수
		// 주의: mPercent는 패배 길드가 잃는 점수의 총합에서 승리 길드가 획득하는 비율을 뜻한다.
		struct Unit
		{
			float mValue;
			float mPercent;
		}
		mWinScore;;

		float	mWinMoneyRate;	// 선포금에서 획득하는 비율

		// 길드전 패배시 잃는 점수
		typedef std::map< DWORD, Unit >		LostScoreMap;
		LostScoreMap						mLostScoreMap;
	};

private:
	ScoreSetting mScoreSetting;


public:
	struct LevelSetting
	{
		DWORD	mRequiredMasterLevel;
		DWORD	mRequiredScore;
		DWORD	mRequiredMoney;
		DWORD	mRequiredQuest;
		DWORD	mMemberSize;
		DWORD	mUnionSize;
		DWORD	mApprenticeSize;
		BOOL	mGuildMark;
		DWORD	mWarehouseSize;
		// 080417 LUJ, 획득 가능한 길드 스킬 개수
		DWORD	mSkillSize;
	};

	const LevelSetting* GetLevelSetting( DWORD level ) const;

private:
	// 키: 길드 레벨
	typedef stdext::hash_map< DWORD, LevelSetting >	LevelSettingMap;
	LevelSettingMap									mLevelSettingMap;


	// 080417 LUJ, 길드 스킬 정보를 읽는다
public:
	struct SkillSetting
	{
		// 080417 LUJ, 사용 가능한 최대 스킬 레벨
		BYTE				mLevel;

		// 080417 LUJ, 사용가능한 직위
		std::set< DWORD >	mRank;
	};

	// 080417 LUJ, 스킬 설정 정보가 담긴 맵, 키: 스킬 인덱스
	typedef stdext::hash_map< DWORD, SkillSetting > SkillSettingMap;

	// 080417 LUJ, 레벨 별 스킬 설정 정보가 담긴 맵. 키: 길드 레벨
	typedef stdext::hash_map< DWORD, SkillSettingMap >	SkillSettingLevelMap;

	// 080417 LUJ, 레벨별 길드 스킬 설정 반환
	const SkillSettingMap* GetSkillSetting( DWORD level )	const;

	// 080417 LUJ, 최대 길드 레벨의 스킬 정보를 얻는다
	const SkillSettingMap* GetMaxLevelSkillSetting() const;
	
private:
	SkillSettingLevelMap	mSkillSettingLevelMap;

	// 080417 LUJ, 길드 스킬 관리
private:
	// 080417 LUJ, 보유한 스킬을 담는다. 키: 스킬 인덱스, 값: 스킬 레벨
	typedef stdext::hash_map< DWORD, LEVELTYPE > SkillMap;
	SkillMap mSkillMap;

public:
	// 보유한 길드 스킬을 플레이어 상태와 비교하여 적용한다
	void UpdateSkill();

	// 080417 LUJ, 스킬이 없다면 0이 반환된다
	BYTE			GetSkillLevel( DWORD index )	const;
	inline DWORD	GetSkillSize()					const	{ return mSkillMap.size(); }
};
