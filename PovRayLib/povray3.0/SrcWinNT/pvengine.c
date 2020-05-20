/****************************************************************************
*                pvengine.c
*
*  This module implements Windows specific routines.
*
*  Copyright © POV-Team(tm) 1996-1997. All Rights Reserved.
*  This windows version of POV-Ray is Copyright 1996-1997 Christopher J. Cason.
*  Authors : Christopher J. Cason and Kendall Bennett (palette/display code).
*
*  NOTE : As this is Windows code, it was edited using a wide Windows-hosted
*         editor. Accordingly, expect text to exceed 80 columns regularly.
*
*  from Persistence of Vision Raytracer(tm)
*  Copyright 1996-1997 Persistence of Vision Team
*
* The terms POV-Ray, POV, and Persistence of Vision Raytracer are trademarks
* of the Persistence of Vision Team.
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray(tm) and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file. If
*  POVLEGAL.DOC is not available or for more info please contact the POV-Ray(tm)
*  Team Coordinator by leaving a message in CompuServe's POVRAY forum. The
*  The latest version of POV-Ray may be found there as well. POVRAY files can
*  also be found on the world wide web at http://www.povray.org/.
*
*  In the case of this particular program, POVLEGAL.DOC is also embedded
*  in the source code. See the file PVLEGAL.H.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
*****************************************************************************/

// This code is not commented enough or as much as I'd like. It also needs
// a major cleanup. However, after all, this is volunteer work and I've
// not got the time to do so now. Rather than further delay the release,
// here it is.

// KNOWN BUGS. Quite a few. Not all are listed here, but here's some -
//   o command-line passing between instances has to be fixed. it used
//     to work but later changes broke it.

#define POVWIN_FILE

#include <math.h>
#include <setjmp.h>
#include <string.h>
#include <windows.h>
#include "frame.h"
#include "colour.h"
#include "povray.h"
#include "povproto.h"
#include "optout.h"
#include "userio.h"
#pragma hdrstop

#define DECLARE_TABLES

#include <direct.h>
#include <io.h>
#include <process.h>
#include <assert.h>
#include <sys\stat.h>
#include <commctrl.h>
#include <excpt.h>
#include "optin.h"
#include "pvengine.h"
#include "pvengine.rh"
#include "pvclient.h"
#include "pvdialog.h"
#include "pvguiext.h"

int                     alert_sound ;
int                     statistics_count = -1 ;
int                     run_count ;
char                    command_line [_MAX_PATH * 3] ;
char                    old_command_line [_MAX_PATH * 3] ;
char                    extra_commands [_MAX_PATH * 3] ;
char                    *argv [MAX_ARGV] ;
char                    source_file_name [_MAX_PATH] ;
char                    ourPath [_MAX_PATH] ;
char                    engineHelpPath [_MAX_PATH] ;
char                    rendererHelpPath [_MAX_PATH] ;
char                    lastRenderName [_MAX_PATH] ;
char                    lastBitmapName [_MAX_PATH] ;
char                    lastRenderPath [_MAX_PATH] ;
char                    lastBitmapPath [_MAX_PATH] ;
char                    lastQueuePath [_MAX_PATH] ;
char                    lastSecondaryIniFilePath [_MAX_PATH] ;
char                    DefaultRenderIniFileName [_MAX_PATH] ;
char                    SecondaryRenderIniFileName [_MAX_PATH] ;
char                    SecondaryRenderIniFileSection [64] ;
char                    background_file [_MAX_PATH] ;
char                    HomePath [_MAX_PATH] ;
char                    EngineIniFileName [_MAX_PATH] ;
char                    RerunIniPath [_MAX_PATH] ;
char                    CurrentRerunFileName [_MAX_PATH] ;
char                    ToolIniFileName [_MAX_PATH] ;
char                    tool_commands [MAX_TOOLCMD] [MAX_TOOLCMDTEXT] ;
char                    tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
char                    requested_render_file [_MAX_PATH] ;
unsigned                class_registered = 0 ;
unsigned                currentX = 0 ;
unsigned                currentY = 0 ;
unsigned                percentage_complete = 0 ;
unsigned                screen_width ;
unsigned                screen_height ;
unsigned                screen_depth ;
unsigned                niceness ;
unsigned                niceness_milliseconds ;
unsigned                background_width ;
unsigned                background_height ;
unsigned                argc ;
unsigned                povray_return_code ;
unsigned                loadRerun ;
unsigned                continueRerun ;
unsigned                seconds = 0 ;
unsigned                pixels = 0 ;
unsigned                toolheight = 0 ;
unsigned                GUI_priority = CM_GUIPRIORITY_NORMAL ;
unsigned                render_priority = CM_RENDERPRIORITY_NORMAL ;
unsigned                on_completion = CM_COMPLETION_BEEP ;
unsigned                window_count = 0 ;
BOOL                    quit = FALSE ;
BOOL                    rendering = FALSE ;
BOOL                    stop_rendering = FALSE ;
BOOL                    alert_on_completion ;
BOOL                    using_ctl3d ;
BOOL                    save_settings ;
BOOL                    IsWin32 = FALSE ;
BOOL                    IsW95UserInterface = FALSE ;
BOOL                    running_demo = FALSE ;
BOOL                    fast_scroll = FALSE ;
BOOL                    no_shellout_wait = FALSE ;
BOOL                    tile_background = TRUE ;
BOOL                    non_client = TRUE ;
BOOL                    renderwin_effects = TRUE ;
BOOL                    debugging = FALSE ;
BOOL                    no_palette_warn = FALSE ;
BOOL                    render_lock_up = FALSE ;
BOOL                    render_main_icon = FALSE ;
BOOL                    hide_render_window = TRUE ;
BOOL                    render_above_main = TRUE ;
BOOL                    tips_enabled = TRUE ;
BOOL                    demo_mode = FALSE ;
BOOL                    ignore_auto_ini = FALSE ;
BOOL                    isMaxiMinimized = FALSE ;
BOOL                    usedMaxiMini = FALSE ;
BOOL                    use_template = FALSE ;
BOOL                    newVersion = FALSE ;
BOOL                    exit_after_render = FALSE ;
BOOL                    system_noactive = FALSE ;
BOOL                    terminating = FALSE ;
BOOL                    need_output_filename = FALSE ;
BOOL                    lock_maximain = TRUE ;
BOOL                    one_instance = TRUE ;
BOOL                    run_renderer = FALSE ;
BOOL                    use_threads = FALSE ;
BOOL                    use_toolbar = TRUE ;
BOOL                    use_tooltips = TRUE ;
BOOL                    use_editors = TRUE ;
BOOL                    resizing = FALSE ;
BOOL                    drop_to_editor ;
BOOL                    show_messages = FALSE ;
BOOL                    restore_command_line = FALSE ;
BOOL                    render_requested = FALSE ;
BOOL                    render_auto_close = FALSE ;
BOOL                    rendersleep = FALSE ;
BOOL                    noexec = FALSE ;
BOOL                    ExtensionsEnabled = TRUE ;
BOOL                    use_taskbar = TRUE ;
BOOL                    main_window_hidden = FALSE ;
HWND                    toolbar_window = NULL ;
HWND                    tabbed_window = NULL ;
HWND                    window_list [MAX_WINDOWS] ;
HWND                    current_window = NULL ;
HWND                    toolbar_combobox = NULL ;
FILE                    *debugFile = NULL ;
HICON                   ourIcon ;
HACCEL                  hAccelerators ;
time_t                  render_start_time ;
time_t                  render_finish_time ;
HANDLE                  hRenderThread = NULL ;
HANDLE                  hMainThread ;
jmp_buf                 jump_buffer ;
HBITMAP                 hBmpBackground ;
HBITMAP                 hBmpMaxiMini ;
HBITMAP                 hBmpRendering ;
HBITMAP                 hBmpIcon ;
HBITMAP                 hBmpSplash ;
HPALETTE                hPalApp = NULL ;
COLORREF                background_colour ;
COLORREF                text_colour ;
COLORREF                custom_colours [16] ;
COLORREF                background_shade = RGB (1, 1, 1) ;
HINSTANCE               hInstance = NULL ;
HINSTANCE               hLibCtl3d ;
HINSTANCE               hLibPovEdit ;
OSVERSIONINFO           version_info ;
CRITICAL_SECTION        critical_section ;

char                    queued_files [MAX_QUEUE] [_MAX_PATH] ;
char                    dir [_MAX_PATH] ;
unsigned                queued_file_count = 0 ;
unsigned                auto_render = TRUE ;
unsigned                timer_id ;

void                    *CurrentEditor ;
unsigned                panel_size ;
HWND                    editor_window = NULL ;
HWND                    status_window = NULL ;
EditStruct              Edit ;
HandleStruct            editor_handles ;

char                    PovMainWinClass [] = "PovMainWinClass" ;
unsigned                mainwin_xpos ;
unsigned                mainwin_ypos ;
HWND                    main_window = NULL ;
HWND                    message_window = NULL ;
WINDOWPLACEMENT         mainwin_placement ;
int                     renderwin_xoffset ;
int                     renderwin_yoffset ;
int                     renderwin_left = CW_USEDEFAULT ;
int                     renderwin_top = CW_USEDEFAULT ;
char                    PovRenderWinClass [] = "PovRenderWinClass" ;
unsigned                renderwin_max_width ;
unsigned                renderwin_max_height ;
unsigned                renderwin_8bits ;
unsigned                renderwin_flags = 0 ;
BOOL                    renderwin_active ;
BOOL                    renderwin_destroyed ;
HWND                    render_window = NULL ;

int                     render_bitmap_depth = -1 ;
long                    render_bitmap_bpl ;
unsigned                render_width ;
unsigned                render_height ;
uchar                   *render_bitmap_surface = NULL ;
BitmapInfo              render_bitmap ;
BitmapInfo              bitmap_template ;

char                    PovSplashWinClass [] = "PovSplashWinClass" ;
HWND                    splash_window = NULL ;
unsigned                splash_width ;
unsigned                splash_height ;

char                    PovMessageWinClass [] = "PovMessageWinClass" ;
extern int              message_xchar ;
extern int              message_ychar ;
extern int              message_scroll_pos_x ;
extern int              message_scroll_pos_y ;
extern int              top_message_row ;
extern int              message_count ;
extern int              message_cols ;
extern int              message_rows ;
extern int              pre_init_flag ;
extern int              input_file_in_memory ;
extern char             message_font_name [256] ;
extern unsigned         message_font_size ;
extern unsigned         message_font_weight ;
extern BOOL             keep_messages ;
extern BOOL             MenuBarDraw ;
extern Opts             opts ;
extern HFONT            message_font ;
extern HFONT            tab_font ;
extern HMENU            hExpertMenu ;
extern HMENU            hNonExpertMenu ;
extern HMENU            hMainMenu ;
extern HMENU            hPopupMenus ;
extern HMENU            hFileMenu ;
extern HMENU            hEditMenu ;
extern HMENU            hRenderMenu ;
extern HMENU            hRerunMenu ;
extern HMENU            hAppearanceMenu ;
extern HMENU            hMiscMenu ;
extern HMENU            hToolsMenu ;
extern HMENU            hPluginsMenu ;
extern HMENU            hHelpMenu ;
extern HMENU            hPopupMenusNE ;
extern HMENU            hFileMenuNE ;
extern HMENU            hEditMenuNE ;
extern HMENU            hRenderMenuNE ;
extern HMENU            hRerunMenuNE ;
extern HMENU            hAppearanceMenuNE ;
extern HMENU            hMiscMenuNE ;
extern HMENU            hToolsMenuNE ;
extern HMENU            hPluginsMenuNE ;
extern HMENU            hHelpMenuNE ;

unsigned (WINAPI *EditGetVersion) (int RecSize, EditStruct *Edit) ;
BOOL handle_main_command (WPARAM wParam, LPARAM lParam) ;

typedef struct
{
  WORD        wVirtkey ;
  int         iMessage ;
  WORD        wRequest ;
} SCROLLKEYS ;

SCROLLKEYS key2scroll [] =
{
  { VK_END,   WM_VSCROLL, SB_BOTTOM   },
  { VK_PRIOR, WM_VSCROLL, SB_PAGEUP   },
  { VK_NEXT,  WM_VSCROLL, SB_PAGEDOWN },
  { VK_UP,    WM_VSCROLL, SB_LINEUP   },
  { VK_DOWN,  WM_VSCROLL, SB_LINEDOWN },
  { VK_LEFT,  WM_HSCROLL, SB_PAGEUP   },
  { VK_RIGHT, WM_HSCROLL, SB_PAGEDOWN },
  { -1,       -1,         -1          }
} ;

LONG PovUnhandledExceptionFilter (EXCEPTION_POINTERS *lpexpExceptionInfo)
{
//int         i ;
  char        str [256] ;
//DWORD       ebp ;
  PCONTEXT    c ;

  c = lpexpExceptionInfo->ContextRecord ;
  sprintf (str, "An exception was generated at address %08lX\n\nPOV-Ray will now exit", c->Eip) ;
  MessageBox (NULL, str, "Error", MB_ICONSTOP | MB_SYSTEMMODAL) ;
//if (MessageBox (NULL, "POV - Unhandled exception. Unwind stack ?", "Error", MB_ICONSTOP | MB_YESNO | MB_SYSTEMMODAL) == IDYES)
//{
//  c = lpexpExceptionInfo->ContextRecord ;
//  sprintf (str, "An exception was generated at address %08lX", c->Eip) ;
//  MessageBox (NULL, str, "Oops", MB_ICONINFORMATION | MB_SYSTEMMODAL) ;
//  ebp = c->Ebp ;
//  // add in stack walking code here.
//}
  ExitProcess (1) ;
  return (EXCEPTION_CONTINUE_SEARCH) ; // make compiler happy
}

void getvars (ExternalVarStruct *v)
{
  strcpy (v->command_line, command_line) ;
  strcpy (v->source_file_name, source_file_name) ;
  strcpy (v->lastRenderName, lastRenderName) ;
  strcpy (v->lastRenderPath, lastRenderPath) ;
  strcpy (v->lastQueuePath, lastQueuePath) ;
  strcpy (v->lastSecondaryIniFilePath, lastSecondaryIniFilePath) ;
  strcpy (v->DefaultRenderIniFileName, DefaultRenderIniFileName) ;
  strcpy (v->SecondaryRenderIniFileName, SecondaryRenderIniFileName) ;
  strcpy (v->SecondaryRenderIniFileSection, SecondaryRenderIniFileSection) ;
  strcpy (v->ourPath, ourPath) ;
  strcpy (v->engineHelpPath, engineHelpPath) ;
  strcpy (v->rendererHelpPath, rendererHelpPath) ;
  strcpy (v->HomePath, HomePath) ;
  strcpy (v->EngineIniFileName, EngineIniFileName) ;
  strcpy (v->ToolIniFileName, ToolIniFileName) ;
  memcpy (v->queued_files, queued_files, sizeof (v->queued_files)) ;
  v->loadRerun = loadRerun ;
  v->continueRerun = continueRerun ;
  v->povray_return_code = povray_return_code ;
  v->rendering = rendering ;
  v->IsWin32 = IsWin32 ;
  v->IsW95UserInterface = IsW95UserInterface ;
  v->running_demo = running_demo ;
  v->debugging = debugging ;
  v->isMaxiMinimized = isMaxiMinimized ;
  v->newVersion = newVersion ;
  v->use_threads = use_threads ;
  v->use_toolbar = use_toolbar ;
  v->use_tooltips = use_tooltips ;
  v->use_editors = use_editors ;
  v->drop_to_editor = drop_to_editor ;
  v->rendersleep = rendersleep ;
  v->ExtensionsEnabled = ExtensionsEnabled ;
  v->queued_file_count = queued_file_count ;
  v->auto_render = auto_render ;
}

void setvars (ExternalVarStruct *v)
{
  strncpy (command_line, v->command_line, sizeof (command_line) - 1) ;
  strncpy (source_file_name, v->source_file_name, sizeof (source_file_name) - 1) ;
  strncpy (lastRenderName, v->lastRenderName, sizeof (lastRenderName) - 1) ;
  strncpy (lastRenderPath, v->lastRenderPath, sizeof (lastRenderPath) - 1) ;
  strncpy (lastQueuePath, v->lastQueuePath, sizeof (lastQueuePath) - 1) ;
  strncpy (lastSecondaryIniFilePath, v->lastSecondaryIniFilePath, sizeof (lastSecondaryIniFilePath) - 1) ;
  strncpy (DefaultRenderIniFileName, v->DefaultRenderIniFileName, sizeof (DefaultRenderIniFileName) - 1) ;
  strncpy (SecondaryRenderIniFileName, v->SecondaryRenderIniFileName, sizeof (SecondaryRenderIniFileName) - 1) ;
  strncpy (SecondaryRenderIniFileSection, v->SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection) - 1) ;
}

BOOL HaveWin32s (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32s) ;
}

BOOL HaveWin95 (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) ;
}

BOOL HaveWinNT (void)
{
  return (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT) ;
}

DWORD render_thread (LPDWORD lpdwParam)
{
  WIN_Povray (argc, argv) ;
  run_renderer = FALSE ;
  return (0) ;
}

void set_render_priority (unsigned priority)
{
  switch (priority)
  {
    case CM_RENDERPRIORITY_LOWEST :
         SetThreadPriority (hRenderThread, THREAD_PRIORITY_LOWEST) ;
         break ;

    case CM_RENDERPRIORITY_LOW :
         SetThreadPriority (hRenderThread, THREAD_PRIORITY_BELOW_NORMAL) ;
         break ;

    case CM_RENDERPRIORITY_NORMAL :
         SetThreadPriority (hRenderThread, THREAD_PRIORITY_NORMAL) ;
         break ;

    case CM_RENDERPRIORITY_HIGH :
         SetThreadPriority (hRenderThread, THREAD_PRIORITY_ABOVE_NORMAL) ;
         break ;

    case CM_RENDERPRIORITY_HIGHEST :
         SetThreadPriority (hRenderThread, THREAD_PRIORITY_HIGHEST) ;
         break ;
  }
}

void set_GUI_priority (unsigned priority)
{
  switch (GUI_priority)
  {
    case CM_GUIPRIORITY_LOWEST :
         SetThreadPriority (hMainThread, THREAD_PRIORITY_LOWEST) ;
         break ;

    case CM_GUIPRIORITY_LOW :
         SetThreadPriority (hMainThread, THREAD_PRIORITY_BELOW_NORMAL) ;
         break ;

    case CM_GUIPRIORITY_NORMAL :
         SetThreadPriority (hMainThread, THREAD_PRIORITY_NORMAL) ;
         break ;

    case CM_GUIPRIORITY_HIGH :
         SetThreadPriority (hMainThread, THREAD_PRIORITY_ABOVE_NORMAL) ;
         break ;

    case CM_GUIPRIORITY_HIGHEST :
         SetThreadPriority (hMainThread, THREAD_PRIORITY_HIGHEST) ;
         break ;
  }
}

void display_cleanup (void)
{
  if (render_window != NULL)
  {
    DestroyWindow (render_window) ;
    render_window = NULL ;
    renderwin_destroyed = FALSE ;
    PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
  }
  if (render_bitmap_surface != NULL)
  {
    free (render_bitmap_surface) ;
    render_bitmap_surface = NULL ;
  }
}

// we can't allow LoadBitmap to load our background bitmaps 'cause if we're running
// a 256-colour mode, it will map the incoming resource to 16 colours ...
// LoadImage () doesn't exist under Win32s, either. sigh.
HBITMAP NonBogusLoadBitmap (HINSTANCE hInst, LPSTR lpszBitmap)
{
  void        *p ;
  HRSRC       hres ;
  HGLOBAL     hg ;
  HBITMAP     hBitmap ;

  if ((hres = FindResource (hInst, lpszBitmap, RT_BITMAP)) == NULL)
    return (NULL) ;
  if ((hg = LoadResource (hInst, hres)) == NULL)
    return (NULL) ;
  if ((p = LockResource (hg)) == NULL)
    return (NULL) ;
  hBitmap = lpDIBToBitmap (p, hPalApp) ;
  GlobalUnlock (hg) ;
  FreeResource (hg) ;
  return (hBitmap) ;
}

HBITMAP NonBogusLoadBitmapAndPalette (HINSTANCE hInst, LPSTR lpszBitmap)
{
  void        *p ;
  HRSRC       hres ;
  HGLOBAL     hg ;
  HBITMAP     hBitmap ;

  if ((hres = FindResource (hInst, lpszBitmap, RT_BITMAP)) == NULL)
    return (NULL) ;
  if ((hg = LoadResource (hInst, hres)) == NULL)
    return (NULL) ;
  if ((p = LockResource (hg)) == NULL)
    return (NULL) ;
  hBitmap = lpDIBToBitmapAndPalette (p) ;
  GlobalUnlock (hg) ;
  FreeResource (hg) ;
  return (hBitmap) ;
}

unsigned set_niceness (unsigned niceness)
{
  switch (niceness)
  {
    case 0 :
         return (1) ;

    case 1 :
         return (25) ;

    case 2 :
         return (75) ;

    case 3 :
         return (175) ;

    case 4 :
         return (550) ;

    case 5 :
         return (1500) ;

    case 6 :
         return (3500) ;

    case 9 :
         return (-1) ;

    default :
         return (50) ;
  }
}

void validatePath (char *s)
{
  if (s [1] == ':' && strlen (s) < 4)
    return ;
  s += strlen (s) - 1 ;
  if (*s == '\\')
    *s = '\0' ;
}

int joinPath (char *out, char *path, char *name)
{
  strcpy (out, path) ;
  if (path [strlen (path) - 1] != '\\')
    strcat (out, "\\") ;
  strcat (out, name) ;
  return (strlen (out)) ;
}

void setRunOnce (void)
{
  char        str [_MAX_PATH] ;
  HKEY        key ;
  DWORD       result ;

  if (RegCreateKeyEx (HKEY_CURRENT_USER,
                      "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
                      0,
                      "",
                      REG_OPTION_NON_VOLATILE,
                      KEY_WRITE,
                      NULL,
                      &key,
                      &result) == ERROR_SUCCESS)
  {
    GetModuleFileName (hInstance, str, sizeof (str)) ;
    RegSetValueEx (key, "POV-Ray for Windows v3.0", 0, REG_SZ, str, strlen (str) + 1) ;
    RegCloseKey (key) ;
  }
}

void getHome (void)
{
  HKEY        key ;
  DWORD       len = sizeof (HomePath) ;

  if (debugFile)
    fprintf (debugFile, "querying registry\n") ;
  HomePath [0] = '\0' ;
  if (IsWin32)
  {
    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                      "Software\\POV-Ray\\Windows",
                      0,
                      KEY_READ,
                      &key) == ERROR_SUCCESS)
    {
      RegQueryValueEx (key, "Home", 0, NULL, HomePath, &len) ;
      RegCloseKey (key) ;
      if (debugFile)
        fprintf (debugFile, "Win32 getHome () succeeded, HomePath is '%s'\n", HomePath) ;
    }
  }
  else
  {
    if (RegOpenKey (HKEY_CLASSES_ROOT, "POV-Ray\\Home", &key) == ERROR_SUCCESS)
    {
      RegQueryValue (key, NULL, HomePath, (LONG *) &len) ;
      RegCloseKey (key) ;
      if (debugFile)
        fprintf (debugFile, "Win16 getHome () succeeded, HomePath is '%s'\n", HomePath) ;
    }
  }
}

void setCommandLine (char *s)
{
  HKEY        key ;
  DWORD       result ;

  if (IsWin32)
  {
    if (RegCreateKeyEx (HKEY_CURRENT_USER,
                        "Software\\POV-Ray\\Windows",
                        0,
                        "",
                        REG_OPTION_NON_VOLATILE,
                        KEY_WRITE,
                        NULL,
                        &key,
                        &result) == ERROR_SUCCESS)
    {
      RegSetValueEx (key, "Command Line", 0, REG_SZ, s, strlen (s) + 1) ;
      RegCloseKey (key) ;
    }
  }
  else
  {
    if (RegCreateKey (HKEY_CLASSES_ROOT, "POV-Ray\\CommandLine", &key) == ERROR_SUCCESS)
    {
      RegSetValue (key, NULL, REG_SZ, s, strlen (s)) ;
      RegCloseKey (key) ;
    }
  }
}

char *getCommandLine (void)
{
  HKEY        key ;
  static char str [2048] ;
  DWORD       len = sizeof (str) ;

  str [0] = '\0' ;
  if (IsWin32)
  {
    if (RegOpenKeyEx (HKEY_CURRENT_USER,
                      "Software\\POV-Ray\\Windows",
                      0,
                      KEY_READ,
                      &key) == ERROR_SUCCESS)
    {
      RegQueryValueEx (key, "Command Line", 0, NULL, str, &len) ;
      RegCloseKey (key) ;
    }
  }
  else
  {
    if (RegOpenKey (HKEY_CLASSES_ROOT, "POV-Ray\\CommandLine", &key) == ERROR_SUCCESS)
    {
      RegQueryValue (key, NULL, str, (LONG *) &len) ;
      RegCloseKey (key) ;
    }
  }
  return (str) ;
}

