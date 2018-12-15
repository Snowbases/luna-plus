#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "ftwbrk.h"

unsigned int TBL_CHTYPE_WITHLEADING[96] =
{
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	// First element is bogus
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONSLOWER,	TCT_THAICONSLOWER,
	
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONSUPPER,	
	TCT_THAICONS,	TCT_THAICONSUPPER,	TCT_THAICONS,	TCT_THAICONSUPPER,	

	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	
	TCT_THAICONSLOWER,	TCT_THAICONS,	TCT_THAICONSLOWER,	TCT_THAICONS,	
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	
	TCT_THAICONS,	TCT_THAICONS,	TCT_THAICONS,	TCT_THAIBREAK,

	TCT_THAI,		TCT_THAIUPPERVOWEL,	TCT_THAI,	TCT_THAIVOWEL,
	TCT_THAIUPPERVOWEL,	TCT_THAIUPPERVOWEL,	TCT_THAIUPPERVOWEL,	TCT_THAIUPPERVOWEL,	
	TCT_THAILOWERVOWEL,	TCT_THAILOWERVOWEL,	TCT_THAILOWERVOWEL,	TCT_BREAK,
	TCT_BREAK,		TCT_CONTROL	,	TCT_CONTROL,	TCT_THAI,
	
	TCT_THAI|TCT_LEADINGVOWEL,		TCT_THAI|TCT_LEADINGVOWEL,		TCT_THAI|TCT_LEADINGVOWEL,		TCT_THAI|TCT_LEADINGVOWEL,	
	TCT_THAI|TCT_LEADINGVOWEL,		TCT_THAI,		TCT_THAI,		TCT_THAIDIAC,	
	TCT_THAITONE,	TCT_THAITONE,	TCT_THAITONE,	TCT_THAITONE,	
	TCT_THAITONE,	TCT_THAI,		TCT_THAI,		TCT_THAI,

	TCT_THAIDIGIT,	TCT_THAIDIGIT,	TCT_THAIDIGIT,	TCT_THAIDIGIT,	
	TCT_THAIDIGIT,	TCT_THAIDIGIT,	TCT_THAIDIGIT,	TCT_THAIDIGIT,	
	TCT_THAIDIGIT,	TCT_THAIDIGIT,	TCT_THAIBREAK,	TCT_THAIBREAK,
	TCT_THAILOWERVOWEL,	TCT_THAILOWERVOWEL	,TCT_THAILOWERVOWEL,	TCT_ENGLISH,
};

unsigned int TBL_CHTYPE[96] = 
{
    TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,	 // first element is bogus
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONSLOWER,  TCT_THAICONSLOWER,
	
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONSUPPER,
	TCT_THAICONS,		TCT_THAICONSUPPER,  TCT_THAICONS,		TCT_THAICONSUPPER,

	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,
	TCT_THAICONSLOWER,  TCT_THAICONS,		TCT_THAICONSLOWER,	TCT_THAICONS,
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,
	TCT_THAICONS,		TCT_THAICONS,		TCT_THAICONS,		TCT_THAIBREAK,

	TCT_THAI,           TCT_THAIUPPERVOWEL, TCT_THAI,			TCT_THAI,
	TCT_THAIUPPERVOWEL, TCT_THAIUPPERVOWEL, TCT_THAIUPPERVOWEL,	TCT_THAIUPPERVOWEL,
	TCT_THAILOWERVOWEL, TCT_THAILOWERVOWEL, TCT_THAILOWERVOWEL, TCT_BREAK,
	TCT_BREAK,          TCT_CONTROL,		TCT_CONTROL,		TCT_THAI,
		
	TCT_THAI,           TCT_THAI,           TCT_THAI,           TCT_THAI,
	TCT_THAI,           TCT_THAI,           TCT_THAI,           TCT_THAIDIAC,
	TCT_THAITONE,		TCT_THAITONE,		TCT_THAITONE,		TCT_THAITONE,
	TCT_THAITONE,		TCT_THAIDIAC,       TCT_THAI,           TCT_THAI,

	TCT_THAIDIGIT,		TCT_THAIDIGIT,		TCT_THAIDIGIT,		TCT_THAIDIGIT,
	TCT_THAIDIGIT,		TCT_THAIDIGIT,		TCT_THAIDIGIT,		TCT_THAIDIGIT,
	TCT_THAIDIGIT,		TCT_THAIDIGIT,		TCT_THAIBREAK,		TCT_THAIBREAK,
	TCT_THAILOWERVOWEL, TCT_THAILOWERVOWEL,	TCT_THAILOWERVOWEL, TCT_ENGLISH,
};

