#ifndef __FTP_PROTOCOL__
#define __FTP_PROTOCOL__

const static int CONNECT_ERROR				= 150;
const static int OPEN_CONNECT_FOR_DIRLIST	= 125;
const static int CHANGE_TYPE_SUCCESS		= 200;
const static int TARGET_FILE_SIZE			= 213;
const static int WELCOME_MESSAGE			= 220;
const static int TRANSFER_COMPLETE			= 226;
const static int ENTER_PASSIVE_MODE			= 227;
const static int LOGIN_SUCCESS				= 230;
const static int SUCCESS_COMMAND			= 250;
const static int PRESENT_WORKING_DIR		= 257;
const static int PASSWORD_REQUIRED			= 331;
const static int WAIT_NEXT_COMMAND			= 350;

const static int UNKNOWN_COMMAND			= 500;
const static int COMMAND_ERROR				= 501;
const static int NO_FILES_FOUND				= 550;

#endif
