#pragma once

#define FILTERTABLE	CFilteringTable::GetInstance()

// ..필터링에서 예외로 처리할 경우 그 타입
enum FILTER_EXCEPTION_TYPE
{
	FET_NONE = 0,
	FET_WHISPER_CHARNAME, // 귓말 캐릭터명
};

class CFilteringTable
{	
protected:
#ifdef _UNICODE
	typedef std::wstring Text;
#else
	typedef std::string Text;
#endif
	friend BOOL FindPartially(const Text&, const Text&);
	typedef std::set< Text > KeywordContainer;
	enum Type
	{
		TypeNone,
		TypePartiallyBlockedName,
		TypeEntirelyBlockedName,
		TypePartiallyBlockedKeyword,
	};
	typedef std::map< Type, KeywordContainer > FilterContainer;
	FilterContainer mFilterContainer;
	FILTER_EXCEPTION_TYPE m_eFilterExceptionType;

protected:
	CFilteringTable();
	virtual ~CFilteringTable();
	void LoadScript();
	BOOL IsInvalidText(Type, LPCTSTR) const;
	BOOL IsInDBCRange(WORD DBChar) const;
	BOOL IsExceptionInvalidCharInclude(unsigned char* pszText);
	void RemoveSpace(LPCTSTR, LPTSTR, size_t) const;

public:
	static CFilteringTable* GetInstance();
	BOOL IsInvalidCharInclude(unsigned char* pStr, BOOL AllowAllDBC = FALSE);
	BOOL IsUsableName(LPCTSTR);
	BOOL FilterChat(LPCTSTR);
	BOOL IsCharInString(const char* pStr, const char* pNoChar);
	void SetExceptionInvalidCharIncludeType(FILTER_EXCEPTION_TYPE eFilterExceptionType) { m_eFilterExceptionType = eFilterExceptionType; }
};