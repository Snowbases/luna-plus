#pragma once


#define SKILLTREEMGR cSkillTreeManager::GetInstance()

class cIcon;
class cSkillBase;
class cImage;



class cSkillTreeManager
{
	CIndexGenerator mIconIndexCreator;
	CYHHashTable< cSkillBase > mSkillBaseTable;
	cSkillTreeManager(void);
	virtual ~cSkillTreeManager(void);

public:
	static cSkillTreeManager* GetInstance();
	void Init();
	void Release();
	void NetworkMsgParse(BYTE Protocol, LPVOID);
	void SetToolTipIcon(cIcon*, DWORD skillIndex);
	void GetIconImage(DWORD Skilldx, cImage * pLowImage , cImage * pHighImage);
	cSkillBase* GetSkill(DWORD SkillIdx);
	const SKILL_BASE* GetSkillBase( DWORD skillIndex );
	void Update(const SKILL_BASE&);
	void AddSetSkill(DWORD skillIndex, LEVELTYPE);
	void RemoveSetSkill(DWORD skillIndex, LEVELTYPE);
	void ResetSetSkill();
	void AddJobSkill( DWORD skillIndex, BYTE level );
	void ClearJobSkill();

private:
	// 세트로 인해 부여된 능력이 제거될 때는, 플레이어가 정적으로 부여한 능력이 복구하기 위해 필요한 정보
	typedef std::map< DWORD, SKILL_BASE >	StoredSkill;
	StoredSkill								mStoredSkill;

	// 세트 아이템은 스킬을 부여하거나 제거한다. 이때 레벨이 다른 동일 스킬(예: 파이어볼1, 파이어볼2)이 있을 경우 복구할 방법이 없다.
	// 이에 스택에 저장하여 세트 스킬을 제거할 때마다 체크하여 역순으로 복구한다.

	typedef std::stack< BYTE >			SetSkill;		// 상대적으로 적용되는 스킬 레벨값
	typedef std::map< DWORD, SetSkill >	StoredSetSkill;
	StoredSetSkill						mStoredSetSkill;

	// 081023 KTH -- 
	std::list<SKILL_BASE>			m_JobSkillList;
public :
	void SetPositionHead();
	SKILL_BASE* GetSkillBase();
};