# Microsoft Developer Studio Project File - Name="povray" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=povray - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "povray.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "povray.mak" CFG="povray - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "povray - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "povray - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "povray - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../.." /I "../../zlib" /I ".." /I "../../libpng" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 bin\libpng.lib bin\zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "povray - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../.." /I "../../zlib" /I ".." /I "../../libpng" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_VRML" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 bin\vrmlin.lib bin\vrmlExt.lib comctl32.lib bin\libpng.lib bin\zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/pvengine.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "povray - Win32 Release"
# Name "povray - Win32 Debug"
# Begin Source File

SOURCE=..\..\ATMOSPH.C
# End Source File
# Begin Source File

SOURCE=..\..\BBOX.C
# End Source File
# Begin Source File

SOURCE=..\..\BCYL.C
# End Source File
# Begin Source File

SOURCE=..\..\BEZIER.C
# End Source File
# Begin Source File

SOURCE=..\..\BLOB.C
# End Source File
# Begin Source File

SOURCE=..\..\BOXES.C
# End Source File
# Begin Source File

SOURCE=..\..\BSPHERE.C
# End Source File
# Begin Source File

SOURCE=..\..\CAMERA.C
# End Source File
# Begin Source File

SOURCE=..\..\CHI2.C
# End Source File
# Begin Source File

SOURCE=..\..\COLOUR.C
# End Source File
# Begin Source File

SOURCE=..\..\CONES.C
# End Source File
# Begin Source File

SOURCE=..\..\CSG.C
# End Source File
# Begin Source File

SOURCE=..\..\DISCS.C
# End Source File
# Begin Source File

SOURCE=..\..\EXPRESS.C
# End Source File
# Begin Source File

SOURCE=..\..\FRACTAL.C
# End Source File
# Begin Source File

SOURCE=..\..\GIF.C
# End Source File
# Begin Source File

SOURCE=..\..\GIFDECOD.C
# End Source File
# Begin Source File

SOURCE=..\..\HALOS.C
# End Source File
# Begin Source File

SOURCE=..\..\HCMPLX.C
# End Source File
# Begin Source File

SOURCE=..\..\HFIELD.C
# End Source File
# Begin Source File

SOURCE=..\..\IFF.C
# End Source File
# Begin Source File

SOURCE=..\..\IMAGE.C
# End Source File
# Begin Source File

SOURCE=..\..\LATHE.C
# End Source File
# Begin Source File

SOURCE=..\..\LBUFFER.C
# End Source File
# Begin Source File

SOURCE=..\..\LIGHTING.C
# End Source File
# Begin Source File

SOURCE=..\..\MATRICES.C
# End Source File
# Begin Source File

SOURCE=..\..\MEM.C
# End Source File
# Begin Source File

SOURCE=..\..\MESH.C
# End Source File
# Begin Source File

SOURCE=..\..\NORMAL.C
# End Source File
# Begin Source File

SOURCE=..\..\OBJECTS.C
# End Source File
# Begin Source File

SOURCE=..\..\OCTREE.C
# End Source File
# Begin Source File

SOURCE=..\..\OPTIN.C
# End Source File
# Begin Source File

SOURCE=..\..\OPTOUT.C
# End Source File
# Begin Source File

SOURCE=..\..\PARSE.C
# End Source File
# Begin Source File

SOURCE=..\..\PARSTXTR.C
# End Source File
# Begin Source File

SOURCE=..\..\PATTERN.C
# End Source File
# Begin Source File

SOURCE=..\..\PGM.C
# End Source File
# Begin Source File

SOURCE=..\..\PIGMENT.C
# End Source File
# Begin Source File

SOURCE=..\..\PLANES.C
# End Source File
# Begin Source File

SOURCE=..\..\PNG_POV.C
# End Source File
# Begin Source File

SOURCE=..\..\POINT.C
# End Source File
# Begin Source File

SOURCE=..\..\POLY.C
# End Source File
# Begin Source File

SOURCE=..\..\POLYGON.C
# End Source File
# Begin Source File

SOURCE=..\..\POLYSOLV.C
# End Source File
# Begin Source File

SOURCE=..\..\POVRAY.C
# End Source File
# Begin Source File

SOURCE=..\..\PPM.C
# End Source File
# Begin Source File

SOURCE=..\..\PRISM.C
# End Source File
# Begin Source File

SOURCE=..\PVBITMAP.C
# End Source File
# Begin Source File

SOURCE=..\PVBMP.C
# End Source File
# Begin Source File

SOURCE=..\PVCLIENT.C
# End Source File
# Begin Source File

SOURCE=..\PVDIALOG.C
# End Source File
# Begin Source File

SOURCE=..\PVENGINE.C
# End Source File
# Begin Source File

SOURCE=..\PVENGINE.RC

!IF  "$(CFG)" == "povray - Win32 Release"

!ELSEIF  "$(CFG)" == "povray - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PVFILES.C
# End Source File
# Begin Source File

SOURCE=..\PVGUIEXT.C
# End Source File
# Begin Source File

SOURCE=..\PVMENU.C
# End Source File
# Begin Source File

SOURCE=..\PVMISC.C
# End Source File
# Begin Source File

SOURCE=..\PVTEXT.C
# End Source File
# Begin Source File

SOURCE=..\PVTINY.C
# End Source File
# Begin Source File

SOURCE=..\..\QUADRICS.C
# End Source File
# Begin Source File

SOURCE=..\..\QUATERN.C
# End Source File
# Begin Source File

SOURCE=..\..\RAD_DATA.C
# End Source File
# Begin Source File

SOURCE=..\..\RADIOSIT.C
# End Source File
# Begin Source File

SOURCE=..\..\RAY.C
# End Source File
# Begin Source File

SOURCE=..\..\RENDER.C
# End Source File
# Begin Source File

SOURCE=..\..\SOR.C
# End Source File
# Begin Source File

SOURCE=..\..\SPHERES.C
# End Source File
# Begin Source File

SOURCE=..\..\SUPER.C
# End Source File
# Begin Source File

SOURCE=..\..\TARGA.C
# End Source File
# Begin Source File

SOURCE=..\..\TEXTURE.C
# End Source File
# Begin Source File

SOURCE=..\..\TOKENIZE.C
# End Source File
# Begin Source File

SOURCE=..\..\TORUS.C
# End Source File
# Begin Source File

SOURCE=..\..\TRIANGLE.C
# End Source File
# Begin Source File

SOURCE=..\..\TRUETYPE.C
# End Source File
# Begin Source File

SOURCE=..\..\TXTTEST.C
# End Source File
# Begin Source File

SOURCE=..\..\USERIO.C
# End Source File
# Begin Source File

SOURCE=..\..\VBUFFER.C
# End Source File
# Begin Source File

SOURCE=..\..\VLBUFFER.C
# End Source File
# Begin Source File

SOURCE=..\..\WARPS.C
# End Source File
# End Target
# End Project
