/***************************************************************************
*                pvmisc.c
*
*  This module implements miscellaneous routines for the Windows build of POV.
*
*  Copyright © POV-Team 1996-1997. All Rights Reserved.
*  This windows version of POV-Ray is Copyright 1996-1997 Christopher J. Cason.
*  Author : Christopher J. Cason.
*
*  NOTE : As this is Windows code, it was edited using a wide Windows-hosted
*         editor. Accordingly, expect text to exceed 80 columns regularly.
*
*  from Persistence of Vision Raytracer
*  Copyright 1996-1997 Persistence of Vision Team
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
*
*****************************************************************************/

// This code is not commented enough or as much as I'd like. It also needs
// a major cleanup. However, after all, this is volunteer work and I've
// not got the time to do so now. Rather than further delay the release,
// here it is.

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
#pragma hdrstop

#include <direct.h>
#include <io.h>
#include <commctrl.h>
#include "optin.h"
#include "pvengine.h"
#include "pvengine.rh"
#include "pvclient.h"
#include "pvguiext.h"

extern int                                  alert_sound ;
extern int                                  message_xchar ;
extern int                                  message_ychar ;
extern int                                  renderwin_left ;
extern int                                  renderwin_top ;
extern char                                 message_font_name [128] ;
extern char                                 ourPath [_MAX_PATH] ;
extern char                                 HomePath [_MAX_PATH] ;
extern char                                 ini_path [_MAX_PATH] ;
extern char                                 EngineIniFileName [_MAX_PATH] ;
extern char                                 RerunIniPath [_MAX_PATH] ;
extern char                                 CurrentRerunFileName [_MAX_PATH] ;
extern char                                 helpPath [_MAX_PATH] ;
extern char                                 lastRenderName [_MAX_PATH] ;
extern char                                 lastBitmapName [_MAX_PATH] ;
extern char                                 lastRenderPath [_MAX_PATH] ;
extern char                                 lastBitmapPath [_MAX_PATH] ;
extern char                                 lastQueuePath [_MAX_PATH] ;
extern char                                 lastSecondaryIniFilePath [_MAX_PATH] ;
extern char                                 SecondaryRenderIniFileName [_MAX_PATH] ;
extern char                                 SecondaryRenderIniFileSection [64] ;
extern char                                 background_file [_MAX_PATH ] ;
extern char                                 tool_commands [MAX_TOOLCMD] [MAX_TOOLCMDTEXT] ;
extern char                                 tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
extern char                                 source_file_name [_MAX_PATH] ;
extern char                                 ToolIniFileName [_MAX_PATH] ;
extern char                                 command_line [256] ;
extern char                                 queued_files [MAX_QUEUE] [_MAX_PATH] ;
extern void                                 *CurrentEditor ;
extern unsigned                             message_font_size ;
extern unsigned                             message_font_weight ;
extern unsigned                             screen_width ;
extern unsigned                             screen_height ;
extern unsigned                             renderwin_8bits ;
extern unsigned                             niceness ;
extern unsigned                             auto_render ;
extern unsigned                             queued_file_count ;
extern unsigned                             renderwin_flags ;
extern unsigned                             GUI_priority ;
extern unsigned                             render_priority ;
extern unsigned                             on_completion ;
extern unsigned                             window_count ;
extern BOOL                                 keep_messages ;
extern BOOL                                 alert_on_completion ;
extern BOOL                                 using_ctl3d ;
extern BOOL                                 save_settings ;
extern BOOL                                 running_demo ;
extern BOOL                                 fast_scroll ;
extern BOOL                                 renderwin_active ;
extern BOOL                                 renderwin_destroyed ;
extern BOOL                                 no_shellout_wait ;
extern BOOL                                 tile_background ;
extern BOOL                                 non_client ;
extern BOOL                                 debugging ;
extern BOOL                                 no_palette_warn ;
extern BOOL                                 hide_render_window ;
extern BOOL                                 render_above_main ;
extern BOOL                                 IsW95UserInterface ;
extern BOOL                                 tips_enabled ;
extern BOOL                                 renderwin_effects ;
extern BOOL                                 usedMaxiMini ;
extern BOOL                                 use_template ;
extern BOOL                                 use_16bit_editor ;
extern BOOL                                 system_noactive ;
extern BOOL                                 need_output_filename ;
extern BOOL                                 IsWin32 ;
extern BOOL                                 lock_maximain ;
extern BOOL                                 one_instance ;
extern BOOL                                 use_toolbar ;
extern BOOL                                 use_tooltips ;
extern BOOL                                 use_threads ;
extern BOOL                                 use_editors ;
extern BOOL                                 expert_menus ;
extern BOOL                                 drop_to_editor ;
extern BOOL                                 render_auto_close ;
extern BOOL                                 ExtensionsEnabled ;
extern BOOL                                 use_taskbar ;
extern HWND                                 render_window ;
extern HWND                                 main_window ;
extern HWND                                 tabbed_window ;
extern HWND                                 editor_window ;
extern HWND                                 toolbar_window ;
extern HMENU                                hMainMenu ;
extern HMENU                                hRerunMenu ;
extern HMENU                                hRerunMenuNE ;
extern HMENU                                hToolsMenu ;
extern HMENU                                hToolsMenuNE ;
extern COLORREF                             background_colour ;
extern COLORREF                             text_colour ;
extern HINSTANCE                            hInstance ;
extern EditStruct                           Edit ;
extern WINDOWPLACEMENT                      mainwin_placement ;
extern CRITICAL_SECTION                     critical_section ;
extern struct Reserved_Word_Struct          Option_Variable [] ;

typedef struct
{
  unsigned    id ;
  BOOL        *varptr ;
  BOOL        rval ;
  BOOL        autowrite ;
  char        *section ;
  char        *entry ;
  BOOL        defval ;
} toggle_struct ;