int parse_commandline (char *s)
{
  char        *prevWord = NULL ;
  BOOL        inQuote = FALSE ;
  BOOL        noStrip = FALSE ;
  static char str [_MAX_PATH * 3] ;
  static char filename [_MAX_PATH] ;

  argc = 0 ;
  GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
  argv [argc++] = filename ;
  s = strncpy (str, s, sizeof (str) - 1) ;
  while (*s)
  {
    switch (*s)
    {
      case '"' :
           if (prevWord != NULL)
           {
             if (inQuote)
             {
               if (!noStrip)
                 *s = '\0' ;
               argv [argc++] = prevWord ;
               prevWord = NULL ;
             }
             else
               noStrip = TRUE ;
           }
           inQuote = !inQuote ;
           break ;

      case ' ' :
      case '\t' :
           if (!inQuote)
           {
             if (prevWord != NULL)
             {
               *s = '\0' ;
               argv [argc++] = prevWord ;
               prevWord = NULL ;
               noStrip = FALSE ;
             }
           }
           break ;

      default :
           if (prevWord == NULL)
             prevWord = s ;
           break ;
    }
    if (argc >= MAX_ARGV - 1)
      break ;
    s++ ;
  }
  if ((prevWord != NULL || (inQuote && prevWord != NULL)) && argc < MAX_ARGV - 1)
  {
    *s = '\0' ;
    argv [argc++] = prevWord ;
  }
  argv [argc] = NULL ;
  return (argc) ;
}

int execute_tool (char *s)
{
  STARTUPINFO           startupInfo ;
  PROCESS_INFORMATION   procInfo ;

  if (strlen (s) == 0)
    return (0) ;

  startupInfo.cb               = sizeof (STARTUPINFO) ;
  startupInfo.lpReserved       = 0 ;
  startupInfo.lpDesktop        = NULL ;
  startupInfo.lpTitle          = NULL ;
  startupInfo.dwX              = 0 ;
  startupInfo.dwY              = 0 ;
  startupInfo.dwXSize          = 0 ;
  startupInfo.dwYSize          = 0 ;
  startupInfo.dwXCountChars    = 0 ;
  startupInfo.dwYCountChars    = 0 ;
  startupInfo.dwFillAttribute  = 0 ;
  startupInfo.dwFlags          = IsWin32 ? STARTF_USESHOWWINDOW : 0 ;
  startupInfo.wShowWindow      = SW_SHOW ;
  startupInfo.cbReserved2      = 0 ;
  startupInfo.lpReserved2      = 0 ;

  if (CreateProcess (NULL, s, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &procInfo) == FALSE)
  {
    PovMessageBox ("Could not run program", "Tool Error") ;
    return (GetLastError ()) ;
  }

  // clean up
  CloseHandle (procInfo.hProcess) ;
  CloseHandle (procInfo.hThread) ;

  return (0) ;
}

void start_rendering (BOOL is_auto_render, BOOL ignore_source_file)
{
  int         i ;
  char        str [_MAX_PATH] ;
  char        filename [_MAX_PATH] ;
  char        rerunfile [_MAX_PATH] ;
  char        section [16] ;
  MSG         msg ;
  DWORD       threadId = 0 ;
  DWORD       threadParam = 0 ;

  extra_commands [0] = '\0' ;
  renderwin_destroyed = FALSE ;
  rendersleep = FALSE ;
  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  update_menu_for_render (TRUE) ;
  display_cleanup () ;

  SendMessage (toolbar_combobox, WM_GETTEXT, sizeof (SecondaryRenderIniFileSection), (LPARAM) SecondaryRenderIniFileSection) ;

  if (debugging)
    message_printf ("Input command line is '%s'\n", command_line) ;

  /*
  ** This is the count used by the write statistics routine for the line number
  */
  statistics_count = 0 ;
  if (loadRerun)
  {
    strcpy (section, "Renderer") ;
    // if loadRerun == 1, load last failed render. otherwise, load render number loadRerun - 2, where render number 0 is the most recent.
    switch (loadRerun)
    {
      case 1 :
           strcpy (rerunfile, CurrentRerunFileName) ;
           break ;

      default :
           sprintf (rerunfile, "%sRerun%02d.Ini", RerunIniPath, loadRerun - 2) ;
           break ;
    }
    // turn off continue trace in case it's on, since it seems to confuse people.
    WritePrivateProfileString (section, "Continue_Trace", "Off", rerunfile) ;
    // flush the INI file
    WritePrivateProfileString (NULL, NULL, NULL, rerunfile) ;
    sprintf (str, "\"Include_Ini=%s[%s]\" ", rerunfile, section) ;
    strcat (extra_commands, str) ;
    if (continueRerun)
      strcat (extra_commands, "+c ") ;
    GetPrivateProfileString (section, "Input_File_Name", "", filename, sizeof (filename), rerunfile) ;
    if (strlen (filename))
    {
      message_printf ("%s file '%s' using rerun information.\n", continueRerun ? "Continuing" : "Rendering", filename) ;
      GetPrivateProfileString ("Environment", "CurrentDirectory", "", dir, sizeof (dir), rerunfile) ;
      if (strlen (dir))
      {
        SetCurrentDirectory (dir) ;
        message_printf ("Render directory is '%s'.\n", dir) ;
      }
    }
    else
    {
      message_printf ("Error : Specified rerun information not found.\n") ;
      loadRerun = continueRerun = 0 ;
      update_menu_for_render (FALSE) ;
      return ;
    }
  }
  else
  {
    // non-rerun processing
    if (running_demo == 0)
    {
      splitpath (SecondaryRenderIniFileName, NULL, str) ;
      if (str [0] != '\0')
      {
        message_printf ("Preset INI file is '%s'", SecondaryRenderIniFileName) ;
        if (SecondaryRenderIniFileSection [0] != '\0')
          message_printf (", section is '%s'", SecondaryRenderIniFileSection) ;
        message_printf (".\n") ;
        strcat (extra_commands, "\"Include_INI=") ;
        strcat (extra_commands, SecondaryRenderIniFileName) ;
        strcat (extra_commands, SecondaryRenderIniFileSection) ;
        strcat (extra_commands, "\" ") ;
      }
      if (!ignore_source_file && strlen (source_file_name) != 0)
      {
        if (input_file_in_memory)
          message_printf ("Source file is in editor memory.\n") ;
        else
          message_printf ("Preset source file is '%s'.\n", source_file_name) ;
        switch (get_file_type (source_file_name))
        {
          case filePOV :
               sprintf (str, "\"Input_File_Name=%s\" ", source_file_name) ;
               break ;

          case fileINI :
               sprintf (str, "\"Include_INI=%s\" ", source_file_name) ;
               break ;

          default :
               message_printf ("POV-Ray for Windows doesn't recognise this file type ; assuming POV source.\n") ;
               sprintf (str, "\"Input_File_Name=%s\" ", source_file_name) ;
               break ;
        }
        strcat (extra_commands, str) ;
        splitpath (source_file_name, dir, NULL) ;
        SetCurrentDirectory (dir) ;
      }
    }
  }
  if (strlen (command_line))
  {
    if (!running_demo)
      message_printf ("Rendering using command line '%s'.\n", command_line) ;
    strncat (extra_commands, command_line, sizeof (extra_commands) - 1) ;
  }
  if (debugging)
    message_printf ("Extra command string is '%s'\n", extra_commands) ;
  parse_commandline (extra_commands) ;
  if (debugging)
  {
    message_printf ("%d arguments parsed\n", argc) ;
    for (i = 0 ; i < argc ; i++)
      message_printf ("  arg %d is '%s'\n", i, argv [i]) ;
  }
  stop_rendering = FALSE ;
  percentage_complete = 0 ;
  loadRerun = continueRerun = 0 ;
  WIN_Pre_Pixel (-1, -1, 0) ;
  if (MenuBarDraw)
  {
    DrawMenuBar (main_window) ;
    MenuBarDraw = FALSE ;
  }
  show_messages = TRUE ;
//if (current_window == editor_window)
    CalculateClientWindows (TRUE) ;
  ShowWindow (message_window, SW_SHOW) ;
  ExternalEvent (EventStartRendering, 0) ;
  if (use_threads)
  {
    hRenderThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) render_thread, &threadParam, 0, &threadId) ;
    if (hRenderThread != NULL)
    {
      run_renderer = TRUE ;
      set_render_priority (render_priority) ;
      set_GUI_priority (GUI_priority) ;
      while (run_renderer)
      {
        while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
          if (!TranslateAccelerator (main_window, hAccelerators, &msg))
          {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
          }
        }
        Sleep (5) ;
      }
      CloseHandle (hRenderThread) ;
      hRenderThread = NULL ;
      set_GUI_priority (CM_GUIPRIORITY_NORMAL) ;
    }
    else
      MessageBox (NULL, "Error", "Cannot create thread for render process!", MB_OK | MB_ICONSTOP) ;
  }
  else
    WIN_Povray (argc, argv) ;
  if (quit)
  {
    WinHelp (main_window, "pvengine.hlp", HELP_QUIT, NULL) ;
    WinHelp (main_window, "povray30.hlp", HELP_QUIT, NULL) ;
    DestroyWindow (main_window) ;
    return ;
  }
  rendersleep = FALSE ;
  SetWindowText (main_window, "POV-Ray for Windows") ;
  if (render_window)
  {
    SetWindowText (render_window, "Render Window") ;
    if (render_auto_close)
      handle_main_command (CM_RENDERCLOSE, 0) ;
  }
  say_status_message (StatusMessage, "") ;
  update_menu_for_render (FALSE) ;
  ExternalEvent (EventStopRendering, povray_return_code) ;
  if (povray_return_code == 0)
  {
    show_messages = FALSE ;
//  if (current_window == editor_window)
      CalculateClientWindows (FALSE) ;
    switch (on_completion)
    {
      case CM_COMPLETION_EXIT :
           WinHelp (main_window, "pvengine.hlp", HELP_QUIT, NULL) ;
           WinHelp (main_window, "povray30.hlp", HELP_QUIT, NULL) ;
           DestroyWindow (main_window) ;
           break ;

      case CM_COMPLETION_BEEP :
           MessageBeep (alert_sound) ;
           break ;

      case CM_COMPLETION_BEEPMESSAGE :
           MessageBeep (alert_sound) ;
           // fall through

      case CM_COMPLETION_MESSAGE :
           PovMessageBox ("Render completed", "Message from POV-Ray for Windows") ;
           break ;
    }
  }
}

UINT WINAPI ofn_hook_fn (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG :
         SetupExplorerDialog (hwnd) ;
         break ;
  }
  return (FALSE) ;
}

void init_ofn (OPENFILENAME *ofn, HWND hWnd, char *title, char *name, int maxlen, char *lastPath, char *defaultExt)
{
  ofn->lStructSize = sizeof (OPENFILENAME) ;
  ofn->hwndOwner = hWnd ;
  ofn->hInstance = hInstance ;
  ofn->lpstrCustomFilter = NULL ;
  ofn->nMaxCustFilter = 0 ;
  ofn->nFilterIndex = 1 ;
  ofn->lpstrTitle = title ;
  ofn->lpstrFile = name ;
  ofn->nMaxFile = maxlen ;
  ofn->lpstrFileTitle = NULL ;
  ofn->nMaxFileTitle = 0 ;
  ofn->lpstrInitialDir = lastPath ;
  ofn->Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR ;
  if (IsW95UserInterface)
    ofn->Flags |= OFN_EXPLORER ;
  if (use_template)
  {
    if (IsW95UserInterface)
      ofn->Flags |= OFN_ENABLEHOOK ;
    else
      ofn->Flags = OFN_ENABLETEMPLATE ;
  }
  ofn->nFileOffset = 0 ;
  ofn->nFileExtension = 0 ;
  ofn->lpstrDefExt = defaultExt ;
  ofn->lCustData = 0L ;
  ofn->lpfnHook = ofn_hook_fn ;
  ofn->lpTemplateName = MAKEINTRESOURCE (HaveWinNT () ? IDD_NTCOMMFILE : IDD_COMMFILE) ;
}

char *file_open (HWND hWnd)
{
  BOOL          result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  strcpy (name, lastRenderName) ;
  validatePath (lastRenderPath) ;
  init_ofn (&ofnTemp, hWnd, "Render File", name, sizeof (name), lastRenderPath, "pov") ;
  ofnTemp.lpstrFilter = "POV source and INI (*.pov;*.ini)\0*.pov;*.ini\0INI files (*.ini)\0*.ini\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != FALSE)
  {
    strcpy (lastRenderPath, name) ;
    // this removes the name AND the trailing '\' [which is what we want]
    lastRenderPath [ofnTemp.nFileOffset - 1] = '\0' ;
    validatePath (lastRenderPath) ;
    strcpy (lastRenderName, name + ofnTemp.nFileOffset) ;
  }
  return (result ? name : NULL) ;
}

void add_queue (HWND hWnd, HWND hlb)
{
  int           queue_count ;
  char          name [8192] ;
  char          *s ;
  char          str [_MAX_PATH] ;
  OPENFILENAME  ofnTemp ;

  queue_count = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
  if (queue_count >= MAX_QUEUE)
  {
    PovMessageBox ("File queue is full", "Cannot add any more files!") ;
    return ;
  }
  strcpy (name, lastRenderName) ;
  name [strlen (name) + 1] = '\0' ;
  validatePath (lastQueuePath) ;
  init_ofn (&ofnTemp, hWnd, "Add to Queue", name, sizeof (name), lastQueuePath, "pov") ;
  ofnTemp.lpstrFilter = "POV source and INI (*.pov;*.ini)\0*.pov;*.ini\0INI files (*.ini)\0*.ini\0All Files (*.*)\0*.*\0" ;
  ofnTemp.Flags |= OFN_ALLOWMULTISELECT ;
  if (GetOpenFileName (&ofnTemp) != FALSE)
  {
    // convert spaces into NULL's if we're not using the new interface so it works with the below code
    if (!IsW95UserInterface)
      for (s = name ; *s ; s++)
        if (*s == ' ')
          *s = '\0' ;
    if (ofnTemp.nFileOffset < strlen (name))
    {
      strcpy (lastQueuePath, name) ;
      lastQueuePath [ofnTemp.nFileOffset - 1] = '\0' ;
      SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) name) ;
    }
    else
    {
      s = name ;
      strcpy (lastQueuePath, name) ;
      for (s += strlen (s) + 1 ; *s ; s += strlen (s) + 1)
      {
        if (queue_count++ >= MAX_QUEUE)
        {
          PovMessageBox ("File queue is full", "Cannot add any more files!") ;
          return ;
        }
        joinPath (str, lastQueuePath, s) ;
        strlwr (str) ;
        SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) str) ;
      }
    }
  }
}

char *get_ini_file (HWND hWnd, char *path)
{
  BOOL          result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  validatePath (path) ;
  init_ofn (&ofnTemp, hWnd, "Choose INI File", name, sizeof (name), path, "ini") ;
  ofnTemp.lpstrFilter = "INI files (*.ini)\0*.ini\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != FALSE)
  {
    strcpy (path, name) ;
    path [ofnTemp.nFileOffset - 1] = '\0' ;
  }
  return (result ? name : NULL) ;
}

char *get_background_file (HWND hWnd)
{
  BOOL          result ;
  OPENFILENAME  ofnTemp ;
  static char   name [_MAX_PATH] ;

  strcpy (name, lastBitmapName) ;
  validatePath (lastBitmapPath) ;
  init_ofn (&ofnTemp, hWnd, "Tile Bitmap File", name, sizeof (name), lastBitmapPath, "bmp") ;
  ofnTemp.lpstrFilter = "BMP files (*.bmp)\0*.bmp\0" ;
  if ((result = GetOpenFileName (&ofnTemp)) != FALSE)
  {
    strcpy (lastBitmapPath, name) ;
    lastBitmapPath [ofnTemp.nFileOffset - 1] = '\0' ;
    strcpy (lastBitmapName, name + ofnTemp.nFileOffset) ;
  }
  return (result ? name : NULL) ;
}

void get_font (void)
{
  HDC         hdc ;
  HFONT       hfont ;
  HFONT       hfontOld ;
  LOGFONT     lf ;
  CHOOSEFONT  cf ;
  TEXTMETRIC  tm ;

  hdc = GetDC (message_window) ;
  memset(&cf, 0, sizeof (CHOOSEFONT)) ;
  cf.lStructSize = sizeof (CHOOSEFONT) ;
  cf.hwndOwner = main_window ;
  cf.lpLogFont = &lf ;
  cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT ;
  cf.nFontType = SCREEN_FONTTYPE ;
  get_logfont (hdc, &lf) ;
  if (ChooseFont (&cf))
  {
    if ((hfont = CreateFontIndirect (&lf)) == NULL)
    {
      PovMessageBox ("Failed to create message font", "Cannot change to selected font") ;
      ReleaseDC (message_window, hdc) ;
      return ;
    }
    hfontOld = SelectObject (hdc, hfont) ;
    GetTextMetrics (hdc, &tm) ;
    message_xchar = tm.tmAveCharWidth ;
    message_ychar = tm.tmHeight + tm.tmExternalLeading ;
    SelectObject (hdc, hfontOld) ;
    DeleteObject (message_font) ;
    message_font = hfont ;
    PovInvalidateRect (message_window, NULL, TRUE) ;
    message_font_size = -MulDiv (lf.lfHeight, 72, GetDeviceCaps (hdc, LOGPIXELSY)) ;
    message_font_weight = lf.lfWeight ;
    strncpy (message_font_name, lf.lfFaceName, sizeof (message_font_name) - 1) ;
  }
  ReleaseDC (message_window, hdc) ;
}

void DragFunction (HANDLE handle)
{
  int         cFiles ;
  int         i ;
  int         modified ;
  char        szFile [_MAX_PATH] ;
  void        *Editor ;
  HDIB        hDIB ;
  BOOL        calc = 0 ;
  BITMAP      bm ;

  cFiles = DragQueryFile (handle, -1, NULL, 0) ;
  if (rendering)
    message_printf ("\n") ;
  for (i = 0 ; i < cFiles ; i++)
  {
    DragQueryFile (handle, i, szFile, sizeof (szFile)) ;
    switch (get_file_type (szFile))
    {
      case filePOV :
      case fileINI :
      case fileINC :
           if (!use_editors || !drop_to_editor)
             break ;
           if (strcmp (Edit.GetTitle (CurrentEditor, NULL, NULL, NULL, &modified), "Untitled") || modified)
           {
             Editor = Edit.GetEditor () ;
             add_window_to_tab (editor_window, Editor, "") ;
             TabCtrl_SetCurSel (tabbed_window, window_count - 1) ;
             UpdateTabbedWindow (window_count - 1, FALSE) ;
             CurrentEditor = Editor ;
             calc++ ;
           }
           Edit.SelectEditor (CurrentEditor) ;
           Edit.OpenFile (CurrentEditor, szFile, 1, 1, 1, TRUE) ;
           continue ;

      case fileBMP :
           if (screen_depth < 8)
           {
             PovMessageBox ("Tiled bitmaps not supported in this colour depth", "File ignored") ;
             continue ;
           }
           if ((hDIB = LoadDIB (szFile)) != NULL)
           {
             strcpy (background_file, szFile) ;
             DeleteObject (hBmpBackground) ;
             hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
             DeleteObject (hDIB) ;
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = TRUE ;
             PovInvalidateRect (message_window, NULL, TRUE) ;
           }
           else
             PovMessageBox ("Failed to load bitmap file", "Error") ;
           continue ;

      default :
           if (!ExternalDragFunction (szFile, dfRealDrop))
             message_printf ("Dropped file '%s' ignored (must be .POV, .INC, .INI or .BMP)\n", szFile) ;
           continue ;
    }
    if (queued_file_count < MAX_QUEUE)
    {
      strcpy (queued_files [queued_file_count++], szFile) ;
      message_printf ("File '%s' dropped as queue entry %d\n", szFile, queued_file_count) ;
    }
    else
      message_printf ("render queue full ; file '%s' ignored\n", szFile) ;
  }
  if (rendering)
    message_printf ("\n") ;
  DragFinish (handle) ;
  update_queue_status (TRUE) ;
  if (calc)
    CalculateClientWindows (TRUE) ;
}

void create_palette (RGBQUAD *rgb, int entries)
{
  int         i ;
  HDC         hdc ;
  LogPal      Palette = { 0x300, 256 } ;

  if (hPalApp)
  {
    DeleteObject (hPalApp) ;
    hPalApp = NULL ;
  }
  if (render_bitmap_depth == 8)
  {
    if (rgb)
    {
      Palette.entries = entries ;
      for (i = 0 ; i < entries ; i++, rgb++)
      {
        Palette.pe [i].peRed = rgb->rgbRed ;
        Palette.pe [i].peGreen = rgb->rgbGreen ;
        Palette.pe [i].peBlue = rgb->rgbBlue ;
        Palette.pe [i].peFlags = PC_NOCOLLAPSE ;
      }
    }
    else
    {
      // Copy the halftone palette into the DIB palette entries, and read the
      // current system palette entries to ensure we have an identity palette mapping.
      hdc = GetDC (main_window) ;
      memcpy (bitmap_template.colors, halftonePal, sizeof (halftonePal)) ;
      GetSystemPaletteEntries (hdc, 0, 256, Palette.pe) ;
      for (i = 0 ; i < 10 ; i++)
      {
        bitmap_template.colors [i].rgbRed = Palette.pe [i].peRed ;
        bitmap_template.colors [i].rgbGreen = Palette.pe [i].peGreen ;
        bitmap_template.colors [i].rgbBlue = Palette.pe [i].peBlue ;
        Palette.pe [i].peFlags = 0 ;
        bitmap_template.colors [i + 246].rgbRed = Palette.pe [i + 246].peRed ;
        bitmap_template.colors [i + 246].rgbGreen = Palette.pe [i + 246].peGreen ;
        bitmap_template.colors [i + 246].rgbBlue = Palette.pe [i + 246].peBlue ;
        Palette.pe [i + 246].peFlags = 0 ;
      }
      while (i < 246)
      {
        Palette.pe [i].peRed = bitmap_template.colors [i].rgbRed ;
        Palette.pe [i].peGreen = bitmap_template.colors [i].rgbGreen ;
        Palette.pe [i].peBlue = bitmap_template.colors [i].rgbBlue ;
        Palette.pe [i++].peFlags = PC_NOCOLLAPSE ;
      }
      ReleaseDC (main_window, hdc) ;
    }
    hPalApp = CreatePalette ((LOGPALETTE *) &Palette) ;
  }
}

BOOL create_render_bitmap (int width, int height)
{
  render_bitmap = bitmap_template ;
  render_bitmap.header.biSize = sizeof (BITMAPINFOHEADER) ;
  render_bitmap.header.biWidth = width ;
  render_bitmap.header.biHeight = height ;
  render_bitmap.header.biPlanes = 1 ;
  render_bitmap.header.biBitCount = (WORD) render_bitmap_depth ;
  render_bitmap.header.biCompression = BI_RGB ;
  if (render_bitmap_depth == 8)
  {
    // round out the bits per line to a multiple of four
    render_bitmap_bpl = (width + 3) & ~3 ;
    render_bitmap.header.biClrUsed = 256 ;
  }
  else
  {
    render_bitmap_bpl = (width * 3 + 3) & ~3 ;
    render_bitmap.header.biClrUsed = 0 ;
  }
  render_bitmap.header.biSizeImage = render_bitmap_bpl * height ;
  render_bitmap.header.biClrImportant = 0 ;
  return ((render_bitmap_surface = calloc (1, render_bitmap.header.biSizeImage)) != NULL) ;
}

