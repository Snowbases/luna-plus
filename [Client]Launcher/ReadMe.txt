================================================== =================================================
Luna Launcher

Date: 13/06/2007
By: yiungju


Developer (first version): gangeunseong
Developer: yiungju additional functions) (CxImage use, skin


Uses MFC and CxImage static library (DLL)


? notification:

- CxImage 5.99c.

- To reduce the file size of the library is to be used only TIFF file to write also been removed. Change Settings
ximcfg.h.



? Reference:

Linking launcher to compile a static version of the library, CxImage

1 To unpack from the launcher to use the open-source is used to decompress the image of Used throughout string of copyright
LNK2005 error occurs because the same variable name, linking does not. Zlib from _deflate_copyright included in the CxImage project
Remove and _inflate_copyright (cant use does not).


2 CxImage open a solution, Zlib, JPEG, TIFF projects to give the following options to compile.

... General> Configuration Type: Static Library, General> MFC used: static library


3 Above has been compiled DLL labeled ones (suffix) library> Additional Dependencies but CxImage compile zlib.lib, jpeg.lib, tiff.lib
Compiled and entered. Warning is ignored.

4 Linking lib files in the launcher now generated DLL does not use.


================================================== =================================================
FTP related corrective actions
Modified: 04/10/2007

Worker: yiungju

? slow transfer speeds
(Formerly 1K) are temporarily stored in the socket buffer size is too small to result in a delay in the processing. We extend the maximum size of the buffer (currently 100K).
However, keep in mind that the buffer can not be blindly greatly. If you specify the size of the stack frame beyond _chkstk ()
Stack overflow exception will occur.


? fixed file size exceeds the transfer occurs
Receive in the form of a binary file transfer by solving

/ / TRUE value flag is a binary form.
(Example) g_pFtp-> SetupExtendDownload (temp2, "\ \", TRUE);


================================================== =================================================
Working with Unicode
Modified: 01/22/2008

Worker: yiungju


setting.cfg on the location of the controls, as well as text are extracted. Thai letters corresponding to ANSI format
When loading the files stored on an error occurs. The project is multi-byte, DLL it is. Also
Multi-byte, multi-lingual work by former mukhyang code it in a short period of time to fix a difficult task.

Fortunately, the working class as CStdioFileEx UNICODE / ANSI regardless of the file to read can be processed
Was

The corresponding class of worker and more information, refer to the following path:

http://www.ucancode.net/faq/CStdioFile-MFC-Example-ReadString-WriteString.htm

As follows: If a compile can handle multilingual.

? setting.cfg of text to change the country
? font_name to set the font for your country. Possible to specify the Windows default font are (Thai AngsanaUPC)
? file format is Unicode and saves



================================================== =================================================
Sent from the proxy server can be modified to
Modified: 02/14/2008

Worker: yiungju

Phenomenon:
Thailand's four cafés school similar to (PC), rather than the company's FTP proxy server (Linux based frox program called ftp proxy
Uses) to download files via reducing the throughput. However, Luna and mukhyang connection
Possible to download one file fails

Analysis:
FTP to transfer files to open a separate socket. In active mode, PORT, PASV passive mode through
Address to receive a port to connect to. PORT is available, so if you may be exposed to hacker attacks, however, most of the
From the FTP server is prohibited. If the proxy server in the middle of a file transfer proxy server to open a socket
Should be performed. However, the existing code only for PASV earned Portman, IP address, FTP address
Was used. Opened a socket to the proxy server, FTP server proxy Beyond open socket for client
Because it was, of course, the transfer will fail.

Resolution:
Solved received PASV command using the IP address and port number to a socket connection.

See also:
with TYPE = AN frox looking at the logs, there are cases This is when files are transferred in ASCII mode parameters
Objection whether the attached file, it does not affect the transmission. Anyway, in binary form to receive the FTP TYPE I command
Is sent to the server. Can be transmitted using CComandSocket :: SetTransferType ()