unsigned long THAICHARTYPETABLE[256] = 
{
/*00	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	//???
		XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,
		XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,
		XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,
10	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,
		XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,
		XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,
		XT_CONTROL,	XT_CONTROL,	XT_CONTROL,	XT_CONTROL,*/
		0,0,0,0,	0,0,0,0,	0,0,0,0,	0,0,0,0,
		0,0,0,0,	0,0,0,0,	0,0,0,0,	0,0,0,0,

/*20*/	XT_WHITESPACE,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
/*28*/	XT_WRDBEG,
		XT_WRDEND,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT | XT_BOTH,
		XT_PUNCT,
/*30*/	XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
/*38*/	XT_DIGIT | XT_ENG,
		XT_DIGIT | XT_ENG,
		XT_PUNCT,
		XT_PUNCT,
		XT_WRDBEG,
		XT_PUNCT,
		XT_WRDEND,
		XT_SNTEND,
/*40*/	XT_PUNCT,
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
/*48*/	XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
/*50*/	XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
/*58*/	XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_ENG | XT_UCASE,		
		XT_WRDBEG,
		XT_PUNCT,
		XT_WRDEND,
		XT_PUNCT,
		XT_PUNCT,
/*60*/	XT_PUNCT,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
/*68*/	XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
/*70*/	XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
/*78*/	XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_ENG | XT_LCASE,
		XT_WRDBEG,
		XT_PUNCT,
		XT_WRDEND,
		XT_PUNCT,
		0,
/*80*/	XT_THA | XT_CONS,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		XT_PUNCT,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
/*88*/	XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_DIAC,
/*90*/	XT_THA | XT_CONS,
		XT_PUNCT | XT_BOTH | XT_WRDBEG,
		XT_PUNCT | XT_BOTH | XT_WRDEND,
		XT_PUNCT | XT_BOTH | XT_WRDBEG,
		XT_PUNCT | XT_BOTH | XT_WRDEND,
		XT_PUNCT,
		XT_PUNCT,
		XT_PUNCT,
/*98*/	XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_DIAC,
/*A0*/	XT_WHITESPACE,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
/*A8*/	XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
/*B0*/	XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
/*B8*/	XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
/*C0*/	XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
/*C8*/	XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_CONS,
		XT_THA | XT_SYMBOL,
/*D0*/	XT_THA | XT_FVOW,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_THA | XT_FVOW,
		XT_THA | XT_FVOW,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
/*D8*/	XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_HARDSPACE,
		XT_PUNCT,
		0,
		0,
		XT_PUNCT,
/*E0*/	XT_THA | XT_LVOW,
		XT_THA | XT_LVOW,
		XT_THA | XT_LVOW,
		XT_THA | XT_LVOW,
		XT_THA | XT_LVOW,
		XT_THA | XT_FVOW,
		XT_THA,
		XT_THA | XT_ZWIDTH | XT_DIAC,
/*E8*/	XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_TONE,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_THA | XT_ZWIDTH | XT_DIAC,
		XT_PUNCT,
/*F0*/	XT_DIGIT | XT_THA,
		XT_DIGIT | XT_THA,
		XT_DIGIT | XT_THA,
		XT_DIGIT | XT_THA,
		XT_DIGIT | XT_THA,
		XT_DIGIT | XT_THA,
		XT_DIGIT | XT_THA,
/*F8*/	XT_DIGIT | XT_THA,
		XT_DIGIT | XT_THA,
		XT_PUNCT,
		XT_PUNCT,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		XT_THA | XT_ZWIDTH,
		0,
};

//-------------------------------------------------------------------------------
// Implementation of ThaiFunc class
//-------------------------------------------------------------------------------
ThaiFunc::ThaiFunc()
{
}

ThaiFunc::~ThaiFunc()
{
}

bool ThaiFunc::IsThaiChar(unsigned short ch)
{ 
	return ((unsigned int(ch) >> 8) & 0x00ff)==0x000e; 
}

unsigned int ThaiFunc::ThaiCharType(unsigned short ch) 
{ 
	return TBL_CHTYPE[(unsigned char)ch]; 
}

void ThaiFunc::InitializeTable() 
{ 
	memcpy(tblCharType, TBL_CHTYPE, 96*sizeof(unsigned int)); 
}

//-------------------------------------------------------------------------------
// Implementation of ThaiLexicon class
//-------------------------------------------------------------------------------
ThaiLexicon::ThaiLexicon()
{
	m_trie = NULL;
	m_cWord = 0;
	m_cNode =
	m_nSize = 0;
	m_MemDict = NULL;
	memset((void*)&m_DictHeader, 0L, sizeof(DICTHEADER));
}

ThaiLexicon::~ThaiLexicon()
{
	if (m_MemDict)
		delete m_MemDict;
	DestroyTrie(m_trie);
}

void ThaiLexicon::DestroyTrie(LPTRIE pTrie)
{
	if (pTrie)
	{
		DestroyTrie(_lptrie(pTrie->child));
		DestroyTrie(_lptrie(pTrie->next));
		delete pTrie;
	}
}

bool ThaiLexicon::AddWord(unsigned char *szNew,unsigned int cb)
{
	unsigned int iNext = 0;
	LPTRIE	pTrie;

	if (cb <= 0)	return false;

	if (m_cWord == 0)		// insert the first word?
	{
		m_trie = new TRIE;	// allocate root note;
		m_trie->next = m_trie->child = NULL;
		m_trie->ch = szNew[iNext];
		m_trie->wrap = 0;
		m_cNode++;
	}
	
	pTrie = m_trie;			// init Trie walker
	while(iNext < cb)
	{
LNewLoop:
		if (pTrie->ch == szNew[iNext])	// matched character
		{
LCharMatched:
			if (++iNext == cb)			// last character?
				break;
			
			if ((!pTrie->child) || (_lptrie(pTrie->child)->ch > szNew[iNext]))
			{
				LPTRIE pTmp = new TRIE;			// create new node and insert in next level
				m_cNode++;
				pTmp->ch = szNew[iNext];
				pTmp->next = pTrie->child;
				pTmp->child = NULL;
				pTmp->wrap = false;
				pTrie->child = pTmp;
				pTrie = _lptrie(pTrie->child);
				goto LCharMatched;
			}
			pTrie = _lptrie(pTrie->child);
			goto LNewLoop;
		}
		else
		if ((!pTrie->next) || (_lptrie(pTrie->next)->ch > szNew[iNext]))
		{
			LPTRIE pTmp = new TRIE;		// create new node and insert in this level
			m_cNode++;
			pTmp->ch = szNew[iNext];
			pTmp->next = pTrie->next;
			pTmp->child = NULL;
			pTmp->wrap = false;
			pTrie->next = pTmp;
			pTrie = pTmp;
			goto LCharMatched;			// we need speed more than spaces
		}
		pTrie = _lptrie(pTrie->next);
		goto LNewLoop;
	}

	if (pTrie->wrap)
		return false;			// this word already exist
	else
	{
		pTrie->wrap = true;
		m_cWord++;				// advanced word count
	}
	return true;

}

void ThaiLexicon::RefreshTrie()
{
	bool fMemValid = false;
	if (!m_MemDict)		// memory has been allocated already ?
	{
		if ((m_MemDict = new unsigned char[m_cNode*10]) != NULL)
			fMemValid = true;
	}
#if 0 // C++ Syntax not available.
	else
	{
		// try to resize existing memory block
		unsigned char *pMemTmp = (unsigned char*)GlobalReAlloc(m_MemDict,m_cNode*10,GMEM_FIXED);
		if (pMemTmp)
		{
			m_MemDict = pMemTmp;
			fMemValid = false;
		}
	}
#endif
	if (fMemValid)
		m_nSize = Trie2Mem(m_trie, 0, false);
}

