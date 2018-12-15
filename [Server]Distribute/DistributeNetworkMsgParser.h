
#ifndef __DISTRIBUTENETWORKMSGPARSER_H__
#define __DISTRIBUTENETWORKMSGPARSER_H__
void MP_POWERUPMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_DISTRIBUTESERVERMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_USERCONNMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_CHATROOMMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength) ;

//void MP_MORNITORTOOLMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);
void MP_MonitorMsgParser(DWORD dwConnectionIndex, char* pMsg, DWORD dwLength);

// 070207 LYW --- Add functions to process network msg.
/// MONITORMSGPARSER PART
void MornitorMapServer_NoticeSend_Syn() ;
void MornitorMapServer_Ping_Syn( DWORD dwConnectionIndex, MSGROOT* pTempMsg, char* pMsg, DWORD dwLength ) ;
void MornitorMapServer_Change_UserLevel_Syn( DWORD dwConnectionIndex, MSGROOT* pTempMsg, char* pMsg, DWORD dwLength ) ;
void MornitorMapServer_QueryUserCount_Syn( DWORD dwConnectionIndex, MSGROOT* pTempMsg, char* pMsg ) ;
void MornitorMapServer_AssertMsgBox_Syn( char* pMsg ) ;
void MornitorMapServer_ServerOff_Syn() ;
void MornitorMapServer_Query_Version_Syn(DWORD dwConnectionIndex) ;
void MornitorMapServer_Change_Version_Syn( DWORD dwConnectionIndex, char* pMsg ) ;
void MornitorMapServer_Query_MaxUser_Syn(DWORD dwConnectionIndex) ;
void MornitorMapServer_Change_MaxUser_Syn( DWORD dwConnectionIndex, char* pMsg ) ;

/// USERCONNMSGPARSER PART
void Userconn_Login_Dynamic_syn(DWORD dwConnectionIndex, char* pMsg) ;
void Userconn_Login_Syn(DWORD dwConnectionIndex, char* pMsg) ;
void Userconn_Force_Disconnect_OverlapLogIn(DWORD dwConnectionIndex) ;
void Userconn_Notify_UserLogIn_Ack(DWORD dwConnectionIndex, char* pMsg) ;
void Userconn_Notify_UserLogIn_Nack(DWORD dwConnectionIndex, char* pMsg) ;
void Userconn_ReQuest_DistOut(DWORD dwConnectionIndex) ;
void Userconn_Connection_Check_Ok(DWORD dwConnectionIndex) ;

#endif //__DISTRIBUTENETWORKMSGPARSER_H__