BOOL create_render_window (void)
{
  unsigned              flags ;
  RECT                  rect ;

  renderwin_destroyed = FALSE ;
  renderwin_xoffset = renderwin_yoffset = 0 ;
  rect.left = 0 ;
  rect.top = 0 ;
  rect.right = render_width ;
  rect.bottom = render_height ;
  flags = WS_OVERLAPPEDWINDOW ;

  if (IsIconic (main_window) && render_above_main)
    render_main_icon = TRUE ;

  if (renderwin_effects)
  {
    flags |= (render_width < render_height) ? IBS_VERTCAPTION : IBS_HORZCAPTION ;
    ibAdjustWindowRect (&rect, flags, CAPTIONXY) ;
  }
  else
    AdjustWindowRect (&rect, flags, FALSE) ;

  // left and top will probably be negative
  renderwin_max_width = rect.right - rect.left ;
  renderwin_max_height = rect.bottom - rect.top ;

  if (renderwin_left < 0)
    renderwin_left = 0 ;
  if (renderwin_top < 0)
    renderwin_top = 0 ;
  if (renderwin_left - screen_width / 8 + renderwin_max_width > screen_width)
    renderwin_left = screen_width - screen_width / 8 ;
  if (renderwin_top - screen_height / 8 + renderwin_max_height > screen_height)
    renderwin_top = screen_height - screen_height / 8 ;

  if ((render_window = CreateWindowEx (0,
                                       PovRenderWinClass,
                                       "POVRAY",
                                       flags | renderwin_flags,
                                       renderwin_left,
                                       renderwin_top,
                                       renderwin_max_width,
                                       renderwin_max_height,
                                       render_above_main ? main_window : NULL,
                                       NULL,
                                       hInstance,
                                       NULL)) == NULL)
  {
    return (FALSE) ;
  }

  if (!render_main_icon && !isMaxiMinimized && !main_window_hidden)
  {
    flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS | SWP_SHOWWINDOW | SWP_NOACTIVATE ;
    if (GetForegroundWindow () == main_window && render_above_main)
      flags &= ~SWP_NOACTIVATE ;
    if (renderwin_active)
      flags &= ~SWP_NOACTIVATE ;
    SetWindowPos (render_window, main_window, 0, 0, 0, 0, flags) ;
  }

  return (TRUE) ;
}

int renderwin_init (void)
{
  display_cleanup () ;
  if (create_render_window () == FALSE)
  {
    PovMessageBox ("Failed to create display window", "Initialise Display - Fatal Error") ;
    display_cleanup () ;
    return (1) ;
  }
  if (create_render_bitmap (render_width, render_height) == FALSE)
  {
    PovMessageBox ("Failed to allocate display bitmap", "Initialise Display - Fatal Error") ;
    display_cleanup () ;
    return (1) ;
  }
  return (0) ;
}

void WIN_Display_Init (int x, int y)
{
  render_width = x ;
  render_height = y ;
  if (!use_threads)
  {
    display_cleanup () ;
    if (create_render_window () == FALSE)
    {
      PovMessageBox ("Failed to create display window", "Initialise Display - Fatal Error") ;
      display_cleanup () ;
      Terminate_POV (-1) ;
    }
    if (create_render_bitmap (render_width, render_height) == FALSE)
    {
      PovMessageBox ("Failed to allocate display bitmap", "Initialise Display - Fatal Error") ;
      display_cleanup () ;
      Terminate_POV (-1) ;
    }
  }
  else
    if (SendMessage (main_window, CREATE_RENDERWIN_MESSAGE, 0, 0L))
      Terminate_POV (-1) ;
  WIN_Display_Plot (-1, -1, 0, 0, 0, 0) ;
  ExternalEvent (EventDisplayInit, MAKELONG (x, y)) ;
}

void WIN_Display_Finished (void)
{
  PovInvalidateRect (render_window, NULL, FALSE) ;
  ExternalEvent (EventDisplayFinished, 0) ;
}

void WIN_Display_Close (void)
{
  ExternalEvent (EventDisplayClose, 0) ;
}

void WIN_Display_Plot (int x, int y, int Red, int Green, int Blue, int Alpha)
{
  int                   oldMode ;
  char                  str [128] ;
  uchar                 *p ;
  uchar                 dither ;
  HDC                   hdc ;
  RECT                  rect ;
  static int            lastx = 0, lasty = 0 ;
  static char           laststr [128] ;

  ExternalDisplayPlot (x, y, Red, Green, Blue, Alpha) ;
  if (x == -1 || y == -1)
  {
    lastx = lasty = 0 ;
    laststr [0] = '\0' ;
    return ;
  }
  if (x < 0 || x >= render_width || y < 0 || y >= render_height)
    return ;
  if (render_bitmap_surface == NULL)
    return ;
  percentage_complete = (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1) ;
  if (render_window != NULL && (x < lastx || y != lasty))
  {
    // Blt the completed scanline to the display
    if (render_main_icon)
    {
      if ((hdc = GetDC (main_window)) == NULL)
        return ;
      oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
      if (hPalApp)
      {
        SelectPalette (hdc, hPalApp, FALSE) ;
        RealizePalette (hdc) ;
      }
      SelectClipRgn (hdc, NULL) ;
      StretchDIBits (hdc, 0,
                          percentage_complete * 36 / 100,
                          36,
                          1,
                          0,
                          render_bitmap.header.biHeight - 1 - (percentage_complete * render_bitmap.header.biHeight / 100),
                          render_bitmap.header.biWidth,
                          (render_bitmap.header.biHeight + 35) / 36,
                          render_bitmap_surface,
                          (LPBITMAPINFO) &render_bitmap,
                          DIB_RGB_COLORS,
                          SRCCOPY) ;
      SetStretchBltMode (hdc, oldMode) ;
      ReleaseDC (main_window, hdc) ;
    }
    else
    {
      if ((hdc = GetDC (render_window)) == NULL)
        return ;
      oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
      if (hPalApp)
      {
        SelectPalette (hdc, hPalApp, FALSE) ;
        RealizePalette (hdc) ;
      }
      if (IsIconic (render_window))
      {
        SelectClipRgn (hdc, NULL) ;
        StretchDIBits (hdc, 0,
                            percentage_complete * 36 / 100,
                            36,
                            1,
                            0,
                            render_bitmap.header.biHeight - 1 - (percentage_complete * render_bitmap.header.biHeight / 100),
                            render_bitmap.header.biWidth,
                            (render_bitmap.header.biHeight + 35) / 36,
                            render_bitmap_surface,
                            (LPBITMAPINFO) &render_bitmap,
                            DIB_RGB_COLORS,
                            SRCCOPY) ;
        sprintf (str, "POV %d%%", (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1)) ;
        SetWindowText (render_window, str) ;
      }
      else
      {
        if (IsZoomed (render_window))
        {
          GetClientRect  (render_window, &rect) ;
          StretchDIBits (hdc,
                         0,
                         (rect.bottom * lasty + render_height - 1) / render_height,
                         rect.right,
                         (rect.bottom + render_height - 1) / render_height,
                         0,
                         render_bitmap.header.biHeight - 1 - lasty,
                         render_bitmap.header.biWidth,
                         1,
                         render_bitmap_surface,
                         (LPBITMAPINFO) &render_bitmap,
                         DIB_RGB_COLORS,
                         SRCCOPY) ;
        }
        else
        {
          StretchDIBits (hdc,
                         -renderwin_xoffset,
                         lasty - renderwin_yoffset,
                         render_width,
                         1,
                         0,
                         render_bitmap.header.biHeight - 1 - lasty,
                         render_bitmap.header.biWidth,
                         1,
                         render_bitmap_surface,
                         (LPBITMAPINFO) &render_bitmap,
                         DIB_RGB_COLORS,
                         SRCCOPY) ;
        }
        sprintf (str, "%d%% complete", (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1)) ;
      }
      if (strcmp (str, laststr) != 0)
      {
        SetWindowText (render_window, str) ;
        strcpy (laststr, str) ;
      }
      SetStretchBltMode (hdc, oldMode) ;
      ReleaseDC (render_window, hdc) ;
    }
  }
  lastx = x ;
  lasty = y ;
  if (render_bitmap_depth == 8)
  {
    p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x ;
    dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
    *p = 20 + div51 [Red] + (mod51 [Red] > dither) +
              mul6 [div51 [Green] + (mod51 [Green] > dither)] +
              mul36 [div51 [Blue] + (mod51 [Blue] > dither)] ;
  }
  else
  {
    p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x * 3 ;
    p [0] = Blue ;
    p [1] = Green ;
    p [2] = Red ;
  }
}

void WIN_Display_Plot_Rect (int x1, int x2, int y1, int y2, int Red, int Green, int Blue, int Alpha)
{
  int         x ;
  int         y ;
  int         width ;
  int         height ;
  int         oldMode ;
  uchar       *p ;
  uchar       dither ;
  HDC         hdc ;
  RECT        rect ;

  ExternalDisplayPlotRect (x1, y1, x2, y2, Red, Green, Blue, Alpha) ;
  if (render_bitmap_surface == NULL)
    return ;
  if (x1 < 0 || x1 >= render_width || x2 < 0 || x2 >= render_width)
    return ;
  if (y1 < 0 || y1 >= render_height || y2 < 0 || y2 >= render_height)
    return ;
  if (x1 == x2 && y1 == y2)
  {
    WIN_Display_Plot (x1, y1, Red, Green, Blue, Alpha) ;
    return ;
  }
  for (y = y1 ; y <= y2 ; y++)
  {
    if (render_bitmap_depth == 8)
    {
      p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x1 ;
      for (x = x1 ; x <= x2 ; x++)
      {
        dither = dither8x8 [((x & 7) << 3) | (y & 7)] ;
        *p++ = 20 + div51 [Red] + (mod51 [Red] > dither) +
                    mul6 [div51 [Green] + (mod51 [Green] > dither)] +
                    mul36 [div51 [Blue] + (mod51 [Blue] > dither)] ;
      }
    }
    else
    {
      p = render_bitmap_surface + (render_bitmap.header.biHeight - 1 - y) * render_bitmap_bpl + x1 * 3 ;
      for (x = x1 ; x <= x2 ; x++)
      {
        *p++ = Blue ;
        *p++ = Green ;
        *p++ = Red ;
      }
    }
  }
  if (render_window != NULL && !IsIconic (main_window) && !IsIconic (render_window))
  {
    hdc = GetDC (render_window) ;
    oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
    if (hPalApp)
    {
      SelectPalette (hdc, hPalApp, FALSE) ;
      RealizePalette (hdc) ;
    }
    if (IsZoomed (render_window))
    {
      GetClientRect  (render_window, &rect) ;
      x = rect.right * x1 / render_width ;
      y = rect.bottom * y1 / render_height ;
      width = (rect.right * (x2 - x1 + 1) + render_width - 1) / render_width ;
      height = (rect.bottom * (y2 - y1 + 1) + render_height - 1) / render_height ;
    }
    else
    {
      x = x1 ;
      y = y1 ;
      width = x2 - x1 + 1 ;
      height = y2 - y1 + 1 ;
    }
    StretchDIBits (hdc,
                   x - renderwin_xoffset,
                   y - renderwin_yoffset,
                   width,
                   height,
                   x1,
                   render_bitmap.header.biHeight - 1 - y2,
                   x2 - x1 + 1,
                   y2 - y1 + 1,
                   render_bitmap_surface,
                   (LPBITMAPINFO) &render_bitmap,
                   DIB_RGB_COLORS,
                   SRCCOPY) ;
    SetStretchBltMode (hdc, oldMode) ;
    ReleaseDC (render_window, hdc) ;
  }
}

void WIN_Pre_Pixel (int x, int y, COLOUR colour)
{
  char                  str1 [_MAX_PATH] ;
  char                  str2 [128] ;
  static int			lastY = -1;

  ExternalWinPrePixel (x, y, colour) ;
  if (need_output_filename)
  {
    WritePrivateProfileString ("LastRender", "HistogramFile", get_full_name (opts.Histogram_File_Name), EngineIniFileName) ;
    if (opts.Output_Path [0] != '\0')
    {
      sprintf (str1, "%s%s", opts.Output_Path, opts.Output_File_Name) ;
      WritePrivateProfileString ("LastRender", "OutputFile", str1, EngineIniFileName) ;
    }
    else
      WritePrivateProfileString ("LastRender", "OutputFile", get_full_name (opts.Output_File_Name), EngineIniFileName) ;
    need_output_filename = 0 ;
  }

  currentX = x ;

  /* Short circuit unless scanline has changed */
  if (y == -1)
  {
	lastY = -1;
	return;
  }
  if (y == lastY)
	return;

  lastY = currentY = y ;

  percentage_complete = (y - opts.First_Line + 1) * 100 / (opts.Last_Line - opts.First_Line + 1) ;
  if (!IsIconic (main_window))
  {
    splitpath (opts.Input_File_Name, NULL, str2) ;
    if (input_file_in_memory)
      strcat (str2, " (In Memory)") ;
	sprintf (str1, "%s:line %d (of %d)", str2, y - opts.First_Line + 1, opts.Last_Line - opts.First_Line) ;
  }
  else
	sprintf (str1, "POV %d%%", percentage_complete) ;
  SetWindowText (main_window, str1) ;
}

void WIN_Post_Pixel (int x, int y, COLOUR colour)
{
  ExternalWinPostPixel (x, y, colour) ;
  pixels++ ;
}

void WIN_Banner (char *s)
{
  buffer_message (mBanner, s) ;
}

void WIN_Warning (char *s)
{
  buffer_message (mWarning, s) ;
}

void WIN_Render_Info (char *s)
{
  buffer_message (mRender, s) ;
}

void WIN_Status_Info (char *s)
{
  if (!non_client)
    buffer_message (mStatus, s) ;
  if (strlen (s) > 3)
    say_status_message (StatusMessage, clean_str (s)) ;
}

void WIN_Debug_Info (char *s)
{
  buffer_message (mDebug, s) ;
}

void WIN_Fatal (char *s)
{
  buffer_message (mFatal, s) ;
}

void WIN_Statistics (char *s)
{
  buffer_message (mStatistics, s) ;
}

void WIN_Startup (void)
{
//if (!keep_messages)
//  clear_messages () ;
  ExternalEvent (EventWinStartup, 0) ;
}

void WIN_Finish (int n)
{
  if (keep_messages)
    buffer_message (None, "\n") ;
  povray_return_code = n ;
  ExternalEvent (EventWinFinish, n) ;
  longjmp (jump_buffer, 1) ;
}

// priority == 0 when called from a higher level (e.g. in Trace_Pixel()).
void WIN_Cooperate (int priority)
{
  MSG                   msg ;

  while (rendersleep && !(quit || stop_rendering))
  {
    if (IsWin32)
      Sleep (100) ;
    if (!use_threads)
    {
      while (quit == FALSE && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
      {
        if (!TranslateAccelerator (main_window, hAccelerators, &msg))
        {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
        }
      }
    }
    ExternalEvent (EventWinCooperate, priority) ;
  }

  ExternalEvent (EventWinCooperate, priority) ;

  if (terminating == FALSE && (quit || stop_rendering))
  {
    terminating = TRUE ;
    // warning - this function call doesn't return !
    Terminate_POV (-1) ;
  }

  if (use_threads)
    return ;

  if (render_lock_up == TRUE)
    return ;

  if (niceness > STOP_BEING_NICE && priority == 1)
    return ;

  while (quit == FALSE && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
  {
    if (!TranslateAccelerator (main_window, hAccelerators, &msg))
    {
      TranslateMessage (&msg) ;
      DispatchMessage (&msg) ;
    }
  }

  if (terminating == FALSE && (quit || stop_rendering))
  {
    terminating = TRUE ;
    // warning - this function call doesn't return !
    Terminate_POV (-1) ;
  }
}

int WIN_Povray (int argc, char **argv)
{
  int         rval ;

  povray_return_code = -1 ;
  if ((rval = setjmp (jump_buffer)) != 0)
  {
    if (povray_return_code == 0)
    {
      PutPrivateProfileInt ("Statistics", "FinishRender", time (NULL), CurrentRerunFileName) ;
      buffer_message (None, "Returned from renderer\n") ;
      rotate_rerun_entries () ;
      buffer_message (None, "POV-Ray finished\n") ;
    }
    else
    {
      buffer_message (None, "\n") ;
      buffer_message (None, "Returned from renderer (non-zero return value)\n") ;
      close_all () ;
      mem_release_all (FALSE) ;
      pre_init_flag = 0 ;
    }
  }
  else
  {
    // setjmp succeeded, so we call POV-Ray. we expect POV to return via a longjmp always.
    rendering = TRUE ;
    terminating = FALSE ;
    render_start_time = time (NULL) ;
    pixels = 0 ;
    alt_main (argc, argv) ;
    assert (FALSE) ;
  }
  render_finish_time = time (NULL) ;
  rendering = FALSE ;
  terminating = FALSE ;
  if (render_finish_time != render_start_time)
    status_printf (StatusPPM, "%u PPS", pixels / (render_finish_time - render_start_time)) ;
  if (render_main_icon || isMaxiMinimized)
    PostMessage (main_window, WM_NCPAINT, 0, 0L) ;
  say_status_message (StatusRendertime, get_elapsed_time (render_start_time, render_finish_time)) ;
  running_demo = FALSE ;
  buffer_message (mDivider, "\n") ;
  add_rerun_to_menu () ;
  fcloseall () ;
  if (debugFile)
    debugFile = fopen ("c:\\povray.dbg", "at") ;
  return (rval) ;
}

int WIN_System (char *s)
{
  int                   n ;
  char                  *message = "POV-Ray is running an external application. Do you want to halt this application ?\n\n"
                                   "If you specify YES this application will be terminated, but note that this could lead\nto an incomplete clean-up. "
                                   "The use of this facility is not recommended.\n\n"
                                   "If you specify NO the application will remain running\n\nIf you specify CANCEL POV-Ray will continue rendering." ;
  MSG                   msg ;
  DWORD                 code = WAIT_TIMEOUT ;
  STARTUPINFO           startupInfo ;
  PROCESS_INFORMATION   procInfo ;

  if (noexec)
  {
    message_printf ("External exec request '%s' made but NOEXEC set\n", s) ;
    MessageBox (main_window, "External exec request made but NOEXEC set\n\nPOV-Ray will now exit", "Security warning", MB_ICONSTOP) ;
    quit = TRUE ;
    return (-1) ;
  }

  if (ExternalWinSystem (s, &n))
    return (n) ;

  buffer_message (mHorzLine, "\n") ;
  buffer_message (None, "POV-Ray is creating another process\n") ;
  message_printf ("  %s\n", s) ;

  startupInfo.cb               = sizeof (STARTUPINFO) ;
  startupInfo.lpReserved       = 0 ;
  startupInfo.lpDesktop        = NULL ;
  startupInfo.lpTitle          = NULL ;
  startupInfo.dwX              = 0 ;
  startupInfo.dwY              = 0 ;
  startupInfo.dwXSize          = 0 ;
  startupInfo.dwYSize          = 0 ;
  startupInfo.dwXCountChars    = 0 ;
  startupInfo.dwYCountChars    = 0 ;
  startupInfo.dwFillAttribute  = 0 ;
  startupInfo.dwFlags          = system_noactive ? STARTF_USESHOWWINDOW : 0 ;
  startupInfo.wShowWindow      = SW_SHOWMINNOACTIVE ;
  startupInfo.cbReserved2      = 0 ;
  startupInfo.lpReserved2      = 0 ;

  if (CreateProcess (NULL, s, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &procInfo) == FALSE)
  {
    buffer_message (None, "Could not create process\n") ;
    buffer_message (None, "\n") ;
    return (GetLastError ()) ;
  }

  if (no_shellout_wait == 0)
  {
    if (IsWin32)
    {
      // now wait for the process to exit
      while (code == WAIT_TIMEOUT && no_shellout_wait == 0)
      {
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
          if (!TranslateAccelerator (main_window, hAccelerators, &msg))
          {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
          }
        }
        code = WaitForSingleObject (procInfo.hProcess, 25) ;
        if (quit || stop_rendering)
        {
          GetExitCodeProcess (procInfo.hProcess, &code) ;
          if (code == STILL_ACTIVE)
          {
            // find out what the user wants to do with the process
            switch (MessageBox (main_window, message, quit ? "Exit POV-Ray" : "Stop rendering", MB_ICONQUESTION | MB_APPLMODAL | MB_YESNOCANCEL | MB_DEFBUTTON2))
            {
              case IDYES :
                   TerminateProcess (procInfo.hProcess, 1) ;
                   break ;

              case IDNO :
                   break ;

              case IDCANCEL :
                   quit = FALSE ;
                   stop_rendering = FALSE ;
                   code = WAIT_TIMEOUT ;
                   continue ;
            }
          }
          CloseHandle (procInfo.hProcess) ;
          CloseHandle (procInfo.hThread) ;
          buffer_message (mHorzLine, "\n") ;
          Terminate_POV (-1) ;
        }
      }
      GetExitCodeProcess (procInfo.hProcess, &code) ;
    }
    else
      MessageBox (main_window,
                  "Due to limitations in Microsoft's implementation of Win32s, "
                  "POV-Ray cannot (easily) detect when the shellout program specified "
                  "for this render has completed execution.\n\n"
                  "Please press the OK button when the program has completed "
                  "execution, or press it now if it is not necessary to wait for it to finish.\n\n"
                  "It is not possible to retrieve the return code.\n\n"
                  "You may disable this dialog by selecting No Shellout Wait in the Render menu",
                  "Partially-supported Feature",
                  MB_ICONSTOP) ;
  }
  else
    code = 0 ;

  // clean up
  CloseHandle (procInfo.hProcess) ;
  CloseHandle (procInfo.hThread) ;
  buffer_message (mHorzLine, "\n") ;

  // code now has the application's return code
  return (code) ;
}

void WIN_SaveRerun (void)
{
  write_rerun_information () ;
  PutPrivateProfileInt ("Statistics", "StartRender", time (NULL), CurrentRerunFileName) ;
}

void WIN_ParseINI (void)
{
  if (!ignore_auto_ini)
  {
    parse_ini_file (DefaultRenderIniFileName) ;
    if (getenv("POVINI") == NULL)
      parse_ini_file ("povray.ini");
  }
}

void WIN_PrintOtherCredits (void)
{
  buffer_message (None, "Base Windows version by Christopher J. Cason\n") ;
  buffer_message (None, "This is an UNSUPPORTED UNOFFICIAL COMPILE by " UNOFFICIALCOMPILE "\n") ;
}

void PovMessageBox (char *message, char *title)
{
  MessageBox (main_window, message, title, MB_ICONEXCLAMATION) ;
}

void detect_graphics_config (void)
{
  HDC   hdc ;

  hdc = GetDC (NULL) ;
  screen_depth = GetDeviceCaps (hdc, BITSPIXEL) ;
  render_bitmap_depth = (GetDeviceCaps (hdc, BITSPIXEL) > 8 && renderwin_8bits == 0) ? 24 : 8 ;
  screen_width = GetDeviceCaps (hdc, HORZRES) ;
  screen_height = GetDeviceCaps (hdc, VERTRES) ;
  ReleaseDC (NULL, hdc) ;
}

// Clear the system palette when we start to ensure an identity palette mapping
void clear_system_palette (void)
{
  int         Counter ;
  HDC         ScreenDC ;
  LogPal      Palette = { 0x300, 256 } ;
  HPALETTE    ScreenPalette ;

  // Reset everything in the system palette to black
  for (Counter = 0 ; Counter < 256 ; Counter++)
  {
    Palette.pe [Counter].peRed = 0 ;
    Palette.pe [Counter].peGreen = 0 ;
    Palette.pe [Counter].peBlue = 0 ;
    Palette.pe [Counter].peFlags = PC_NOCOLLAPSE ;
  }

  // Create, select, realize, deselect, and delete the palette
  ScreenDC = GetDC (NULL) ;
  ScreenPalette = CreatePalette ((LOGPALETTE *) &Palette) ;
  if (ScreenPalette)
  {
    ScreenPalette = SelectPalette (ScreenDC, ScreenPalette, FALSE) ;
    RealizePalette (ScreenDC) ;
    ScreenPalette = SelectPalette (ScreenDC, ScreenPalette, FALSE) ;
    DeleteObject (ScreenPalette) ;
  }
  ReleaseDC (NULL, ScreenDC) ;
}

BOOL FAR PASCAL PovAboutDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  struct stat st ;

  switch (message)
  {
    case WM_INITDIALOG :
         CenterWindowRelative ((HWND) lParam, hDlg, TRUE) ;
         FitWindowInWindow (NULL, hDlg) ;
         SendMessage (GetDlgItem (hDlg, IDC_VERSION), WM_SETTEXT, 0, (LPARAM) "Version " POV_RAY_VERSION COMPILER_VER "." PVENGINE_VER " [" OPTIMISATION " optimised]") ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
           case IDCANCEL :
                EndDialog (hDlg, TRUE) ;
                return (TRUE) ;

           case IDC_READPOVLEGAL :
                // if help file is missing or something, default to internal viewer
                if (stat (engineHelpPath, &st) == 0)
                  WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 0L) ;
                else
                  DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), hDlg, (DLGPROC) PovLegalDialogProc, (LPARAM) hDlg) ;
                return (TRUE) ;

           case IDC_SAVEPOVLEGAL :
                save_povlegal () ;
                return (TRUE) ;

           case IDC_ACCESSORIES :
                WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 4L) ;
                return (TRUE) ;
         }
         break ;
  }
  return (FALSE) ;
}