toggle_struct toggles [] =
{
//  ID                       VarPtr                         RVal AutoWrite  Section        Name                     DefVal
  { CM_SAVE_SETTINGS,       (BOOL *) &save_settings,        FALSE, TRUE,  "General",       "SaveSettingsOnExit"   , TRUE  },
  { CM_PRESERVEMESSAGES,    (BOOL *) &keep_messages,        TRUE , TRUE,  "Messages",      "KeepMessages"         , TRUE  },
  { CM_FORCE8BITS,          (BOOL *) &renderwin_8bits,      FALSE, TRUE,  "RenderWindow",  "Use8BitMode"          , FALSE },
  { CM_RENDERACTIVE,        (BOOL *) &renderwin_active,     TRUE , TRUE,  "RenderWindow",  "MakeActive"           , TRUE  },
  { CM_RENDERABOVEMAIN,     (BOOL *) &render_above_main,    FALSE, TRUE,  "RenderWindow",  "KeepAboveMain"        , TRUE  },
  { CM_RENDERHIDE,          (BOOL *) &hide_render_window,   TRUE , TRUE,  "RenderWindow",  "HideWhenMainMinimised", TRUE  },
  { CM_TINYCAPTION,         (BOOL *) &renderwin_effects,    FALSE, TRUE,  "RenderWindow",  "TinyCaption"          , FALSE },
  { CM_ALERT,               (BOOL *) &alert_on_completion,  TRUE , TRUE,  "Renderer",      "AlertOnCompletion"    , TRUE  },
  { CM_AUTORENDER,          (BOOL *) &auto_render,          TRUE , TRUE,  "Renderer",      "AutoRender"           , TRUE  },
  { CM_SHELLOUTWAIT,        (BOOL *) &no_shellout_wait,     TRUE , TRUE,  "Renderer",      "NoShelloutWait"       , FALSE },
  { CM_TIPOFTHEDAY,         (BOOL *) &tips_enabled,         TRUE,  TRUE,  "TipOfTheDay",   "Enabled"              , TRUE  },
  { CM_TILEDBACKGROUND,     (BOOL *) &tile_background,      FALSE, TRUE,  "General",       "TileBackground"       , TRUE  },
  { CM_USETEMPLATE,         (BOOL *) &use_template,         TRUE,  TRUE,  "General",       "UseFileTemplate"      , 2     },
  { CM_SYSTEMNOACTIVE,      (BOOL *) &system_noactive,      TRUE,  TRUE,  "Renderer",      "SystemNoActive"       , FALSE },
  { CM_LOCKMAXIMAIN,        (BOOL *) &lock_maximain,        TRUE,  TRUE,  "MainWindow",    "LockMaxiMain"         , TRUE  },
  { CM_SINGLEINSTANCE,      (BOOL *) &one_instance,         FALSE, FALSE, "General",       "OneInstance"          , FALSE },
  { CM_USETOOLBAR,          (BOOL *) &use_toolbar,          FALSE, TRUE,  "MainWindow",    "UseToolbar"           , TRUE  },
  { CM_USETOOLTIPS,         (BOOL *) &use_tooltips,         TRUE,  TRUE,  "MainWindow",    "UseTooltips"          , TRUE  },
  { CM_EXPERTMENUS,         (BOOL *) &expert_menus,         FALSE, TRUE,  "General",       "ExpertMenus"          , TRUE  },
  { CM_RENDERAUTOCLOSE,     (BOOL *) &render_auto_close,    TRUE,  TRUE,  "RenderWindow",  "AutoClose"            , FALSE },
  { CM_USEEXTENSIONS,       (BOOL *) &ExtensionsEnabled,    TRUE,  TRUE,  "GUIExtensions", "UseExtensions"        , TRUE  },
  { -1,                     (BOOL *) NULL,                  FALSE, TRUE,  "",              ""                     , FALSE }
} ;

BOOL PutPrivateProfileInt (LPCSTR lpszSection, LPCSTR lpszEntry, UINT uiValue, LPCSTR lpszFilename)
{
  char        str [16] ;

  sprintf (str, "%d", uiValue) ;
  return (WritePrivateProfileString (lpszSection, lpszEntry, str, lpszFilename)) ;
}

BOOL process_toggles (WPARAM wParam)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
  {
    if (t->id == LOWORD (wParam))
    {
      *t->varptr = !*t->varptr ;
      PVCheckMenuItem (t->id, *t->varptr ? MF_CHECKED : MF_UNCHECKED) ;
      return (t->rval) ;
    }
  }
  return (FALSE) ;
}

void set_toggles (void)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    PVCheckMenuItem (t->id, *t->varptr ? MF_CHECKED : MF_UNCHECKED) ;
}

void read_toggles (char *iniFilename)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    *t->varptr = GetPrivateProfileInt (t->section, t->entry, t->defval, iniFilename) ;
}

void write_toggles (char *iniFilename)
{
  toggle_struct         *t ;

  for (t = toggles ; (int) t->id != -1 ; t++)
    if (t->autowrite)
      PutPrivateProfileInt (t->section, t->entry, *t->varptr, iniFilename) ;
}