int ThaiLexicon::Trie2Mem(LPTRIE pTrie, int iStart, bool fWrap)
{
	int iNext,iCur;
	DIFFINDEX cbDiff;
	int iNode,cNode = EnumNode(pTrie);			// enum node in same level
	LPTRIE pTrieWalk = pTrie;

	iCur = iStart;
	iNext = iStart + 1 + 3 * cNode;				// we need 3 bytes to store address to child node

	for (iNode = 0; iNode < cNode; iNode++)
	{
		cbDiff.l = iNext;
		m_MemDict[iCur++] = (cbDiff.b.b2 | (fWrap?ENDWORD:0));		// store address of node 'iNode'
		m_MemDict[iCur++] = cbDiff.b.b1;
		m_MemDict[iCur++] = cbDiff.b.b0;
		unsigned char nSubWordLen = CountUniqueStr(_lptrie(pTrieWalk));
		if (nSubWordLen > 1)
			iNext = InsertSubWord(_lptrie(pTrieWalk),nSubWordLen,iNext);
		else
		{
			m_MemDict[iNext++] = pTrieWalk->ch;

			if (pTrieWalk->child)
				iNext = Trie2Mem(_lptrie(pTrieWalk->child),iNext,pTrieWalk->wrap);
			else
			{
				m_MemDict[iNext++] = (ENDCHILD | ENDWORD);
			}
		}

		pTrieWalk = _lptrie(pTrieWalk->next);
	}

	m_MemDict[iCur++] = (ENDCHILD | (fWrap?ENDWORD:0));	// write end of child
	return iNext;
}

int ThaiLexicon::EnumNode(LPTRIE pTrie)
{
	int cNode = 0;
	while(pTrie)
	{
		pTrie = _lptrie(pTrie->next);
		cNode++;
	}
	return cNode;
}

unsigned char ThaiLexicon::CountUniqueStr(LPTRIE pTrie)
{
	LPTRIE pTrieWalk = pTrie;
	unsigned char nLevel = 0;

	while (pTrieWalk && !(pTrieWalk->next))
	{
		pTrieWalk = _lptrie(pTrieWalk->child);
		nLevel++;
	}
	
	return nLevel;
}

int ThaiLexicon::InsertSubWord(LPTRIE pTrie, unsigned char nLen, int iStart)
{
	LPTRIE pTrieWalk = pTrie;
	unsigned char iLoop;
	int iNext = iStart;
	bool fEndWord = false;
	
	m_MemDict[iNext++] = nLen;	// store length
	
	for (iLoop = 0; iLoop < nLen; iLoop++)
	{
		m_MemDict[iNext++] = pTrieWalk->ch;
		if ((fEndWord = pTrieWalk->wrap) == true)
			m_MemDict[iNext++] = SUBSTRENDWORD;
		pTrieWalk = _lptrie(pTrieWalk->child);
	}
	if (pTrieWalk)
	{
		if ((unsigned int)m_MemDict[iNext-1] < MAX_DICT_CTRL_CHAR)
			m_MemDict[iNext-1] |= SUBSTRHAVECHILD;
		else
			m_MemDict[iNext++] = SUBSTRHAVECHILD;
		iNext = Trie2Mem(_lptrie(pTrieWalk),iNext,fEndWord);
	}
	return iNext;
}

bool ThaiLexicon::SearchMemDict(unsigned char *pchWord)
{
	unsigned char *pchDictWalk = m_MemDict;
	unsigned char *pchWordWalk = pchWord;
	DIFFINDEX cbDiff;
	cbDiff.l = 0;
	unsigned char iLoop,nSubStrLen;

	while (*pchWordWalk)
	{
LSrcLoopStart:
		if (*pchDictWalk & ENDCHILD)				// not found word
			break;				
		cbDiff.b.b2 = ((*(pchDictWalk++)) & 0x3F);  		// restore index to next character
		cbDiff.b.b1 = *(pchDictWalk++);
		cbDiff.b.b0 = *(pchDictWalk++);
		if (((nSubStrLen = m_MemDict[cbDiff.l]) < MAX_DICT_CTRL_CHAR) &&
			(m_MemDict[cbDiff.l+1] == *pchWordWalk))
		{
			cbDiff.l ++;
			for (iLoop = 0; iLoop < nSubStrLen; iLoop++)
			{
				if (m_MemDict[cbDiff.l++] != *pchWordWalk)	// word not match
					return false;

				pchWordWalk++;

				if (!(*pchWordWalk))
					return ((m_MemDict[cbDiff.l] < MAX_DICT_CTRL_CHAR) &&
							(m_MemDict[cbDiff.l] & SUBSTRENDWORD));
				else
				if (m_MemDict[cbDiff.l] < MAX_DICT_CTRL_CHAR)
				{
					if (m_MemDict[cbDiff.l] & SUBSTRHAVECHILD)	// this means end of substr
					{											// next 3 byte will be 'Address'
						pchDictWalk = m_MemDict + cbDiff.l + 1;	// of next node.
						goto LSrcLoopStart;
					}
					else
						cbDiff.l++;								// skip flag SUBENDWORD
				}
			}
			return false;
		}
		else
		{
			if (m_MemDict[cbDiff.l] == *pchWordWalk)			// Is character match ?
			{
				pchWordWalk++;									// check next character
				if (!(*pchWordWalk) && (m_MemDict[cbDiff.l + 1] & ENDWORD))
					return true;
				pchDictWalk = m_MemDict + cbDiff.l + 1;
			}
		}
	}
	return false;
}