BOOL FAR PASCAL PovCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char        *s ;
  char        str [_MAX_PATH * 2] ;
  char        str1 [_MAX_PATH] ;
  char        str2 [_MAX_PATH] ;
  void        *editor ;
  struct stat statbuf1 ;
  struct stat statbuf2 ;
  static char editINIname [_MAX_PATH] ;
  static char editFilename [_MAX_PATH] ;
  static char lastIniFilename [_MAX_PATH] ;

  switch (message)
  {
    case WM_INITDIALOG :
         if (use_editors)
         {
           if (current_window == editor_window)
           {
             EnableWindow (GetDlgItem (hDlg, IDC_PRESETSOURCEPATH), FALSE) ;
             EnableWindow (GetDlgItem (hDlg, IDC_PRESETSOURCENAME), FALSE) ;
             EnableWindow (GetDlgItem (hDlg, IDC_SOURCEDEFAULT), FALSE) ;
             EnableWindow (GetDlgItem (hDlg, IDC_BROWSESOURCEFILE), FALSE) ;
             EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), FALSE) ;
             s = Edit.GetTitle (CurrentEditor, NULL, NULL, NULL, NULL) ;
             splitpath (s, lastRenderPath, lastRenderName) ;
             EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), FALSE) ;
          }
           SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath) ;
           SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName) ;
         }
         else
         {
           SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath) ;
           SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName) ;
           EnableWindow (GetDlgItem (hDlg, IDC_EDITRENDER), FALSE) ;
           EnableWindow (GetDlgItem (hDlg, IDC_EDITINI), FALSE) ;
         }
         SendDlgItemMessage (hDlg, IDC_PRESETSOURCENAME, EM_LIMITTEXT, 64, 0L) ;
         SendDlgItemMessage (hDlg, IDC_INIFILENAME, EM_LIMITTEXT, 64, 0L) ;
         strupr (SecondaryRenderIniFileName) ;
//       strupr (lastRenderName) ;
//       strupr (lastRenderPath) ;
         validatePath (lastRenderPath) ;
         CenterWindowRelative ((HWND) lParam, hDlg, TRUE) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetDlgItemText (hDlg, IDC_COMMANDLINE, command_line) ;
         splitpath (SecondaryRenderIniFileName, str1, str2) ;
         validatePath (str1) ;
         strcpy (editINIname, str2) ;
         SetDlgItemText (hDlg, IDC_INIFILEPATH, str1) ;
         SetDlgItemText (hDlg, IDC_INIFILENAME, str2) ;
         extract_ini_sections (SecondaryRenderIniFileName, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
         SendMessage (toolbar_combobox, WM_GETTEXT, sizeof (SecondaryRenderIniFileSection), (LPARAM) SecondaryRenderIniFileSection) ;
         SendDlgItemMessage (hDlg, IDC_INIFILESECTION, CB_SELECTSTRING, -1, (LPARAM) SecondaryRenderIniFileSection) ;
         strcpy (lastIniFilename, SecondaryRenderIniFileName) ;
         stat (SecondaryRenderIniFileName, &statbuf1) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_INIFILESECTION :
                if (HIWORD (wParam) == CBN_SETFOCUS)
                {
                  stat (lastIniFilename, &statbuf2) ;
                  if (statbuf1.st_atime != statbuf2.st_atime)
                  {
                    statbuf1 = statbuf2 ;
                    GetDlgItemText (hDlg, IDC_INIFILESECTION, str, sizeof (str)) ;
                    extract_ini_sections (lastIniFilename, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    SendDlgItemMessage (hDlg, IDC_INIFILESECTION, CB_SELECTSTRING, -1, (LPARAM) str) ;
                  }
                  return (TRUE) ;
                }
                return (FALSE) ;

           case IDC_EDITRENDER :
                GetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcat (str1, "\\") ;
                strcat (str1, str2) ;
                if ((editor = Edit.GetEditor ()) != NULL)
                {
                  CurrentEditor = editor ;
                  Edit.SelectEditor (editor) ;
                  add_window_to_tab (editor_window, editor, "") ;
                  if (!Edit.OpenFile (editor, str1, 0, 1, 1, FALSE))
                  {
                    Edit.ReleaseEditor (editor) ;
                    TabCtrl_DeleteItem (tabbed_window, --window_count) ;
                  }
                  else
                  {
                    Edit.SelectEditor (CurrentEditor) ;
                    TabCtrl_SetCurSel (tabbed_window, window_count - 1) ;
                    UpdateTabbedWindow (window_count - 1, FALSE) ;
                    EndDialog (hDlg, FALSE) ;
                    return (TRUE) ;
                  }
                }
                return (TRUE) ;

           case IDC_EDITINI :
                GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcat (str1, "\\") ;
                strcat (str1, str2) ;
                if ((editor = Edit.GetEditor ()) != NULL)
                {
                  CurrentEditor = editor ;
                  Edit.SelectEditor (editor) ;
                  add_window_to_tab (editor_window, editor, "") ;
                  if (!Edit.OpenFile (editor, str1, 0, 1, 1, FALSE))
                  {
                    Edit.ReleaseEditor (editor) ;
                    TabCtrl_DeleteItem (tabbed_window, --window_count) ;
                  }
                  else
                  {
                    Edit.SelectEditor (CurrentEditor) ;
                    TabCtrl_SetCurSel (tabbed_window, window_count - 1) ;
                    UpdateTabbedWindow (window_count - 1, FALSE) ;
                    EndDialog (hDlg, FALSE) ;
                    return (TRUE) ;
                  }
                }
                return (TRUE) ;

           case IDC_BROWSEINIFILE :
                if ((s = get_ini_file (hDlg, lastSecondaryIniFilePath)) != NULL)
                {
                  strupr (s) ;
                  splitpath (s, str1, str2) ;
                  validatePath (str1) ;
                  SetDlgItemText (hDlg, IDC_INIFILEPATH, str1) ;
                  SetDlgItemText (hDlg, IDC_INIFILENAME, str2) ;
                  if (strcmp (s, lastIniFilename))
                  {
                    extract_ini_sections (s, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    strcpy (lastIniFilename, s) ;
                    stat (lastIniFilename, &statbuf1) ;
                  }
                }
                return (TRUE) ;

           case IDC_INIFILENAME :
                if (HIWORD (wParam) == EN_KILLFOCUS)
                {
                  GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                  GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                  validatePath (str1) ;
                  joinPath (str, str1, str2) ;
                  if (stricmp (str, lastIniFilename))
                  {
                    extract_ini_sections (str, GetDlgItem (hDlg, IDC_INIFILESECTION)) ;
                    strcpy (lastIniFilename, str) ;
                  }
                  return (TRUE) ;
                }
                if (HIWORD (wParam) == EN_UPDATE)
                {
                  GetDlgItemText (hDlg, IDC_INIFILENAME, str, sizeof (str)) ;
                  if (strchr (str, '\\') != NULL)
                    SetDlgItemText (hDlg, IDC_INIFILENAME, editINIname) ;
                  else
                    strcpy (editINIname, str) ;
                }
                return (TRUE) ;

           case IDC_RESETINI :
                SetDlgItemText (hDlg, IDC_INIFILENAME, "") ;
                GetDlgItemText (hDlg, IDC_INIFILEPATH, lastIniFilename, sizeof (lastIniFilename)) ;
                SendMessage (GetDlgItem (hDlg, IDC_INIFILESECTION), CB_RESETCONTENT, 0, 0L) ;
                return (TRUE) ;

           case IDC_INIDEFAULT :
                sprintf (str, "%sRENDERER", HomePath) ;
                SetDlgItemText (hDlg, IDC_INIFILEPATH, str) ;
                SetDlgItemText (hDlg, IDC_INIFILENAME, "QUICKRES.INI") ;
                SendMessage (hDlg, WM_COMMAND, (EN_KILLFOCUS << 16) | IDC_INIFILENAME, 0L) ;
                return (TRUE) ;

           case IDC_PRESETSOURCENAME :
                if (HIWORD (wParam) == EN_UPDATE)
                {
                  GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str, sizeof (str)) ;
                  if (strchr (str, '\\') != NULL)
                    SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, editFilename) ;
                  else
                    strcpy (editFilename, str) ;
                }
                return (TRUE) ;

           case IDC_BROWSESOURCEFILE :
                if ((s = file_open (hDlg)) != NULL)
                {
                  splitpath (s, str1, str2) ;
                  validatePath (str1) ;
                  SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str1) ;
                  SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, str2) ;
                }
                return (TRUE) ;

           case IDC_SOURCEDEFAULT :
                sprintf (str, "%sPOV3DEMO\\OBJECTS", HomePath) ;
                SetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, str) ;
                SetDlgItemText (hDlg, IDC_PRESETSOURCENAME, "TORUS1.POV") ;
                return (TRUE) ;

           case IDC_SET :
           case IDC_RENDER :
                if (!running_demo)
                {
                  GetDlgItemText (hDlg, IDC_PRESETSOURCEPATH, lastRenderPath, sizeof (lastRenderPath)) ;
                  GetDlgItemText (hDlg, IDC_PRESETSOURCENAME, lastRenderName, sizeof (lastRenderName)) ;
                  validatePath (lastRenderPath) ;
//                strupr (lastRenderPath) ;
//                strupr (lastRenderName) ;
                  joinPath (source_file_name, lastRenderPath, lastRenderName) ;
                }
                GetDlgItemText (hDlg, IDC_INIFILEPATH, str1, sizeof (str1)) ;
                GetDlgItemText (hDlg, IDC_INIFILENAME, str2, sizeof (str2)) ;
                validatePath (str1) ;
                strcpy (lastSecondaryIniFilePath, str1) ;
                joinPath (SecondaryRenderIniFileName, str1, str2) ;
                strupr (SecondaryRenderIniFileName) ;
                GetDlgItemText (hDlg, IDC_INIFILESECTION, SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection)) ;
                GetDlgItemText (hDlg, IDC_COMMANDLINE, command_line, sizeof (command_line)) ;
                extract_ini_sections (SecondaryRenderIniFileName, toolbar_combobox) ;
                SendMessage (toolbar_combobox, CB_SELECTSTRING, -1, (LPARAM) SecondaryRenderIniFileSection) ;

                // return TRUE only if IDC_RENDER was selected
                EndDialog (hDlg, LOWORD (wParam) == IDC_RENDER) ;
                return (TRUE) ;

           case IDC_CONTEXTHELP :
                WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 1L) ;
                return (TRUE) ;

           case IDCANCEL :
                EndDialog (hDlg, FALSE) ;
                return (TRUE) ;
         }
         break ;
  }
  return (FALSE) ;
}

BOOL FAR PASCAL PovShortCommandLineDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG :
         CenterWindowRelative ((HWND) lParam, hDlg, TRUE) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetDlgItemText (hDlg, IDC_COMMANDLINE, command_line) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDC_RENDER :
                GetDlgItemText (hDlg, IDC_COMMANDLINE, command_line, sizeof (command_line) - 1) ;
                EndDialog (hDlg, TRUE) ;
                return (TRUE) ;

           case IDC_SET :
                GetDlgItemText (hDlg, IDC_COMMANDLINE, command_line, sizeof (command_line) - 1) ;
                EndDialog (hDlg, FALSE) ;
                return (TRUE) ;

           case IDCANCEL :
                EndDialog (hDlg, FALSE) ;
                return (TRUE) ;

           case IDC_COMMANDHELP :
                WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 1L) ;
                return (TRUE) ;
         }
         break ;
  }
  return (FALSE) ;
}

BOOL FAR PASCAL PovFileQueueDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   i ;
  char                  str [64] ;
  HWND                  hlb ;
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_CTLCOLORLISTBOX :
         return ((BOOL) hbr) ;

    case WM_INITDIALOG :
         CenterWindowRelative ((HWND) lParam, hDlg, TRUE) ;
         FitWindowInWindow (NULL, hDlg) ;
         hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         for (i = 0 ; i < queued_file_count ; i++)
           SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) queued_files [i]) ;
         sprintf (str, "Queue has %d entr%s", queued_file_count, queued_file_count != 1 ? "ies" : "y") ;
         SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
         CheckDlgButton (hDlg, IDC_RELOADQUEUE, GetPrivateProfileInt ("FileQueue", "ReloadOnStartup", 0, EngineIniFileName)) ;
         CheckDlgButton (hDlg, IDC_AUTORENDER, auto_render) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                queued_file_count = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                if (queued_file_count > MAX_QUEUE)
                  queued_file_count = MAX_QUEUE ;
                for (i = 0 ; i < queued_file_count ; i++)
                  SendMessage (hlb, LB_GETTEXT, i, (LPARAM) queued_files [i]) ;
                auto_render = IsDlgButtonChecked (hDlg, IDC_AUTORENDER) ;
                PVCheckMenuItem (CM_AUTORENDER, auto_render ? MF_CHECKED : MF_UNCHECKED) ;
                PutPrivateProfileInt ("FileQueue", "ReloadOnStartup", IsDlgButtonChecked (hDlg, IDC_RELOADQUEUE), EngineIniFileName) ;
                update_queue_status (TRUE) ;
                DeleteObject (hbr) ;
                EndDialog (hDlg, TRUE) ;
                return (TRUE) ;

           case IDCANCEL :
                DeleteObject (hbr) ;
                EndDialog (hDlg, TRUE) ;
                return (TRUE) ;

           case IDC_DELETEFILE :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                if ((i = SendMessage (hlb, LB_GETCURSEL, 0, 0)) != LB_ERR)
                {
                  SendMessage (hlb, LB_DELETESTRING, i, 0) ;
                  if (i)
                    i-- ;
                  SendMessage (hlb, LB_SETCURSEL, i, 0) ;
                }
                i = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                sprintf (str, "Queue will have %d entr%s", i, i != 1 ? "ies" : "y") ;
                SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
                return (TRUE) ;

           case IDC_ADDFILE :
                hlb = GetDlgItem (hDlg, IDC_FILEQUEUE) ;
                add_queue (hDlg, hlb) ;
                i = SendMessage (hlb, LB_GETCOUNT, 0, 0) ;
                sprintf (str, "Queue will have %d entr%s", i, i != 1 ? "ies" : "y") ;
                SetDlgItemText (hDlg, IDC_QUEUEENTRIES, str) ;
                return (TRUE) ;

           case IDC_CONTEXTHELP :
                WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 1L) ;
                return (TRUE) ;
         }
         break ;

    case WM_MEASUREITEM :
         if (wParam == IDC_FILEQUEUE)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = message_ychar ;
           return (TRUE) ;
         }
         else
           return (FALSE) ;

    case WM_DRAWITEM :
         if (wParam == IDC_FILEQUEUE)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           draw_ordinary_listbox (d) ;
           return (TRUE) ;
         }
         else
           return (FALSE) ;
  }
  return (FALSE) ;
}

BOOL FAR PASCAL PovStatisticsDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  unsigned long         *data = (unsigned long *) lParam ;
  HWND                  hlb ;
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_CTLCOLORLISTBOX :
         return ((BOOL) hbr) ;

    case WM_INITDIALOG :
         resize_listbox_dialog (hDlg, IDC_LISTBOX, 76) ;
         CenterWindowRelative ((HWND) data [0], hDlg, TRUE) ;
         FitWindowInWindow (NULL, hDlg) ;
         SetWindowText (hDlg, "Render Statistics") ;
         hlb = GetDlgItem (hDlg, IDC_LISTBOX) ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         fill_statistics_listbox (hlb, (int) data [1]) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
           case IDCANCEL :
                DeleteObject (hbr) ;
                EndDialog (hDlg, 0) ;
                return (TRUE) ;
         }
         break ;

    case WM_MEASUREITEM :
         if (wParam == IDC_LISTBOX)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = message_ychar ;
           return (TRUE) ;
         }
         else
           return (FALSE) ;

    case WM_DRAWITEM :
         if (wParam == IDC_LISTBOX)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           d->itemState &= ~ODS_SELECTED ;
           draw_ordinary_listbox (d) ;
           return (TRUE) ;
         }
         else
           return (FALSE) ;
  }
  return (FALSE) ;
}

BOOL FAR PASCAL PovRerunDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  int                   selItem ;
  unsigned long         data [2] ;
  HWND                  hlb ;
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static char           rerunID [MAX_RERUN] ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_CTLCOLORLISTBOX :
         return ((BOOL) hbr) ;

    case WM_INITDIALOG :
         hlb = GetDlgItem (hDlg, IDC_RERUNLIST) ;
         fill_rerun_listbox (hlb, rerunID) ;
         SendMessage (hlb, LB_SETCURSEL, 0, 0) ;
         CenterWindowRelative ((HWND) lParam, hDlg, TRUE) ;
         FitWindowInWindow (NULL, hDlg) ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         if (rendering)
           EnableWindow (GetDlgItem (hDlg, IDC_RERUN), FALSE) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDCANCEL :
                DeleteObject (hbr) ;
                EndDialog (hDlg, 0) ;
                return (TRUE) ;

           case IDC_RERUN :
                selItem = SendDlgItemMessage (hDlg, IDC_RERUNLIST, LB_GETCURSEL, 0, 0) ;
                if (!rendering && selItem != LB_ERR)
                {
                  DeleteObject (hbr) ;
                  EndDialog (hDlg, rerunID [selItem] + 2) ;
                }
                return (TRUE) ;

           case IDC_RERUNLIST :
                if (HIWORD (wParam) == LBN_DBLCLK)
                {
                  selItem = SendMessage ((HWND) lParam, LB_GETCURSEL, 0, 0) ;
                  if (!rendering && selItem != LB_ERR)
                  {
                    DeleteObject (hbr) ;
                    EndDialog (hDlg, rerunID [selItem] + 2) ;
                  }
                }
                return (TRUE) ;

           case IDC_STATISTICS :
                selItem = SendDlgItemMessage (hDlg, IDC_RERUNLIST, LB_GETCURSEL, 0, 0) ;
                if (selItem != LB_ERR)
                {
                  data [0] = (unsigned long) hDlg ;
                  data [1] = (unsigned long) rerunID [selItem] ;
                  DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), hDlg, (DLGPROC) PovStatisticsDialogProc, (LPARAM) data) ;
                }
                return (TRUE) ;

           case IDC_CONTEXTHELP :
                WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 2L) ;
                return (TRUE) ;
         }
         break ;

    case WM_MEASUREITEM :
         if (wParam == IDC_RERUNLIST)
         {
           m = (MEASUREITEMSTRUCT *) lParam ;
           m->itemHeight = message_ychar ;
           return (TRUE) ;
         }
         else
           return (FALSE) ;

    case WM_DRAWITEM :
         if (wParam == IDC_RERUNLIST)
         {
           d = (DRAWITEMSTRUCT *) lParam ;
           draw_rerun_listbox (d) ;
           return (TRUE) ;
         }
         else
           return (FALSE) ;
  }
  return (FALSE) ;
}

void UpdateTabbedWindow (int current, BOOL force)
{
  HWND        old_window ;
  TC_ITEM     tabitem ;

  old_window = current_window ;
  current_window = window_list [current] ;
  if (use_editors)
  {
    tabitem.mask = TCIF_PARAM ;
    if (TabCtrl_GetItem (tabbed_window, current, &tabitem))
    {
      if (tabitem.lParam != 0)
      {
        CurrentEditor = (void *) tabitem.lParam ;
        Edit.SelectEditor (CurrentEditor) ;
      }
    }
  }
  if ((old_window != current_window) || force)
  {
    if (show_messages)
      CalculateClientWindows (FALSE) ;
    if (current_window == message_window)
      build_main_menu (hMainMenu, use_editors) ;
    else
      build_editor_menu (hMainMenu) ;
    ShowWindow (current_window, SW_SHOW) ;
    if (old_window != message_window)
      if (old_window != current_window)
        ShowWindow (old_window, SW_HIDE) ;
    if (use_editors)
      Edit.SetActive (current_window == editor_window) ;
  }
  if (use_editors)
  {
    if (current_window == editor_window)
    {
      // this hide/show will force the window to be correctly updated. (W95 problem ?)
      if (old_window == message_window)
        ShowWindow (message_window, SW_HIDE) ;
      if (show_messages)
        ShowWindow (message_window, SW_SHOW) ;
      Edit.Show (TRUE) ;
    }
  }
  SetForegroundWindow (current_window) ;
  // in theory this is redundant (at least according to Microsoft's SetForegroundwindow () documentation)
  // in practice, it fixes an annoying problem with the message window not receiving focus after switching
  // from the editor window, until focus is changed away from POV-Ray for Windows and then back again.
  // [according to the Microsoft Win32 help file, SetForegroundWindow () does set the focus.]
  SetFocus (current_window) ;
}

void CalculateClientWindows (BOOL redraw)
{
  int         left ;
  int         top ;
  int         width ;
  int         height ;
  int         y ;
  RECT        rect ;

  if (tabbed_window != NULL)
  {
    // what this following code does is work out the size of the tabbed window such that
    // the bottom, left and right borders are intentionally outside of the client area of
    // the main window. this is because there doesn't seem to be a neat way of hiding them
    // and still retaining the 'tab' appearance that I want.
    GetClientRect (main_window, &rect) ;
    rect.left = 0 ;
    rect.top = toolheight ;
    rect.bottom -= toolheight - 1 ;
    TabCtrl_AdjustRect (tabbed_window, TRUE, &rect) ;
    MoveWindow (tabbed_window, rect.left, toolheight, rect.right - rect.left + 1, rect.bottom, FALSE) ;
    GetClientRect (tabbed_window, &rect) ;
    TabCtrl_AdjustRect (tabbed_window, FALSE, &rect) ;
    left = rect.left ;
    top = rect.top + 2 ;
    width = rect.right - rect.left + 1 ;
//  height = rect.bottom - rect.top + 1 ;
    height = rect.bottom - rect.top - 1 ;
    if (show_messages)
    {
      if (current_window == editor_window)
      {
        y = message_ychar * 9 ;
        MoveWindow (editor_window, left, top, width, height - y - panel_size, redraw) ;
        MoveWindow (message_window, left, top + height - y - panel_size, width, y, redraw) ;
        MoveWindow (status_window, left, top + height - panel_size, width, panel_size, redraw) ;
      }
      else
        MoveWindow (message_window, left, top, width, height, redraw) ;
    }
    else
    {
      MoveWindow (message_window, left, top, width, height, redraw) ;
      if (use_editors)
      {
        MoveWindow (editor_window, left, top, width, height - panel_size, redraw) ;
        MoveWindow (status_window, left, top + height - panel_size, width, panel_size, redraw) ;
      }
    }
  }
  PovInvalidateRect (tabbed_window, NULL, redraw) ;
}