void read_INI_settings (char *iniFilename)
{
  char        str [_MAX_PATH] ;

  mainwin_placement.showCmd = GetPrivateProfileInt ("MainWindow", "ShowCmd", SW_SHOWNORMAL, iniFilename) ;
  mainwin_placement.ptMinPosition.x = GetPrivateProfileInt ("MainWindow", "MinPositionX", -1, iniFilename) ;
  mainwin_placement.ptMinPosition.y = GetPrivateProfileInt ("MainWindow", "MinPositionY", -1, iniFilename) ;
  mainwin_placement.ptMaxPosition.x = GetPrivateProfileInt ("MainWindow", "MaxPositionX", -1, iniFilename) ;
  mainwin_placement.ptMaxPosition.y = GetPrivateProfileInt ("MainWindow", "MaxPositionY", -1, iniFilename) ;
  mainwin_placement.rcNormalPosition.left = GetPrivateProfileInt ("MainWindow", "NormalPositionLeft", 128, iniFilename) ;
  mainwin_placement.rcNormalPosition.top = GetPrivateProfileInt ("MainWindow", "NormalPositionTop", 128, iniFilename) ;
  mainwin_placement.rcNormalPosition.right = GetPrivateProfileInt ("MainWindow", "NormalPositionRight", -1, iniFilename) ;
  mainwin_placement.rcNormalPosition.bottom = GetPrivateProfileInt ("MainWindow", "NormalPositionBottom", -1, iniFilename) ;
  usedMaxiMini = GetPrivateProfileInt ("MainWindow", "UsedMaxiMini", 0, iniFilename) ;
  GUI_priority = GetPrivateProfileInt ("MainWindow", "Priority", CM_GUIPRIORITY_NORMAL, iniFilename) ;
  renderwin_left = GetPrivateProfileInt ("RenderWindow", "NormalPositionX", 256, iniFilename) ;
  renderwin_top = GetPrivateProfileInt ("RenderWindow", "NormalPositionY", 256, iniFilename) ;
  renderwin_flags = GetPrivateProfileInt ("RenderWindow", "Flags", 0, iniFilename) ;
  GetPrivateProfileString ("Messages", "Font", "BorTE", message_font_name, sizeof (message_font_name), iniFilename) ;
  message_font_size = GetPrivateProfileInt ("Messages", "FontSize", 8, iniFilename) ;
  message_font_weight = GetPrivateProfileInt ("Messages", "FontWeight", 700, iniFilename) ;
  fast_scroll = GetPrivateProfileInt ("Messages", "FastScroll", FALSE, iniFilename) ;
  alert_sound = GetPrivateProfileInt ("Renderer", "AlertSound", MB_ICONASTERISK, iniFilename) ;
  niceness = GetPrivateProfileInt ("Renderer", "Niceness", 1, iniFilename) ;
  render_priority = GetPrivateProfileInt ("Renderer", "Priority", CM_RENDERPRIORITY_NORMAL, iniFilename) ;
  on_completion = GetPrivateProfileInt ("Renderer", "Completion", CM_COMPLETION_BEEP, iniFilename) ;
  no_palette_warn = GetPrivateProfileInt ("General", "NoPaletteWarn", FALSE, iniFilename) ;
  GetPrivateProfileString ("General", "LastRenderName", "", lastRenderName, sizeof (lastRenderName), iniFilename) ;
  GetPrivateProfileString ("General", "LastRenderPath", "", lastRenderPath, sizeof (lastRenderPath), iniFilename) ;
  GetPrivateProfileString ("General", "LastQueuePath", "", lastQueuePath, sizeof (lastQueuePath), iniFilename) ;
  GetPrivateProfileString ("General", "LastBitmapName", "*.bmp", lastBitmapName, sizeof (lastBitmapName), iniFilename) ;
  GetPrivateProfileString ("General", "LastBitmapPath", "", lastBitmapPath, sizeof (lastBitmapPath), iniFilename) ;
  sprintf (str, "%srenderer", HomePath) ;
  GetPrivateProfileString ("General", "LastINIPath", str, lastSecondaryIniFilePath, sizeof (lastSecondaryIniFilePath), iniFilename) ;
  sprintf (str, "%srenderer\\quickres.ini", HomePath) ;
  GetPrivateProfileString ("General", "SecondaryINIFile", str, SecondaryRenderIniFileName, sizeof (SecondaryRenderIniFileName), iniFilename) ;
  GetPrivateProfileString ("General", "SecondaryINISection", "[160x120, No AA]", SecondaryRenderIniFileSection, sizeof (SecondaryRenderIniFileSection), iniFilename) ;
  GetPrivateProfileString ("General", "BackgroundFile", "0", background_file, sizeof (background_file), iniFilename) ;
  text_colour = GetPrivateProfileInt ("General", "TextColour", RGB (255, 255, 255), iniFilename) ;
  background_colour = GetPrivateProfileInt ("General", "BackgroundColour", GetSysColor (COLOR_BTNFACE), iniFilename) ;
  debugging = GetPrivateProfileInt ("General", "Debug", FALSE, iniFilename) ;
  non_client = GetPrivateProfileInt ("General", "NonClientEffects", 1, iniFilename) ;
  using_ctl3d = GetPrivateProfileInt ("General", "UseCtl3d", 1, iniFilename) ;
  drop_to_editor = GetPrivateProfileInt ("General", "DropToEditor", 1, iniFilename) ;
  use_editors = GetPrivateProfileInt ("General", "UseEditors", 1, iniFilename) ;
  if (IsWin32)
    use_threads = GetPrivateProfileInt ("General", "UseThreads", TRUE, iniFilename) ;
  read_toggles (iniFilename) ;
}

void write_editor_settings (char *iniFilename)
{
  int         topline ;
  int         line ;
  int         col ;
  int         i ;
  int         count ;
  int         items ;
  int         tabno ;
  char        *title ;
  char        str [128] ;
  void        *editor ;
  TC_ITEM     tab ;

  for (i = 0 ; i < 16 ; i++)
  {
    sprintf (str, "Editor%d", i + 1) ;
    WritePrivateProfileString (str, NULL, NULL, iniFilename) ;
  }
  count = TabCtrl_GetItemCount (tabbed_window) ;
  for (items = i = 0 ; i < count ; i++)
  {
    tab.mask = TCIF_PARAM ;
    TabCtrl_GetItem (tabbed_window, i, &tab) ;
    if (tab.lParam != NULL)
    {
      editor = (void *) tab.lParam ;
      title = Edit.GetTitle (editor, &topline, &line, &col, NULL) ;
      if (strcmp (title, "Untitled"))
      {
        sprintf (str, "Editor%d", ++items) ;
        WritePrivateProfileString (str, "FileName", title, iniFilename) ;
        PutPrivateProfileInt (str, "TopLine", topline, iniFilename) ;
        PutPrivateProfileInt (str, "Line", line, iniFilename) ;
        PutPrivateProfileInt (str, "Column", col, iniFilename) ;
      }
    }
  }
  PutPrivateProfileInt ("Editor", "OpenCount", items, iniFilename) ;
  tabno = TabCtrl_GetCurSel (tabbed_window) ;
  if (tabno > items)
    tabno = items ;
  PutPrivateProfileInt ("MainWindow", "CurrentTab", tabno, iniFilename) ;
}

int load_editors (char *iniFilename)
{
  int         topline ;
  int         line ;
  int         col ;
  int         i ;
  int         count ;
  int         items ;
  char        title [_MAX_PATH] ;
  char        str [128] ;
  void        *editor ;

  count = GetPrivateProfileInt ("Editor", "OpenCount", 0, iniFilename) ;
  for (items = i = 0 ; i < count ; i++)
  {
    sprintf (str, "Editor%d", i + 1) ;
    GetPrivateProfileString (str, "FileName", "", title, sizeof (title), iniFilename) ;
    topline = GetPrivateProfileInt (str, "TopLine", 1, iniFilename) ;
    line = GetPrivateProfileInt (str, "Line", 1, iniFilename) ;
    col = GetPrivateProfileInt (str, "Column", 1, iniFilename) ;
    if ((editor = Edit.GetEditor ()) != NULL)
    {
      CurrentEditor = editor ;
      Edit.SelectEditor (editor) ;
      add_window_to_tab (editor_window, editor, "") ;
      if (!Edit.OpenFile (editor, title, topline, line, col, FALSE))
      {
        Edit.ReleaseEditor (editor) ;
        TabCtrl_DeleteItem (tabbed_window, --window_count) ;
      }
      else
        items++ ;
    }
  }
  return (items) ;
}

