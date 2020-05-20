/****************************************************************************
*                   source\windows\config.h
*
*  This file contains the Windows specific defines.
*
*  from Persistence of Vision Raytracer
*  Copyright 1996-1997 Persistence of Vision Team
*  This windows version of POV-Ray is Copyright 1996-1997 Christopher J. Cason.
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by leaving a message in CompuServe's POVRAY forum. The
*  The latest version of POV-Ray may be found there as well. POVRAY files can
*  also be found on the world wide web at http://www.povray.org/.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*****************************************************************************/

/* This code is not commented enough or as much as I'd like. It also needs
 a major cleanup. However, after all, this is volunteer work and I've
 not got the time to do so now. Rather than further delay the release,
 here it is.
*/

#ifndef CONFIG_H
#define CONFIG_H

/*
#if defined(__linux__)
#include <sys/param.h>
#define _MAX_PATH	MAXPATHLEN
#endif
*/

#define FILE_NAME_LENGTH          _MAX_PATH

#define IFF_SWITCH_CAST (long)

/* NOTE NOTE NOTE - do NOT define PRECISION_TIMER_AVAILABLE unless you know
 what you are doing. and have read the comments in ZTIMER/READ.ME. You
 will need a working ZTimer library if this is switched on. The only thing
 you lose from not having it is the ability to do scene profiling.
*/

#ifdef __WATCOMC__                            /* Watcom C/C++ C32 */
  #include <float.h>
  #define COMPILER_VER                        ".watcom"
  #include "ztimer.h"
  #define PRECISION_TIMER_AVAILABLE           1
  #define PRECISION_TIMER_INIT                ZTimerInit () ;
  #define PRECISION_TIMER_START               LZTimerOn() ;
  #define PRECISION_TIMER_STOP                LZTimerOff() ;
  #define PRECISION_TIMER_COUNT               LZTimerCount() ;
  #define QSORT_FUNCT_PARM                    const void _FAR *
  #define QSORT_FUNCT_RET                     int _Cdecl
  #define QSORT(a,b,c,d)                      qsort(a, b, c, d)
#endif

#ifdef __BORLANDC__                           /* Borland C/C++ */
  #define COMPILER_VER                        ".borland"
  #define IFF_SWITCH_CAST                     (long)
  #define QSORT_FUNCT_PARM                    const void *
  #define QSORT_FUNCT_RET                     int CDECL
  #include "ztimer.h"
  #define PRECISION_TIMER_AVAILABLE           1
  #define PRECISION_TIMER_INIT                ZTimerInit () ;
  #define PRECISION_TIMER_START               LZTimerOn() ;
  #define PRECISION_TIMER_STOP                LZTimerOff() ;
  #define PRECISION_TIMER_COUNT               LZTimerCount() ;
  #define MSDOS
  #define QSORT(a,b,c,d)                      qsort((void *) a, (size_t) b, (size_t) c, (int (_USERENTRY *) (const void *, const void *)) d)
#endif

/* this is for VC and other compilers that don't use ztimer.h */
#ifndef __GENDEFS
#define __GENDEFS
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned long   ulong;
#endif

#ifndef COMPILER_VER
#define COMPILER_VER ".win_unsupported"
#endif

#include <stdarg.h>
#include <stdlib.h>

/* Un comment at most one of these options */
/* #define MEM_TAG     // Enables memory tag debugging */
/* #define MEM_TRACE   // Enables garbage collection and memory tracing */
#define MEM_RECLAIM    // Enables memory reclaimation

#define RED_RAW_FILE_EXTENSION   ".r8"  /* PICLAB's "RAW" file format */
#define GREEN_RAW_FILE_EXTENSION ".g8"  /* PICLAB's "RAW" file format */
#define BLUE_RAW_FILE_EXTENSION  ".b8"  /* PICLAB's "RAW" file format */

#define PLATFORM_CONSTANT 1.622
#define FILENAME_SEPARATOR '\\'

#define EPSILON 1.0e-5

#define DEFAULT_OUTPUT_FORMAT   't'

