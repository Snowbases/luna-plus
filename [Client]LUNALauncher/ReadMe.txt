-------------------------------------------------- -------------------------------------------------
Luna launcher (06/13/2007) yiungju

Initial Developer and gangeunseong modify it uses MFC and CxImage static library (DLL Not required)

- CxImage 5.99c.
- To reduce the file size of the library is to be used only TIFF file to write also been removed. Change Settings
ximcfg.h.


? Reference:

Linking launcher to compile a static version of the library, CxImage

1 To unpack from the launcher to use the open-source is used to decompress the image of Used throughout string of copyright
LNK2005 error occurs because the same variable name, linking does not. Zlib from _deflate_copyright included in the CxImage project
Remove and _inflate_copyright (cant use does not).

2 CxImage open a solution, Zlib, JPEG, TIFF projects compiled by the properties of the Unicode project and give the following options to compile.
... General> Configuration Type: Static Library, General> MFC used: static library
... Release - Code Generation> Runtime Library: Debug Multi-threaded Debug (/ MTd), a multi-threaded (/ MT)

3 Above has been compiled DLL labeled ones (suffix) library> Additional Dependencies but CxImage compile zlib.lib, jpeg.lib, tiff.lib
Compiled and entered. Warning is ignored.

4 Linking lib files in the launcher now generated DLL does not use.


-------------------------------------------------- -------------------------------------------------
Unicode (01/22/2008) yiungju


setting.cfg on the location of the controls, as well as text are extracted. Thai letters corresponding to ANSI format
When loading the files stored on an error occurs. The project is multi-byte, DLL it is. Also
Multi-byte, multi-lingual work by former mukhyang code it in a short period of time to fix a difficult task.

Fortunately, the working class as CStdioFileEx UNICODE / ANSI regardless of the file to read can be processed.

The corresponding class of worker and more information, refer to the following path:

http://www.ucancode.net/faq/CStdioFile-MFC-Example-ReadString-WriteString.htm

As follows: If a compile can handle multilingual.

? setting.cfg of text to change the country
? font_name to set the font for your country. Possible to specify the Windows default font are (Thai AngsanaUPC)
? file format is Unicode and saves


-------------------------------------------------- -------------------------------------------------
HTTP / FTP protocol, available for download (05/15/2008) yiungju

Libcurl using Curl 7.18.1. HTTP / FTP, regardless of available The launcher execution path must
libcurl.dll should include


-------------------------------------------------- -------------------------------------------------
Unicode (15/05/2008) yiungju

[Lib] ZipArchive of the project to compile, but Static_Debug / Release the string set [Unicode] setting.

cximage600 (full).
CxImage project properties compile Unicode debug / release is as follows. zlib, jpeg, tiff, CxImage
Get the library files compiled for each. xImage.cfg edit the format that does not use a turn off, do not forget.
... General> Configuration Type: Static Library, General> MFC used: static library
... Release - Code Generation> Runtime Library: Debug Multi-threaded Debug (/ MTd), a multi-threaded (/ MT)

Tiff warnings directly in the project file after clearing the TIFF file for warnings annoying enough common
Let's compile again.


-------------------------------------------------- -------------------------------------------------
Way to change the download path set (05/15/2008) yiungju

determined system \ \ setting.cfg local constant string by changing the loading path to download and patch file names.
Path is included in the value of the source (CApplication :: SetLocal ()). The following values ??are currently set.

korea - South Korea
korea.test - Korea test server
thai - Thailand


-------------------------------------------------- -------------------------------------------------
Log display (15/05/2008) yiungju

If you run with the / g option LunaLauncher log window is displayed, log LunaLauncher are stored in the log file [today's date].


-------------------------------------------------- -------------------------------------------------
libcurl.dll runtime linking (06/25/2008) yiungju

This file may be libcurl.dll compressed file during runtime, so you can see the on / off.


-------------------------------------------------- -------------------------------------------------
Resolution sensing (12/21/2009) yiungju

Clearly mark on the launcher, to be selected results support the resolution of the video card recognized.
To use this feature, set the system \ setting.cfg flag_detect_resolution = true
DirectX8 code was used in order to maintain backward compatibility