#ifndef _THAIFUNC_HXX_
#define _THAIFUNC_HXX_

#include <string.h>


#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#define KARAN			236		/* same as Thanthakhat */

///////////////////////////////////////////////////////
//
// f +-
// e |+-
// d ||+- Diacritic (MAITAIKHU , NIKAHIT)
// c |||+- Break char
// b ||||+- Left aligned
// a |||||+- Tone mark  (KARAN included)
// 9 ||||||+- Vowel  (MAIHANAKAT included)
// 8 |||||||+- Lower tail  (eg.
// 7 ||||||||+- Upper tail      (eg. popla)
// 6 |||||||||+- Math sign
// 5 ||||||||||+- English
// 4 |||||||||||+- Thai
// 3 ||||||||||||+- Capital
// 2 |||||||||||||+- Punctuation
// 1 ||||||||||||||+- Digit
// 0 |||||||||||||||+- Consonance
//   ||||||||||||||||
//   fedcba9876543210

#define TCT_CONSONANCE          0x0001
#define TCT_DIGIT               0x0002
#define TCT_PUNCT               0x0004
#define TCT_CAPITAL             0x0008
#define TCT_THAI                0x0010
#define TCT_ENGLISH             0x0020
#define TCT_MATH                0x0040
#define TCT_UPPER               0x0080
#define TCT_LOWER               0x0100
#define TCT_VOWEL               0x0200
#define TCT_TONEMARK            0x0400
#define TCT_LEFTALIGNED         0x0800
#define TCT_BREAK               0x1000
#define TCT_DIACRITIC           0x2000
#define TCT_CONTROL             0x0000

#define TCT_LEADINGVOWEL		0x4000

#define TCT_THAIZEROWIDTH       (TCT_VOWEL | TCT_DIACRITIC | TCT_TONEMARK)

#define TCT_UPPERVOWEL          (TCT_VOWEL | TCT_UPPER)
#define TCT_LOWERVOWEL          (TCT_VOWEL | TCT_LOWER)

#define TCT_THAIVOWEL           (TCT_THAI | TCT_VOWEL)
#define TCT_THAIUPPERVOWEL      (TCT_THAI | TCT_VOWEL | TCT_UPPER)
#define TCT_THAILOWERVOWEL      (TCT_THAI | TCT_VOWEL | TCT_LOWER)
#define TCT_THAIUPPERVOWELLEFT  (TCT_THAIUPPERVOWEL | TCT_LEFTALIGNED)
#define TCT_THAILOWERVOWELLEFT  (TCT_THAILOWERVOWEL | TCT_LEFTALIGNED)
#define TCT_THAIDIGIT           (TCT_THAI | TCT_DIGIT)
#define TCT_THAICONS            (TCT_THAI | TCT_CONSONANCE)
#define TCT_THAIDIAC            (TCT_THAI | TCT_DIACRITIC)
////// Ex. TOPATAK,DOCHADA
#define TCT_THAICONSLOWER       (TCT_THAICONS | TCT_LOWER)
////// Ex. POPLA, FOFUN
#define TCT_THAICONSUPPER       (TCT_THAICONS | TCT_UPPER)

#define TCT_THAITONE            (TCT_THAI | TCT_TONEMARK)
#define TCT_THAIBREAK           (TCT_THAI | TCT_BREAK)

#define TCT_ENGCAPS             (TCT_ENGLISH | TCT_CAPITAL | TCT_CONSONANCE)
#define TCT_ENGCONS             (TCT_ENGLISH | TCT_CONSONANCE)
#define TCT_ENGDIGIT            (TCT_ENGLISH | TCT_DIGIT)

#define TCT_SEPERATOR           (TCT_BREAK | TCT_PUNCT)

class ThaiLexicon;
class ThaiBreakIterator;

class ThaiFunc
{
public:
	ThaiFunc();
	~ThaiFunc();

public:
	void		InitializeTable();
	bool		IsThaiChar(unsigned short ch);
	unsigned int	ThaiCharType(unsigned short ch);

protected:
	unsigned int	tblCharType[96];
};

////////// Mem-Trie Control Code ////////
#define	ENDWORD			0x80
#define ENDCHILD		0x40