void write_INI_settings (char *iniFilename)
{
  if (use_editors)
    write_editor_settings (iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "ShowCmd", mainwin_placement.showCmd, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionLeft", mainwin_placement.rcNormalPosition.left, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionTop", mainwin_placement.rcNormalPosition.top, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionRight", mainwin_placement.rcNormalPosition.right, iniFilename) ;
  PutPrivateProfileInt ("MainWindow", "NormalPositionBottom", mainwin_placement.rcNormalPosition.bottom, iniFilename) ;
  if (IsWin32)
    PutPrivateProfileInt ("MainWindow", "Priority", GUI_priority, iniFilename) ;
  PutPrivateProfileInt ("RenderWindow", "NormalPositionX", renderwin_left, iniFilename) ;
  PutPrivateProfileInt ("RenderWindow", "NormalPositionY",  renderwin_top, iniFilename) ;
  PutPrivateProfileInt ("RenderWindow", "Flags",  renderwin_flags, iniFilename) ;
  PutPrivateProfileInt ("Messages", "FontSize", message_font_size, iniFilename) ;
  PutPrivateProfileInt ("Messages", "FontWeight", message_font_weight, iniFilename) ;
  PutPrivateProfileInt ("Messages", "FastScroll", fast_scroll, iniFilename) ;
  PutPrivateProfileInt ("Renderer", "AlertSound", alert_sound, iniFilename) ;
  if (!IsWin32)
    PutPrivateProfileInt ("Renderer", "Niceness", niceness, iniFilename) ;
  PutPrivateProfileInt ("Renderer", "Completion", on_completion, iniFilename) ;
  if (IsWin32)
    PutPrivateProfileInt ("Renderer", "Priority", render_priority, iniFilename) ;
  PutPrivateProfileInt ("General", "TextColour", text_colour, iniFilename) ;
  PutPrivateProfileInt ("General", "BackgroundColour", background_colour, iniFilename) ;
  PutPrivateProfileInt ("General", "DropToEditor", drop_to_editor, iniFilename) ;
  WritePrivateProfileString ("Messages", "Font", message_font_name, iniFilename) ;
  WritePrivateProfileString ("General", "LastRenderName",  lastRenderName, iniFilename) ;
  WritePrivateProfileString ("General", "LastRenderPath", lastRenderPath, iniFilename) ;
  WritePrivateProfileString ("General", "LastQueuePath", lastQueuePath, iniFilename) ;
  WritePrivateProfileString ("General", "LastBitmapName",  lastBitmapName, iniFilename) ;
  WritePrivateProfileString ("General", "LastBitmapPath", lastBitmapPath, iniFilename) ;
  WritePrivateProfileString ("General", "CommandLine", command_line, iniFilename) ;
  WritePrivateProfileString ("General", "LastINIPath", lastSecondaryIniFilePath, iniFilename) ;
  WritePrivateProfileString ("General", "SecondaryINIFile", SecondaryRenderIniFileName, iniFilename) ;
  WritePrivateProfileString ("General", "SecondaryINISection", SecondaryRenderIniFileSection, iniFilename) ;
  WritePrivateProfileString ("General", "BackgroundFile", background_file, iniFilename) ;
  write_toggles (iniFilename) ;
  WritePrivateProfileString ("General", "Version", PVENGINE_VER, iniFilename) ;
}

void GetRelativeClientRect (HWND hParent, HWND hChild, RECT *rect)
{
  POINT       *points = (POINT *) rect ;

  GetWindowRect (hChild, rect) ;
  ScreenToClient (hParent, points++) ;
  ScreenToClient (hParent, points) ;
}

void CenterWindowRelative (HWND hRelativeTo, HWND hTarget, BOOL bRepaint)
{
  int         difference ;
  int         width ;
  int         height ;
  int         x ;
  int         y ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  if (hRelativeTo == NULL)
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;
  GetWindowRect (hTarget, &targetRect) ;
  width = targetRect.right - targetRect.left ;
  height = targetRect.bottom - targetRect.top ;
  difference = relativeToRect.right - relativeToRect.left - width ;
  x = relativeToRect.left + difference / 2 ;
  difference = relativeToRect.bottom - relativeToRect.top - height ;
  y = relativeToRect.top + difference / 2 ;
  MoveWindow (hTarget, x, y, width, height, bRepaint) ;
}

void FitWindowInWindow (HWND hRelativeTo, HWND hTarget)
{
  int         rwidth ;
  int         rheight ;
  int         twidth ;
  int         theight ;
  int         x ;
  int         y ;
  RECT        relativeToRect ;
  RECT        targetRect ;

  if (hRelativeTo == NULL)
    hRelativeTo = GetDesktopWindow () ;
  GetWindowRect (hRelativeTo, &relativeToRect) ;
  GetWindowRect (hTarget, &targetRect) ;

  twidth = targetRect.right - targetRect.left ;
  theight = targetRect.bottom - targetRect.top ;
  rwidth = relativeToRect.right - relativeToRect.left ;
  rheight = relativeToRect.bottom - relativeToRect.top ;
  x = targetRect.left ;
  y = targetRect.top ;

  if (twidth > rwidth)
    twidth = rwidth ;
  if (theight > rheight)
    theight = rheight ;
  if (x < relativeToRect.left)
    x = relativeToRect.left ;
  if (y < relativeToRect.top)
    y = relativeToRect.top ;
  if (x + twidth > relativeToRect.right)
    x = relativeToRect.right - twidth ;
  if (y + theight > relativeToRect.bottom)
    y = relativeToRect.bottom - theight ;

  MoveWindow (hTarget, x, y, twidth, theight, TRUE) ;
}

void CenterOffset (HWND win, int id, int offx, int offy, int offw, int offh)
{
  int         x ;
  int         y ;
  int         w ;
  int         h ;
  RECT        R ;
  HWND        dlg = GetDlgItem (win, id) ;
  POINT       P ;

  if (dlg)
  {
    GetWindowRect (dlg, &R) ;

    P.x = R.left ;
    P.y = R.top ;

    ScreenToClient (win, &P) ;

    x = P.x ;
    y = P.y ;
    w = R.right - R.left ;
    h = R.bottom - R.top  ;

    SetWindowPos (dlg, NULL, x + offx, y + offy, w + offw, h + offh, SWP_NOZORDER) ;
  }
}

void SetupExplorerDialog (HWND win)
{
  int         dx ;
  int         dy ;
  RECT        winPos ;

  win = GetParent (win) ;
  GetWindowRect (win, &winPos) ;

  dx = screen_width * 2 / 3 - (winPos.right  - winPos.left) ;
  dy = screen_height * 2 / 3 - (winPos.bottom - winPos.top) ;

  SetWindowPos (win, NULL, 0, 0, screen_width * 2 / 3, screen_height * 2 / 3, SWP_NOZORDER | SWP_NOACTIVATE) ;
  CenterWindowRelative (main_window, win, TRUE) ;
  FitWindowInWindow (NULL, win) ;

  CenterOffset (win, 1091,    0,  0,   0,  0) ;
  CenterOffset (win, 1137,    0,  0,   0,  0) ;
  CenterOffset (win, 1088,    0,  0,   0,  0) ;
  CenterOffset (win, 1120,    0,  0,  dx, dy) ;
  CenterOffset (win, 1090,    0, dy,   0,  0) ;
  CenterOffset (win, 1152,    0, dy,  dx,  0) ;
  CenterOffset (win, 1089,    0, dy,   0,  0) ;
  CenterOffset (win, 1136,    0, dy,  dx,  0) ;
  CenterOffset (win, 1040,    0, dy,   0,  0) ;
  CenterOffset (win,    1, dx  , dy,   0,  0) ;
  CenterOffset (win,    2, dx  , dy,   0,  0) ;
  CenterOffset (win, 1038, dx  , dy,   0,  0) ;
}

int get_file_type (char *filename)
{
  char        ext [_MAX_EXT] ;

  splitfn (filename, NULL, NULL, ext) ;
  strupr (ext) ;

  if (strcmp (ext, ".POV") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".INC") == 0)
    return (filePOV) ;
  else if (strcmp (ext, ".INI") == 0)
    return (fileINI) ;
  else if (strcmp (ext, ".PPM") == 0)
    return (filePPM) ;
  else if (strcmp (ext, ".PGM") == 0)
    return (filePGM) ;
  else if (strcmp (ext, ".PBM") == 0)
    return (filePBM) ;
  else if (strcmp (ext, ".PNG") == 0)
    return (filePNG) ;
  else if (strcmp (ext, ".GIF") == 0)
    return (fileGIF) ;
  else if (strcmp (ext, ".BMP") == 0)
    return (fileBMP) ;
  else
    return (fileUnknown) ;
}

