# Microsoft Developer Studio Project File - Name="zlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "zlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_VRML" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "zlib - Win32 Release"
# Name "zlib - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\ZLIB\ADLER32.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\COMPRESS.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\CRC32.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\DEFLATE.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\GZIO.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\INFBLOCK.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\INFCODES.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\INFFAST.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\INFLATE.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\INFTREES.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\INFUTIL.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\MAKE_VMS.COM
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\TREES.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\UNCOMPR.C
# End Source File
# Begin Source File

SOURCE=..\..\..\ZLIB\ZUTIL.C
# End Source File
# End Target
# End Project