void WIN_Banner(char *s);
void WIN_Warning(char *s);
void WIN_Render_Info(char *s);
void WIN_Status_Info(char *s);
void WIN_Debug_Info(char *s);
void WIN_Fatal(char *s);
void WIN_Statistics(char *s);
void WIN_Startup(void);
void WIN_Finish(int n);
void WIN_Cooperate(int n);
void WIN_Display_Init(int width, int height);
void WIN_Display_Plot(int x, int y, int r, int g, int b, int a);
void WIN_Display_Plot_Rect(int x1, int x2, int y1, int y2, int r, int g, int b, int a);
void WIN_Pre_Pixel (int x, int y, float colour [5]);  /* we don't yet have COLOUR defined. colour is really of type COLOUR. */
void WIN_Post_Pixel (int x, int y, float colour [5]); /* COLOUR is typdef_ed in FRAME.H as COLC [5], where COLC is float. */
void WIN_Display_Finished(void);
void WIN_Display_Close(void);
void WIN_LoadRerun(void);
void WIN_SaveRerun(void);
void WIN_ParseINI(void);
void WIN_PrintOtherCredits (void) ;
int WIN_System(char *s);
int WIN_Povray (int argc, char **argv) ;

#define STARTUP_POVRAY            WIN_Startup();
#define FINISH_POVRAY(n)          WIN_Finish(n);
#define COOPERATE_0               WIN_Cooperate(0);
#define COOPERATE_1               WIN_Cooperate(1);
#define TEST_ABORT                WIN_Cooperate(0);

#define POV_BANNER(s)             WIN_Banner(s)
#define POV_WARNING(s)            WIN_Warning(s)
#define POV_RENDER_INFO(s)        WIN_Render_Info(s)
#define POV_STATUS_INFO(s)        WIN_Status_Info(s)
#define POV_DEBUG_INFO(s)         WIN_Debug_Info(s)
#define POV_FATAL(s)              WIN_Fatal(s)
#define POV_STATISTICS(s)         WIN_Statistics(s)
#define POV_DISPLAY_INIT(w,h)     WIN_Display_Init((w),(h));
#define POV_DISPLAY_CLOSE         WIN_Display_Close();
#define POV_DISPLAY_FINISHED      WIN_Display_Finished();
#define POV_DISPLAY_PLOT(x,y,r,g,b,a) WIN_Display_Plot((x),(y),(r),(g),(b),(a));
#define POV_DISPLAY_PLOT_RECT(x1,x2,y1,y2,r,g,b,a) WIN_Display_Plot_Rect((x1),(x2),(y1),(y2),(r),(g),(b),(a));
#define POV_PRE_PIXEL(x,y,c)      WIN_Pre_Pixel (x, y, c) ;
#define POV_POST_PIXEL(x,y,c)     WIN_Post_Pixel (x, y, c) ;
#define POV_SYSTEM(s)             WIN_System (s) ;

#define READ_ENV_VAR              if ((Option_String_Ptr = getenv("POVINI")) != NULL) parse_ini_file(Option_String_Ptr);
#define ALT_WRITE_INI_FILE        WIN_SaveRerun() ;
#define PROCESS_POVRAY_INI        WIN_ParseINI() ;

#define ALTMAIN
#define WORD                      PVWORD
#define RENAME_FILE(orig,new)     rename(orig,new)
#define NEW_LINE_STRING           "\r\n"
#define READ_SYS_IMAGE(i,n)       Read_BMP_Image((i),(n))
#define GET_SYS_FILE_HANDLE       Get_BMP_File_Handle
#define SYS_DEF_EXT               ".bmp"

#define PRINT_OTHER_CREDITS       WIN_PrintOtherCredits () ;

#define MEM_STATS                 1

#ifndef POVWIN_FILE
#undef getc
#define fopen                     pov_fopen
#define fclose                    pov_fclose
#define getc                      pov_getc
#define fseek                     pov_fseek
#define ftell                     pov_ftell
#endif

FILE *pov_fopen (const char *filename, const char *mode) ;
int pov_fclose (FILE *stream) ;
int pov_getc (FILE *stream) ;
int pov_fseek (FILE *stream, long offset, int whence) ;
int pov_ftell (FILE *stream) ;

#endif