char *get_full_name (char *s)
{
  char                  dir [_MAX_PATH + 1] ;
  static char           str [_MAX_PATH] ;

  if (*s == 0)
    return (s) ;
  splitpath (s, str, NULL) ;
  if (str [0] == '\0')
  {
    // workaround for suspected Win32s bug
    SetCurrentDirectory (".") ;
    GetCurrentDirectory (sizeof (dir), dir) ;
    joinPath (str, dir, s) ;
    return (str) ;
  }
  return (s) ;
}

char *get_previous_rerun (int id, char *section, char *entry)
{
  char        filename [_MAX_PATH] ;
  static char text [_MAX_PATH] ;

  sprintf (filename, "%sRerun%02d.Ini", RerunIniPath, id) ;
  GetPrivateProfileString (section, entry, "(NULL)", text, sizeof (text), filename) ;
  return (strcmp (text, "(NULL)") == 0 ? NULL : text) ;
}

char *get_current_rerun (char *section, char *entry)
{
  static char text [_MAX_PATH] ;

  GetPrivateProfileString (section, entry, "(NULL)", text, sizeof (text), CurrentRerunFileName) ;
  return (strcmp (text, "(NULL)") == 0 ? NULL : text) ;
}

char *get_current_stats (char *entry)
{
  static char text [_MAX_PATH] ;

  GetPrivateProfileString ("Statistics", entry, "(NULL)", text, sizeof (text), CurrentRerunFileName) ;
  return (strcmp (text, "(NULL)") == 0 ? NULL : text) ;
}

// note !!! Windows NT's INI to Registry file mapping must never be enabled on these files !
// POV-Ray allows multiple entries with the same name in the same section. so we access them
// via file I/O rather than the INI file functions.
void write_rerun_information (void)
{
  int         op ;
  int         i ;
  char        dir [_MAX_PATH] ;
  FILE        *f ;
  time_t      tm ;

  if (running_demo)
    return ;

  // workaround for suspected Win32s bug
  SetCurrentDirectory (".") ;
  GetCurrentDirectory (sizeof (dir), dir) ;

  // flush the INI file cache
  WritePrivateProfileString (NULL, NULL, NULL, CurrentRerunFileName) ;

  // clobber the old file by creating a new one
  if ((f = fopen (CurrentRerunFileName, "wt")) != NULL)
  {
    fprintf (f, "[Renderer]\n") ;
    for (op = 0 ; op < MAX_OPTION ; op++)
    {
      switch (op)
      {
        case LIBRARY_PATH_OP :
        case BITS_PER_COLOUR_OP :
        case INCLUDE_INI_OP :
             break ;

        default :
             fprintf (f, "%s=%s\n", Option_Variable [op].Token_Name, get_ini_value (op, 0)) ;
             break ;
      }
    }
    for (i = 0 ; i < opts.Library_Path_Index ; i++)
      fprintf (f, "%s=%s\n", Option_Variable [LIBRARY_PATH_OP].Token_Name, get_ini_value (LIBRARY_PATH_OP, i)) ;

    fprintf (f, "\n[Environment]\n") ;
    tm = time (NULL) ;
    fprintf (f, "Date=%s", ctime (&tm)) ;
    fprintf (f, "CurrentDirectory=%s\n", dir) ;
    fclose (f) ;
  }
  else
    message_printf ("Cannot open file %s to write rerun information\r\n", CurrentRerunFileName) ;

  WritePrivateProfileString ("LastRender", "CurrentDirectory", dir, EngineIniFileName) ;
  WritePrivateProfileString ("LastRender", "SourceFile", get_full_name (opts.Input_File_Name), EngineIniFileName) ;
  WritePrivateProfileString ("LastRender", "SceneFile", opts.Scene_Name, EngineIniFileName) ;
  WritePrivateProfileString ("LastRender", "IniOutputFile", get_full_name (opts.Ini_Output_File_Name), EngineIniFileName) ;
  need_output_filename = TRUE ; // always on
}

void rotate_rerun_entries (void)
{
  int         i ;
  char        filename1 [_MAX_PATH] ;
  char        filename2 [_MAX_PATH] ;
  char        *s1 ;
  char        *s2 ;

  if (!running_demo)
  {
    s1 = get_current_rerun ("Renderer", "Input_File_Name") ;
    s2 = get_previous_rerun (0, "Renderer", "Input_File_Name") ;
    if (s1 == NULL || s2 == NULL || stricmp (s1, s2))
    {
      sprintf (filename1, "%sRerun%02d.Ini", RerunIniPath, MAX_RERUN - 1) ;
      WritePrivateProfileString (NULL, NULL, NULL, filename1) ; // flush the INI file cache
      unlink (filename1) ;
      for (i = MAX_RERUN - 1 ; i >= 0 ; i--)
      {
        sprintf (filename1, "%sRerun%02d.Ini", RerunIniPath, i) ;
        sprintf (filename2, "%sRerun%02d.Ini", RerunIniPath, i + 1) ;
        WritePrivateProfileString (NULL, NULL, NULL, filename1) ;
        rename (filename1, filename2) ;
      }
      sprintf (filename2, "%sRerun00.Ini", RerunIniPath) ;
      // flush the INI file cache
      WritePrivateProfileString (NULL, NULL, NULL, filename2) ;
      rename (CurrentRerunFileName, filename2) ;
    }
    else
    {
      sprintf (filename2, "%sRerun00.Ini", RerunIniPath) ;
      // flush the INI file cache
      WritePrivateProfileString (NULL, NULL, NULL, filename2) ;
      unlink (filename2) ;
      rename (CurrentRerunFileName, filename2) ;
    }
  }
}

