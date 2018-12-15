#include <windows.h>


/* DataGrid definitions */
#define DG_MAXGRID          20
#define DG_MAXCOLUMN        1000
#define DG_MAXROW           32000
#define DGTA_LEFT           DT_LEFT
#define DGTA_CENTER         DT_CENTER
#define DGTA_RIGHT          DT_RIGHT
#define DGBGR_COLOR         RGB(255,255,255)
#define DGTXT_COLOR         RGB(0,0,0)
#define DGRONLY_COLOR       RGB(220,220,220)


/* DataGrid messages */
#define DGM_ITEMCHANGED          0x0001
#define DGM_ITEMTEXTCHANGED      0x0002
#define DGM_ITEMADDED            0x0003
#define DGM_ITEMREMOVED          0x0004
#define DGM_COLUMNRESIZED        0x0005
#define DGM_COLUMNCLICKED        0x0006
#define DGM_STARTSORTING         0x0007
#define DGM_ENDSORTING           0x0008


/* Custom callback procedure */
typedef int(CALLBACK *DGCOMPARE)(char* item1, char* item2, int column);


/* DataGrid column definition structure */
typedef struct _DG_COLUMN
{
    char columnText[1024];
    int columnWidth;
    int textAlign;
    bool pressed;
    
} DG_COLUMN;


/* DataGrid row definition structure */
typedef struct _DG_ROW
{
    char** rowText;
    int* textAlign;
    bool selected;
    bool* readOnly;
    COLORREF bgColor;
    
} DG_ROW;


/* DataGrid enumerations */
enum DG_MASK {DG_TEXTEDIT, DG_TEXTALIGN, DG_TEXTHIGHLIGHT, DG_TEXTRONLY, DG_TEXTBGCOLOR};


/* DataGrid item information structure */
typedef struct _DG_ITEMINFO
{
    DG_MASK dgMask;
    int dgItem;
    int dgSubitem;
    char* dgText;
	int dgTextLen;
    int dgTextAlign;
    bool dgSelected;
    bool dgReadOnly;
    COLORREF dgBgColor;
} DG_ITEMINFO;


/* DataGrid definition */
typedef struct _DG_LIST
{
       HWND dg_hWnd;
       HWND dg_hParent;
       DG_COLUMN* dg_Columns;
       DG_ROW* dg_Rows;
       int dg_ColumnNumber;
       int dg_RowNumber;
       int dg_Column;
       int dg_Row;
       int dg_SelectedColumn;
       int dg_SelectedRow;
       DG_ROW* dg_Selected;
       BOOL dg_Cursor;
       BOOL dg_Resize;
       BOOL dg_Click;
       HFONT dg_hColumnFont;
       HFONT dg_hRowFont;
       LOGFONT dg_LFColumnFont;
       LOGFONT dg_LFRowFont;
       HBRUSH dg_hBgBrush;
       HPEN dg_hCellPen;
       HBITMAP dg_hMemBitmap;
       HDC dg_hMemDC;
       HBITMAP dg_hOldMemBitmap;
       HWND dg_hEditWnd;
       RECT dg_EditRect;
       BOOL dg_Edit;
       BOOL dg_EnableEdit;
       BOOL dg_EnableSort;
       BOOL dg_EnableResize;
       BOOL dg_EnableGrid;
       DGCOMPARE dg_CompareFunc;
       COLORREF dg_ColumnTextColor;
       COLORREF dg_RowTextColor;
       struct _DG_LIST* next;
} DG_LIST;


/* DataGrid global procedures */
LRESULT CALLBACK DataGridProc(HWND, UINT, WPARAM, LPARAM);
void DrawColumns(HWND hWnd);
void DrawRows(HWND hWnd);
BOOL CheckColumnResize(HWND hWnd, int x, int y, int* col, RECT* colRect);
BOOL CheckColumnClick(HWND hWnd, int x, int y, int* col);
BOOL CheckRows(HWND hWnd, int x, int y, int* row);
void GetColumnRect(HWND hWnd, int col, RECT* colRect);
void GetRowRect(HWND hWnd, int row, RECT* rowRect);
void RecalcWindow(HWND hWnd);
void SortDGItems(HWND hWnd, int column);
void Sort(HWND hWnd, int col, int size);
void SetDGSelection(HWND hWnd, int rowIndex, int columnIndex);
void SelectNextItem(HWND hWnd, DG_ROW* item);
void SelectPreviousItem(HWND hWnd, DG_ROW* item);
void SelectNextSubitem(HWND hWnd);
void SelectPreviousSubitem(HWND hWnd);
void EnsureRowVisible(HWND hWnd, int rowIndex);
void EnsureColumnVisible(HWND hWnd, int columnIndex);
void EnsureVisible(HWND hWnd, int rowIndex, int colIndex);
void GetCellRect(HWND hWnd, RECT* cellRect);
BOOL GetDGItemText(HWND hWnd, int rowIndex, int columnIndex, char* buffer, int buffer_size);
BOOL SetDGItemText(HWND hWnd, int rowIndex, int columnIndex, char* buffer);
void InitDGGlobals(HWND hParent, HWND hWnd);
BOOL AddDGGrid(HWND hWnd, HWND hParent);
void DestroyDGGrid(HWND hWnd);
DG_LIST* GetDGGrid(HWND hWnd);
DG_LIST* DetachDGGrid(HWND hWnd);
void SiftDown( HWND hWnd, int root, int bottom, int col );


class CDataGrid
{
private:
        /* DataGrid members */
        HWND m_hWnd;
        HWND m_hParentWnd;
        RECT m_DGRect;
        
public:
       /* Basic DataGrid methods */
       CDataGrid();
       virtual ~CDataGrid();
       BOOL Create(RECT wndRect, HWND hParent, int numCols);
       void Resize();
       void Update();
       HWND GetWindowHandle();
       
       /* General DataGrid methods */
       BOOL InsertItem(char* itemText, int textAlign);
       BOOL RemoveItem(int row);
       void RemoveAllItems();
       void EnableSort(BOOL enable);
       void EnableEdit(BOOL enable);
       void EnableResize(BOOL enable);
       void EnableGrid(BOOL enable);
	   void EnsureVisible(int row, int column);
	   void SelectItem(int row, int column);
       int GetResizedColumn();
       int GetRowNumber();
       int GetSelectedRow();
       int GetSelectedColumn();
       void SetCompareFunction(DGCOMPARE CompareFunction);
       
       /* DataGrid SET attribute methods */
       BOOL SetColumnInfo(int columnIndex, char* columnText, int columnWidth, int textAlign);
       void SetColumnTxtColor(COLORREF txtColor);
       void SetColumnFont(LOGFONT* lf);
       BOOL SetItemInfo(int rowIndex, int columnIndex, char* itemText, int textAlign, bool readOnly);
       BOOL SetItemInfo(DG_ITEMINFO* itemInfo);
       void SetItemBgColor(int rowIndex, COLORREF bgColor);
       void SetRowFont(LOGFONT* lf);
       void SetRowTxtColor(COLORREF txtColor);
       
       /* DataGrid GET attribute methods */
       COLORREF GetColumnTxtColor();
       void GetColumnFont(LOGFONT* lf);
       void GetItemInfo(DG_ITEMINFO* itemInfo);
       BOOL GetItemText(int rowIndex, int columnIndex, char* buffer, int buffer_size);
       void GetRowFont(LOGFONT* lf);
       COLORREF GetRowTxtColor();
};
