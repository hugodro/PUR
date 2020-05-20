# Microsoft Developer Studio Project File - Name="libPovRay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libPovRay - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libPovRay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libPovRay.mak" CFG="libPovRay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libPovRay - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libPovRay - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "libPovRay - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
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

!ELSEIF  "$(CFG)" == "libPovRay - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"bin\libPovRay.lib"

!ENDIF 

# Begin Target

# Name "libPovRay - Win32 Release"
# Name "libPovRay - Win32 Debug"
# Begin Source File

SOURCE=..\..\atmosph.c
# End Source File
# Begin Source File

SOURCE=..\..\atmosph.h
# End Source File
# Begin Source File

SOURCE=..\..\bbox.c
# End Source File
# Begin Source File

SOURCE=..\..\bbox.h
# End Source File
# Begin Source File

SOURCE=..\..\bcyl.c
# End Source File
# Begin Source File

SOURCE=..\..\bcyl.h
# End Source File
# Begin Source File

SOURCE=..\..\bezier.c
# End Source File
# Begin Source File

SOURCE=..\..\bezier.h
# End Source File
# Begin Source File

SOURCE=..\..\blob.c
# End Source File
# Begin Source File

SOURCE=..\..\blob.h
# End Source File
# Begin Source File

SOURCE=..\..\boxes.c
# End Source File
# Begin Source File

SOURCE=..\..\boxes.h
# End Source File
# Begin Source File

SOURCE=..\..\bsphere.c
# End Source File
# Begin Source File

SOURCE=..\..\bsphere.h
# End Source File
# Begin Source File

SOURCE=..\..\camera.c
# End Source File
# Begin Source File

SOURCE=..\..\camera.h
# End Source File
# Begin Source File

SOURCE=..\..\chi2.c
# End Source File
# Begin Source File

SOURCE=..\..\chi2.h
# End Source File
# Begin Source File

SOURCE=..\..\colour.c
# End Source File
# Begin Source File

SOURCE=..\..\colour.h
# End Source File
# Begin Source File

SOURCE=..\..\cones.c
# End Source File
# Begin Source File

SOURCE=..\..\cones.h
# End Source File
# Begin Source File

SOURCE=..\..\config.h
# End Source File
# Begin Source File

SOURCE=..\..\csg.c
# End Source File
# Begin Source File

SOURCE=..\..\csg.h
# End Source File
# Begin Source File

SOURCE=..\..\discs.c
# End Source File
# Begin Source File

SOURCE=..\..\discs.h
# End Source File
# Begin Source File

SOURCE=..\..\express.c
# End Source File
# Begin Source File

SOURCE=..\..\express.h
# End Source File
# Begin Source File

SOURCE=..\..\fractal.c
# End Source File
# Begin Source File

SOURCE=..\..\fractal.h
# End Source File
# Begin Source File

SOURCE=..\..\frame.h
# End Source File
# Begin Source File

SOURCE=..\..\gif.c
# End Source File
# Begin Source File

SOURCE=..\..\gif.h
# End Source File
# Begin Source File

SOURCE=..\..\gifdecod.c
# End Source File
# Begin Source File

SOURCE=..\..\gifdecod.h
# End Source File
# Begin Source File

SOURCE=..\..\halos.c
# End Source File
# Begin Source File

SOURCE=..\..\halos.h
# End Source File
# Begin Source File

SOURCE=..\..\hcmplx.c
# End Source File
# Begin Source File

SOURCE=..\..\hcmplx.h
# End Source File
# Begin Source File

SOURCE=..\..\hfield.c
# End Source File
# Begin Source File

SOURCE=..\..\hfield.h
# End Source File
# Begin Source File

SOURCE=..\..\iff.c
# End Source File
# Begin Source File

SOURCE=..\..\iff.h
# End Source File
# Begin Source File

SOURCE=..\..\image.c
# End Source File
# Begin Source File

SOURCE=..\..\image.h
# End Source File
# Begin Source File

SOURCE=..\..\lathe.c
# End Source File
# Begin Source File

SOURCE=..\..\lathe.h
# End Source File
# Begin Source File

SOURCE=..\..\lbuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\lbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\lighting.c
# End Source File
# Begin Source File

SOURCE=..\..\lighting.h
# End Source File
# Begin Source File

SOURCE=..\..\matrices.c
# End Source File
# Begin Source File

SOURCE=..\..\matrices.h
# End Source File
# Begin Source File

SOURCE=..\..\mem.c
# End Source File
# Begin Source File

SOURCE=..\..\mem.h
# End Source File
# Begin Source File

SOURCE=..\..\mesh.c
# End Source File
# Begin Source File

SOURCE=..\..\mesh.h
# End Source File
# Begin Source File

SOURCE=..\..\normal.c
# End Source File
# Begin Source File

SOURCE=..\..\normal.h
# End Source File
# Begin Source File

SOURCE=..\..\objects.c
# End Source File
# Begin Source File

SOURCE=..\..\objects.h
# End Source File
# Begin Source File

SOURCE=..\..\octree.c
# End Source File
# Begin Source File

SOURCE=..\..\octree.h
# End Source File
# Begin Source File

SOURCE=..\..\optin.c
# End Source File
# Begin Source File

SOURCE=..\..\optin.h
# End Source File
# Begin Source File

