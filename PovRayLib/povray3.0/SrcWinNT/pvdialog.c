/***************************************************************************
*                pvdialog.c
*
*  This module implements dialog-box routines for the Windows build of POV.
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

#include "pvengine.h"
#include "pvengine.rh"
#include "pvdialog.h"

extern char                                 ourPath [_MAX_PATH] ;
extern char                                 home [_MAX_PATH] ;
extern char                                 helpPath [_MAX_PATH] ;
extern char                                 home [_MAX_PATH] ;
extern char                                 rerun_ini_path [_MAX_PATH] ;
extern char                                 EngineIniFileName [_MAX_PATH] ;
extern char                                 *tips [] ;
extern unsigned                             screen_width ;
extern unsigned                             screen_height ;
extern unsigned                             renderwin_8bits ;
extern unsigned                             niceness ;
extern unsigned                             auto_render ;
extern unsigned                             queued_file_count ;
extern BOOL                                 IsW95UserInterface ;
extern BOOL                                 tips_enabled ;
extern HWND                                 render_window ;
extern HMENU                                hMainMenu ;
extern HINSTANCE                            hInstance ;

BOOL FAR PASCAL PovTipDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  char                  **s ;
  HDC                   hdc ;
  HWND                  hItem ;
  RECT                  rect ;
  HPEN                  hpen ;
  HBRUSH                hbrush ;
  PAINTSTRUCT           ps ;
  WINDOWPLACEMENT       p ;
  static int            tipCount = 0 ;
  static int            tipID ;

  switch (message)
  {
    case WM_INITDIALOG :
         CenterWindowRelative ((HWND) lParam, hDlg, TRUE) ;
         FitWindowInWindow (NULL, hDlg) ;
         tipID = GetPrivateProfileInt ("TipOfTheDay", "NextTip", 0, EngineIniFileName) ;
         if (tipCount == 0)
           for (s = tips ; *s ; s++)
             tipCount++ ;
         if (tipID >= tipCount)
           tipID = 0 ;
         PutPrivateProfileInt ("TipOfTheDay", "NextTip", tipID + 1, EngineIniFileName) ;
         CheckDlgButton (hDlg, IDC_SHOWTIPS, tips_enabled) ;
         return (TRUE) ;

    case WM_PAINT :
         hdc = BeginPaint (hDlg, &ps) ;
         hpen = GetStockObject (BLACK_PEN) ;
//       hbrush = GetStockObject (WHITE_BRUSH) ;
         hbrush = CreateSolidBrush (RGB (255, 255, 128)) ;
         hpen = SelectObject (hdc, hpen) ;
         hbrush = SelectObject (hdc, hbrush) ;
         hItem = GetDlgItem (hDlg, IDC_TIPFRAME) ;
         p.length = sizeof (WINDOWPLACEMENT) ;
         GetWindowPlacement (hItem, &p) ;
         rect = p.rcNormalPosition ;
         InflateRect (&rect, -5, -5) ;
         Rectangle (hdc, rect.left, rect.top, rect.right, rect.bottom) ;
         InflateRect (&rect, -5, -5) ;
         tip_of_the_day (hdc, &rect, tips [tipID]) ;
         SelectObject (hdc, hpen) ;
         SelectObject (hdc, hbrush) ;
         DeleteObject (hbrush) ;
         EndPaint (hDlg, &ps) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
           case IDCANCEL :
                EndDialog (hDlg, TRUE) ;
                return (TRUE) ;

           case IDC_SHOWTIPS :
                tips_enabled = IsDlgButtonChecked (hDlg, IDC_SHOWTIPS) ;
                PVCheckMenuItem (CM_TIPOFTHEDAY, tips_enabled ? MF_CHECKED : MF_UNCHECKED) ;
                return (TRUE) ;

           case IDC_NEXTTIP :
                if (++tipID >= tipCount)
                  tipID = 0 ;
                PutPrivateProfileInt ("TipOfTheDay", "NextTip", tipID + 1, EngineIniFileName) ;
                PovInvalidateRect (hDlg, NULL, FALSE) ;
                return (TRUE) ;

           case IDC_PREVIOUSTIP :
                if (tipID-- == 0)
                  tipID = tipCount - 1 ;
                PovInvalidateRect (hDlg, NULL, FALSE) ;
                return (TRUE) ;

           default :
                return (TRUE) ;
         }
  }
  return (FALSE) ;
}

