/***************************************************************************
*                pvmenu.c
*
*  This module implements menu-related routines for the Windows build of POV.
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

#include <commctrl.h>
#include "pvengine.h"
#include "pvengine.rh"

unsigned                expert_menu_ids [] = {
                                              CM_SOURCEFILE,
                                              CM_AUTORENDER,
                                              CM_FORCE8BITS,
                                              CM_RENDERACTIVE,
                                              CM_RENDERHIDE,
                                              CM_COMPLETION_HAL,
                                              CM_USECTL3D,
                                              CM_NONCLIENT,
                                              CM_TINYCAPTION,
                                              CM_USETEMPLATE,
                                              CM_LOCKMAXIMAIN,
                                              CM_SINGLEINSTANCE,
                                              CM_SAVE_SETTINGS,
                                              CM_PRESERVEMESSAGES,
                                              CM_USEEDITOR,
                                              0
                                             } ;

BOOL                    MenuBarDraw = FALSE ;
BOOL                    expert_menus = FALSE ;
HMENU                   hMainMenu ;
HMENU                   hExpertMenu ;
HMENU                   hNonExpertMenu ;
HMENU                   hPopupMenus ;
HMENU                   hFileMenu ;
HMENU                   hEditMenu ;
HMENU                   hRenderMenu ;
HMENU                   hRerunMenu ;
HMENU                   hAppearanceMenu ;
HMENU                   hMiscMenu ;
HMENU                   hToolsMenu ;
HMENU                   hPluginsMenu ;
HMENU                   hHelpMenu ;
HMENU                   hPopupMenusNE ;
HMENU                   hFileMenuNE ;
HMENU                   hEditMenuNE ;
HMENU                   hRenderMenuNE ;
HMENU                   hRerunMenuNE ;
HMENU                   hAppearanceMenuNE ;
HMENU                   hMiscMenuNE ;
HMENU                   hToolsMenuNE ;
HMENU                   hPluginsMenuNE ;
HMENU                   hHelpMenuNE ;

extern HWND             main_window ;
extern HWND             toolbar_window ;
extern BOOL             IsWin32 ;
extern HINSTANCE        hInstance ;
extern HandleStruct     editor_handles ;

BOOL PVEnableMenuItem (UINT idItem, UINT state)
{
  EnableMenuItem (hPopupMenus, idItem, state) ;
  EnableMenuItem (hExpertMenu, idItem, state) ;
  EnableMenuItem (hNonExpertMenu, idItem, state) ;
  SendMessage (toolbar_window, TB_ENABLEBUTTON, idItem, MAKELONG (state == MF_ENABLED, 0)) ;
  return (TRUE) ;
}

BOOL PVCheckMenuItem (UINT idItem, UINT state)
{
  CheckMenuItem (hPopupMenus, idItem, state) ;
  CheckMenuItem (hExpertMenu, idItem, state) ;
  CheckMenuItem (hNonExpertMenu, idItem, state) ;
  return (TRUE) ;
}

BOOL PVModifyMenu (UINT idItem, UINT flags, UINT idNewItem, LPCSTR lpNewItem)
{
  ModifyMenu (hPopupMenus, idItem, flags, idNewItem, lpNewItem) ;
  ModifyMenu (hExpertMenu, idItem, flags, idNewItem, lpNewItem) ;
  ModifyMenu (hNonExpertMenu, idItem, flags, idNewItem, lpNewItem) ;
  MenuBarDraw++ ;
  return (TRUE) ;
}

BOOL PVDeleteMenuItem (UINT idItem)
{
  DeleteMenu (hPopupMenus, idItem, MF_BYCOMMAND) ;
  DeleteMenu (hExpertMenu, idItem, MF_BYCOMMAND) ;
  DeleteMenu (hNonExpertMenu, idItem, MF_BYCOMMAND) ;
  return (TRUE) ;
}

void init_menus (void)
{
  int         i = 0 ;

  hExpertMenu = LoadMenu (hInstance, MAKEINTRESOURCE (IsWin32 ? PVENGINE_MENU32 : PVENGINE_MENU)) ;
  hFileMenu = GetSubMenu (hExpertMenu, 0) ;
  hEditMenu = GetSubMenu (hExpertMenu, 1) ;
  hRenderMenu = GetSubMenu (hExpertMenu, 2) ;
  hRerunMenu = GetSubMenu (hExpertMenu, 3) ;
  hAppearanceMenu = GetSubMenu (hExpertMenu, 4) ;
  hMiscMenu = GetSubMenu (hExpertMenu, 5) ;
  hToolsMenu = GetSubMenu (hExpertMenu, 6) ;
  hPluginsMenu = GetSubMenu (hExpertMenu, 7) ;
  hHelpMenu = GetSubMenu (hExpertMenu, 8) ;
  AppendMenu (hRenderMenu, MF_POPUP, (UINT) hRerunMenu, "&Rerun") ;

  hNonExpertMenu = LoadMenu (hInstance, MAKEINTRESOURCE (IsWin32 ? PVENGINE_MENU32 : PVENGINE_MENU)) ;
  while (expert_menu_ids [i] != 0)
    RemoveMenu (hNonExpertMenu, expert_menu_ids [i++], MF_BYCOMMAND) ;
  hFileMenuNE = GetSubMenu (hNonExpertMenu, 0) ;
  hEditMenuNE = GetSubMenu (hNonExpertMenu, 1) ;
  hRenderMenuNE = GetSubMenu (hNonExpertMenu, 2) ;
  hRerunMenuNE = GetSubMenu (hNonExpertMenu, 3) ;
  hAppearanceMenuNE = GetSubMenu (hNonExpertMenu, 4) ;
  hMiscMenuNE = GetSubMenu (hNonExpertMenu, 5) ;
  hToolsMenuNE = GetSubMenu (hNonExpertMenu, 6) ;
  hPluginsMenuNE = GetSubMenu (hNonExpertMenu, 7) ;
  hHelpMenuNE = GetSubMenu (hNonExpertMenu, 8) ;
  AppendMenu (hRenderMenuNE, MF_POPUP, (UINT) hRerunMenuNE, "&Rerun") ;
}

void clear_menu (HMENU hMenu)
{
  while (RemoveMenu (hMenu, 0, MF_BYPOSITION)) ;
}

void setup_menus (BOOL have_editor)
{
  if (have_editor)
  {
    AppendMenu (hAppearanceMenu, MF_POPUP, (UINT) editor_handles.AppearanceMenu, "&Editor Window") ;
    AppendMenu (hAppearanceMenuNE, MF_POPUP, (UINT) editor_handles.AppearanceMenu, "&Editor Window") ;
  }
  DrawMenuBar (main_window) ;
}

void build_main_menu (HMENU hMenu, BOOL have_editor)
{
  clear_menu (hMenu) ;
  if (have_editor)
  {
    AppendMenu (hMenu, MF_POPUP, (UINT) editor_handles.FileMenu, "&File") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) editor_handles.EditMenu, "&Edit") ;
  }
  else
  {
    if (expert_menus)
    {
      AppendMenu (hMenu, MF_POPUP, (UINT) hFileMenu, "&File") ;
      AppendMenu (hMenu, MF_POPUP, (UINT) hEditMenu, "&Edit") ;
    }
    else
    {
      AppendMenu (hMenu, MF_POPUP, (UINT) hFileMenuNE, "&File") ;
      AppendMenu (hMenu, MF_POPUP, (UINT) hEditMenuNE, "&Edit") ;
    }
  }
  if (expert_menus)
  {
    AppendMenu (hMenu, MF_POPUP, (UINT) hRenderMenu, "&Render") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hAppearanceMenu, "A&ppearance") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hMiscMenu, "&Misc") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hToolsMenu, "&Tools") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hPluginsMenu, "G&UI-Extensions") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hHelpMenu, "&Help") ;
  }
  else
  {
    AppendMenu (hMenu, MF_POPUP, (UINT) hRenderMenuNE, "&Render") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hAppearanceMenuNE, "A&ppearance") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hMiscMenuNE, "&Misc") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hToolsMenuNE, "&Tools") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hPluginsMenuNE, "G&UI-Extensions") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hHelpMenuNE, "&Help") ;
  }
  DrawMenuBar (main_window) ;
}

void build_editor_menu (HMENU hMenu)
{
  clear_menu (hMenu) ;
  AppendMenu (hMenu, MF_POPUP, (UINT) editor_handles.FileMenu, "&File") ;
  AppendMenu (hMenu, MF_POPUP, (UINT) editor_handles.EditMenu, "&Edit") ;
  AppendMenu (hMenu, MF_POPUP, (UINT) editor_handles.SearchMenu, "Se&arch") ;
  AppendMenu (hMenu, MF_POPUP, (UINT) editor_handles.InsertMenu, "&Insert") ;
  AppendMenu (hMenu, MF_POPUP, (UINT) editor_handles.OptionsMenu, "E&ditor") ;
  if (expert_menus)
  {
    AppendMenu (hMenu, MF_POPUP, (UINT) hRenderMenu, "&Render") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hAppearanceMenu, "A&ppearance") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hMiscMenu, "&Misc") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hToolsMenu, "&Tools") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hPluginsMenu, "G&UI-Extensions") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hHelpMenu, "&Help") ;
  }
  else
  {
    AppendMenu (hMenu, MF_POPUP, (UINT) hRenderMenuNE, "&Render") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hAppearanceMenuNE, "A&ppearance") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hMiscMenuNE, "&Misc") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hToolsMenuNE, "&Tools") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hPluginsMenuNE, "G&UI-Extensions") ;
    AppendMenu (hMenu, MF_POPUP, (UINT) hHelpMenuNE, "&Help") ;
  }
  DrawMenuBar (main_window) ;
}