SOURCE=..\..\optout.c
# End Source File
# Begin Source File

SOURCE=..\..\optout.h
# End Source File
# Begin Source File

SOURCE=..\..\parse.c
# End Source File
# Begin Source File

SOURCE=..\..\parse.h
# End Source File
# Begin Source File

SOURCE=..\..\parstxtr.c
# End Source File
# Begin Source File

SOURCE=..\..\parstxtr.h
# End Source File
# Begin Source File

SOURCE=..\..\pattern.c
# End Source File
# Begin Source File

SOURCE=..\..\pattern.h
# End Source File
# Begin Source File

SOURCE=..\..\pgm.c
# End Source File
# Begin Source File

SOURCE=..\..\pgm.h
# End Source File
# Begin Source File

SOURCE=..\..\pigment.c
# End Source File
# Begin Source File

SOURCE=..\..\pigment.h
# End Source File
# Begin Source File

SOURCE=..\..\planes.c
# End Source File
# Begin Source File

SOURCE=..\..\planes.h
# End Source File
# Begin Source File

SOURCE=..\..\LIBPNG\PNG.H
# End Source File
# Begin Source File

SOURCE=..\..\png_pov.c
# End Source File
# Begin Source File

SOURCE=..\..\png_pov.h
# End Source File
# Begin Source File

SOURCE=..\..\point.c
# End Source File
# Begin Source File

SOURCE=..\..\point.h
# End Source File
# Begin Source File

SOURCE=..\..\poly.c
# End Source File
# Begin Source File

SOURCE=..\..\poly.h
# End Source File
# Begin Source File

SOURCE=..\..\polygon.c
# End Source File
# Begin Source File

SOURCE=..\..\polygon.h
# End Source File
# Begin Source File

SOURCE=..\..\polysolv.c
# End Source File
# Begin Source File

SOURCE=..\..\polysolv.h
# End Source File
# Begin Source File

SOURCE=..\..\povproto.h
# End Source File
# Begin Source File

SOURCE=..\..\povray.h
# End Source File
# Begin Source File

SOURCE=..\..\povray_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\ppm.c
# End Source File
# Begin Source File

SOURCE=..\..\ppm.h
# End Source File
# Begin Source File

SOURCE=..\..\prism.c
# End Source File
# Begin Source File

SOURCE=..\..\prism.h
# End Source File
# Begin Source File

SOURCE=..\pvbmp.c
# End Source File
# Begin Source File

SOURCE=..\..\quadrics.c
# End Source File
# Begin Source File

SOURCE=..\..\quadrics.h
# End Source File
# Begin Source File

SOURCE=..\..\quatern.c
# End Source File
# Begin Source File

SOURCE=..\..\quatern.h
# End Source File
# Begin Source File

SOURCE=..\..\rad_data.c
# End Source File
# Begin Source File

SOURCE=..\..\radiosit.c
# End Source File
# Begin Source File

SOURCE=..\..\radiosit.h
# End Source File
# Begin Source File

SOURCE=..\..\ray.c
# End Source File
# Begin Source File

SOURCE=..\..\ray.h
# End Source File
# Begin Source File

SOURCE=..\..\render.c
# End Source File
# Begin Source File

SOURCE=..\..\render.h
# End Source File
# Begin Source File

SOURCE=..\..\sor.c
# End Source File
# Begin Source File

SOURCE=..\..\sor.h
# End Source File
# Begin Source File

SOURCE=..\..\spheres.c
# End Source File
# Begin Source File

SOURCE=..\..\spheres.h
# End Source File
# Begin Source File

SOURCE=..\..\super.c
# End Source File
# Begin Source File

SOURCE=..\..\super.h
# End Source File
# Begin Source File

SOURCE=..\..\targa.c
# End Source File
# Begin Source File

SOURCE=..\..\targa.h
# End Source File
# Begin Source File

SOURCE=..\..\texture.c
# End Source File
# Begin Source File

SOURCE=..\..\texture.h
# End Source File
# Begin Source File

SOURCE=..\..\tokenize.c
# End Source File
# Begin Source File

SOURCE=..\..\tokenize.h
# End Source File
# Begin Source File

SOURCE=..\..\torus.c
# End Source File
# Begin Source File

SOURCE=..\..\torus.h
# End Source File
# Begin Source File

SOURCE=..\..\triangle.c
# End Source File
# Begin Source File

SOURCE=..\..\triangle.h
# End Source File
# Begin Source File

SOURCE=..\..\truetype.c
# End Source File
# Begin Source File

SOURCE=..\..\truetype.h
# End Source File
# Begin Source File

SOURCE=..\..\txttest.c
# End Source File
# Begin Source File

SOURCE=..\..\txttest.h
# End Source File
# Begin Source File

SOURCE=..\..\userio.c
# End Source File
# Begin Source File

SOURCE=..\..\userio.h
# End Source File
# Begin Source File

SOURCE=..\..\vbuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\vbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\vector.h
# End Source File
# Begin Source File

SOURCE=..\..\vlbuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\vlbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\warps.c
# End Source File
# Begin Source File

SOURCE=..\..\warps.h
# End Source File
# Begin Source File

SOURCE=.\winLibSupport.c
# End Source File
# End Target
# End Project