bool ThaiLexicon::fSaveLexicon(unsigned char* lpFileName)
{
	bool fSuccess = false;

	// initialize header
	//lstrcpy(m_DictHeader.szDesc, "Thai Lexicon Data Version 1.1"); // <<== NYI Please check...!!!!
	m_DictHeader.chEOF = -1;	//0xFF
	m_DictHeader.version.fv = 2;
	m_DictHeader.version.dv = 2;
	m_DictHeader.cWord = m_cWord;
	m_DictHeader.cNode = (unsigned int)m_cNode;
	m_DictHeader.code = ((int)m_cWord << 1) + m_cNode;
	m_DictHeader.nSize = m_nSize;
	m_DictHeader.fValid = true;

	FILE* fn=0;
	fn = fopen((const char*)lpFileName, "a+b");
	if (!fn)
		return fSuccess;
	if (fwrite(&m_DictHeader,sizeof(unsigned char), sizeof(DICTHEADER), fn)!=sizeof(DICTHEADER))
	{
		fclose(fn);
		return fSuccess;
	}
#ifdef _DEBUG
	fpos_t fpos;
	fgetpos(fn, &fpos);
#endif
	if (fwrite(m_MemDict, sizeof(unsigned char), m_nSize, fn)!=(size_t)m_nSize)
	{
		fclose(fn);
		return fSuccess;
	}
#ifdef _DEBUG
	fgetpos(fn, &fpos);
#endif
	fclose(fn);
	fSuccess = true;
	
	return fSuccess;
}

bool ThaiLexicon::fLoadLexicon(unsigned char *lpFileName)
{
	bool fSuccess = false;
	FILE* fn=0;

	//if(cfDict.Open( szDictName, CFile::modeRead, &cfeDict ) )
	if ((fn=fopen((const char*)lpFileName, "r+b")) != NULL)
	{
		// Read data header block
		if (sizeof(DICTHEADER) != fread(&m_DictHeader,sizeof(unsigned char), sizeof(DICTHEADER), fn))
			; // Error report
		else
			// validate data
			if (((int)m_DictHeader.code) != (int)(((int)m_DictHeader.cWord << 1) + (int)m_DictHeader.cNode))
				; // Error report
			else
				// allocate memory for dict
				if ((m_MemDict = new unsigned char[m_DictHeader.nSize]) == NULL)
					; // Error report
				else
				{
#if 0 // Don't need to justify pointer
					sal_Int64 pos = sizeof(DICTHEADER);
					fsetpos(fn, &pos);
#endif
					// read data
					if ((size_t)m_DictHeader.nSize != fread(m_MemDict, sizeof(unsigned char), m_DictHeader.nSize, fn))
						; // Error report
					else
					{
						m_cWord = m_DictHeader.cWord;
						m_cNode = m_DictHeader.cNode;
						m_nSize = m_DictHeader.nSize;
						fSuccess = true;
					}
				}
		fclose(fn);
	}
	else
		return false;
	
	if (!fSuccess && (m_MemDict))
	{
		delete m_MemDict;
		m_MemDict = NULL;
	}
	m_DictHeader.fValid = fSuccess;
	return fSuccess;
}

//-------------------------------------------------------------------------------
// Implementation of ThaiLexicon class
//-------------------------------------------------------------------------------
ThaiBreakIterator::ThaiBreakIterator(unsigned char* lpLexiconPath) : m_pLexicon(0), m_sLexiconFile((char*)lpLexiconPath)
{
	m_pLexicon = NULL;
	InitializeLexicon();
}

ThaiBreakIterator::~ThaiBreakIterator()
{
	if (m_pLexicon)
		delete m_pLexicon;
}

bool ThaiBreakIterator::InitializeLexicon()
{
	if (m_pLexicon)
		delete m_pLexicon;
	m_pLexicon = new ThaiLexicon;
//	m_pLexicon->fLoadLexicon((unsigned char*)"ftwrk.lex");
	m_pLexicon->fLoadLexicon((unsigned char*)m_sLexiconFile);
	memcpy(m_rgThaiCharTypeTable, THAICHARTYPETABLE, 256*sizeof(unsigned long));
	if (!m_pLexicon)
		return false;

	return true;
}

#define _DICTIONARY20_