void add_rerun_to_menu (void)
{
  int                   i ;
  int                   count ;
  char                  *s ;
  char                  str1 [_MAX_PATH + 5] ;
  char                  str2 [_MAX_PATH] ;
  char                  chars [] = "0123456789ABCDEF" ;

  clear_menu (hRerunMenu) ;
  clear_menu (hRerunMenuNE) ;
  AppendMenu (hRerunMenu, MF_STRING, CM_RERUNDIALOG, "Rerun &Dialog") ;
  AppendMenu (hRerunMenuNE, MF_STRING, CM_RERUNDIALOG, "Rerun &Dialog") ;
  if ((s = get_current_rerun ("Renderer", "Input_File_Name")) != NULL)
  {
    AppendMenu (hRerunMenu, MF_SEPARATOR, -1, "-") ;
    AppendMenu (hRerunMenuNE, MF_SEPARATOR, -1, "-") ;
    splitpath (s, NULL, str2) ;
    sprintf (str1, "Rer&un %s", str2) ;
    AppendMenu (hRerunMenu, MF_STRING, CM_RERUNCURRENT, str1) ;
    AppendMenu (hRerunMenuNE, MF_STRING, CM_RERUNCURRENT, str1) ;
    if ((s = get_current_rerun ("Renderer", "Output_To_File")) != NULL)
    {
      if (stricmp (s, "ON") == 0)
      {
        sprintf (str1, "&Continue %s", str2) ;
        AppendMenu (hRerunMenu, MF_STRING, CM_CONTINUECURRENT, str1) ;
        AppendMenu (hRerunMenuNE, MF_STRING, CM_CONTINUECURRENT, str1) ;
      }
      else
      {
        AppendMenu (hRerunMenu, MF_STRING | MF_GRAYED, CM_CONTINUECURRENT, "&Continue not available (no output file)") ;
        AppendMenu (hRerunMenuNE, MF_STRING | MF_GRAYED, CM_CONTINUECURRENT, "&Continue not available (no output file)") ;
      }
    }
  }

  for (i = count = 0 ; i < MAX_RERUN ; i++)
  {
    if ((s = get_previous_rerun (i, "Renderer", "Input_File_Name")) != NULL)
    {
      if (count == 0)
      {
        AppendMenu (hRerunMenu, MF_SEPARATOR, -1, "-") ;
        AppendMenu (hRerunMenuNE, MF_SEPARATOR, -1, "-") ;
      }
      splitpath (s, NULL, str2) ;
      sprintf (str1, "&%c - %s", chars [count++], str2) ;
      AppendMenu (hRerunMenu, MF_STRING, CM_RERUN + i, str1) ;
      AppendMenu (hRerunMenuNE, MF_STRING, CM_RERUN + i, str1) ;
    }
  }
  DrawMenuBar (main_window) ;
}

void update_menu_for_render (BOOL rendering)
{
  int         i ;
  char        *s ;

  if (rendering)
  {
    PVModifyMenu (CM_FILERENDER, MF_STRING, CM_FILERENDER, "&Stop Rendering\tAlt+G") ;
    PVEnableMenuItem (CM_COMMANDLINE, MF_GRAYED) ;
    PVEnableMenuItem (CM_SOURCEFILE, MF_GRAYED) ;
    for (i = 0 ; i < MAX_RERUN ; i++)
      PVEnableMenuItem (CM_RERUN + i, MF_GRAYED) ;
    PVEnableMenuItem (CM_RERUNCURRENT, MF_GRAYED) ;
    PVEnableMenuItem (CM_CONTINUECURRENT, MF_GRAYED) ;
    PVEnableMenuItem (CM_DEMO, MF_GRAYED) ;
    PVEnableMenuItem (CM_FORCE8BITS, MF_GRAYED) ;
    PVEnableMenuItem (CM_RENDERSLEEP, MF_ENABLED) ;
    SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0L) ;
  }
  else
  {
    PVEnableMenuItem (CM_SOURCEFILE, MF_ENABLED) ;
    PVEnableMenuItem (CM_COMMANDLINE, MF_ENABLED) ;
    PVEnableMenuItem (CM_DEMO, MF_ENABLED) ;
    PVModifyMenu (CM_FILERENDER, MF_STRING, CM_FILERENDER, "&Start Rendering\tAlt+G") ;
    for (i = 0 ; i < MAX_RERUN ; i++)
      PVEnableMenuItem (CM_RERUN + i, MF_ENABLED) ;
    PVEnableMenuItem (CM_RERUNCURRENT, MF_ENABLED) ;
    if ((s = get_current_rerun ("Renderer", "Output_To_File")) != NULL)
      if (stricmp (s, "ON") == 0)
        PVEnableMenuItem (CM_CONTINUECURRENT, MF_ENABLED) ;
    PVEnableMenuItem (CM_FORCE8BITS, MF_ENABLED) ;
    PVEnableMenuItem (CM_RENDERSLEEP, MF_GRAYED) ;
    SendMessage (toolbar_window, TB_CHECKBUTTON, (WPARAM) CM_RENDERSLEEP, 0L) ;
  }
  PVEnableMenuItem (CM_RENDERSHOW, renderwin_destroyed ? MF_ENABLED : MF_GRAYED) ;
  DrawMenuBar (hMainMenu) ;
}

void update_queue_status (BOOL write_files)
{
  int         i ;
  char        str [64] ;

  if (queued_file_count == 0)
  {
    PVModifyMenu (CM_CLEARQUEUE, MF_STRING, CM_CLEARQUEUE, "C&lear Queue (no entries)") ;
    PVEnableMenuItem (CM_CLEARQUEUE, MF_GRAYED) ;
  }
  else
  {
    sprintf (str, "C&lear Queue (%d %s)", queued_file_count, queued_file_count == 1 ? "entry" : "entries") ;
    PVModifyMenu (CM_CLEARQUEUE, MF_STRING, CM_CLEARQUEUE, str) ;
    PVEnableMenuItem (CM_CLEARQUEUE, queued_file_count ? MF_ENABLED : MF_GRAYED) ;
  }
  if (write_files)
  {
    PutPrivateProfileInt ("FileQueue", "QueueCount", queued_file_count, EngineIniFileName) ;
    for (i = 0 ; i < MAX_QUEUE ; i++)
    {
      sprintf (str, "QueuedFile%d", i) ;
      WritePrivateProfileString ("FileQueue", str, i < queued_file_count ? queued_files [i] : NULL, EngineIniFileName) ;
    }
  }
}