////////// Dict Control Code //////////
#define MAX_DICT_CTRL_CHAR	0x20
#define SUBSTRENDWORD		0x02
#define SUBSTRHAVECHILD		0x04


#define MAKEINDEX(b1,b2,b3)	((LONG)((DWORD)(b1&0x3F)<<16 | (WORD)b2<<8 | b3))
#define MAXWORDLEN		50
#define fDictAddWord	CDictGenDlg::AddWord
#define _lptrie(lpv)	((LPTRIE)(lpv))

typedef struct _TRIE
{
	unsigned char	ch;		// char
	bool	wrap;	// can break word?
	void*	next;	// pointer to next node in save level
	void*	child;	// pointer to next level
}TRIE,*LPTRIE;

typedef struct	_DIFFBYTE
{
#if defined SOLARIS && defined SPARC
	byte b3,b2,b1,b0;
#else
	unsigned char b0,b1,b2,b3;
#endif
}DIFFBYTE;

typedef union _DIFFINDEX
{
	long	l;
	DIFFBYTE	b;
}DIFFINDEX;

typedef struct _DICTVERSION
{
	unsigned char fv;			// version of 'Find Thai Word Break' routine.
	unsigned char dv;			// version of 'Dictionary'.
}DICTVERSION;

typedef struct _DICTHEADER
{
	char			szDesc[256];	// readable description of file
	char			chEOF;			// end of file flag
	DICTVERSION		version;		// version number
	//unsigned int		cWord;			// number of word in dictionary
	//unsigned int		cNode;			// number of all char node
	unsigned int		cWord;			// number of word in dictionary
	unsigned int		cNode;			// number of all char node
	long		code;			// code for data validation
	long		nSize;			// dict's size
	bool		fValid;			// status of dict
}DICTHEADER;

//--------------------------------------------------------------------------
// Thai Lexicon Support class..
//
// TODO : 
//--------------------------------------------------------------------------
class ThaiLexicon
{
public:
	ThaiLexicon();
	~ThaiLexicon();

// Implementation
public:
	bool AddWord(unsigned char *szNew,unsigned int cb);
	void RefreshTrie();

	// Implementation of lexicon functions
	bool fSaveLexicon(unsigned char *lpFileName);
	bool fLoadLexicon(unsigned char *lpFileName);

	// Implemented of return protected attributes.
	unsigned int uiGetCountWord() { return m_cWord; };
	long lGetCountNode() { return m_cNode; };
	long lGetCountSize() { return m_nSize; };
	LPTRIE lpGetTriePacked() { return m_trie; };
	unsigned char* pGetMemDict() { return m_MemDict; };

	// Test and debug function
	bool SearchMemDict(BYTE *pchWord);	// Search the given word in memory lexicon dictionary.

	// Retrieve information;
	unsigned int GetTotalWord() { return m_cWord; };
	unsigned int GetTotalNode() { return m_cNode; };

// Implementation
protected:		 
	int Trie2Mem(LPTRIE pTrie, int iStart, bool fWrap);
	int EnumNode(LPTRIE pTrie);
	unsigned char CountUniqueStr(LPTRIE pTrie);
	int InsertSubWord(LPTRIE pTrie, unsigned char nLen, int iStart);

	void DestroyTrie(LPTRIE pTrie);

// Attributes
protected:
	LPTRIE		m_trie;
	unsigned int	m_cWord;
	long	m_cNode;
	long	m_nSize;
	unsigned char	*m_MemDict;	
	DICTHEADER	m_DictHeader;
};

#ifndef IsBetween
#define IsBetween(a, b, c)	(((b) <= (a)) && ((a) <= (c)))
#endif

// Specific type of breaking iterator.
#define FTWB_CARETMOVEMENT	0
#define FTWB_WORDWRAP		1
#define FTWB_SEPARATESYMBOL	2
#define FTWB_SPELLER		4

//
// FindThaiWordBreak parser status
//		LT_THAI	: Loop for parse Thai char
//		LT_THAI	: Loop for parse English char
//
enum
{
	LT_THACH,
	LT_ENGCH,
	LT_LIBREAK,
	LT_THAPUNCT,
	LT_ENGPUNCT,
	LT_MAX
};

