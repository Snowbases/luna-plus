#pragma once

class CSimpleTextListCtrl
{
	typedef std::map< int, std::string > TEXTMAP;		// col
	std::map< int, TEXTMAP > m_textList;				// row

	int m_maxColumn;

public:
	CSimpleTextListCtrl(void);
	~CSimpleTextListCtrl(void);

	void SetItemText( int row, int col, LPCTSTR text );
	LPCTSTR GetItemText( int row, int col ) const;

	int GetRowCount() const;
	int GetColCount() const;
};