void fill_rerun_listbox (HWND hlb, char *idList)
{
  char                  str [256] ;
  char                  *s ;
  char                  dhms [32] = "No Time\t" ;
  unsigned              i ;
  unsigned long         seconds ;
  RECT                  rect ;

  GetClientRect (hlb, &rect) ;
  for (i = 0 ; i < MAX_RERUN ; i++)
  {
    if ((s = get_previous_rerun (i, "Renderer", "Input_File_Name")) == NULL)
      continue ;
    splitpath (s, NULL, str) ;
    strcat (str, "\t") ;
    if ((s = get_previous_rerun (i, "Statistics", "StartRender")) != NULL)
    {
      seconds = atol (s) ;
      if ((s = get_previous_rerun (i, "Statistics", "FinishRender")) != NULL)
      {
        seconds = atol (s) - seconds ;
        sprintf (dhms, "%ud %02uh %02um %02us\t", seconds / 86400, seconds % 86400 / 3600, seconds % 3600 / 60, seconds % 60) ;
      }
    }
    strcat (str, dhms) ;
    if ((s = get_previous_rerun (i, "Environment", "Date")) == NULL)
      s = "No Date" ;
    strcat (str, s) ;
    SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) str) ;
    *idList++ = (char) i ;
  }
}

void fill_statistics_listbox (HWND hlb, int id)
{
  char                  str [64] ;
  char                  *s ;
  unsigned              i ;

  for (i = 0 ; i < 256 ; i++)
  {
    sprintf (str, "StatLn%02d", i) ;
    if ((s = get_previous_rerun (id, "Statistics", str)) != NULL)
      SendMessage (hlb, LB_ADDSTRING, 0, (LPARAM) s) ;
    else
      break ;
  }
}

void resize_listbox_dialog (HWND hDlg, int idLb, int chars)
{
  int         difference ;
  HWND        hLb ;
  HWND        hBtn ;
  RECT        lbRect ;
  RECT        btnRect ;
  RECT        dlgRect ;

  hLb = GetDlgItem (hDlg, idLb) ;
  hBtn = GetDlgItem (hDlg, IDOK) ;
  GetRelativeClientRect (hDlg, hLb, &lbRect) ;
  GetRelativeClientRect (hDlg, hBtn, &btnRect) ;
  GetWindowRect (hDlg, &dlgRect) ;
  difference = message_xchar * (chars + 2) - (lbRect.right - lbRect.left) ;
  lbRect.right += difference ;
  MoveWindow (hLb, lbRect.left, lbRect.top, lbRect.right - lbRect.left, lbRect.bottom - lbRect.top, TRUE) ;
  btnRect.left += difference / 2 ;
  btnRect.right += difference / 2 ;
  MoveWindow (hBtn, btnRect.left, btnRect.top, btnRect.right - btnRect.left, btnRect.bottom - btnRect.top, TRUE) ;
  dlgRect.right += difference ;
  MoveWindow (hDlg, dlgRect.left, dlgRect.top, dlgRect.right - dlgRect.left, dlgRect.bottom - dlgRect.top, TRUE) ;
}

#if 0
char *save_demo_file (void)
{
  GetTempPath (sizeof (filename), filename) ;
  strcat (filename, "POVDEMO.$$$") ;
  if ((hrsc = FindResource (hInst, MAKEINTRESOURCE (ID_DEMOFILE), RT_RCDATA)) == NULL)
  {
    PovMessageBox ("Cannot locate file resource\r\n(internal error)", "Cannot run demo") ;
    return (NULL) ;
  }
  if ((hglobal = LoadResource (hInst, hrsc)) == NULL)
  {
    PovMessageBox ("Cannot load file resource", "Cannot run demo") ;
    return (NULL) ;
  }
  if ((s = LockResource (hglobal)) == NULL)
  {
    PovMessageBox ("Cannot lock file resource", "Cannot run demo") ;
    return (NULL) ;
  }
  size = SizeofResource (hInst, hrsc) ;
  if ((outH = _lcreat (filename, 0)) == HFILE_ERROR)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    return (NULL) ;
  }

  if (_lwrite (outH, s, size) != size)
  {
    PovMessageBox ("Cannot write temporary file", "Cannot run demo") ;
    return (NULL) ;
  }
  _lclose (outH) ;
}
#endif

int splitfn (char *filename, char *path, char *name, char *ext)
{
  char        *s ;
  char        str [_MAX_PATH] ;

  filename = strcpy (str, filename) ;

  if (path != NULL)
    *path = '\0' ;
  if (name != NULL)
    *name = '\0' ;
  if (ext != NULL)
    *ext = '\0' ;

  if ((s = strrchr (filename, '.')) != NULL)
  {
    if (ext)
      strcpy (ext, s) ;
    *s = '\0' ;
  }

  if ((s = strrchr (filename, SEPARATOR)) != NULL)
  {
    if (name)
      strcpy (name, s + 1) ;
    *++s = '\0' ;
    if (path)
      strcpy (path, filename) ;
  }
  else
    if (name)
      strcpy (name, filename) ;

  return (0) ;
}

void splitpath (char *filename, char *path, char *name)
{
  char        str [_MAX_PATH] ;

  splitfn (filename, path, name, str) ;
  if (name != NULL)
    strcat (name, str) ;
}