int ThaiBreakIterator::FindThaiWordBreak(const char* szText,unsigned int nStrlen, unsigned char* rgbBrk, unsigned int nMaxBrk, unsigned int uFlags)
{
	unsigned char	*vmOurDict = m_pLexicon->pGetMemDict();
	unsigned char	*pchFst = (unsigned char*)szText;
	unsigned char	*pchLim;
	unsigned char	rgWL[CWL_MAX];			// Buffer of possible word length
	unsigned char	iWL,iBrk,cch;
	int iLoop;
	unsigned char	*pchDictWalk;
	unsigned char	*pchWordWalk/*,*pchWordWalkSav*/;
	DIFFINDEX	cbDiff;
	unsigned char	chDict;
	bool	fCharInfoNotMatch;
	bool	fCanBreakNext;
//	bool	fPrevWordValid = false;
#if defined(_DICTIONARY20_)
	int	nSubStrLen;
	int	iSubLoop;  
#endif
	unsigned long	dwTypeCur;
	unsigned char	chCur;
	unsigned long	THAGROUP,SYMGROUP,ENGGROUP;

	int cchMaxMatch;
//	bool fWordWrap = (uFlags & FTWB_WORDWRAP);
	bool fSeparateSymbol = ((uFlags & FTWB_SEPARATESYMBOL)?true:false);
//	bool fSpeller = ((uFlags & FTWB_SPELLER)?true:false);

	//
	// Validate Dictinary Buffer
	//
	if (!m_pLexicon->pGetMemDict())
	{
		InitializeLexicon();
		vmOurDict = m_pLexicon->pGetMemDict();
		if (!vmOurDict)
			return 0;
	}

	//
	//	Check argument.
	//
	if ((nMaxBrk == 0) || (!pchFst) || (!rgbBrk))
		return 0;
	//
	//	if nStrLen = 0, Find word break point until
	//	end of given string.
	//
	if (nStrlen)
		pchLim = pchFst + nStrlen;
	else
		pchLim = (unsigned char*)(szText + strlen(szText));
	//
	//	Init local variables.
	//
	rgbBrk[0] = iBrk = 0;
	cbDiff.l = 0;
	//
	// Format group
	//
	if (fSeparateSymbol)
	{
		ENGGROUP = XT_ENG;
		SYMGROUP = XT_PUNCT | XT_WRDBEG | XT_WRDEND | XT_SNTEND;
		THAGROUP = XT_THA;
	}
	else
	{
		ENGGROUP = XT_ENG | XT_PUNCT;
		SYMGROUP = XT_WRDBEG | XT_WRDEND | XT_SNTEND;
		THAGROUP = XT_THA;
	}
	//
	//	Loop until end of input string, or word break buffer is full.
	//
	pchWordWalk = pchFst;
	while (	(*pchFst) &&
			(*pchWordWalk) &&
			(pchFst < pchLim) &&
			(iBrk < nMaxBrk))
	{
		rgWL[0] = iWL = cch = 0;
		cbDiff.l = cchMaxMatch = 0;
		fCharInfoNotMatch = fCanBreakNext = false;
		pchDictWalk = vmOurDict;
		pchWordWalk = pchFst;

		//
		//	Loop for get all possible next word length.
		//
LStartLoop:
		while (((chCur=*pchWordWalk)!=NULL) && (!fCharInfoNotMatch))
		{
//			BOOL fParsedMaiYaMok = false;
			dwTypeCur = XCharType(chCur);

			//
			// Special case for 'Dot'
			//
			if ((dwTypeCur & THAGROUP) ||
				((*pchWordWalk == 0x2E) &&
				 (cch != 0)	&&
				 (*(pchWordWalk-1) > 0x7F)))
			{
				goto LParseThaiChar;
			}
			else if (dwTypeCur & ENGGROUP)
			{
				//STORE_CUR_LEN
				if (cch) { goto LStoreWordLen; }
				//PREPARE_BREAK_ARRAY
				if (rgbBrk[iBrk]) {rgbBrk[++iBrk] = 0;}
//				do {
					++pchWordWalk;
					++cch;
//					++pchFst;

//				goto LStoreWordLen;

//					rgbBrk[iBrk] = cch;	//KES CHANGE
//					rgbBrk[++iBrk] = 0;	//
//					cch = 0;			//
//					++pchFst;			//

//				}while ((pchWordWalk < pchLim) &&
//						HaveType(*pchWordWalk,ENGGROUP))/* &&
//						(!HaveType(*pchWordWalk,XT_WRDBEG) || fSeparateSymbol))*/;

//				do {	//KES CHANGE
//					++pchWordWalk;
//					++cch;
//				}while ((pchWordWalk < pchLim) &&
//						HaveType(*pchWordWalk,ENGGROUP))/* &&
//						(!HaveType(*pchWordWalk,XT_WRDBEG) || fSeparateSymbol))*/;

				//COLLECT_SPACE;
				goto LCollectSpace;
			}
			else if (dwTypeCur & SYMGROUP)
			{
//				unsigned long dwSav = dwTypeCur;
				//STORE_CUR_LEN
				if (cch) { goto LStoreWordLen; }
				//PREPARE_BREAK_ARRAY
				if (rgbBrk[iBrk]) {rgbBrk[++iBrk] = 0;}
//				do {
					++pchWordWalk;
					++cch;
//					++pchFst;

//				goto LStoreWordLen;
//					rgbBrk[iBrk] = cch;	//KES CHANGE
//					rgbBrk[++iBrk] = 0;	//
//					++pchFst;			//
//					cch = 0;			//

//				}while ((pchWordWalk < pchLim) &&
//					    HaveType(*pchWordWalk,dwSav));
				//COLLECT_SPACE;
				goto LCollectSpace;
			}
			else if (dwTypeCur & XT_WRDBEG)
			{
				//STORE_CUR_LEN
				if (cch) { goto LStoreWordLen; }
				//PREPARE_BREAK_ARRAY
				if (rgbBrk[iBrk]) {rgbBrk[++iBrk] = 0;}
				do{
					++pchWordWalk;
					++cch;
				}while((pchWordWalk < pchLim) &&
					   HaveType(*pchWordWalk,XT_WRDBEG));
				//GO_NEXTLOOP
				goto LStartLoop;
			}
			else if (dwTypeCur & XT_WRDEND)
			{
				while ((pchWordWalk < pchLim) && (XT_WRDEND & XCharType(*pchWordWalk)))
				{
					++pchWordWalk;
					++cch;
				}
				//COLLECT_SPACE;
				goto LCollectSpace;
			}
			else if (chCur == 0x20)
			{
				++pchWordWalk;
				++cch;
				if ((0xE6 == *pchWordWalk) && 
					(((unsigned long)pchWordWalk > (unsigned long)(szText+1)) && (XCharType(*(pchWordWalk-2)) & XT_THAI)))	/* MAI_YAMOK */
				{
					++pchWordWalk;
					++cch;
				}
LCollectSpace:
				while(*pchWordWalk == 0x20)
				{
					++pchWordWalk;
					++cch;
				}
				//STORE_CUR_LEN
				if (cch) { goto LStoreWordLen; }
			}
			else if (chCur == 0xE6)
			{
				++pchWordWalk;
				++cch;
				//COLLECT_SPACE
				goto LCollectSpace;
			}
			else if (chCur < 0x20)		/* control char */
			{
				//STORE_CUR_LEN
				if (cch) { goto LStoreWordLen; }
				//PREPARE_BREAK_ARRAY
				if (rgbBrk[iBrk]) {rgbBrk[++iBrk] = 0;}
				if ((*pchWordWalk == 0x0D) && 
					(pchWordWalk+1 < pchLim) &&
					(*(pchWordWalk+1) == 0x0A))
				{
					pchWordWalk += 2;
					cch += 2;
				}
				else
				{
					pchWordWalk++;
					cch++;
				}
				//STORE_CUR_LEN
				if (cch) { goto LStoreWordLen; }
			}
			else
			{
				//
				// Collect non-group char
				//
				pchWordWalk++;
				cch++;
				//GO_NEXTLOOP;
				goto LStartLoop;
			}

LParseThaiChar:
			/* No more child node? */
			if (*pchDictWalk & ENDCHILD)
				break;
			/*  Get Index */
			cbDiff.b.b2 = ((*(pchDictWalk++)) & 0x3F);
			cbDiff.b.b1 = *(pchDictWalk++);
			cbDiff.b.b0 = *(pchDictWalk++);

#if defined(_DICTIONARY20_)			
			//
			// Not only 1 char?
			// 
			if (((nSubStrLen = vmOurDict[cbDiff.l]) < MAX_DICT_CTRL_CHAR) &&
				(vmOurDict[cbDiff.l+1] == *pchWordWalk))
			{
				++cbDiff.l;
				for (iSubLoop = 0; iSubLoop < nSubStrLen; iSubLoop++)
				{
					if (vmOurDict[cbDiff.l++] != *pchWordWalk)	// word not match
					{
						fCharInfoNotMatch = true;
						break;
					}
					else
					{
						cch++;
						pchWordWalk++;
						if (vmOurDict[cbDiff.l] < MAX_DICT_CTRL_CHAR)
						{
							if ((vmOurDict[cbDiff.l] & SUBSTRENDWORD) &&
								(!HaveType(*pchWordWalk,XT_ZWIDTH)))
							{
								rgWL[iWL++] = cch;
								rgWL[iWL] = 0;
							}
							if (vmOurDict[cbDiff.l] & SUBSTRHAVECHILD)	// this means end of substr
							{											// next 3 byte will be 'Address'
								pchDictWalk = vmOurDict + cbDiff.l + 1;	// of next node.
								break;	//	goto LFTWBLoop1;
							}
							cbDiff.l++;
						}
					}
				}
			}
			else
#endif // _DICTIONARY20_
			//
			// Char match?
			//
			if ((chDict = vmOurDict[cbDiff.l]) < *pchWordWalk)
			{	
				//do nothing. just for speed
			}
			else if (chDict == *pchWordWalk)
			{
				++cch;
				++pchWordWalk;
				//
				//	Can break word here ?
				//
				if ((vmOurDict[cbDiff.l+1] & ENDWORD) &&
					(!HaveType(*pchWordWalk,XT_ZWIDTH)))
				{
					rgWL[iWL++] = cch;
					rgWL[iWL] = 0;
				}
				//
				//	Move to first child node.
				//
				pchDictWalk = vmOurDict + cbDiff.l + 1;
			}
			else	// chDict > *pchWordWalk
				break; // exit while loop ... same as set fCharInfoNotMatch = true;
		} // while
		
		//
		// is there a possible word length?
		//
#ifndef DONTUSEWEIGHT
		if (iWL)
		{
			int w,i = iWL;
			int wMaxPri, wMaxSnd, wMaxSum, iMax;

			wMaxPri = wMaxSnd = wMaxSum = iMax = 0;

			if (iWL == 1 &&
				0 == GetWeight(vmOurDict,pchFst,pchFst+rgWL[0],pchLim,0))
			{
				wMaxSum = rgWL[0];

				for (i = 1; i < rgWL[iWL-1]; i++)
				{
					w = GetWeight(vmOurDict,pchFst,pchFst+i,pchLim,0);
					if (w + i > wMaxSum)
					{
						rgWL[0] = static_cast<unsigned char>(i);
						iMax = 0;
						break;
					}
				}
			}
			else
			for (i = iWL-1; i>=0;i--)
			{
				w = GetWeight(vmOurDict,pchFst,pchFst+rgWL[i],pchLim,0);
				//
				// Store max weight
				//
				if ((w + rgWL[i] > wMaxSum) ||
					((w + rgWL[i] == wMaxSum) && (max(w,rgWL[i]) <= wMaxPri)))
				{
					//
					// if the weight is equal, choose the pair that have same len
					//
					wMaxSum = max(w,1) + rgWL[i];
					wMaxPri = max(w,rgWL[i]);
					wMaxSnd = wMaxSum - wMaxPri;
					iMax = i;
				}
			}
			
			iLoop = iMax;
			cch = rgWL[iLoop];
		}
#else
		if (iWL)
		{
			unsigned char *pchWordWalk;
			BOOL fTryLastChance = false;
			INT cchLastChance = 0;
			//
			//	Select the properly length.
			//	(longest and can break next word)
			//
			fCanBreakNext = false;
			for(iLoop = iWL-1; (iLoop >= 0 || fTryLastChance) && (!fCanBreakNext); iLoop--)
			{
				cch = 0;
				if (fTryLastChance)
				{
					iLoop = 0;	// iLoop must be 0 while we're in LastChance Loop
					//
					// No more chance?
					//
					if (cchLastChance == 0)
						break;
					//
					// Go ahead!
					//
					pchWordWalk = pchFst + cchLastChance;
				}
				else
				{
					pchWordWalk = pchFst + rgWL[iLoop];
				}
				pchDictWalk = vmOurDict;
				//pchWordWalkSav = NULL;
				if (!(*pchWordWalk))
				{
					fCanBreakNext = true;
					break;
				}
				fCharInfoNotMatch = false;
				while ((*pchWordWalk) && (!fCanBreakNext) && (!fCharInfoNotMatch))
				{
					//
					//	Can not break next word...
					//
					if (*pchDictWalk & ENDCHILD)
					{
							//
							// Without this it may be forever loop also.
							//
							if (fTryLastChance && cchLastChance > 0)
								cchLastChance--;
						break;
					}
					//
					// get next TCB node's address.
					//
					cbDiff.b.b2 = ((*(pchDictWalk++)) & 0x3F);
					cbDiff.b.b1 = *(pchDictWalk++);
					cbDiff.b.b0 = *(pchDictWalk++);

#ifdef	_DICTIONARY20_
					//
					// Not only 1 char?
					// 
					if (((nSubStrLen = vmOurDict[cbDiff.l]) < MAX_DICT_CTRL_CHAR) &&
						(vmOurDict[cbDiff.l+1] == *pchWordWalk))
					{
						++cbDiff.l;
						for (iSubLoop = 0; iSubLoop < nSubStrLen; iSubLoop++)
						{
							if (vmOurDict[cbDiff.l++] != *pchWordWalk)	// word not match
							{
								fCharInfoNotMatch = true;
								if (fTryLastChance && cchLastChance > 0)
									--cchLastChance;
								break;
							}
							else
							{
								cch++;
								pchWordWalk++;
								if (vmOurDict[cbDiff.l] < MAX_DICT_CTRL_CHAR)
								{
									if (vmOurDict[cbDiff.l] & SUBSTRENDWORD)
			 						{
										if (fTryLastChance)
										{
											if (rgWL[iWL-1] < cch || !*pchWordWalk)
											{
												//
												// Ok!!!! We got it!!!
												// Change the first item of WordLength
												// and assume it's correct break position
												//
												rgWL[0] = cchLastChance; 
												fCanBreakNext = true;
											}
										}
										else
											fCanBreakNext = true;
									}
									if (vmOurDict[cbDiff.l] & SUBSTRHAVECHILD)	// this means end of substr
									{											// next 3 byte will be 'Address'
										pchDictWalk = vmOurDict + cbDiff.l + 1;	// of next node.
										break;	//	goto LFTWBLoop1;
									}
									cbDiff.l++;
								}
							}
						}
					}
					else
#endif	//_DICTIONARY20_
					//
					//	Compare char
					//
					if ((chDict = vmOurDict[cbDiff.l]) < *pchWordWalk)
					{
					}
					else if (chDict == *pchWordWalk)
					{
						++cch;
						++pchWordWalk;
						//
						//	Can break word ?
						//
						if (vmOurDict[cbDiff.l+1] & ENDWORD)
						{
							if (fTryLastChance)
							{
								if (rgWL[iWL-1] < cch || !*pchWordWalk)
								{
									//
									// Ok!!!! We got it!!!
									// Change the first item of WordLength
									// and assume it's correct break position
									//
									rgWL[0] = cchLastChance; 
									fCanBreakNext = true;
								}
							}
							else
								fCanBreakNext = true;
						}
						//
						//	Move to first child node.
						//
						pchDictWalk = vmOurDict + cbDiff.l + 1;
						if (!*pchWordWalk && fTryLastChance && (cchLastChance > 0))
							cchLastChance--;
					}
					else // cbDict > *pchWord
					{
						//
						// If this is last chance to break the word that can also 
						// match the next word. (iLoop = 0) 
						// Than, Can we this card the first word for the next word that longer?
						// (If it is, ofcause) :)
						//
						if (iLoop == 0)
						{
							if (!fTryLastChance)
							{
								fTryLastChance = true;
								cchLastChance = rgWL[0]-1;
							}
							else
							if (cchLastChance > 0)
							{
								cchLastChance--;
							}
							else
							{
								// we tried.
								fTryLastChance = false;
							}
						}
						fCharInfoNotMatch  = true;
						break;
					}
				}//while
				if (fCanBreakNext)
					break;	// exit for loop;
			}
			//
			// Cannot break next word, then select longest length.
			//
			if (!fCanBreakNext)
				iLoop = iWL-1;
LCanBreakNext:
			cch = rgWL[iLoop];
		}
#endif	// dontuseweight
		else
		{
			cch = 1;
		}
		//
		// Store wordbreak position;
		//
		if (cch == 1)
		{
			if (HaveType(*pchFst,XT_THA) &&
				((pchFst + 1) < pchLim) &&
				(*(pchFst+1) != KARAN))
			{
				pchWordWalk = pchFst+1;
				while ((pchWordWalk < pchLim) && HaveType(*pchWordWalk,XT_ZWIDTH|XT_FVOW))
				{
					++pchWordWalk;
					++cch;
				}
			}
			else if((*pchFst == 0x0D) && 
					((pchFst + 1) < pchLim) && 
					(*(pchFst+1) == 0x0A))
			{
				cch = 2;
			}
		}
LStoreWordLen:
		if (((unsigned long)pchFst > (unsigned long)szText) &&		/* not a first char */
			fCanCat(*(pchFst-1),*pchFst,uFlags))
		{
			if (iBrk && rgbBrk[iBrk] == 0)
				rgbBrk[iBrk-1] = rgbBrk[iBrk-1] + cch;
			else
				rgbBrk[iBrk] = rgbBrk[iBrk] + cch;
		}
		else
		if (
			(!HaveType(*pchFst,XT_ENG)) && /* not eglish */	//KES CHANGE
		    ((unsigned long)pchFst > (unsigned long)szText) &&								/* not the first char */
			(XCharType(*(pchFst-1)) & XCharType(*pchFst)) &&	/* type can mearge */
			(
			 (
			  (cch == 1) &&
			  (!(HaveType(*pchFst,XT_WRDBEG))) &&
			  (!(HaveType(*pchFst,XT_LVOW) && ((pchFst + 1) < pchLim) && HaveType(*(pchFst+1),XT_THA)))
			 ) 
			 ||
			 (
			  (rgbBrk[iBrk] == 1) || 
			  (iBrk && (rgbBrk[iBrk] == 0) && (rgbBrk[iBrk-1] == 1))
			 )
			)
		   )
		{
			if (rgbBrk[iBrk])
				rgbBrk[iBrk] = rgbBrk[iBrk] + cch;
			else
				rgbBrk[iBrk-1] = rgbBrk[iBrk-1] + cch;
		}
		else
		{
			if (rgbBrk[iBrk])
				++iBrk;
			rgbBrk[iBrk] = cch;
		}

		pchFst += cch;
		pchWordWalk = pchFst;
		
		if (iBrk == nMaxBrk)
		{
			rgbBrk[iBrk] = 0;		// there are no room to store word break
			break;
		}
	}
	
	if (rgbBrk[iBrk])		// length of last word in given string
		iBrk++;
	rgbBrk[iBrk] = 0;		// close array

	return iBrk;
}

