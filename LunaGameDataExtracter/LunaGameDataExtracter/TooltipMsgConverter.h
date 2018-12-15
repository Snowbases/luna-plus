#pragma once

class CTooltipMsgConverter
{
	// 100312 ShinJS --- ToolTipMsg 예약어 정보
	std::map< DWORD, int > m_mapToolTipMsgKeyWord;

public:
	CTooltipMsgConverter(void);
	~CTooltipMsgConverter(void);

	// 100315 ShinJS --- 사용가능 장비타입에 대한 Msg 반환
	const char* GetSkillEquipTypeText( const ACTIVE_SKILL_INFO* const pActiveSkillInfo, char* msg, size_t msgSize );
	const char* GetSkillWeaponTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  );
	const char* GetSkillArmorTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  );

	// 100316 ShinJS --- 소모포인트에 대한 Msg 반환
	const char* GetConsumePointTypeText( const ACTIVE_SKILL_INFO* const pActiveSkillInfo, char* msg, size_t msgSize  );

	// 100315 ShinJS --- 버프 조건에 대한 Msg 반환
	const char* GetBuffConditionTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize  );
	const char* GetBuffCountTypeText( const BUFF_SKILL_INFO* const pBuffSkillInfo, char* msg, size_t msgSize , const int nCurrentCount=0 );


	// 100312 ShinJS --- ToolTipMsg 예약어 변환 추가
	// 툴팁문자열테이블 정보를 초기화한다.
	void InitToolTipMsgKeyWordTable();
	// 문자열에서 툴팁예약어를 찾는다.
	BOOL GetToolTipMsgKeyWordFromText( std::string& strMsg, std::string& keyword, std::string::size_type& keywordStart, std::string::size_type& keywordSize );
	// 툴팁예약어Type으로부터 변환된 문자열을 반환한다.
	const char* GetTextFromToolTipMsgKeyWordType( DWORD& dwToolTipIndex, int keywordType, char* txt, size_t txtSize, int prevKeyWordType = 0 );
	// Script문자열에서 툴팁예약어를 변환한다.
	void ConvertToolTipMsgKeyWordFormText( DWORD dwToolTipIndex, std::string& strMsg, std::string::size_type startPos = 0, int prevKeyWordType = 0 );
};
