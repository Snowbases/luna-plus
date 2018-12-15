
#ifndef _cWINDOWSYSTEMFUNC_H_
#define _cWINDOWSYSTEMFUNC_H_

class cIcon;
class cWindow;
class cDialog ;

void cbDragDropProcess(LONG curDragX, LONG curDragY, LONG id, LONG beforeDragX, LONG beforeDragY);
// 070611 LYW --- cWindowSystemFunc : Add function to processing drag and drop of favor item part.
void favorDragDropProcess(LONG curDragX, LONG curDragY, LONG id, LONG beforeDragX, LONG beforeDragY) ;
// 070611 LYW --- cWindowSystemFunc : Add function to return icon.
void ReturnIconToPrevDlg(cDialog* srcDlg, LONG beforeDragX, LONG beforeDragY) ;

void SendDeleteIconMsg(cIcon * pIcon);

void CreateMainTitle_m();
void NewCreateCharSelect_m();
// 061215 LYW --- Add profile dialog for character information.
void CreateProfileDlg() ;
// 061217 LYW --- Delete this code.
//void CreateCharMakeDlg();
// 061214 LYW --- CharMake : Add new function to create dialog for character property.
void CreateCharMakeNewDlg() ;
// 061218 LYW --- Add dialog for certificate some condition.
void CreateCertificateDlg() ;
cWindow * CreateGameLoading_m();
void CreateInitLogo_m();
void CreateMonsterGuageDlg();
void CreateChannelDlg();

#endif // _cWINDOWSYSTEMFUNC_H_
