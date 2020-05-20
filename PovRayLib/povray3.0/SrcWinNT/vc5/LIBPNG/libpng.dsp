# Microsoft Developer Studio Project File - Name="libpng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libpng - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak" CFG="libpng - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libpng - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "libpng - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\..\..\zlib" /I "..\..\.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_VRML" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libpng - Win32 Release"
# Name "libpng - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNG.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNG.H
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGCONF.H
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGERROR.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGMEM.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGPREAD.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGRCB.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGREAD.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGRIO.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGRTRAN.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGRUTIL.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGTRANS.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGWIO.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGWRITE.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGWTRAN.C
# End Source File
# Begin Source File

SOURCE=..\..\..\LIBPNG\PNGWUTIL.C
# End Source File
# End Target
# End Project