int ThaiBreakIterator::GetWeight(unsigned char* vmOurDict, unsigned char* szText,unsigned char* pchFst,unsigned char* pchLim, unsigned long dwFlags)
{
	unsigned char	*pchWordWalk;
	unsigned char	*pchDictWalk;
//	bool	fTryLastChance = false, 
	bool fCharInfoNotMatch = false;
//	int	cchLastChance = 0;
	DIFFINDEX	cbDiff;
	unsigned char	chDict;
	int	weight = 0;
	int	nSubStrLen;
	int	cch = 0;
	//
	// End of string
	//
	if (!(*pchFst)) return 1000;	/* maximum weight */
	//
	// Initialize
	//
	pchWordWalk = pchFst;
	pchDictWalk = vmOurDict;
	cbDiff.l = 0;
	//	pchWordWalkSav = NULL;

	//
	// Loop thru end of string or get the longest len
	//
	while ( (*pchWordWalk) && 
			(pchWordWalk < pchLim) &&
			(!fCharInfoNotMatch))
	{
		*pchWordWalk;
		//
		//	Can not break next word...
		//
		if (*pchDictWalk & ENDCHILD)	/*  */
		{
			weight = cch;
			break;
		}

		//
		// get next node's address.
		//
		cbDiff.b.b2 = ((*(pchDictWalk++)) & 0x3F);
		cbDiff.b.b1 = *(pchDictWalk++);
		cbDiff.b.b0 = *(pchDictWalk++);

		//
		// Not only 1 char?
		// 
		if (((nSubStrLen = vmOurDict[cbDiff.l]) < MAX_DICT_CTRL_CHAR) &&
			(vmOurDict[cbDiff.l+1] == *pchWordWalk))
		{
			int iSubLoop;
			
			++cbDiff.l;
			
			for (iSubLoop = 0; iSubLoop < nSubStrLen; iSubLoop++)
			{
				if (vmOurDict[cbDiff.l++] != *pchWordWalk)	// word not match
				{
					fCharInfoNotMatch = true;
					break;
				}
				else
				{
					cch++;
					pchWordWalk++;
					if (vmOurDict[cbDiff.l] < MAX_DICT_CTRL_CHAR)
					{
						if (vmOurDict[cbDiff.l] & SUBSTRENDWORD)
							weight = cch;

						if (vmOurDict[cbDiff.l] & SUBSTRHAVECHILD)	// this means end of substr
						{											// next 3 byte will be 'Address'
							pchDictWalk = vmOurDict + cbDiff.l + 1;	// of next node.
							break;	//	goto LFTWBLoop1;
						}
						cbDiff.l++;
					}
				}
			}
		}
		else
		//
		//	Compare char
		//
		if ((chDict = vmOurDict[cbDiff.l]) < *pchWordWalk)
		{
		}
		else if (chDict == *pchWordWalk)
		{
			++cch;
			++pchWordWalk;
			//
			//	Can break word ?
			//
			if (vmOurDict[cbDiff.l+1] & ENDWORD)
				weight = cch;
			//
			//	Move to first child node.
			//
			pchDictWalk = vmOurDict + cbDiff.l + 1;
		}
		else // cbDict > *pchWord
		{
			fCharInfoNotMatch  = true;
			break;
		}
	}//while
	
	return (weight);
}