#define CWL_MAX	100
#define IsEnglishChar(ch)	(IsBetween(ch,'0','9') || \
							IsBetween(ch,'a','z') || \
							IsBetween(ch,'A','Z'))

#define XT_THA			XT_THAI
#define XT_ENG			XT_ENGLISH
#define XT_WRDBEG		XT_WORDBEGIN
#define XT_WRDEND		XT_WORDEND
#define XT_SNTEND		XT_SENTENCEEND
#define XT_SYM			XT_SYMBOL
#define XT_PUNCT		XT_PUNCTUATION
#define XT_LVOW			XT_LEADINGVOWEL
#define	XT_FVOW			XT_FOLLOWINGVOWEL
#define XT_DIAC			XT_DIACRITIC
#define XT_TONE			XT_TONEMARK
#define	XT_CONS			XT_CONSONANCE
#define XT_LTAIL		XT_LOWERTAIL
#define XT_UTAIL		XT_UPPERTAIL
#define XT_UCASE		XT_UPPERCASE
#define XT_LCASE		XT_LOWERCASE
#define XT_ZWIDTH		XT_ZEROWIDTH
#define XT_ALLSYMBOL	(XT_PUNCT | XT_WRDEND | XT_WRDBEG | XT_SNTEND)

#define XT_THAI				0x00000001
#define XT_ENGLISH			0x00000002
#define XT_BOTH				0x00000004

#define XT_WORDBEGIN		0x00000010
#define XT_WORDEND			0x00000020
#define	XT_SENTENCEEND		0x00000040

#define XT_ZEROWIDTH		0x00000100
#define XT_SYMBOL			0x00000200
#define XT_PUNCTUATION		0x00000400

#define XT_LEADINGVOWEL		0x00001000
#define XT_FOLLOWINGVOWEL	0x00002000
#define	XT_DIACRITIC		0x00004000
#define XT_TONEMARK			0x00008000

#define	XT_DIGIT			0x00010000
#define XT_CONSONANCE		0x00020000
#define XT_UPPERTAIL		0x00040000
#define XT_LOWERTAIL		0x00080000

#define XT_UPPERCASE		0x00100000
#define XT_LOWERCASE		0x00200000

#define XT_WHITESPACE		0x01000000
#define	XT_HARDSPACE		0x02000000
#define XT_SOFTBREAK		0x04000000
#define XT_NONBREAK			0x08000000

#define	XT_CONTROL			0x10000000

// Private macro :-)
#define STORE_CUR_LEN		if (cch) { goto LStoreWordLen; }
#define PREPARE_BREAK_ARRAY	if (rgbBrk[iBrk]) {rgbBrk[++iBrk] = 0;}
#define GO_NEXTLOOP			goto LStartLoop;
#define COLLECT_SPACE		goto LCollectSpace;
#define FNonbreakChar(ch)	( ch == 221 )

//--------------------------------------------------------------------------
// Thai Break Iterator.. (AKA FindThaiWordBreak)
//
// TODO : change to read specific lexicon file name form XML.
//--------------------------------------------------------------------------
class ThaiBreakIterator
{
public:
	ThaiBreakIterator(unsigned char* lpLexiconPath);
	~ThaiBreakIterator();

public:
	bool	InitializeLexicon();
	int	FindThaiWordBreak(const char* szText,unsigned int nStrlen, unsigned char* rgbBrk, unsigned int nMaxBrk, unsigned int uFlags);

protected:
	bool	HaveType(char ch, unsigned long IsType); //	{ return (bool)(XCharType(ch) & IsType); };
	unsigned long	XCharType(char ch); // { m_rgThaiCharTypeTable[ch]; };
	int	GetWeight(unsigned char* vmOurDict, unsigned char* szText,unsigned char* pchFst,unsigned char* pchLim, unsigned long dwFlags);
	bool	fCanCat(unsigned char chPrev, unsigned char ch, unsigned int uFlags);


	ThaiLexicon*	m_pLexicon;
	unsigned long	m_rgThaiCharTypeTable[256];
	char*		m_sLexiconFile;
};

#endif //_THAIFUNC_HXX_