void load_tool_menu (char *iniFilename)
{
  int         i ;
  int         count ;
  char        str [32] ;
  char        entry [256] ;
  char        *s ;

  memset (tool_commands, 0, sizeof (tool_commands)) ;
  memset (tool_help, 0, sizeof (tool_help)) ;

  DeleteMenu (hToolsMenu, 1, MF_BYCOMMAND) ;
  DeleteMenu (hToolsMenuNE, 1, MF_BYCOMMAND) ;
  for (i = 0 ; i < MAX_TOOLCMD ; i++)
  {
    DeleteMenu (hToolsMenu, CM_FIRSTTOOL + i, MF_BYCOMMAND) ;
    DeleteMenu (hToolsMenuNE, CM_FIRSTTOOL + i, MF_BYCOMMAND) ;
  }

  for (i = count = 0 ; i < MAX_TOOLCMD ; i++)
  {
    sprintf (str, "Item%d", i) ;

    GetPrivateProfileString ("Command", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
      continue ;
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) >= MAX_TOOLCMDTEXT)
    {
      message_printf ("Tool command %s is too long\n", str) ;
      s [MAX_TOOLCMDTEXT - 1] = '\0' ;
    }
    strcpy (tool_commands [count], s) ;

    GetPrivateProfileString ("Menu", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
    {
      message_printf ("Tool menu entry %s is missing\n", str) ;
      continue ;
    }
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) > 31)
    {
      message_printf ("Tool menu entry %s is too long\n", str) ;
      continue ;
    }
    if (count == 0)
    {
      AppendMenu (hToolsMenu, MF_SEPARATOR, 1, "-") ;
      AppendMenu (hToolsMenuNE, MF_SEPARATOR, 1, "-") ;
    }
    AppendMenu (hToolsMenu, MF_STRING, CM_FIRSTTOOL + count, s) ;
    AppendMenu (hToolsMenuNE, MF_STRING, CM_FIRSTTOOL + count, s) ;

    GetPrivateProfileString ("Help", str, "", entry, sizeof (entry) - 1, iniFilename) ;
    if (strlen (entry) == 0)
    {
      count++ ;
      continue ;
    }
    s = entry ;
    while (*s == ' ')
      s++ ;
    if (strlen (s) >= MAX_TOOLHELPTEXT)
    {
      message_printf ("Tool help %s is too long\n", str) ;
      s [MAX_TOOLHELPTEXT - 1] = '\0' ;
    }
    strcpy (tool_help [count++], s) ;
  }
  message_printf ("Loaded %d %s from %s into Tool Menu.\n", count, (count != 1 ? "tools" : "tool"), iniFilename) ;
}

char *parse_tool_command (char *command)
{
  char                  *s ;
  char                  ExternalStr [512] ;
  static char           str [512] ;

  str [0] = '\0' ;
  while (*command == ' ')
    command++ ;
  strcpy (ExternalStr, command) ;
  ExternalParseToolCommand (ExternalStr) ;
  if (strlen (ExternalStr))
    command = ExternalStr ;
  for (s = str ; *command ; command++)
  {
    if (strlen (str) >= sizeof (str) - _MAX_PATH)
      break ;
    if (*command == '%')
    {
      if (*++command == '%')
      {
        *s++ = *command ;
        continue ;
      }
      switch (toupper (*command))
      {
        case 'I' :
             s += sprintf (s, "%sini\\", HomePath) ;
             break ;

        case 'T' :
             s += sprintf (s, "%s", ToolIniFileName) ;
             break ;

        case 'H' :
             s += sprintf (s, "%s", HomePath) ;
             break ;

        case 'R' :
             s += joinPath (s, lastRenderPath, lastRenderName) ;
             break ;

        case 'S' :
             s += sprintf (s, "%s", source_file_name) ;
             break ;

        case 'N' :
             s += sprintf (s, "%s", SecondaryRenderIniFileName) ;
             break ;

        case 'D' :
             s += GetPrivateProfileString ("LastRender", "CurrentDirectory", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '0' :
             s += GetPrivateProfileString ("LastRender", "SourceFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '1' :
             s += GetPrivateProfileString ("LastRender", "OutputFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '2' :
             s += GetPrivateProfileString ("LastRender", "SceneFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '3' :
             s += GetPrivateProfileString ("LastRender", "HistogramFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;

        case '4' :
             s += GetPrivateProfileString ("LastRender", "IniOutputFile", "", s, _MAX_PATH, EngineIniFileName) ;
             break ;
      }
      continue ;
    }
    *s++ = *command ;
  }
  *s = '\0' ;
  return (str) ;
}

char *get_elapsed_time (time_t start, time_t finish)
{
  static char str [19] ;

  str [0] = '\0' ;
  if (finish < start)
    return (str) ;
  finish -= start ;
  sprintf (str, "%ud %02uh %02um %02us",
           finish / 86400,
           finish % 86400 / 3600,
           finish % 3600 / 60,
           finish % 60) ;
  return (str) ;
}

char *clean (char *s)
{
  static char           str [256] ;

  while (*s == ' ' || *s == '\r' || *s == '\n' )
    s++ ;
  s = strcpy (str, s) ;
  if (*s == '\0')
    return (str) ;
  for (s += strlen (s) - 1 ; *s == ' ' || *s == '\r' || *s == '\n' ; s--)
    *s = '\0' ;
  return (str) ;
}

void extract_ini_sections (char *filename, HWND hwnd)
{
  char                  str [256] ;
  char                  *s1 ;
  char                  *s2 ;
  FILE                  *inF ;
  static char           inbuf [16384] ;

  /*
  ** flush the INI file cache
  */
  WritePrivateProfileString (NULL, NULL, NULL, filename) ;
  SendMessage (hwnd, CB_RESETCONTENT, 0, 0L) ;

  if ((inF = fopen (filename, "rt")) == NULL)
    return ;

  setvbuf (inF, inbuf, _IOFBF, sizeof (inbuf)) ;
  while (fgets (str, sizeof (str), inF) != NULL)
  {
    s1 = clean (str) ;
    if (*s1 == '[')
    {
      if ((s2 = strchr (s1, ']')) != NULL)
      {
        *++s2  = '\0' ;
        SendMessage (hwnd, CB_ADDSTRING, 0, (LPARAM) s1) ;
      }
    }
  }
  fclose (inF) ;
  SendMessage (hwnd, CB_SETCURSEL, 0, 0L) ;
}

BOOL PovInvalidateRect (HWND hWnd, CONST RECT *lpRect, BOOL bErase)
{
  if (hWnd != NULL)
    return (InvalidateRect (hWnd, lpRect, bErase)) ;
  return (0) ;
}

BOOL TaskBarAddIcon (HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
{
  NOTIFYICONDATA        tnid ;

  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;
  tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;
  tnid.uCallbackMessage = TASKBAR_NOTIFY_MESSAGE ;
  tnid.hIcon = hicon ;
  lstrcpyn (tnid.szTip, lpszTip, sizeof(tnid.szTip)) ;

  return (Shell_NotifyIcon (NIM_ADD, &tnid)) ;
}

BOOL TaskBarDeleteIcon (HWND hwnd, UINT uID)
{
  NOTIFYICONDATA        tnid ;

  tnid.cbSize = sizeof(NOTIFYICONDATA) ;
  tnid.hWnd = hwnd ;
  tnid.uID = uID ;

  return (Shell_NotifyIcon (NIM_DELETE, &tnid)) ;
}