bool ThaiBreakIterator::fCanCat(unsigned char chPrev, unsigned char ch, unsigned int uFlags)
{
	//
	// If the prev char is a break char, that's it!
	// It's break pos, cannot cat any char to it.
	// (except breakchar)
	//

	//KES CHANGE
	if ( HaveType(ch,XT_ENG | XT_ALLSYMBOL ) )
		return false;

//	if (FBreakChar(chPrev))
//		return FBreakChar(ch);
	//
	// Non stop?
	//
	if (FNonbreakChar(chPrev))
		return true;

	//
	// Mode independence case
	//
	if ( HaveType(chPrev,XT_THA) && 
		HaveType(ch,XT_FVOW | XT_ZWIDTH))
		return true;

	if (ch == ' ')
		return true;
	else if (chPrev == ' ')
		return false;

	// N_Y_I

	//
	// Look at the mode or option of wordbreak proc
	//
	// SeparateSymbol :: mean symbol can't cat to anytype of
	// char except symbol type. Normally use this option for
	// caret movement.
	//
	if (uFlags & FTWB_SEPARATESYMBOL)
	{
		// Not symbol both
		if ((HaveType(chPrev,XT_ALLSYMBOL)?1:0) != (HaveType(ch,XT_ALLSYMBOL)?1:0))
			return false;

		return (
				(HaveType(chPrev,XT_LVOW) && HaveType(ch,XT_THA)) ||
				(HaveType(chPrev,XT_ALLSYMBOL) && HaveType(ch,XT_ALLSYMBOL)) ||
				(HaveType(chPrev,XT_ENG) && HaveType(ch,XT_ENG))
			   );
	}
	//
	// WordWrap :: use this option only for word wrapping. Such as when
	// user resize the windows.
	//
	else // if (uFlag & FTWB_WORDWRAP)
	{
		if (HaveType(ch,XT_WRDBEG) && !HaveType(chPrev,XT_WRDBEG))
			return false;

		return ( 
			HaveType(ch,XT_WRDEND | XT_SNTEND) ||
			HaveType(ch,XT_SNTEND) ||
			HaveType(chPrev,XT_WRDBEG) ||
			(HaveType(chPrev,XT_LVOW) && HaveType(ch,XT_THA)) ||
			(HaveType(chPrev,XT_PUNCT) && HaveType(ch,XT_ENG|XT_DIGIT)) ||
			HaveType(ch,XT_PUNCT)
			   );
	}
	//
	// default
	//
//	return false;
}

bool ThaiBreakIterator::HaveType(char ch, unsigned long IsType)
{
#ifdef _DEBUG
	unsigned long uType = m_rgThaiCharTypeTable[(unsigned char)ch];
	bool bReturn = ((uType & IsType)?true:false);
	return bReturn;
#else
	return 0 < (m_rgThaiCharTypeTable[(unsigned char)ch] & IsType);
#endif
}

unsigned long	ThaiBreakIterator::XCharType(char ch)
{
#ifdef _DEBUG
	unsigned long nReturn = m_rgThaiCharTypeTable[(unsigned char)ch];
	return nReturn;
#else
	return 0 < m_rgThaiCharTypeTable[(unsigned char)ch];
#endif
}