/*
** Return TRUE if we are to return 0 to Windows, FALSE if we are to continue.
*/
BOOL handle_main_command (WPARAM wParam, LPARAM lParam)
{
  int         oldRows ;
  int         n ;
  char        *s ;
  char        filename [_MAX_PATH] ;
  char        str [512] ;
  void        *Editor ;
  unsigned    i ;
  HDIB        hDIB ;
  HWND        oldHwnd ;
  RECT        rect ;
  BOOL        modified ;
  BITMAP      bm ;
  HBITMAP     hBMP ;
  pvncStruct  pvnc ;
  CHOOSECOLOR cc ;
  struct stat st ;
  static char str1 [_MAX_PATH] ;
  static char str2 [_MAX_PATH] ;

  if (process_toggles (wParam))
    return (TRUE) ;

  if (LOWORD (wParam) >= CM_FIRSTTOOL && LOWORD (wParam) <= CM_LASTTOOL)
  {
    s = parse_tool_command (tool_commands [LOWORD (wParam) - CM_FIRSTTOOL]) ;
    if (GetPrivateProfileInt ("General", "Debug", 0, ToolIniFileName))
      message_printf ("Tool request - in '%s', out '%s'\n", tool_commands [LOWORD (wParam) - CM_FIRSTTOOL], s) ;
    else
      execute_tool (s) ;
    return (TRUE) ;
  }

  switch (LOWORD (wParam))
  {
    case CM_SHOWMAINWINDOW :
         if (main_window_hidden)
         {
           ShowWindow (main_window, SW_SHOW) ;
           if (render_window)
             ShowWindow (render_window, SW_SHOW) ;
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Put &Main Window in Taskbar\tALT+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (isMaxiMinimized)
         {
           pvncGetStruct (main_window, &pvnc) ;
           pvnc.isMaxiMinimized = FALSE ;
           pvncSetStruct (main_window, &pvnc) ;
           SendMessage (main_window, WM_SIZE, PVNC_MAXIMINIMIZE, 0) ;
           ShowWindow (main_window, SW_SHOW) ;
           if (use_taskbar)
           {
             PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Put &Main Window in Taskbar\tALT+W") ;
             PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_ENABLED) ;
           }
           else
             PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_GRAYED) ;
           return (0) ;
         }
         if (use_taskbar)
         {
           if (TaskBarAddIcon (main_window, 0, ourIcon, "POV-Ray (click to restore, right button for menu)"))
           {
             ShowWindow (main_window, SW_HIDE) ;
             if ((render_above_main || hide_render_window) && render_window != NULL)
               ShowWindow (render_window, SW_HIDE) ;
             PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Restore &Main Window from Taskbar") ;
             main_window_hidden++ ;
             return (0) ;
           }
         }
         return (0) ;

    case CM_FILENEW :
         if (use_editors)
         {
           if ((Editor = Edit.GetEditor ()) != NULL)
           {
             oldRows = TabCtrl_GetRowCount (tabbed_window) ;
             add_window_to_tab (editor_window, Editor, Edit.GetTitle (Editor, NULL, NULL, NULL, NULL)) ;
             TabCtrl_SetCurSel (tabbed_window, window_count - 1) ;
             UpdateTabbedWindow (window_count - 1, FALSE) ;
             if (TabCtrl_GetRowCount (tabbed_window) != oldRows)
               CalculateClientWindows (TRUE) ;
             return (0) ;
           }
           else
             MessageBox (main_window, "No more editors available", "Error", MB_ICONEXCLAMATION) ;
         }
         return (1) ;

    case CM_FILEOPEN :
         if (use_editors)
         {
           Edit.OpenInNew () ;
           return (0) ;
         }
         return (1) ;

    case CM_FILESAVE :
         if (use_editors)
         {
           Edit.SaveFile () ;
           return (0) ;
         }
         return (1) ;

    case CM_RENDERSLEEP :
         if (!(rendersleep = SendMessage (toolbar_window, TB_ISBUTTONCHECKED, (WPARAM) CM_RENDERSLEEP, 0L)))
         {
           SetWindowText (main_window, "POV-Ray for Windows") ;
           FlashWindow (main_window, 0) ;
         }
         else
           SetWindowText (main_window, "POV-Ray - render paused") ;
         break ;

    case CM_EXPERTMENUS :
         if (current_window == message_window)
           build_main_menu (hMainMenu, use_editors) ;
         else
           build_editor_menu (hMainMenu) ;
         return (TRUE) ;

    case CM_DROPEDITOR :
    case CM_DROPRENDERER :
         PVCheckMenuItem (CM_DROPEDITOR, LOWORD (wParam) == CM_DROPEDITOR ? MF_CHECKED : MF_UNCHECKED) ;
         PVCheckMenuItem (CM_DROPRENDERER, LOWORD (wParam) == CM_DROPRENDERER ? MF_CHECKED : MF_UNCHECKED) ;
         drop_to_editor = LOWORD (wParam) == CM_DROPEDITOR ;
         break ;

    case CM_GUIPRIORITY_LOWEST :
    case CM_GUIPRIORITY_LOW :
    case CM_GUIPRIORITY_NORMAL :
    case CM_GUIPRIORITY_HIGH :
    case CM_GUIPRIORITY_HIGHEST :
         PVCheckMenuItem (GUI_priority, MF_UNCHECKED) ;
         GUI_priority = LOWORD (wParam) ;
         PVCheckMenuItem (LOWORD (wParam), MF_CHECKED) ;
         // only change GUI priority when the renderer is running
         if (hRenderThread != NULL)
           set_GUI_priority (GUI_priority) ;
         return (TRUE) ;

    case CM_RENDERPRIORITY_LOWEST :
    case CM_RENDERPRIORITY_LOW :
    case CM_RENDERPRIORITY_NORMAL :
    case CM_RENDERPRIORITY_HIGH :
    case CM_RENDERPRIORITY_HIGHEST :
         PVCheckMenuItem (render_priority, MF_UNCHECKED) ;
         render_priority = LOWORD (wParam) ;
         PVCheckMenuItem (LOWORD (wParam), MF_CHECKED) ;
         if (hRenderThread != NULL)
           set_render_priority (render_priority) ;
         return (TRUE) ;

    case CM_COMPLETION_EXIT :
    case CM_COMPLETION_BEEP :
    case CM_COMPLETION_NOTHING :
    case CM_COMPLETION_MESSAGE :
    case CM_COMPLETION_BEEPMESSAGE :
         PVCheckMenuItem (on_completion, MF_UNCHECKED) ;
         on_completion = LOWORD (wParam) ;
         PVCheckMenuItem (on_completion, MF_CHECKED) ;
         return (TRUE) ;

    case CM_PREVWINDOW :
    case CM_NEXTWINDOW :
         i = TabCtrl_GetCurSel (tabbed_window) ;
         if (LOWORD (wParam) == CM_PREVWINDOW)
         {
           if (i-- == 0)
             i = window_count - 1 ;
         }
         else
         {
           if (++i >= window_count)
             i = 0 ;
         }
         TabCtrl_SetCurSel (tabbed_window, i) ;
         UpdateTabbedWindow (i, FALSE) ;
         return (TRUE) ;

    case CM_USETOOLBAR :
         if (toolbar_window == NULL)
           return (TRUE) ;
         ShowWindow (toolbar_window, use_toolbar ? SW_SHOW : SW_HIDE) ;
         toolheight = 0 ;
         GetClientRect (main_window, &rect) ;
         SendMessage (main_window, WM_SIZE, SIZE_RESTORED, MAKELPARAM (rect.right + 1, rect.bottom + 1)) ;
         return (TRUE) ;

    case CM_SINGLEINSTANCE :
         PutPrivateProfileInt ("General", "OneInstance", one_instance, EngineIniFileName) ;
         return (TRUE) ;

    case CM_FILEEXIT :
         if (rendering || hRenderThread)
         {
           if (MessageBox (main_window,
                           "POV-Ray is currently rendering - do you want to stop ?",
                           "Stop rendering ?",
                           MB_ICONQUESTION | MB_YESNO) == IDYES)
           {
             if (use_editors)
               if (!Edit.CanClose ())
                 return (TRUE) ;
             quit = TRUE ;
           }
         }
         else
         {
           if (use_editors)
             if (!Edit.CanClose ())
               return (TRUE) ;
           WinHelp (main_window, "pvengine.hlp", HELP_QUIT, NULL) ;
           WinHelp (main_window, "povray30.hlp", HELP_QUIT, NULL) ;
           DestroyWindow (main_window) ;
         }
         return (TRUE) ;

    case CM_FILERENDER :
         if (!rendering && !hRenderThread)
         {
           if (use_editors && current_window == editor_window)
           {
             s = Edit.GetTitle (CurrentEditor, NULL, NULL, NULL, &modified) ;
             if (modified)
             {
               if (get_file_type (s) != filePOV)
               {
                 sprintf (str, "The file\n  '%s'\nHas been modified. Save it so render can proceed ?", s) ;
                 if (MessageBox (main_window, str, "Save file ?", MB_ICONQUESTION | MB_YESNO) == IDYES)
                 {
                   Edit.SaveFile () ;
                   modified = FALSE ;
                 }
                 else
                   return (TRUE) ;
               }
             }
             n = get_file_type (s) ;
             if (n == filePOV || n == fileINI || !ExternalDragFunction (s, dfRenderEditor))
               PostMessage (main_window, EDITOR_RENDER_MESSAGE, modified, (LPARAM) s) ;
             return (TRUE) ;
           }
           SetForegroundWindow (main_window) ;
           if (!ExternalDragFunction (source_file_name, dfRenderMessage))
             start_rendering (FALSE, FALSE) ;
         }
         else
           stop_rendering = TRUE ;
         return (TRUE) ;

    case CM_SAVE_SETTINGS :
         PutPrivateProfileInt ("General", "SaveSettingsOnExit", save_settings, EngineIniFileName) ;
         return (TRUE) ;

    case CM_DUMPPANE :
         dump_pane_to_clipboard () ;
         return (TRUE) ;

    case CM_CLEARMESSAGES :
         clear_messages () ;
         PovInvalidateRect (message_window, NULL, FALSE) ;
         UpdateWindow (message_window) ;
         return (TRUE) ;

    case CM_FORCE8BITS :
         detect_graphics_config () ;
         create_palette (NULL, 0) ;
         buffer_message (None, render_bitmap_depth == 24 ? "Using 24-bit internal bitmap\n" :
                                                           renderwin_8bits ? "Using 8-bit dithered internal bitmap (menu setting)\n" :
                                                                             "Using 8-bit dithered internal bitmap (4 or 8-bit video mode)\n") ;
         return (TRUE) ;

    case CM_RENDERABOVEMAIN :
         // simply re-parenting doesn't seem to have the desired effect. sigh.
         if (render_window != NULL)
         {
           oldHwnd = render_window ;
           render_window = NULL ;
           ShowWindow (oldHwnd, SW_HIDE) ;
           SetForegroundWindow (main_window) ;
           DestroyWindow (oldHwnd) ;
           create_render_window () ;
         }
         PVEnableMenuItem (CM_RENDERHIDE, render_above_main ? MF_GRAYED : MF_ENABLED) ;
         return (TRUE) ;

    case CM_TINYCAPTION :
         if (render_window != NULL)
         {
           oldHwnd = render_window ;
           render_window = NULL ;
           ShowWindow (oldHwnd, SW_HIDE) ;
           SetForegroundWindow (main_window) ;
           DestroyWindow (oldHwnd) ;
           create_render_window () ;
         }
         return (TRUE) ;

    case CM_USECTL3D :
         if (MessageBox (main_window,
                        "POV-Ray for Windows needs to re-start for this to take effect.\n\n"
                        "Re-start POV-ray ?",
                        "Re-start POV-Ray for Windows ?",
                        MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
         {
           PutPrivateProfileInt ("General", "UseCtl3D", !using_ctl3d, EngineIniFileName) ;
           GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox, WM_GETTEXT, sizeof (SecondaryRenderIniFileSection), (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
               strcpy (command_line, old_command_line) ;
             write_INI_settings (EngineIniFileName) ;
             if (use_editors)
               Edit.SaveSettings () ;
           }
           DestroyWindow (main_window) ;
           execute_tool (filename) ;
         }
         return (TRUE) ;

    case CM_NONCLIENT :
         if (MessageBox (main_window,
                        "POV-Ray for Windows needs to re-start for this to take effect.\n\n"
                        "Re-start POV-ray ?",
                        "Re-start POV-Ray for Windows ?",
                        MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
         {
           PutPrivateProfileInt ("General", "NonClientEffects", !non_client, EngineIniFileName) ;
           GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox, WM_GETTEXT, sizeof (SecondaryRenderIniFileSection), (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
               strcpy (command_line, old_command_line) ;
             write_INI_settings (EngineIniFileName) ;
             if (use_editors)
               Edit.SaveSettings () ;
           }
           DestroyWindow (main_window) ;
           execute_tool (filename) ;
         }
         return (TRUE) ;

    case CM_USEEDITOR :
         if (MessageBox (main_window,
                        "POV-Ray for Windows needs to re-start for this to take effect.\n\n"
                        "Re-start POV-ray ?",
                        "Re-start POV-Ray for Windows ?",
                        MB_ICONEXCLAMATION | MB_YESNO) == IDYES)
         {
           PutPrivateProfileInt ("General", "UseEditors", !use_editors, EngineIniFileName) ;
           GetModuleFileName (hInstance, filename, sizeof (filename) - 1) ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox, WM_GETTEXT, sizeof (SecondaryRenderIniFileSection), (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
               strcpy (command_line, old_command_line) ;
             write_INI_settings (EngineIniFileName) ;
             if (use_editors)
               Edit.SaveSettings () ;
           }
           DestroyWindow (main_window) ;
           execute_tool (filename) ;
         }
         return (TRUE) ;

    case CM_HELPABOUT :
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_ABOUT), main_window, (DLGPROC) PovAboutDialogProc, (LPARAM) main_window) ;
         return (TRUE) ;

    case CM_SHOWNEXTTIP :
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_TIP), main_window, (DLGPROC) PovTipDialogProc, (LPARAM) main_window) ;
		 return (TRUE) ;

    case CM_COMMANDLINE :
         if (!rendering && !hRenderThread)
         {
           if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_COMMANDLINE), main_window, (DLGPROC) PovCommandLineDialogProc, (LPARAM) main_window))
             if (!ExternalDragFunction (source_file_name, dfRenderCommandLine))
               start_rendering (FALSE, FALSE) ;
         }
         return (TRUE) ;

    case CM_TILEDBACKGROUND :
         PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, tile_background ? "&Select Plain Background" : "&Select Tiled Background") ;
         if (tile_background && hBmpBackground == NULL)
         {
           if ((hBmpBackground = NonBogusLoadBitmap (hInstance, MAKEINTRESOURCE (BMP_BACKGROUND00))) != NULL)
           {
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = TRUE ;
             PovInvalidateRect (message_window, NULL, TRUE) ;
           }
           else
           {
             tile_background = FALSE ;
             // make sure this messagebox is AFTER we set tile_background to false !
             PovMessageBox ("Failed to load internal bitmap", "Error") ;
             PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Tiled Background") ;
             background_file [0] = '\0' ;
           }
           return (TRUE) ;
         }
         else
           PovInvalidateRect (message_window, NULL, TRUE) ;
         return (TRUE) ;

    case CM_BACKGROUNDCOLOUR :
         memset (&cc, 0, sizeof (CHOOSECOLOR)) ;
         cc.lStructSize = sizeof (CHOOSECOLOR) ;
         cc.hwndOwner = main_window ;
         cc.rgbResult = background_colour ;
         cc.Flags = CC_PREVENTFULLOPEN | CC_RGBINIT ;
         cc.lpCustColors = custom_colours ;
         if (ChooseColor (&cc))
         {
           background_colour = cc.rgbResult ;
           PovInvalidateRect (message_window, NULL, TRUE) ;
         }
         return (TRUE) ;

    case CM_BACKGROUNDBITMAP :
         if ((s = get_background_file (main_window)) != NULL)
         {
           if ((hDIB = LoadDIB (s)) != NULL)
           {
             strcpy (background_file, s) ;
             DeleteObject (hBmpBackground) ;
             hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
             DeleteObject (hDIB) ;
             GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
             background_width = bm.bmWidth ;
             background_height = bm.bmHeight ;
             tile_background = TRUE ;
             PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Plain Background") ;
             background_shade = RGB (1, 1, 1) ;
             PovInvalidateRect (message_window, NULL, TRUE) ;
           }
           else
             PovMessageBox ("Failed to load bitmap file", "Error") ;
         }
         return (TRUE) ;

    case CM_BACKGROUNDSTD + 0 :
    case CM_BACKGROUNDSTD + 1 :
    case CM_BACKGROUNDSTD + 2 :
    case CM_BACKGROUNDSTD + 3 :
    case CM_BACKGROUNDSTD + 4 :
    case CM_BACKGROUNDSTD + 5 :
    case CM_BACKGROUNDSTD + 6 :
    case CM_BACKGROUNDSTD + 7 :
    case CM_BACKGROUNDSTD + 8 :
    case CM_BACKGROUNDSTD + 9 :
         if ((hBMP = NonBogusLoadBitmap (hInstance, MAKEINTRESOURCE (BMP_BACKGROUND00 + (LOWORD (wParam) - CM_BACKGROUNDSTD)))) != NULL)
         {
           DeleteObject (hBmpBackground) ;
           hBmpBackground = hBMP ;
           GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
           background_width = bm.bmWidth ;
           background_height = bm.bmHeight ;
           background_file [0] = '0' + (char) (LOWORD (wParam) - CM_BACKGROUNDSTD) ;
           background_file [1] = '\0' ;
           switch (LOWORD (wParam))
           {
             case CM_BACKGROUNDSTD + 0 :
                  background_shade = RGB (0, 0, 0) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 1 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 2 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 3 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 4 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (255, 255, 255) ;
                  break ;

             case CM_BACKGROUNDSTD + 5 :
                  background_shade = RGB (1, 1, 1) ;
                  if (lParam != 1)
                    text_colour = RGB (0, 0, 0) ;
                  break ;
           }
           tile_background = TRUE ;
           PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, "&Select Plain Background") ;
           PovInvalidateRect (message_window, NULL, TRUE) ;
         }
         else
           PovMessageBox ("Failed to load internal bitmap", "Error") ;
         return (TRUE) ;

    case CM_TEXTCOLOUR :
         memset (&cc, 0, sizeof (CHOOSECOLOR)) ;
         cc.lStructSize = sizeof (CHOOSECOLOR) ;
         cc.hwndOwner = main_window ;
         cc.rgbResult = text_colour ;
         cc.Flags = CC_PREVENTFULLOPEN | CC_RGBINIT ;
         cc.lpCustColors = custom_colours ;
         if (ChooseColor (&cc))
         {
           text_colour = cc.rgbResult ;
           PovInvalidateRect (message_window, NULL, TRUE) ;
         }
         return (TRUE) ;

    case CM_FONT :
         get_font () ;
         return (TRUE) ;

    case CM_RENDERSHOW :
         if (renderwin_destroyed)
         {
           create_render_window () ;
           PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
           PVEnableMenuItem (CM_RENDERCLOSE, MF_ENABLED) ;
           renderwin_destroyed = FALSE ;
         }
         return (TRUE) ;

    case CM_RENDERCLOSE :
         if (render_above_main && GetForegroundWindow () == render_window)
           SetForegroundWindow (main_window) ;
         if (render_window != NULL)
           DestroyWindow (render_window) ;
         return (TRUE) ;

    case CM_CLEARQUEUE :
         queued_file_count = 0 ;
         update_queue_status (TRUE) ;
         return (TRUE) ;

    case CM_FILEQUEUE :
         DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_FILEQUEUE), main_window, (DLGPROC) PovFileQueueDialogProc, (LPARAM) main_window) ;
         return (TRUE) ;

    case CM_SOURCEFILE :
         if (!rendering && !hRenderThread)
         {
           if ((s = file_open (main_window)) != NULL)
           {
             strcpy (source_file_name, s) ;
             splitpath (source_file_name, lastRenderPath, lastRenderName) ;
             validatePath (lastRenderPath) ;
             if (!ExternalDragFunction (source_file_name, dfRenderSourceFile))
               start_rendering (FALSE, FALSE) ;
           }
         }
         return (TRUE) ;

    case CM_NICENESS + 0 :
    case CM_NICENESS + 1 :
    case CM_NICENESS + 2 :
    case CM_NICENESS + 3 :
    case CM_NICENESS + 4 :
    case CM_NICENESS + 5 :
    case CM_NICENESS + 6 :
         if (render_lock_up)
           niceness = 9 ;
         PVCheckMenuItem (CM_NICENESS + niceness, MF_UNCHECKED) ;
         niceness = LOWORD (wParam) - CM_NICENESS ;
         PVCheckMenuItem (CM_NICENESS + niceness, MF_CHECKED) ;
         niceness_milliseconds = set_niceness (niceness) ;
         render_lock_up = FALSE ;
         return (TRUE) ;

    case CM_NICENESS + 9 :
         if (MessageBox (main_window,
                        "WARNING : THIS WILL LOCK UP YOUR SYSTEM\n\n"
                        "Selecting this option will lock up Windows 3.1x until "
                        "any render you start completes!\n\n"
                        "Are you sure you want to do this ?",
                        "WARNING",
                        MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
         {
           PVCheckMenuItem (CM_NICENESS + niceness, MF_UNCHECKED) ;
           PVCheckMenuItem (CM_NICENESS + 9, MF_CHECKED) ;
           render_lock_up = TRUE ;
         }
         return (TRUE) ;

    case CM_RERUN + 0 :
    case CM_RERUN + 1 :
    case CM_RERUN + 2 :
    case CM_RERUN + 3 :
    case CM_RERUN + 4 :
    case CM_RERUN + 5 :
    case CM_RERUN + 6 :
    case CM_RERUN + 7 :
    case CM_RERUN + 8 :
    case CM_RERUN + 9 :
    case CM_RERUN + 10 :
    case CM_RERUN + 11 :
    case CM_RERUN + 12 :
    case CM_RERUN + 13 :
    case CM_RERUN + 14 :
    case CM_RERUN + 15 :
         if (!rendering && !hRenderThread)
         {
           loadRerun = LOWORD (wParam) - CM_RERUN + 2 ;
           if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
             start_rendering (FALSE, FALSE) ;
         }
         return (TRUE) ;

    case CM_RERUNCURRENT :
         if (!rendering && !hRenderThread)
         {
           loadRerun = 1 ;
           if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
             start_rendering (FALSE, FALSE) ;
         }
         return (TRUE) ;

    case CM_CONTINUECURRENT :
         if (!rendering && !hRenderThread)
         {
           loadRerun = 1 ;
           continueRerun = 1 ;
           start_rendering (FALSE, FALSE) ;
         }
         return (TRUE) ;

    case CM_RERUNDIALOG :
         if ((loadRerun = DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_RERUN), main_window, (DLGPROC) PovRerunDialogProc, (LPARAM) main_window)) != 0)
           if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
             start_rendering (FALSE, FALSE) ;
         return (TRUE) ;

    case CM_DEMO :
         if (!rendering && !hRenderThread)
         {
           if (save_demo_file (str1, str2) != NULL)
           {
             if (!demo_mode)
             {
               running_demo = TRUE ;
               if (DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_SHORTCOMMANDLINE), main_window, (DLGPROC) PovShortCommandLineDialogProc, (LPARAM) main_window))
               {
                 if (!restore_command_line)
                   strcpy (old_command_line, command_line) ;
                 restore_command_line = TRUE ;
                 sprintf (command_line, "\"Include_Ini=%s\" \"Input_File_Name=%s\" ", str2, str1) ;
                 strupr (command_line) ;
                 strcat (command_line, old_command_line) ;
                 ignore_auto_ini = TRUE ;
                 start_rendering (FALSE, TRUE) ;
                 ignore_auto_ini = FALSE ;
                 strcpy (command_line, old_command_line) ;
                 restore_command_line = FALSE ;
               }
               running_demo = FALSE ;
             }
             else
             {
               if (!restore_command_line)
                 strcpy (old_command_line, command_line) ;
               restore_command_line = TRUE ;
               sprintf (command_line, "\"Include_Ini=%s\" \"Input_File_Name=%s\" ", str2, str1) ;
               running_demo = TRUE ;
               start_rendering (FALSE, TRUE) ;
               running_demo = FALSE ;
               strcpy (command_line, old_command_line) ;
               restore_command_line = FALSE ;
             }
             unlink (str1) ;
             unlink (str2) ;
           }
         }
         return (TRUE) ;

    case CM_LOADTOOLMENU :
         ExternalEvent (EventLoadToolMenu, 0) ;
         load_tool_menu (ToolIniFileName) ;
         break ;

    case CM_HELPCONTENTS :
         WinHelp (main_window, engineHelpPath, HELP_INDEX, 0L) ;
         return (TRUE) ;

    case CM_HELPCOMPUSERVE :
         WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 8L) ;
         return (TRUE) ;

    case CM_GOPOVRAY :
         execute_tool ("gocserve povray") ;
         return (TRUE) ;

    case CM_GOPOVRAYORG :
         ShellExecute (NULL, NULL, "http://www.povray.org/index.html", NULL, NULL, SW_SHOWNORMAL) ;
         return (TRUE) ;

    case CM_HELPBUGS :
         WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 5L) ;
         return (TRUE) ;

    case CM_HELPBOOKS :
         WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 6L) ;
         return (TRUE) ;

    case CM_HELPCDROM :
    case CM_HELPBOOKCDROM :
         WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 7L) ;
         return (TRUE) ;

    case CM_POVLEGAL :
         if (stat (engineHelpPath, &st) == 0)
           WinHelp (main_window, engineHelpPath, HELP_CONTEXT, 0L) ;
         else
           DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_VIEW), main_window, (DLGPROC) PovLegalDialogProc, (LPARAM) main_window) ;
         return (TRUE) ;

    case CM_HELPPOVRAY :
         WinHelp (main_window, rendererHelpPath, HELP_INDEX, 0L) ;
         return (TRUE) ;

    case CM_HELPUSING_HELP :
         WinHelp (main_window, engineHelpPath, HELP_HELPONHELP, 0L) ;
         return (TRUE) ;
   }
   return (FALSE) ;
}

