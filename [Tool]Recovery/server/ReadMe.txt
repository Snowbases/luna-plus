07/10/25 yiungju

Overview:

Remodeling an existing monitoring server program. As a result, a significant number of existing monitoring programs remain


08/03/06

Automating the build number

See also: http://www.codeproject.com/KB/dotnet/build_versioning.aspx

Using the pre-build event, after the event server to automate the build number.
The client can refer to a constant at compile-time, open the file version.h is
Through it to the server and the version comparison.

Changed, the client server version must be an unconditional again compiled. Server variability synchronization
Is to match the In order to prevent frequent server changes, the client and the server, the major number, minor number]
Comparisons. Build number should be ignored. To compile the server if the client's change history
Should not even need to recompile.

When you compile the client [build number] only counting when you compile the server, and the minor number is counting.

Program version information VerCopy.exe to extract and copy the
VerHeader.exe header file is updated
Update of RCStamp.exe resource file FILEVERSION
VerCopyFromHeader.exe header file version by extracting information from the copy

08/03/25

Build number for automated removal

Due to source management tools, such as automated build number Build before / after the event to go as the file is read-only
Without the normal processing state, the developers also can not recognize. Such version.h manually change
To that


08/10/13

DB owners to DBO -> dbo. Strict name checking some local (Japan), the owner or the procedure
Name should be matched to the unit case. This batch of changes