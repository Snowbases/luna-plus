07/10/22 yiungju


Process:

1 RM server to run the DB server is located wherever
2 Everywhere RM client connect to the server. RM tools for accessing the login information should TB_RECOVERY_MANAGER
Otherwise, the Agent, such as centralized management server requires difficult summit.
3 Client on each server. The client, the server information is stored in the script file. If you tell the server Login failed.


Grid control tips
1 READ ONLY handle of the cell
Note CGridCtrl :: SetItem (GV_ITEM *) from GV_ITEM.nState not properly apply.
After setting the value using CGridCtrl :: SetItemState () should operate.


07/12/10 yiungju

To replace the GIF image
BIN, change the file name, Visual Studio, you can drag to copy the contents of the binary.
After you copy the entire is open, simply copy the appropriate resources.


08/01/21

Localizing Resources

Let's work based client.rc. The English version, based on this file. Client_english.rc
When a new resource is added, do not forget the tools to change history, WinMerge (easy to understand)
Using additional resources to allow updates. Debug version should not be localized.
Excluded from the build to debug by right-clicking on the file, select Properties, and then select the
Resources the resource that you want to debug and select 'No', otherwise select 'yes'

Internal module that does not support Unicode good, but when you apply.


08/09/01

Release at compile time, you may encounter the following error

C: \ Program Files \ Microsoft Visual Studio. NET 2003 \ Vc7 \ atlmfc \ include \ afxwin2.inl (1037): error C2039: 'Enable3dControlsStatic': 'CWinApp' members are not.


Clear if the column defined afxwin2.inl to resolve this error.

# Ifndef _AFXDLL
_AFXWIN_INLINE BOOL CWinApp :: Enable3dControlsStatic ()
{Return TRUE;}
# Endif


Source is as follows.