LRESULT CALLBACK PovRenderWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int         dest_width ;
  int         dest_height ;
  int         dest_xoffset ;
  int         dest_yoffset ;
  int         oldMode ;
  HDC         hdc ;
  RECT        rect ;
  POINT       pt ;
  MINMAXINFO  *pInfo ;
  PAINTSTRUCT ps ;

  switch (message)
  {
    case WM_CREATE :
         PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
         PVEnableMenuItem (CM_RENDERCLOSE, MF_ENABLED) ;
         break ;

    case WM_COMMAND :
         if (handle_main_command (wParam, lParam))
           return (0) ;
         if (renderwin_effects)
           return (ibDefWindowProc (hwnd, message, wParam, lParam)) ;
         break ;

    case WM_RBUTTONDOWN :
         if (hPopupMenus != NULL)
         {
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           ClientToScreen (render_window, &pt) ;
           TrackPopupMenu (GetSubMenu (hPopupMenus, 1), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
         }
         return (0) ;

    case WM_GETMINMAXINFO :
         if (IsZoomed (hwnd))
           break ;
         pInfo = (MINMAXINFO *) lParam ;
         pInfo->ptMaxTrackSize.x = renderwin_max_width ;
         pInfo->ptMaxTrackSize.y = renderwin_max_height ;
         break ;

    case WM_MOVE :
         if (render_window == NULL)
           break ;
         if (!IsIconic (render_window) && !IsZoomed (render_window))
         {
           GetWindowRect (render_window, &rect) ;
           renderwin_left = rect.left ;
           renderwin_top = rect.top ;
         }
         return (0) ;

    case WM_SIZE :
         if (render_window == NULL)
           break ;
         switch (wParam)
         {
           case SIZE_MINIMIZED :
                renderwin_flags = WS_MINIMIZE ;
                return (0) ;

           case SIZE_MAXIMIZED :
                renderwin_flags = WS_MAXIMIZE ;
                SetScrollRange (render_window, SB_HORZ, 0, 0, TRUE) ;
                SetScrollRange (render_window, SB_VERT, 0, 0, TRUE) ;
                PovInvalidateRect (render_window, NULL, FALSE) ;
                UpdateWindow (render_window) ;
                return (0) ;

           case SIZE_RESTORED :
                renderwin_flags = 0 ;
                PovInvalidateRect (render_window, NULL, FALSE) ;
                UpdateWindow (render_window) ;
                break ;

           default :
                return (0) ;
         }

         // to get here we must be handling SIZE_RESTORED.

         // one problem we have here is that if we create one scroll bar, it takes away some of the client
         // area of the other direction (i.e. if we create a scroll bar for the X direction, it takes away
         // some of the Y client area). therefore we should create a scroll bar for that direction also.
         // but we don't do this for now.
         GetWindowRect (render_window, &rect) ;
         if (rect.right - rect.left < renderwin_max_width)
         {
           if (renderwin_xoffset >= render_width - LOWORD (lParam))
             renderwin_xoffset = render_width - LOWORD (lParam) ;
           SetScrollRange (render_window, SB_HORZ, 0, render_width - LOWORD (lParam), FALSE) ;
           SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, TRUE) ;
         }
         else
         {
           renderwin_xoffset = 0 ;
           SetScrollRange (render_window, SB_HORZ, 0, 0, TRUE) ;
         }
         if (rect.bottom - rect.top < renderwin_max_height)
         {
           if (renderwin_yoffset >= render_height - HIWORD (lParam))
             renderwin_yoffset = render_height - HIWORD (lParam) ;
           SetScrollRange (render_window, SB_VERT, 0, render_height - HIWORD (lParam), FALSE) ;
           SetScrollPos (render_window, SB_VERT, renderwin_yoffset, TRUE) ;
         }
         else
         {
           renderwin_yoffset = 0 ;
           SetScrollRange (render_window, SB_VERT, 0, 0, TRUE) ;
         }
         return (0) ;

    case WM_VSCROLL :
         GetClientRect (render_window, &rect) ;
         switch (LOWORD (wParam))
         {
           case SB_LINEDOWN :
                if (renderwin_yoffset >= render_height - rect.bottom) break ;
                SetScrollRange (render_window, SB_VERT, 0, render_height - rect.bottom, FALSE) ;
                SetScrollPos (render_window, SB_VERT, ++renderwin_yoffset, TRUE) ;
                ScrollWindow (render_window, 0, -1, NULL, NULL) ;
                break ;

             case SB_LINEUP :
                if (renderwin_yoffset == 0) break ;
                SetScrollRange (render_window, SB_VERT, 0, render_height - rect.bottom, FALSE) ;
                SetScrollPos (render_window, SB_VERT, --renderwin_yoffset, TRUE) ;
                ScrollWindow (render_window, 0, 1, NULL, NULL) ;
                break ;

           case SB_PAGEDOWN :
                renderwin_yoffset += rect.bottom ;
                if (renderwin_yoffset > render_height - rect.bottom)
                  renderwin_yoffset = render_height - rect.bottom ;
                SetScrollPos (render_window, SB_VERT, renderwin_yoffset, TRUE) ;
                PovInvalidateRect (hwnd, NULL, FALSE) ;
                break ;

           case SB_PAGEUP :
                renderwin_yoffset -= rect.bottom ;
                if (renderwin_yoffset < 0)
                  renderwin_yoffset = 0 ;
                SetScrollPos (render_window, SB_VERT, renderwin_yoffset, TRUE) ;
                PovInvalidateRect (hwnd, NULL, FALSE) ;
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                renderwin_yoffset = HIWORD (wParam) ;
                SetScrollPos (render_window, SB_VERT, renderwin_yoffset, TRUE) ;
                PovInvalidateRect (hwnd, NULL, FALSE) ;
                break ;
         }
         return (0) ;

    case WM_HSCROLL :
         GetClientRect (render_window, &rect) ;
         switch (LOWORD (wParam))
         {
           case SB_LINERIGHT :
                if (renderwin_xoffset >= render_width - rect.right) break ;
                SetScrollRange (render_window, SB_HORZ, 0, render_width - rect.right, FALSE) ;
                SetScrollPos (render_window, SB_HORZ, ++renderwin_xoffset, TRUE) ;
                ScrollWindow (render_window, -1, 0, NULL, NULL) ;
                break ;

             case SB_LINELEFT :
                if (renderwin_xoffset == 0) break ;
                SetScrollRange (render_window, SB_HORZ, 0, render_width - rect.right, FALSE) ;
                SetScrollPos (render_window, SB_HORZ, --renderwin_xoffset, TRUE) ;
                ScrollWindow (render_window, 1, 0, NULL, NULL) ;
                break ;

           case SB_PAGERIGHT :
                renderwin_xoffset += rect.right ;
                if (renderwin_xoffset > render_width - rect.right)
                  renderwin_xoffset = render_width - rect.right ;
                SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, TRUE) ;
                PovInvalidateRect (hwnd, NULL, FALSE) ;
                break ;

           case SB_PAGELEFT :
                renderwin_xoffset -= rect.right ;
                if (renderwin_xoffset < 0)
                  renderwin_xoffset = 0 ;
                SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, TRUE) ;
                PovInvalidateRect (hwnd, NULL, FALSE) ;
                break ;

           case SB_THUMBPOSITION :
           case SB_THUMBTRACK :
                renderwin_xoffset = HIWORD (wParam) ;
                SetScrollPos (render_window, SB_HORZ, renderwin_xoffset, TRUE) ;
                PovInvalidateRect (hwnd, NULL, FALSE) ;
                break ;
         }
         return (0) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
         if (hPalApp)
         {
           SelectPalette (hdc, hPalApp, FALSE) ;
           RealizePalette (hdc) ;
         }
         if (IsIconic (render_window))
         {
           SelectClipRgn (hdc, NULL) ;
           dest_xoffset = 0 ;
           dest_yoffset = 0 ;
           dest_width = 36 ;
           dest_height = 36 ;
         }
         else
         {
           if (IsZoomed (render_window))
           {
             GetClientRect (render_window, &rect) ;
             dest_xoffset = 0 ;
             dest_yoffset = 0 ;
             dest_width = rect.right ;
             dest_height = rect.bottom ;
           }
           else
           {
             dest_xoffset = -renderwin_xoffset ;
             dest_yoffset = -renderwin_yoffset ;
             dest_width = render_width ;
             dest_height = render_height ;
             GetClientRect (render_window, &rect) ;
             if (rect.right > dest_width)
               BitBlt (hdc, dest_width, 0, rect.right - dest_width, rect.bottom, NULL, 0, 0, BLACKNESS) ;
             if (rect.bottom > dest_height)
               BitBlt (hdc, 0, dest_height, rect.right, rect.bottom - dest_height, NULL, 0, 0, BLACKNESS) ;
           }
         }
         StretchDIBits (hdc, dest_xoffset,
                             dest_yoffset,
                             dest_width,
                             dest_height,
                             0,
                             0,
                             render_bitmap.header.biWidth,
                             render_bitmap.header.biHeight,
                             render_bitmap_surface,
                             (LPBITMAPINFO) &render_bitmap,
                             DIB_RGB_COLORS,
                             SRCCOPY) ;
         SetStretchBltMode (hdc, oldMode) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_DESTROY :
         // it is possible for an old render window to be in the process of being destroyed
         // when a new one has already been created. check for this here.
         if (render_window == hwnd)
         {
           render_window = NULL ;
           renderwin_destroyed = TRUE ;
           PVEnableMenuItem (CM_RENDERSHOW, MF_ENABLED) ;
           PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
         }
         return (0) ;
  }
  if (renderwin_effects)
    return (ibDefWindowProc (hwnd, message, wParam, lParam)) ;
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovMainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int         i ;
  int         lRet ;
  int         oldMode ;
  int         oldRows ;
  char        *s ;
  void        *Editor ;
  HDC         hdc ;
  HDC         hdcMemory ;
  BOOL        f ;
  RECT        rect ;
  POINT       pt ;
  NMHDR       *nmh ;
  HBITMAP     oldBmp ;
  HPALETTE    oldPalette ;
  TOOLTIPTEXT *t  ;
  static int  maxiMiniX ;
  static int  maxiMiniY ;
  static int  maxiMiniW = 128 ;
  static int  maxiMiniH = 128 ;
  static int  maxiMiniStyle ;
  static RECT maxiRect = {-1, -1, -1, -1} ;

  switch (message)
  {
    case TASKBAR_NOTIFY_MESSAGE :
         if (lParam == WM_LBUTTONDOWN)
         {
           ShowWindow (main_window, SW_SHOW) ;
           if (render_window)
             ShowWindow (render_window, SW_SHOW) ;
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Put &Main Window in Taskbar\tALT+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (lParam == WM_RBUTTONDOWN)
         {
           if (hPopupMenus != NULL)
           {
             GetCursorPos (&pt) ;
             SetForegroundWindow (main_window) ;
             TrackPopupMenu (GetSubMenu (hPopupMenus, 0), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
             return (0) ;
           }
         }
         return (0) ;

    case WM_ENTERSIZEMOVE :
         if (!IsW95UserInterface)
           break ;
         resizing = TRUE ;
         if (current_window == editor_window)
           ShowWindow (current_window, SW_HIDE) ;
         break ;

    case WM_EXITSIZEMOVE :
         if (!IsW95UserInterface)
           break ;
         resizing = FALSE ;
         InvalidateRect (current_window, NULL, TRUE) ;
         InvalidateRect (message_window, NULL, TRUE) ;
         ShowWindow (current_window, SW_SHOW) ;
         break ;

    case WM_SETFOCUS :
         if (use_editors)
           if (current_window == editor_window)
             SetFocus (editor_window) ;
         break ;

    case SHOW_MESSAGES_MESSAGE :
         if (wParam)
           return (show_messages) ;
         show_messages = !show_messages ;
         if (current_window == editor_window)
         {
           if (show_messages)
             ShowWindow (message_window, SW_SHOW) ;
           CalculateClientWindows (TRUE) ;
         }
         return (show_messages) ;

    case EDITOR_RENDER_MESSAGE :
         if (rendering || hRenderThread)
         {
           stop_rendering++ ;
           return (0) ;
         }
         strcpy (source_file_name, (char *) lParam) ;
         splitpath (source_file_name, lastRenderPath, lastRenderName) ;
         input_file_in_memory = wParam ;
         if (!ExternalDragFunction (source_file_name, dfRenderEditor))
           start_rendering (FALSE, FALSE) ;
         input_file_in_memory = FALSE ;
         return (0) ;

    case CREATE_RENDERWIN_MESSAGE :
         return (renderwin_init ()) ;

    case CLOSE_EDITOR_MESSAGE :
         if (use_editors)
         {
           if (Edit.ReleaseEditor ((void *) lParam))
           {
             if ((i = get_tab_index (tabbed_window, (void *) lParam)) != 0)
             {
               oldRows = TabCtrl_GetRowCount (tabbed_window) ;
               TabCtrl_DeleteItem (tabbed_window, i) ;
               memcpy (window_list + i, window_list + i + 1, --window_count - i) ;
               TabCtrl_SetCurSel (tabbed_window, --i) ;
               UpdateTabbedWindow (i, FALSE) ;
               if (TabCtrl_GetRowCount (tabbed_window) != oldRows)
                 CalculateClientWindows (TRUE) ;
             }
           }
         }
         return (0) ;

    case CREATE_EDITOR_MESSAGE :
         if (use_editors)
         {
           if ((Editor = Edit.GetEditor ()) != NULL)
           {
             oldRows = TabCtrl_GetRowCount (tabbed_window) ;
             add_window_to_tab (editor_window, Editor, Edit.GetTitle (Editor, NULL, NULL, NULL, NULL)) ;
             TabCtrl_SetCurSel (tabbed_window, window_count - 1) ;
             UpdateTabbedWindow (window_count - 1, FALSE) ;
             if (TabCtrl_GetRowCount (tabbed_window) != oldRows)
               CalculateClientWindows (TRUE) ;
             return (0) ;
           }
           else
             MessageBox (main_window, "No more editors available", "Error", MB_ICONEXCLAMATION) ;
         }
         return (1) ;

    case WM_NOTIFY :
         nmh = (NMHDR *) lParam ;
         if (nmh->hwndFrom == tabbed_window)
         {
           switch (nmh->code)
           {
             case NM_RCLICK :
                  break ;

             case TCN_SELCHANGE :
                  UpdateTabbedWindow (TabCtrl_GetCurSel (tabbed_window), FALSE) ;
                  break ;
           }
           break ;
         }
         switch (nmh->code)
         {
           case TTN_NEEDTEXT :
                t = (TOOLTIPTEXT *) lParam ;
                if (use_tooltips == 0)
                {
                  t->lpszText = NULL ;
                  t->hinst = 0 ;
                  break ;
                }
                t->hinst = hInstance ;
                t->lpszText = MAKEINTRESOURCE (t->hdr.idFrom) ;
                return (0) ;
         }
         break ;

    case RENDER_MESSAGE :
         s = getCommandLine () ;
         if ((rendering || hRenderThread) && (strlen (s) || wParam))
         {
           PovMessageBox ("Cannot accept new command - already rendering", "Warning") ;
           return (0) ;
         }
         if (main_window_hidden)
         {
           ShowWindow (main_window, SW_SHOW) ;
           if (render_window != NULL)
             ShowWindow (render_window, SW_SHOW) ;
           PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Put &Main Window in Taskbar\tALT+W") ;
           main_window_hidden = 0 ;
           TaskBarDeleteIcon (main_window, 0) ;
           return (0) ;
         }
         if (wParam == 0)
         {
           if (strlen (s) == 0)
             return (0) ;
           if (!restore_command_line)
             strcpy (old_command_line, command_line) ;
           restore_command_line = TRUE ;
           strcpy (command_line, s) ;
           start_rendering (FALSE, TRUE) ;
           strcpy (command_line, old_command_line) ;
           restore_command_line = FALSE ;
         }
         else
           handle_main_command (CM_DEMO, 0) ;
         return (0) ;

    case WM_CREATE :
         main_window = hwnd ;
         hMainMenu = CreateMenu () ;
         build_main_menu (hMainMenu, FALSE) ;
         SetMenu (main_window, hMainMenu) ;
         break ;

    case WM_ENDSESSION :
         if ((BOOL) wParam)
         {
           setRunOnce () ;
           if (save_settings)
           {
             SendMessage (toolbar_combobox, WM_GETTEXT, sizeof (SecondaryRenderIniFileSection), (LPARAM) SecondaryRenderIniFileSection) ;
             if (restore_command_line)
               strcpy (command_line, old_command_line) ;
             write_INI_settings (EngineIniFileName) ;
             if (use_editors)
               Edit.SaveSettings () ;
           }
         }
         break ;

    case WM_NCCALCSIZE :
         // this takes care of assigning main_window while the window is still being created
         if (main_window == NULL)
           main_window = hwnd ;
         if (!non_client || isMaxiMinimized)
           break ;
         lRet = ncDefWindowProc (hwnd, message, wParam, lParam) ;
         calculate_statusbar () ;
         return (lRet) ;

    case WM_NCPAINT :
         if (isMaxiMinimized)
         {
           hdc = GetWindowDC (hwnd) ;
           oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
           if (hPalApp)
           {
             SelectPalette (hdc, hPalApp, FALSE) ;
             RealizePalette (hdc) ;
           }
           if (rendering && render_bitmap_surface != NULL && !rendersleep)
           {
             StretchDIBits (hdc, 4,
                                 4,
                                 64,
                                 64,
                                 0,
                                 0,
                                 render_bitmap.header.biWidth,
                                 render_bitmap.header.biHeight,
                                 render_bitmap_surface,
                                 (LPBITMAPINFO) &render_bitmap,
                                 DIB_RGB_COLORS,
                                 SRCCOPY) ;
           }
           else
           {
             hdcMemory = CreateCompatibleDC (hdc) ;
             oldBmp = SelectObject (hdcMemory, hBmpMaxiMini) ;
             BitBlt (hdc, 4, 4, 64, 64, hdcMemory, 0, 0, SRCCOPY) ;
             SelectObject (hdcMemory, oldBmp) ;
             DeleteDC (hdcMemory) ;
           }
           SetStretchBltMode (hdc, oldMode) ;
           ReleaseDC (hwnd, hdc) ;
         }
         if (IsIconic (main_window) && !IsW95UserInterface && rendering && render_bitmap_surface != NULL)
         {
           hdc = GetWindowDC (main_window) ;
           oldMode = SetStretchBltMode (hdc, STRETCH_DELETESCANS) ;
           if (hPalApp)
           {
             SelectPalette (hdc, hPalApp, FALSE) ;
             RealizePalette (hdc) ;
           }
           StretchDIBits (hdc, 0,
                               0,
                               36,
                               36,
                               0,
                               0,
                               render_bitmap.header.biWidth,
                               render_bitmap.header.biHeight,
                               render_bitmap_surface,
                               (LPBITMAPINFO) &render_bitmap,
                               DIB_RGB_COLORS,
                               SRCCOPY) ;
           SetStretchBltMode (hdc, oldMode) ;
           ReleaseDC (main_window, hdc) ;
           return (0) ;
         }
         if (!non_client)
           break ;
         lRet = ncDefWindowProc (hwnd, message, wParam, lParam) ;
         calculate_statusbar () ;
         paint_statusbar (-1) ;
         return (lRet) ;

    case WM_NCRBUTTONDOWN :
         if (hPopupMenus != NULL)
         {
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           TrackPopupMenu (GetSubMenu (hPopupMenus, 0), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
           return (0) ;
         }
         break ;

    case WM_COMMAND :
         if (use_editors)
           if (LOWORD (wParam) < CM_FIRST)
             return (Edit.Dispatch (LOWORD (wParam))) ;
         if (ExtensionsEnabled)
           if (LOWORD (wParam) >= CM_FIRSTGUIEXT && LOWORD (wParam) <= CM_LASTGUIEXT)
             return (ExternalMenuSelect (LOWORD (wParam))) ;
         if (handle_main_command (wParam, lParam))
           return (0) ;
         if (non_client)
           return (ncDefWindowProc (hwnd, message, wParam, lParam)) ;
         break ;

    case WM_INITMENU :
    case WM_INITMENUPOPUP :
         if (use_editors)
           Edit.UpdateMenus (CurrentEditor) ;
         break ;

    case WM_TIMER :
         seconds++ ;
         ExternalEvent (EventTimer, seconds) ;
         if (MenuBarDraw)
         {
           DrawMenuBar (main_window) ;
           MenuBarDraw = FALSE ;
         }
         if (!rendering && !hRenderThread)
         {
           // repaint this every now and then in case the defproc code writes to it
           if (isMaxiMinimized && seconds % 15 == 0)
             PostMessage (main_window, WM_NCPAINT, 0, 0L) ;
           if (auto_render)
           {
             if (queued_file_count)
             {
               queued_file_count-- ;
               update_queue_status (TRUE) ;
               strcpy (source_file_name, queued_files [0]) ;
               memcpy (queued_files [0], queued_files [1], sizeof (queued_files) - sizeof (queued_files [0])) ;
               splitpath (source_file_name, dir, NULL) ;
               SetCurrentDirectory (dir) ;
               if (!ExternalDragFunction (source_file_name, dfRenderFileQueue))
                 start_rendering (TRUE, FALSE) ;
             }
           }
         }
         else
         {
           render_finish_time = time (NULL) ;
           if (render_finish_time != render_start_time)
             status_printf (StatusPPM, "%u PPS", pixels / (render_finish_time - render_start_time)) ;
           say_status_message (StatusRendertime, get_elapsed_time (render_start_time, render_finish_time)) ;
           if (((IsIconic (main_window) && !IsW95UserInterface) || isMaxiMinimized) && render_bitmap_surface != NULL)
             PostMessage (main_window, WM_NCPAINT, 0, 0L) ;
           if (rendersleep)
             FlashWindow (main_window, seconds & 0x01) ;
         }
         return (0) ;

    case WM_PALETTECHANGED :
         // make sure it wasn't us who changed the palette, otherwise we can get into an infinite loop.
         if ((HWND) wParam == main_window)
           return (0) ;
         // FALL THROUGH to WM_QUERYNEWPALETTE

    case WM_QUERYNEWPALETTE :
         if (hPalApp)
         {
           hdc = GetDC (main_window) ;
           oldPalette = SelectPalette (hdc, hPalApp, FALSE) ;
           f = RealizePalette (hdc) ;
           SelectPalette (hdc, oldPalette, FALSE) ;
           ReleaseDC (main_window, hdc) ;
           if (f)
           {
             PovInvalidateRect (hwnd, NULL, TRUE) ;
             PovInvalidateRect (current_window, NULL, TRUE) ;
             PovInvalidateRect (render_window, NULL, TRUE) ;
           }
         }
         return (0) ;

    case WM_SIZE :
         if (!isMaxiMinimized)
         {
           mainwin_placement.length = sizeof (WINDOWPLACEMENT) ;
           GetWindowPlacement (main_window, &mainwin_placement) ;
         }
         switch (wParam)
         {
           case PVNC_MAXIMINIMIZE :
                GetWindowRect (main_window, &rect) ;
                if (lParam)
                {
                  if (toolbar_window)
                    ShowWindow (toolbar_window, SW_HIDE) ;
                  ShowWindow (tabbed_window, SW_HIDE) ;
                  maxiMiniX = rect.left ;
                  maxiMiniY = rect.top ;
                  maxiMiniW = rect.right - rect.left ;
                  maxiMiniStyle = GetWindowLong (main_window, GWL_STYLE) ;
                  SetWindowLong (main_window, GWL_STYLE, maxiMiniStyle & ~(WS_BORDER | WS_CAPTION | WS_SYSMENU)) ;
                  maxiMiniH = rect.bottom - rect.top ;
                  isMaxiMinimized = TRUE ;
                  if (render_window)
                    ShowWindow (render_window, SW_HIDE) ;
                  if (!lock_maximain || (maxiRect.left == -1 && maxiRect.right == -1))
                  {
                    maxiRect.left = rect.right - 72 ;
                    maxiRect.top = rect.top ;
                    if (maxiRect.left + 8 > screen_width)
                      maxiRect.left = screen_width - 8 ;
                  }
                  SetWindowPos (main_window, HWND_TOPMOST, maxiRect.left, maxiRect.top, 72, 72, SWP_SHOWWINDOW) ;
                  PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_ENABLED) ;
                  if (usedMaxiMini == FALSE)
                  {
                    MessageBox (main_window,
                                "As it does not appear that you have used this feature before, please note "
                                "that to exit the mini-window state that you activated, first dismiss "
                                "this dialog box, and then double-click on the small window.\n\n"
                                "Until then, the window will float on top of other applications. "
                                "The right-mouse button menu and accelerator commands still work if the "
                                "window has focus.",
                                "A word of advice",
                                MB_ICONINFORMATION) ;
                    PutPrivateProfileInt ("MainWindow", "UsedMaxiMini", TRUE, EngineIniFileName) ;
                    usedMaxiMini = TRUE ;
                  }
                  PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Restore &Main Window") ;
                  PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_ENABLED) ;
                }
                else
                {
                  isMaxiMinimized = FALSE ;
                  SetWindowLong (main_window, GWL_STYLE, maxiMiniStyle) ;
                  if (lock_maximain)
                    GetWindowRect (main_window, &maxiRect) ;
                  if (render_window)
                    ShowWindow (render_window, SW_SHOW) ;
                  if (toolbar_window && use_toolbar)
                    ShowWindow (toolbar_window, SW_SHOW) ;
                  ShowWindow (tabbed_window, SW_SHOW) ;
                  if (mainwin_placement.showCmd == SW_SHOWMAXIMIZED || lock_maximain)
                    SetWindowPos (main_window, HWND_NOTOPMOST, maxiMiniX, maxiMiniY, maxiMiniW, maxiMiniH, SWP_SHOWWINDOW) ;
                  else
                    SetWindowPos (main_window, HWND_NOTOPMOST, rect.right - maxiMiniW, rect.top, maxiMiniW, maxiMiniH, SWP_SHOWWINDOW) ;
                  SetFocus (main_window) ;
                  update_message_display (None) ;
                  message_scroll_pos_x = 0 ;
                  message_scroll_pos_y = top_message_row ;
                  if (use_taskbar)
                  {
                    PVModifyMenu (CM_SHOWMAINWINDOW, MF_STRING, CM_SHOWMAINWINDOW, "Put &Main Window in Taskbar\tALT+W") ;
                    PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_ENABLED) ;
                  }
                  else
                    PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_GRAYED) ;
                }
                ExternalEvent (EventSize, wParam) ;
                return (0) ;

           case SIZE_MINIMIZED :
                SetWindowText (main_window, rendersleep ? "POV-Ray for Windows (paused)" : "POV-Ray for Windows") ;
                if ((render_above_main || hide_render_window) && render_window != NULL)
                {
                  ShowWindow (render_window, SW_HIDE) ;
                  render_main_icon = TRUE ;
                }
                ExternalEvent (EventSize, wParam) ;
                return (0) ;

           case SIZE_MAXIMIZED :
           case SIZE_RESTORED :
                if (!isMaxiMinimized && render_main_icon && render_window != NULL)
                  ShowWindow (render_window, renderwin_active ? SW_SHOW : SW_SHOWNA) ;
                render_main_icon = FALSE ;
                SendMessage (toolbar_window, TB_AUTOSIZE, 0, 0) ;
                if (use_toolbar && toolheight == 0 && toolbar_window != NULL)
                {
                  GetClientRect (toolbar_window, &rect) ;
                  toolheight = rect.bottom + 1 ;
                }
                CalculateClientWindows (TRUE) ;
                // perhaps there's a bug in Windows 95 ?
                if (top_message_row)
                {
                  ShowScrollBar (message_window, SB_VERT, FALSE) ;
                  ShowScrollBar (message_window, SB_VERT, TRUE) ;
                }
                if (need_hscroll ())
                {
                  ShowScrollBar (message_window, SB_HORZ, FALSE) ;
                  ShowScrollBar (message_window, SB_HORZ, TRUE) ;
                }
                ExternalEvent (EventSize, wParam) ;
                break ;

           case SIZE_MAXHIDE :
           case SIZE_MAXSHOW :
           default :
                ExternalEvent (EventSize, wParam) ;
                return (0) ;
         }
         return (0) ;

    case WM_MOVE :
         if (!isMaxiMinimized)
         {
           mainwin_placement.length = sizeof (WINDOWPLACEMENT) ;
           GetWindowPlacement (main_window, &mainwin_placement) ;
         }
         ExternalEvent (EventMove, lParam) ;
         return (0) ;

    case WM_ERASEBKGND :
         if (IsIconic (main_window))
         {
           BitBlt ((HDC) wParam, 0, 0, 36, 36, NULL, 0, 0, BLACKNESS) ;
           return (1) ;
         }
         break ;

    case WM_DROPFILES :
         DragFunction ((HANDLE) wParam) ;
         return (0) ;

    case WM_KEYDOWN :
         for (i = 0 ; key2scroll [i].wVirtkey != -1 ; i++)
         {
           if (wParam == key2scroll [i].wVirtkey)
           {
             SendMessage (message_window, key2scroll [i].iMessage, key2scroll [i].wRequest, 0L) ;
             return (0) ;
           }
         }
         break ;

    case WM_MENUSELECT :
         if (!isMaxiMinimized)
           handle_menu_select (wParam, lParam) ;
         return (0) ;

    case WM_CLOSE :
         if (debugging)
           message_printf ("DEBUG : Close requested, rendering is %d, quit is %d\n", rendering, quit) ;
         if ((rendering || hRenderThread) && !quit)
         {
           if (MessageBox (main_window,
                           "POV-Ray is currently rendering - do you want to stop ?",
                           "Stop rendering",
                           MB_ICONQUESTION | MB_YESNO) == IDNO)
           {
             return (0) ;
           }
         }
         if (use_editors)
           if (!Edit.CanClose ())
             return (0) ;
         ExternalEvent (EventClose, 0) ;
         if (timer_id != 0)
           KillTimer (main_window, timer_id) ;
         DragAcceptFiles (main_window, FALSE) ;
         if (!rendering || quit)
         {
           WinHelp (main_window, "pvengine.hlp", HELP_QUIT, NULL) ;
           WinHelp (main_window, "povray30.hlp", HELP_QUIT, NULL) ;
           DestroyWindow (main_window) ;
         }
         else
           quit = TRUE ;
         return (0) ;

    case WM_DESTROY :
         ExternalEvent (EventDestroy, 0) ;
         if (save_settings)
         {
           SendMessage (toolbar_combobox, WM_GETTEXT, sizeof (SecondaryRenderIniFileSection), (LPARAM) SecondaryRenderIniFileSection) ;
           if (restore_command_line)
             strcpy (command_line, old_command_line) ;
           write_INI_settings (EngineIniFileName) ;
           if (use_editors)
             Edit.SaveSettings () ;
         }
         PostQuitMessage (0) ;
         return (0) ;
  }
  if (non_client)
    return (ncDefWindowProc (hwnd, message, wParam, lParam)) ;
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovMessageWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int         nhs ;
  HDC         hdc ;
  RECT        rect ;
  POINT       pt ;
  PAINTSTRUCT ps ;

  switch (message)
  {
    case WM_KEYDOWN :
         PostMessage (main_window, message, wParam, lParam) ;
         return (0) ;

    case WM_RBUTTONDOWN :
         if (hPopupMenus != NULL)
         {
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           ClientToScreen (hwnd, &pt) ;
           TrackPopupMenu (GetSubMenu (hPopupMenus, 0), TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, main_window, NULL) ;
         }
         return (0) ;

    case WM_ERASEBKGND :
         return (1) ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (!isMaxiMinimized)
         {
           if (hPalApp)
           {
             SelectPalette (hdc, hPalApp, FALSE) ;
             RealizePalette (hdc) ;
           }
           paint_display_window (hdc) ;
         }
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_SIZE :
         if (!isMaxiMinimized)
         {
           if (message_count)
           {
             GetClientRect (hwnd, &rect) ;
             message_scroll_pos_x = 0 ;
             message_scroll_pos_y = message_count - rect.bottom / message_ychar ;
             if (message_scroll_pos_y < 0)
               message_scroll_pos_y = 0 ;
           }
           update_message_display (None) ;
           PovInvalidateRect (hwnd, NULL, TRUE) ;
         }
         return (0) ;

    case WM_VSCROLL :
         if (!isMaxiMinimized)
         {
           switch (LOWORD (wParam))
           {
             case SB_LINEDOWN :
                  if (message_scroll_pos_y < message_count - message_rows)
                  {
                    message_scroll_pos_y++ ;
                    ScrollWindow (hwnd, 0, -message_ychar, NULL, NULL) ;
                    update_message_display (None) ;
                    UpdateWindow (hwnd) ;
                  }
                  break ;

             case SB_LINEUP :
                  if (message_scroll_pos_y > 0)
                  {
                    message_scroll_pos_y-- ;
                    ScrollWindow (hwnd, 0, message_ychar, NULL, NULL) ;
                    update_message_display (None) ;
                    UpdateWindow (hwnd) ;
                  }
                  break ;

             case SB_PAGEDOWN :
                  if (message_scroll_pos_y < message_count - message_rows)
                  {
                    message_scroll_pos_y += message_rows ;
                    if (message_scroll_pos_y > message_count - message_rows)
                      message_scroll_pos_y = message_count - message_rows ;
                    PovInvalidateRect (hwnd, NULL, TRUE) ;
                    update_message_display (None) ;
                  }
                  break ;

             case SB_PAGEUP :
                  if (message_scroll_pos_y > 0)
                  {
                    message_scroll_pos_y -= message_rows ;
                    if (message_scroll_pos_y < 0)
                      message_scroll_pos_y = 0 ;
                    PovInvalidateRect (hwnd, NULL, TRUE) ;
                    update_message_display (None) ;
                  }
                  break ;

             case SB_THUMBPOSITION :
             case SB_THUMBTRACK :
                  message_scroll_pos_y = HIWORD (wParam) ;
                  PovInvalidateRect (hwnd, NULL, TRUE) ;
                  update_message_display (None) ;
                  break ;
           }
         }
         return (0) ;

    case WM_HSCROLL :
         if (!isMaxiMinimized)
         {
           nhs = need_hscroll () ;
           switch (LOWORD (wParam))
           {
             case SB_LINERIGHT :
                  if (message_scroll_pos_x < nhs)
                  {
                    message_scroll_pos_x++ ;
                    ScrollWindow (hwnd, -message_xchar, 0, NULL, NULL) ;
                    update_message_display (None) ;
                    UpdateWindow (hwnd) ;
                  }
                  break ;

             case SB_LINELEFT :
                  if (message_scroll_pos_x > 0)
                  {
                    message_scroll_pos_x-- ;
                    ScrollWindow (hwnd, message_xchar, 0, NULL, NULL) ;
                    update_message_display (None) ;
                    UpdateWindow (hwnd) ;
                  }
                  break ;

             case SB_PAGERIGHT :
                  if (message_scroll_pos_x < nhs)
                  {
                    message_scroll_pos_x += message_cols ;
                    if (message_scroll_pos_x > nhs)
                      message_scroll_pos_x = nhs ;
                    PovInvalidateRect (hwnd, NULL, TRUE) ;
                    update_message_display (None) ;
                  }
                  break ;

             case SB_PAGELEFT :
                  if (message_scroll_pos_x > 0)
                  {
                    message_scroll_pos_x -= message_cols ;
                    if (message_scroll_pos_x < 0)
                      message_scroll_pos_x = 0 ;
                    PovInvalidateRect (hwnd, NULL, TRUE) ;
                    update_message_display (None) ;
                  }
                  break ;

             case SB_THUMBPOSITION :
             case SB_THUMBTRACK :
                  message_scroll_pos_x = HIWORD (wParam) ;
                  PovInvalidateRect (hwnd, NULL, TRUE) ;
                  update_message_display (None) ;
                  break ;
           }
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

LRESULT CALLBACK PovSplashWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HDC         hdc ;
  HDC         hdcMemory ;
  BOOL        f ;
  HBITMAP     oldBmp ;
  HPALETTE    oldPalette ;
  PAINTSTRUCT ps ;

  switch (message)
  {
    case WM_DESTROY :
         if (screen_depth <= 8)
         {
           hdc = GetDC (hwnd) ;
           BitBlt (hdc, 0, 0, splash_width, splash_height, NULL, 0, 0, BLACKNESS) ;
           ReleaseDC (hwnd, hdc) ;
         }
         break ;

    case WM_PAINT :
         hdc = BeginPaint (hwnd, &ps) ;
         if (hPalApp)
         {
           SelectPalette (hdc, hPalApp, FALSE) ;
           RealizePalette (hdc) ;
         }
         hdcMemory = CreateCompatibleDC (hdc) ;
         oldBmp = SelectObject (hdcMemory, hBmpSplash) ;
         BitBlt (hdc, 0, 0, splash_width, splash_height, hdcMemory, 0, 0, SRCCOPY) ;
         SelectObject (hdcMemory, oldBmp) ;
         DeleteDC (hdcMemory) ;
         EndPaint (hwnd, &ps) ;
         return (0) ;

    case WM_PALETTECHANGED :
         // make sure it wasn't us who changed the palette, otherwise we can get into an infinite loop.
         if ((HWND) wParam == hwnd)
           return (0) ;
         // FALL THROUGH to WM_QUERYNEWPALETTE

    case WM_QUERYNEWPALETTE :
         if (hPalApp)
         {
           hdc = GetDC (hwnd) ;
           oldPalette = SelectPalette (hdc, hPalApp, FALSE) ;
           f = RealizePalette (hdc) ;
           SelectPalette (hdc, oldPalette, FALSE) ;
           ReleaseDC (hwnd, hdc) ;
           if (f)
             PovInvalidateRect (hwnd, NULL, TRUE) ;
         }
         return (0) ;
  }
  return (DefWindowProc (hwnd, message, wParam, lParam)) ;
}

int register_classes (void)
{
  WNDCLASS    wc ;

  // Register the main window class.
  wc.style         = 0 ;
  wc.lpfnWndProc   = PovMainWndProc ;
  wc.cbClsExtra    = 0 ;
  wc.cbWndExtra    = 0 ;
  wc.hInstance     = hInstance ;
  wc.hIcon         = ourIcon ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.hbrBackground = NULL ;
  wc.lpszMenuName  = NULL ;
  wc.lpszClassName = PovMainWinClass ;
  if (RegisterClass (&wc) == FALSE)
    return (FALSE) ;

  // Register the message window class.
  wc.style         = 0 ;
  wc.lpfnWndProc   = PovMessageWndProc ;
  wc.cbClsExtra    = 0 ;
  wc.cbWndExtra    = 0 ;
  wc.hInstance     = hInstance ;
  wc.hIcon         = NULL ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.hbrBackground = NULL ;
  wc.lpszMenuName  = NULL ;
  wc.lpszClassName = PovMessageWinClass ;
  if (RegisterClass (&wc) == FALSE)
    return (FALSE) ;

  // Register the render window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovRenderWndProc ;
  wc.cbClsExtra    = 0 ;
  wc.cbWndExtra    = 0 ;
  wc.hInstance     = hInstance ;
  wc.hIcon         = NULL ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.hbrBackground = NULL ;
  wc.lpszMenuName  = NULL ;
  wc.lpszClassName = PovRenderWinClass ;
  if (RegisterClass (&wc) == FALSE)
    return (FALSE) ;

  // Register the splash window class.
  wc.style         = CS_BYTEALIGNCLIENT ;
  wc.lpfnWndProc   = PovSplashWndProc ;
  wc.cbClsExtra    = 0 ;
  wc.cbWndExtra    = 0 ;
  wc.hInstance     = hInstance ;
  wc.hIcon         = NULL ;
  wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
  wc.hbrBackground = NULL ;
  wc.lpszMenuName  = NULL ;
  wc.lpszClassName = PovSplashWinClass ;
  if (RegisterClass (&wc) == FALSE)
    return (FALSE) ;

  return (TRUE) ;
}

void cleanup_all (void)
{
  ExternalCleanupAll () ;
  if (use_taskbar)
    TaskBarDeleteIcon (main_window, 0) ;
  WinHelp (main_window, "pvengine.hlp", HELP_QUIT, NULL) ;
  WinHelp (main_window, "povray30.hlp", HELP_QUIT, NULL) ;
  DeleteCriticalSection (&critical_section) ;
  if (hLibCtl3d != NULL)
    FreeLibrary (hLibCtl3d) ;
  if (hLibPovEdit != NULL)
    FreeLibrary (hLibPovEdit) ;
  display_cleanup () ;
  if (hBmpBackground != NULL)
    DeleteObject (hBmpBackground) ;
  if (hBmpRendering != NULL)
    DeleteObject (hBmpRendering) ;
  if (hBmpMaxiMini != NULL)
    DeleteObject (hBmpMaxiMini) ;
  if (hBmpIcon != NULL)
    DeleteObject (hBmpIcon) ;
  if (hBmpSplash != NULL)
    DeleteObject (hBmpSplash) ;
  if (hExpertMenu)
    DestroyMenu (hExpertMenu) ;
  if (hNonExpertMenu)
    DestroyMenu (hNonExpertMenu) ;
  if (hMainMenu)
    DestroyMenu (hMainMenu) ;
  if (hPopupMenus)
    DestroyMenu (hPopupMenus) ;
  if (hPalApp)
    DeleteObject (hPalApp) ;
  if (message_font)
    DeleteObject (message_font) ;
  if (tab_font)
    DeleteObject (tab_font) ;
  if (ourIcon)
    DestroyIcon (ourIcon) ;
  UnregisterClass (PovRenderWinClass, hInstance) ;
  UnregisterClass (PovMessageWinClass, hInstance) ;
  UnregisterClass (PovMainWinClass, hInstance) ;
  UnregisterClass (PovSplashWinClass, hInstance) ;
}

void SplashScreen (void)
{
  BITMAP                bm ;

  if ((hBmpSplash = NonBogusLoadBitmapAndPalette (hInstance, MAKEINTRESOURCE (BMP_SPLASH))) != NULL)
  {
    GetObject (hBmpSplash, sizeof (BITMAP), (LPSTR) &bm) ;
    splash_width = bm.bmWidth ;
    splash_height = bm.bmHeight ;
    splash_window = CreateWindowEx (0, // no longer using WS_EX_TOPMOST,
                                    PovSplashWinClass,
                                    "POV-Ray",
                                    WS_VISIBLE | WS_POPUP,
                                    0,
                                    0,
                                    splash_width,
                                    splash_height,
                                    NULL,
                                    NULL,
                                    hInstance,
                                    NULL) ;
    if (splash_window != NULL)
      CenterWindowRelative (NULL, splash_window, FALSE) ;
  }
}

int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
  int                   show_state ;
  int                   i ;
  int                   id = 0 ;
  int                   w, h ;
  char                  str [_MAX_PATH] ;
  char                  dir [_MAX_PATH] ;
  char                  *s ;
  void                  *Editor ;
  unsigned              n ;
  unsigned              splash_time = 1 ;
  MSG                   msg ;
  HDC                   hDC ;
  HDIB                  hDIB ;
  RECT                  rect ;
  HWND                  hwnd ;
  BITMAP                bm ;
  HBITMAP               hBMP ;
  struct stat           statbuf ;
  BOOL                  (WINAPI *Ctl3dAutoSubclass) (HINSTANCE);
  BOOL                  (WINAPI *Ctl3dRegister) (HINSTANCE);
  BOOL                  (WINAPI *Ctl3dUnregister) (HINSTANCE);
  WINDOWPLACEMENT       placement ;

//SetUnhandledExceptionFilter ((LPTOP_LEVEL_EXCEPTION_FILTER) PovUnhandledExceptionFilter) ;

  hInstance = hInst ;
  ourIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_NEW_PVENGINE)) ;
  if (hPrev == NULL)
    if (register_classes () == FALSE)
      MessageBox (NULL, "ERROR : Could not register classes", "Error", MB_ICONSTOP) ;

  detect_graphics_config () ;
  clear_system_palette () ;
  SplashScreen () ;

  strcpy (str, szCmdLine) ;
  strupr (str) ;
  if ((s = strstr (str, "PVENGINE.EXE")) != NULL)
  {
    szCmdLine += (int) (s - str) + 12 ;
    if (*szCmdLine == '"')
      szCmdLine++ ;
  }

  if (strnicmp (szCmdLine, "/DEBUG", 6) == 0)
  {
    debugging++ ;
    debugFile = fopen ("c:\\povray.dbg", "wt") ;
  }

  version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  GetVersionEx (&version_info) ;
  hMainThread = GetCurrentThread () ;
  use_editors = TRUE ;

  IsWin32 = HaveWin95 () || HaveWinNT () ;
  if (HaveWin95 ())
  {
    using_ctl3d = FALSE ;
    IsW95UserInterface = TRUE ;
  }

  detect_graphics_config () ;
  init_menus () ;

  getHome () ;
  if (HomePath [0] == '\0')
  {
    if (debugFile)
      fprintf (debugFile, "querying INI file\n") ;
    GetProfileString ("POV-Ray v3.0 for Windows", "Home", "", HomePath, sizeof (HomePath)) ;
    if (debugFile)
      fprintf (debugFile, "windows returned '%s' for Home\n", HomePath) ;
    if (HomePath [0] == '\0')
    {
      if (debugFile)
      {
        fprintf (debugFile, "invalid home entry. GetLastError () is %08lx\n", GetLastError ()) ;
        GetWindowsDirectory (dir, sizeof (dir)) ;
        fprintf (debugFile, "Windows directory is %s\n", dir) ;
        sprintf (str, "%s\\win.ini", dir) ;
        GetPrivateProfileString ("POV-Ray v3.0 for Windows", "Home", "", HomePath, sizeof (HomePath), str) ;
        fprintf (debugFile, "Tried GetPrivateProfileString (), result is '%s'\n", HomePath) ;
        GetProfileString ("POV", "Home", "", HomePath, sizeof (HomePath)) ;
        fprintf (debugFile, "Tried using 'POV', result is '%s'\n", HomePath) ;
      }
      MessageBox (NULL,
                  "ERROR : Cannot find Home entry in registry or WIN.INI\n\n"
                  "This entry should have been set by the installation program.\n\n"
                  "If you did not install using the correct installation procedure, please "
                  "do this before running POV-Ray for Windows. Otherwise, consult the README.DOC "
                  "file that should have accompanied this executable file.",
                  "Critical Error",
                  MB_ICONSTOP) ;
      return (1) ;
    }
  }
  if (HomePath [strlen (HomePath) - 1] != '\\')
    strcat (HomePath, "\\") ;
  strupr (HomePath) ;
  sprintf (EngineIniFileName, "%sINI", HomePath) ;

  if (debugFile)
    fprintf (debugFile, "INI path is '%s'\n", EngineIniFileName) ;

  if (stat (EngineIniFileName, &statbuf) != 0 || (statbuf.st_mode & S_IFREG) != 0)
  {
    if (debugFile)
      fprintf (debugFile, "INI directory not found\n") ;
    MessageBox (NULL,
                "ERROR : Cannot find INI directory in expected location\n\n"
                "This directory should have been created by the installation program.\n\n"
                "If you did not install using the correct installation procedure, please "
                "do this before running POV-Ray for Windows. Otherwise, consult the README.DOC "
                "file that should have accompanied this executable file.",
                "Critical Error",
                MB_ICONSTOP) ;
    return (1) ;
  }

  strcat (EngineIniFileName, "\\"INIFILENAME) ;
  sprintf (DefaultRenderIniFileName, "%sRENDERER\\POVRAY.INI", HomePath) ;
  sprintf (RerunIniPath, "%sRENDERER\\RERUN\\", HomePath) ;
  sprintf (CurrentRerunFileName, "%sRENDERER\\RERUN\\CURRENT.INI", HomePath) ;
  sprintf (ToolIniFileName, "%sINI\\"TOOLFILENAME, HomePath) ;

  GetModuleFileName (hInst, str, sizeof (str) - 1) ;
  splitpath (str, ourPath, NULL) ;

  sprintf (engineHelpPath, "%shelp\\PVENGINE.HLP", HomePath) ;
  sprintf (rendererHelpPath, "%shelp\\POVRAY30.HLP", HomePath) ;

  if (GetPrivateProfileInt ("General", "UseW95UserInterface", 0, EngineIniFileName))
    IsW95UserInterface = TRUE ;

  read_INI_settings (EngineIniFileName) ;
  if (!IsWin32)
    IsW95UserInterface = FALSE ;
  if (IsW95UserInterface)
    using_ctl3d = FALSE ;
  if (!IsWin32)
    use_threads = FALSE ;

  if (!IsW95UserInterface)
  {
    PVEnableMenuItem (CM_SHOWMAINWINDOW, MF_GRAYED) ;
    use_taskbar = FALSE ;
  }

  InitializeCriticalSection (&critical_section) ;

  if (GetPrivateProfileInt ("General", "BigSplash", 0, EngineIniFileName) + 86400 < time (NULL))
    splash_time = 4 ;

  GetPrivateProfileString ("General", "CommandLine", "", old_command_line, sizeof (old_command_line), EngineIniFileName) ;
  if ((szCmdLine = preparse_commandline (szCmdLine)) != NULL)
  {
    parse_commandline (szCmdLine) ;
    if (!demo_mode)
    {
      restore_command_line = TRUE ;
      strncpy (command_line, szCmdLine, sizeof (command_line) - 1) ;
    }
  }

  if (one_instance && (hwnd = FindWindow (PovMainWinClass, NULL)) != NULL)
  {
    setCommandLine (command_line) ;
    if (IsIconic (hwnd))
      ShowWindow (hwnd, SW_RESTORE) ;
    SetForegroundWindow (hwnd) ;
    PostMessage (hwnd, RENDER_MESSAGE, demo_mode, 0) ;
    return (0) ;
  }

  if (hPrev == NULL)
  {
#ifdef __USECTL3D__
    if (using_ctl3d)
    {
      if ((hLibCtl3d = LoadLibrary ("CTL3D32.DLL")) != NULL)
      {
        Ctl3dRegister = GetProcAddress (hLibCtl3d, "Ctl3dRegister") ;
        Ctl3dAutoSubclass = GetProcAddress (hLibCtl3d, "Ctl3dAutoSubclass") ;
        Ctl3dUnregister = GetProcAddress (hLibCtl3d, "Ctl3dUnregister") ;
        if (Ctl3dRegister != NULL && Ctl3dAutoSubclass != NULL && Ctl3dUnregister != NULL)
        {
          if (Ctl3dRegister (hInst))
            Ctl3dAutoSubclass (hInst) ;
          else
            PovMessageBox ("CTL3D initialisation failed - check version of CTL3D32.DLL\n\n[Execution will continue normally]", "CTL3D Error") ;
        }
        else
          PovMessageBox ("CTL3D initialisation failed [GetProcAddress for CTL3D32.DLL failed.]", "CTL3D Error") ;
      }
      else
        PovMessageBox ("CTL3D initialisation failed [LoadLibrary for CTL3D32.DLL failed.]", "CTL3D Error") ;
    }
#endif
  }

  if (use_editors)
  {
    sprintf (str, "%sBIN\\EDITDLL.DLL", HomePath) ;
    if (debugFile)
    {
      fprintf (debugFile, "PovEdit path is '%s'\n", str) ;
      if (stat (str, &statbuf) != 0)
        fprintf (debugFile, "(We didn't find it)\n") ;
    }
    if ((hLibPovEdit = LoadLibrary (str)) == NULL)
    {
      if (debugFile)
        fprintf (debugFile, "Could not load PovEdit, error code is %08lx\n", GetLastError ()) ;
      sprintf (str, "PovEdit initialisation failed [LoadLibrary failed, code is %08lx]", GetLastError ()) ;
      PovMessageBox (str, "POV-Ray Editor error") ;
      PovMessageBox ("See the 'Built-In Editors' section in the help file", "Important!") ;
      PutPrivateProfileInt ("General", "UseEditors", use_editors = 0, EngineIniFileName) ;
    }
    else
    {
      EditGetVersion = (void *) GetProcAddress (hLibPovEdit, "EditGetVersion") ;
      if (EditGetVersion == NULL)
      {
        if (debugFile)
          fprintf (debugFile, "Could not get Editor process address, error code is %08lx\n", GetLastError ()) ;
        PovMessageBox ("PovEdit initialisation failed [GetProcAddress for EDITDLL.DLL failed.]", "POV-Ray Editor Error") ;
        PovMessageBox ("See the 'Built-In Editors' section in the help file", "Important!") ;
        PutPrivateProfileInt ("General", "UseEditors", use_editors = 0, EngineIniFileName) ;
      }
      if (EditGetVersion (sizeof (Edit), &Edit) != 101)
      {
        MessageBox (NULL, "ERROR : Wrong editor DLL version.", "Critical Error", MB_ICONSTOP) ;
        PovMessageBox ("See the 'Built-In Editors' section in the help file", "Important!") ;
        PutPrivateProfileInt ("General", "UseEditors", use_editors = 0, EngineIniFileName) ;
      }
    }
  }

  GetPrivateProfileString ("General", "Version", "", str, strlen (str), EngineIniFileName) ;
  if (debugFile)
    fprintf (debugFile, "INI version is %s, and we are %s\n", str, PVENGINE_VER) ;
  newVersion = strcmp (str, PVENGINE_VER) ;

  if ((run_count = GetPrivateProfileInt ("General", "RunCount", 0, EngineIniFileName)) == 0 || newVersion)
  {
    if (screen_depth < 8)
    {
      MessageBox (NULL,
                  "NOTE : POV-Ray for Windows was not designed to run in 16-colour mode. "
                  "While the program will operate, it is recommended that you use a minimum "
                  "graphics mode of 800x600x256.",
                  "Warning - running in 16-colour mode",
                  MB_ICONEXCLAMATION) ;
      tile_background = FALSE ;
    }
    if (screen_width < 800)
    {
      MessageBox (NULL,
                  "NOTE : POV-Ray for Windows was not designed to run at less than 800x600.\n\n"
                  "While the program will operate, it is recommended that you use a minimum "
                  "graphics mode of 800x600x256.",
                  "Warning - running at less than 800x600",
                  MB_ICONEXCLAMATION) ;
    }
  }
  PutPrivateProfileInt ("General", "RunCount", ++run_count, EngineIniFileName) ;

  if (screen_depth < 8)
    tile_background = FALSE ;

  hPopupMenus = LoadMenu (hInstance, MAKEINTRESOURCE (IsWin32 ? POPUP_MENUS32 : POPUP_MENUS)) ;
  hAccelerators = LoadAccelerators (hInstance, MAKEINTRESOURCE (PVENGINE_MENU)) ;

  initialise_statusbar (FALSE) ;

  /* Create the main window */
  placement = mainwin_placement ;
  placement.length = sizeof (WINDOWPLACEMENT) ;
  w = mainwin_placement.rcNormalPosition.right - mainwin_placement.rcNormalPosition.left ;
  h = mainwin_placement.rcNormalPosition.bottom - mainwin_placement.rcNormalPosition.top ;
  if (w <= 0)
    w = 512 ;
  if (h <= 0)
    h = 512 ;

  main_window = CreateWindowEx (0,
                                PovMainWinClass,
                                "POV-Ray for Windows",
                                WS_OVERLAPPEDWINDOW,
                                mainwin_placement.rcNormalPosition.left,
                                mainwin_placement.rcNormalPosition.top,
                                w,
                                h,
                                NULL,
                                NULL,
                                hInst,
                                NULL) ;

  if (main_window == NULL)
  {
    MessageBox (NULL, "ERROR : Could not create main window.", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  if (use_template == 2)
  {
    use_template = TRUE ;
    PVCheckMenuItem (CM_USETEMPLATE, use_template ? MF_CHECKED : MF_UNCHECKED) ;
  }

  if ((timer_id = SetTimer (main_window, 1, 1000, NULL)) != 0)
    DragAcceptFiles (main_window, TRUE) ;

  if (splash_window != NULL)
  {
    if (timer_id != 0)
    {
      while (seconds < splash_time && GetMessage (&msg, NULL, 0, 0) == TRUE)
      {
        if (!TranslateAccelerator (main_window, hAccelerators, &msg))
        {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
        }
      }
    }
    DestroyWindow (splash_window) ;
    DeleteObject (hBmpSplash) ;
    hBmpSplash = NULL ;
    if (splash_time == 4)
      PutPrivateProfileInt ("General", "BigSplash", time (NULL), EngineIniFileName) ;
  }

  create_palette (NULL, 0) ;
  if (tile_background && background_file [1])
  {
    if ((hDIB = LoadDIB (background_file)) != NULL)
    {
      hBmpBackground = DIBToBitmap (hDIB, hPalApp) ;
      DeleteObject (hDIB) ;
      GetObject (hBmpBackground, sizeof (BITMAP), (LPSTR) &bm) ;
      background_width = bm.bmWidth ;
      background_height = bm.bmHeight ;
    }
    else
    {
      PovMessageBox ("Failed to load bitmap file", "Error") ;
      strcpy (background_file, "0") ;
    }
  }

  if (tile_background && hBmpBackground == NULL && screen_depth >= 8)
  {
    if (isdigit (background_file [0]) && background_file [1] == '\0')
      id = background_file [0] - '0' ;
    SendMessage (main_window, WM_COMMAND, CM_BACKGROUNDSTD + id, 1L) ;
  }

  if ((hBMP = NonBogusLoadBitmap (hInstance, MAKEINTRESOURCE (BMP_MAXIMINI))) != NULL)
    hBmpMaxiMini = hBMP ;

  if ((hBMP = LoadBitmap (hInstance, MAKEINTRESOURCE (BMP_ICON))) != NULL)
    hBmpIcon = hBMP ;

  niceness_milliseconds = set_niceness (niceness) ;

  if (lastBitmapPath [0] == '\0')
    sprintf (lastBitmapPath, "%sTILES", HomePath) ;
  if (lastRenderPath [0] == '\0')
  {
    sprintf (lastRenderPath, "%sPOV3DEMO\\OBJECTS", HomePath) ;
    strcpy (lastRenderName, "TORUS1.POV") ;
  }
  if (lastQueuePath [0] == '\0')
    sprintf (lastQueuePath, "%sPOV3DEMO", HomePath) ;
  GetPrivateProfileString ("Editor", "LastPath", "", str, sizeof (str), EngineIniFileName) ;
  validatePath (lastRenderPath) ;
  if (str [0] == '\0')
  {
    WritePrivateProfileString ("Editor", "LastPath", lastRenderPath, EngineIniFileName) ;
    joinPath (str, lastRenderPath, lastRenderName) ;
    WritePrivateProfileString ("Editor", "Recent1", str, EngineIniFileName) ;
  }
  if (lastRenderName [0] != '\0' && !demo_mode)
    joinPath (source_file_name, lastRenderPath, lastRenderName) ;

  add_rerun_to_menu () ;

  tabbed_window = create_tabbed_window (main_window) ;
  if (tabbed_window == NULL)
  {
    MessageBox (NULL, "ERROR : Could not create tab window.", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  /* Create the message window */
  message_window = CreateWindowEx (WS_EX_CLIENTEDGE,
                                   PovMessageWinClass,
                                   "",
                                   WS_CHILDWINDOW | WS_CLIPSIBLINGS,
                                   0,
                                   0,
                                   0,
                                   0,
                                   tabbed_window,
                                   NULL,
                                   hInst,
                                   NULL) ;
  if (message_window == NULL)
  {
    MessageBox (NULL, "ERROR : Could not create message window.", "Critical Error", MB_ICONSTOP) ;
    cleanup_all () ;
    return (1) ;
  }

  if (initialise_message_display ())
  {
    cleanup_all () ;
    return (1) ;
  }

  toolbar_window = create_toolbar (main_window) ;
  extract_ini_sections (SecondaryRenderIniFileName, toolbar_combobox) ;
  initialise_tabbed_window (tabbed_window) ;
  add_window_to_tab (message_window, 0, "Messages") ;
  SendMessage (toolbar_combobox, CB_SELECTSTRING, -1, (LPARAM) SecondaryRenderIniFileSection) ;
  if (use_editors)
  {
    Edit.Init (hInstance) ;
    if ((editor_window = Edit.CreateEditor (main_window, tabbed_window, HomePath)) == NULL)
    {
      MessageBox (main_window, "ERROR : Could not create editor window.", "Critical Error", MB_ICONSTOP) ;
      cleanup_all () ;
      return (1) ;
    }
    if (load_editors (EngineIniFileName) == 0)
    {
      if ((Editor = Edit.GetEditor ()) == NULL)
      {
        MessageBox (main_window, "ERROR : Could not create editor.", "Critical Error", MB_ICONSTOP) ;
        cleanup_all () ;
        return (1) ;
      }
      add_window_to_tab (editor_window, Editor, Edit.GetTitle (Editor, NULL, NULL, NULL, NULL)) ;
      CurrentEditor = Editor ;
    }
    Edit.SelectEditor (CurrentEditor) ;
    Edit.GetHandles (&editor_handles) ;
    status_window = editor_handles.StatusWindow ;
    GetClientRect (status_window, &rect) ;
    panel_size = rect.bottom + 1 ;
  }
  else
    add_window_to_tab (message_window, 0, "Messages") ;

  setup_menus (use_editors) ;
  build_main_menu (hMainMenu, use_editors) ;
  current_window = message_window ;

  set_toggles () ;

  if (non_client)
    PVCheckMenuItem (CM_NONCLIENT, MF_CHECKED) ;
  if (use_editors)
    PVCheckMenuItem (CM_USEEDITOR, MF_CHECKED) ;
  PVEnableMenuItem (CM_RENDERHIDE, render_above_main ? MF_GRAYED : MF_ENABLED) ;
  PVCheckMenuItem (on_completion, MF_CHECKED) ;
  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  PVEnableMenuItem (CM_RENDERSLEEP, MF_GRAYED) ;

  if (IsWin32)
  {
    PVCheckMenuItem (render_priority, MF_CHECKED) ;
    PVCheckMenuItem (GUI_priority, MF_CHECKED) ;
  }
  else
  {
    PVCheckMenuItem (CM_NICENESS + niceness, MF_CHECKED) ;
    PVCheckMenuItem (CM_USECTL3D, using_ctl3d ? MF_CHECKED : MF_UNCHECKED) ;
  }

  PVCheckMenuItem (drop_to_editor ? CM_DROPEDITOR : CM_DROPRENDERER, MF_CHECKED) ;

  PVEnableMenuItem (CM_RENDERSHOW, MF_GRAYED) ;
  PVEnableMenuItem (CM_RENDERCLOSE, MF_GRAYED) ;
  PVModifyMenu (CM_TILEDBACKGROUND, MF_STRING, CM_TILEDBACKGROUND, tile_background ? "&Select Plain Background" : "&Select Tiled Background") ;
  if (screen_depth < 8)
  {
    PVEnableMenuItem (CM_TILEDBACKGROUND, MF_GRAYED) ;
    PVEnableMenuItem (CM_BACKGROUNDBITMAP, MF_GRAYED) ;
    for (i = 0 ; i < 16 ; i++)
      PVEnableMenuItem (CM_BACKGROUNDSTD + i, MF_GRAYED) ;
  }

  if (!use_editors)
  {
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILENEW, 0L) ;
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILEOPEN, 0L) ;
    SendMessage (toolbar_window, TB_ENABLEBUTTON, CM_FILESAVE, 0L) ;
  }

  GetWindowsDirectory (str, sizeof (str)) ;
  strcat (str, "\\gocserve.exe") ;
  if (stat (str, &statbuf) != 0)
    PVDeleteMenuItem (CM_GOPOVRAY) ;
  if (!IsW95UserInterface)
    PVDeleteMenuItem (CM_GOPOVRAYORG) ;

  switch (placement.showCmd)
  {
    case SW_SHOWNORMAL :
         show_state = SW_SHOW ;
         break ;

    case SW_SHOWMINIMIZED :
         show_state = SW_SHOWMINNOACTIVE ;
         break ;

    case SW_SHOWMAXIMIZED :
         show_state = SW_SHOWMAXIMIZED ;
         break ;

    default :
         show_state = SW_SHOW ;
         break ;
  }

  placement.showCmd = show_state ;
  placement.flags = (placement.ptMinPosition.x == -1 && placement.ptMinPosition.y == -1) ? 0 : WPF_SETMINPOSITION ;
  if (placement.rcNormalPosition.right <= 0 || placement.rcNormalPosition.bottom <= 0)
  {
    placement.rcNormalPosition.right = placement.rcNormalPosition.left + message_xchar * 115 ;
    placement.rcNormalPosition.bottom = placement.rcNormalPosition.top + message_ychar * 45 ;
  }

  placement.length = sizeof (WINDOWPLACEMENT) ;
  SetWindowPlacement (main_window, &placement) ;
  if (show_state != SW_SHOWMAXIMIZED)
    FitWindowInWindow (NULL, main_window) ;

  hDC = GetDC (toolbar_window) ;
  GetClientRect (toolbar_window, &rect) ;
  FillRect (hDC, &rect, GetStockObject (LTGRAY_BRUSH)) ;
  ReleaseDC (toolbar_window, hDC) ;
  GetClientRect (tabbed_window, &rect) ;
  TabCtrl_AdjustRect (tabbed_window, FALSE, &rect) ;
  rect.bottom = rect.top + 2 ;
  rect.top = 0 ;
  hDC = GetDC (tabbed_window) ;
  FillRect (hDC, &rect, GetStockObject (LTGRAY_BRUSH)) ;
  ReleaseDC (tabbed_window, hDC) ;

  if (!use_editors)
    TabCtrl_DeleteItem (tabbed_window, 1) ;

  if (run_count > 1 || !demo_mode)
  {
    n = GetPrivateProfileInt ("General", "ItsAboutTime", 0, EngineIniFileName) ;
    if (time (NULL) > n || newVersion)
    {
      DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_ABOUT), main_window, (DLGPROC) PovAboutDialogProc, (LPARAM) main_window) ;
      PutPrivateProfileInt ("General", "ItsAboutTime", n ? time (NULL) + 14L * 86400L : time (NULL) + 86400L, EngineIniFileName) ;
    }
  }

  if (ExtensionsEnabled)
    LoadGUIExtensions () ;

  buffer_message (None, "Persistence of Vision Ray Tracer(tm) for Windows.\n") ;
  buffer_message (None, "POV-Ray for Windows is part of the POV-Ray(tm) suite of programs.\n") ;
  buffer_message (None, "  This is version " POV_RAY_VERSION COMPILER_VER "." PVENGINE_VER " [" OPTIMISATION " optimised].\n") ;
  buffer_message (None, "The POV-Ray software is Copyright 1991-1997 POV-Team(tm).\n") ;
  buffer_message (None, "Base Windows version is Copyright 1996-1997 Christopher J. Cason.\n") ;
  buffer_message (None, "This is an UNSUPPORTED UNOFFICIAL COMPILE by " UNOFFICIALCOMPILE "\n") ;
  buffer_message (None, "  It is distributable only under the conditions in POVLEGAL.DOC.\n") ;
  buffer_message (None, "  Select Help|About (or press Alt+B) for more information and a copy of POVLEGAL.DOC.\n") ;
  buffer_message (None, "\n") ;
  buffer_message (None, "The terms POV-Ray, POV-Team, and Persistence of Vision Raytracer are trademarks of the\n") ;
  buffer_message (None, "  Persistence of Vision Development Team (POV-Team)\n") ;
  if (render_bitmap_depth != 24)
  {
    buffer_message (None, "\n") ;
    buffer_message (None, renderwin_8bits ? "Using 8-bit dithered internal bitmap (menu setting)\n" :
                                            "Using 8-bit dithered internal bitmap (4 or 8-bit video mode)\n") ;
  }
  pre_init_povray () ;
  Usage (-1, FALSE) ;
  if (IsWin32)
  {
    buffer_message (None, "\n") ;
    strcpy (tool_commands [0], "notepad.exe \"%ipovray.ini\"") ;
  }
  else
    strcpy (tool_commands [0], "notepad.exe %ipovray.ini") ;
  buffer_message (None, "Base Windows port by Christopher J. Cason.\n") ;
  buffer_message (None, "This is an UNSUPPORTED UNOFFICIAL COMPILE by " UNOFFICIALCOMPILE "\n") ;
  buffer_message (mDivider, "\n") ;
  load_tool_menu (ToolIniFileName) ;
  if (GetPrivateProfileInt ("FileQueue", "ReloadOnStartup", 0, EngineIniFileName))
  {
    queued_file_count = GetPrivateProfileInt ("FileQueue", "QueueCount", 0, EngineIniFileName) ;
    if (queued_file_count > MAX_QUEUE)
      queued_file_count = MAX_QUEUE ;
    for (i = 0 ; i < queued_file_count ; i++)
    {
      sprintf (str, "QueuedFile%d", i) ;
      GetPrivateProfileString ("FileQueue", str, "", queued_files [i], sizeof (queued_files [0]), EngineIniFileName) ;
    }
    if (queued_file_count != 0)
      message_printf ("Loaded %d entr%s into file queue\n", queued_file_count, queued_file_count == 1 ? "y" : "ies") ;
    update_queue_status (FALSE) ;
  }
  buffer_message (mDivider, "\n") ;

  /*
  ** FUNKY CODE ALERT  FUNKY CODE ALERT  FUNKY CODE ALERT  FUNKY CODE ALERT
  **
  ** The following code _used_ to be executed before the main window was
  ** created. With Borland it was fine. With Watcom, however, unless I was
  ** running a specific version of Win32s (the one on the 10.5 CDROM), the
  ** call to the stat function would fail with a processor fault.
  **
  ** I found during debugging that displaying a window would make the problem
  ** go away (it's always the way, isn't it ? the debugging code fixes the
  ** problem making it impossible to debug :) but iterations homed me in on
  ** this call. So I moved it to after the main window was created and all is
  ** hunky-dory (or so it seems). as to why, I don't know.
  */

  if (GetPrivateProfileInt ("General", "CheckColorsInc", 1, EngineIniFileName) == 1)
  {
    sprintf (str, "%sinclude\\colors.inc", HomePath) ;
    if (stat (str, &statbuf) != 0)
    {
      if (MessageBox (NULL,
                      "WARNING : Cannot find COLORS.INC in expected location.\n\n"
                      "This file is important for the normal operation of POV-Ray. It is included "
                      "with the POV-Ray for Windows distribution. If you did not install using the "
                      "correct installation procedure please attend to this before running POV-Ray "
                      "for Windows.\n\nIf, however, you have chosen to change the location of this file "
                      "or do not need it, you may ignore this warning as long as you have updated "
                      "POVRAY.INI to the new path, or do not use any standard scenes that require it.\n\n"
                      "Do you want to see this warning again ?",
                      "Warning - COLORS.INC is missing",
                      MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
                        PutPrivateProfileInt ("General", "CheckColorsInc", 0, EngineIniFileName) ;
    }
  }

  n = GetPrivateProfileInt ("MainWindow", "CurrentTab", 0, EngineIniFileName) ;
  if (!use_editors && n > 0)
    n = 0 ;
  if (n >= window_count)
    n = window_count - 1 ;
  TabCtrl_SetCurSel (tabbed_window, n) ;
  UpdateTabbedWindow (n, TRUE) ;
  CalculateClientWindows (TRUE) ;

  // this works around what appears to be a bug in the tabbed control's initialisation
  // [it wouldn't draw part of its surrounds]
  SetWindowLong (tabbed_window, GWL_STYLE, GetWindowLong (tabbed_window, GWL_STYLE) | WS_CLIPCHILDREN) ;

  if (demo_mode)
  {
    message_printf ("Running demonstration\n") ;
    argc = 0 ;
    handle_main_command (CM_DEMO, 0) ;
    PovMessageBox ("Demonstration completed. POV-Ray will now exit.", "Finished test run") ;
    WinHelp (main_window, "pvengine.hlp", HELP_QUIT, NULL) ;
    WinHelp (main_window, "povray30.hlp", HELP_QUIT, NULL) ;
    DestroyWindow (main_window) ;
    cleanup_all () ;
    return (0) ;
  }

  // automatically call the rendering engine if there were any parameters on the command line
  if (argc > 1 || render_requested)
  {
    if (render_requested)
    {
      message_printf ("Requested render file is '%s'\n", requested_render_file) ;
      strcpy (source_file_name, requested_render_file) ;
    }
    if (argc > 1)
      message_printf ("Calling rendering engine with parameters '%s'\n", command_line) ;
    start_rendering (TRUE, !render_requested) ;
    if (exit_after_render)
    {
      WinHelp (main_window, "pvengine.hlp", HELP_QUIT, NULL) ;
      WinHelp (main_window, "povray30.hlp", HELP_QUIT, NULL) ;
      DestroyWindow (main_window) ;
      cleanup_all () ;
      return (0) ;
    }
  }

  GetPrivateProfileString ("General", "CommandLine", "", command_line, sizeof (command_line), EngineIniFileName) ;
  restore_command_line = FALSE ;

  if (tips_enabled && argc <= 1)
  {
    n = GetPrivateProfileInt ("TipOfTheDay", "LastTipTime", 0, EngineIniFileName) ;
    if (time (NULL) >= n + 86400L)
    {
      PutPrivateProfileInt ("TipOfTheDay", "LastTipTime", time (NULL), EngineIniFileName) ;
      DialogBoxParam (hInstance, MAKEINTRESOURCE (IDD_TIP), main_window, (DLGPROC) PovTipDialogProc, (LPARAM) main_window) ;
    }
  }

  if (debugging)
  {
    message_printf ("My window handle is %08lx\n", main_window) ;
    if (HaveWin95 ())
      message_printf ("Win95 detected\n") ;
    if (HaveWinNT ())
      message_printf ("WinNT detected\n") ;
    if (HaveWin32s ())
      message_printf ("Win32s detected\n") ;
    if (IsW95UserInterface)
      message_printf ("Windows 95 user interface flag is set\n") ;
  }

  if (debugFile)
    fprintf (debugFile, "Entering GetMessage () loop\n") ;

  while (GetMessage (&msg, NULL, 0, 0) == TRUE)
  {
    if (!TranslateAccelerator (main_window, hAccelerators, &msg))
    {
      TranslateMessage (&msg) ;
      DispatchMessage (&msg) ;
    }
  }

#ifdef __USECTL3D__
  if (using_ctl3d)
  {
    if (hPrev == NULL)
      if (Ctl3dUnregister != NULL)
        Ctl3dUnregister (hInst) ;
  }
#endif
  cleanup_all () ;
  fcloseall () ;

  return (msg.wParam) ;
}

