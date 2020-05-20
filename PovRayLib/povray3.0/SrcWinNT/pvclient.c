/****************************************************************************
*                pvclient.c
*
*  This module implements Windows non-client drawing routines.
*
*  Authors : See comment block below. Taken from Microsoft's ITSYBITSY demo.
*            Substantial modifications by Christopher J. Cason to add POV-Ray's
*            new window style. Add support for drawing borders and a third
*            'Minimise to 72-pixel icon and keep on top' button in the caption.
*
*  NOTE : As this is Windows code, it was edited using a wide Windows-hosted
*         editor. Accordingly, expect text to exceed 80 columns regularly.
*
*  from Persistence of Vision Raytracer
*  All modifications are Copyright 1996-1997 Persistence of Vision Team
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
*  DISCLAIMER : The above copyright and usage comments apply only to new
*               sections of code written for POV-Ray.
*
*  COMMENT : I was going to try to make the code in pvtiny.c multi-purpose,
*            since this code duplicates some of it. However, with the pvtiny.c
*            code only using 8k or so out of an 800k program, it didn't seem
*            worth the effort.
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

/////////////////////////////////////////////////////////////////////////
//
// Project:     ItsyBitsy window support module
// Module:      itsybits.c
//
//
//    ItsyBitsy is a support module that allows you to create windows
//    that look and act very much like a popup window with a system
//    menu and caption bar, except everything is scaled to about 2/3
//    scale.
//
//    For documentation on how to use ItsyBits, read the document
//    ITSYBITS.DOC.
//
//   Revisions:
//      9/27/91     Charlie Kindel (cek/ckindel)
//                          Wrote and documented it.
//
//      1/14/93     cek
//      2/23/93     cek     Added minimize/maximize buttons.
//      3/18/93     cek     Fixed system menu bug where system menu
//                          popped back up if you clicked on the
//                          icon again while it was up.
//      3/24/93     cek     More comments.  Fixed DS_MODALDIALOG style
//                          problem.  Use auto precompiled headers
//                          in MSVC.
//      1/02/96     cjc     revised to suit POV-Ray for Windows.
//      1/03/96     cjc     added border and 3D title-bar support.
//
//////////////////////////////////////////////////////////////////////////

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

#include <windowsx.h>
#include "pvengine.h"
#include "pvclient.h"

#define TestWinStyle(hWnd, dwStyleBit) \
          (((DWORD) GetWindowLong (hWnd, GWL_STYLE) & (DWORD) dwStyleBit) ? TRUE : FALSE)

#define CAPTIONBITS           WS_CAPTION
#define BORDERBITS            (WS_BORDER | WS_DLGFRAME | WS_THICKFRAME)
#define HAS_CAPTION(hwnd)     TestWinStyle (hwnd, CAPTIONBITS)
#define HAS_BORDER(hwnd)      TestWinStyle (hwnd, BORDERBITS)

// The pvncDrawArrow function takes the following to indicate what
// kind of arrow to draw.

#define ARROW_CLOSE     0
#define ARROW_UP        1
#define ARROW_DOWN      2
#define ARROW_RESTORE   3
#define ARROW_NEW       4
#define ARROW_SYSTEM    5

BOOL NEAR PASCAL pvncDepressButton (pvncStruct *pvnc, UINT nPos, LPRECT rect) ;
BOOL NEAR PASCAL pvncGetCaptionRect (pvncStruct *pvnc, LPRECT lprc, BOOL isInternal) ;
BOOL NEAR PASCAL pvncGetStatusBarRect (pvncStruct *pvnc, LPRECT lprc, BOOL isInternal) ;
BOOL NEAR PASCAL pvncGetSysIconRect (pvncStruct *pvnc, LPRECT lprc, BOOL origonal) ;
BOOL NEAR PASCAL pvncGetButtonRect (pvncStruct *pvnc, UINT nPos, LPRECT lprc) ;
BOOL NEAR PASCAL pvncGetNewButtonRect (pvncStruct *pvnc, LPRECT lprc) ;
BOOL NEAR PASCAL pvncGetMinButtonRect (pvncStruct *pvnc, LPRECT lprc) ;
BOOL NEAR PASCAL pvncGetMaxButtonRect (pvncStruct *pvnc, LPRECT lprc) ;
BOOL NEAR PASCAL pvncGetCloseButtonRect (pvncStruct *pvnc, LPRECT lprc) ;
BOOL NEAR PASCAL pvncDrawCaption (HDC hDC, pvncStruct *pvnc, LPRECT lprc,
                                  BOOL fSysMenu, BOOL fNew, BOOL fMin,
                                  BOOL fMax, BOOL fActive, BOOL updateTextOnly) ;
VOID NEAR PASCAL pvncInvertSysMenu (HDC hDC, pvncStruct *pvnc) ;
VOID NEAR PASCAL pvncDrawSysMenu (HDC hDC, pvncStruct *pvnc) ;
VOID NEAR PASCAL pvncDrawButton (HDC hDC, pvncStruct *pvnc, UINT nPos, BOOL fDepressed) ;
VOID NEAR PASCAL pvncDrawArrow (HDC hdc, LPRECT lprc, UINT uiStyle) ;
VOID NEAR PASCAL pvncDrawBorder (HDC hDC, pvncStruct *pvnc, BOOL fActive) ;
VOID NEAR PASCAL pvncDraw3DRect (HDC hDC, RECT *rect, UINT width, UINT height, BOOL blackBorder, BOOL inset, BOOL fActive) ;
VOID NEAR PASCAL pvncDraw3DFrame (HDC hDC, RECT *rect, BOOL inset, BOOL fActive) ;

int                     globalData = 0 ;

extern BOOL             non_client ;
extern BOOL             IsW95UserInterface ;
extern HMENU            hMainMenu ;
extern HBITMAP          hBmpIcon ;
extern HBITMAP          hBmpMaxiMini ;
extern HPALETTE         hPalApp ;

void dumpStruct (pvncStruct *pvnc)
{
  FILE        *f ;

  if ((f = fopen ("c:\\data.txt", "wt")) != NULL)
  {
    fprintf (f, "captionTotal : %d\n", pvnc->captionTotal) ;
    fprintf (f, "captionInternal : %d\n", pvnc->captionInternal) ;
    fprintf (f, "captionBorderLeft : %d\n", pvnc->captionBorderLeft) ;
    fprintf (f, "captionBorderRight : %d\n", pvnc->captionBorderRight) ;
    fprintf (f, "captionBorderTop : %d\n", pvnc->captionBorderTop) ;
    fprintf (f, "captionBorderBottom : %d\n", pvnc->captionBorderBottom) ;
    fprintf (f, "borderWidth : %d\n", pvnc->borderWidth) ;
    fprintf (f, "borderHeight : %d\n", pvnc->borderHeight) ;
    fprintf (f, "buttonWidth : %d\n", pvnc->buttonWidth) ;
    fprintf (f, "buttonHeight : %d\n", pvnc->buttonHeight) ;
    fprintf (f, "sizing : %d\n", pvnc->sizing) ;
    fprintf (f, "statusBarTotal : %d\n", pvnc->statusBarTotal) ;
    fprintf (f, "statusBarBorder : %d\n", pvnc->statusBarBorder) ;
    fprintf (f, "menuBarTotal : %d\n", pvnc->menuBarTotal) ;
    fprintf (f, "menuBarBorder : %d\n\n", pvnc->menuBarBorder) ;
    fprintf (f, "globalData : %d\n\n", globalData) ;
    fclose (f) ;
  }
}

void NEAR PASCAL pvncCreateStruct (HWND hWnd, pvncStruct *pvnc)
{
  HGLOBAL      h ;
  pvncStruct  *p ;

  if ((h = GlobalAlloc (GMEM_MOVEABLE, sizeof (pvncStruct))) != NULL)
  {
    p = GlobalLock (h) ;
    *p = *pvnc ;
    GlobalUnlock (h) ;
    SetProp (hWnd, "pvncHandle", h) ;
  }
}

void NEAR PASCAL pvncGetStruct (HWND hWnd, pvncStruct *pvnc)
{
  HGLOBAL               h ;
  pvncStruct            *p ;
  static pvncStruct     pvncDefault = { TRUE } ;

  if ((h = GetProp (hWnd, "pvncHandle")) != NULL)
  {
    p = GlobalLock (h) ;
    *pvnc = *p ;
    GlobalUnlock (h) ;
  }
  else
    *pvnc = pvncDefault ;
}

void NEAR PASCAL pvncSetStruct (HWND hWnd, pvncStruct *pvnc)
{
  HGLOBAL      h ;
  pvncStruct  *p ;

  if ((h = GetProp (hWnd, "pvncHandle")) != NULL)
  {
    p = GlobalLock (h) ;
    *p = *pvnc ;
    GlobalUnlock (h) ;
  }
}

void NEAR PASCAL pvncDestroyStruct (HWND hWnd)
{
  HGLOBAL      h ;
  pvncStruct  *p ;

  if ((h = GetProp (hWnd, "pvncHandle")) != NULL)
  {
    p = GlobalLock (h) ;
    if (p->hStatusBarFont)
      DeleteObject (p->hStatusBarFont) ;
    if (p->hMenuBarFont)
      DeleteObject (p->hMenuBarFont) ;
    if (p->hSystemFont)
      DeleteObject (p->hSystemFont) ;
    GlobalUnlock (h) ;
    GlobalFree (h) ;
    RemoveProp (hWnd, "pvncHandle") ;
  }
}

BOOL NEAR PASCAL pvncGetStatusbarClient (HWND hWnd, RECT *rect, HFONT *font)
{
  RECT        windowRect ;
  pvncStruct  pvnc ;

  pvncGetStruct (hWnd, &pvnc) ;
  if (pvnc.isMaxiMinimized || !pvnc.hasStatusBar)
    return (FALSE) ;
  GetWindowRect (hWnd, &windowRect) ;
  if (!pvncGetStatusBarRect (&pvnc, rect, TRUE))
    return (FALSE) ;
  OffsetRect (rect, -windowRect.left, -windowRect.top) ;
  *font = pvnc.hStatusBarFont ;
  return (TRUE) ;
}

void NEAR PASCAL pvncGetLeftBorderRect (pvncStruct *pvnc, RECT *winrc, RECT *rect)
{
  SetRectEmpty (rect) ;
  if (pvnc->hasBorder)
  {
    *rect = *winrc ;
    rect->right = rect->left + pvnc->borderWidth ;
  }
}

void NEAR PASCAL pvncGetRightBorderRect (pvncStruct *pvnc, RECT *winrc, RECT *rect)
{
  SetRectEmpty (rect) ;
  if (pvnc->hasBorder)
  {
    *rect = *winrc ;
    rect->left = rect->right - pvnc->borderWidth ;
  }
}

void NEAR PASCAL pvncGetTopBorderRect (pvncStruct *pvnc, RECT *winrc, RECT *rect)
{
  SetRectEmpty (rect) ;
  if (pvnc->hasBorder)
  {
    *rect = *winrc ;
    rect->bottom = rect->top + pvnc->borderHeight ;
  }
}

void NEAR PASCAL pvncGetBottomBorderRect (pvncStruct *pvnc, RECT *winrc, RECT *rect)
{
  SetRectEmpty (rect) ;
  if (pvnc->hasBorder)
  {
    *rect = *winrc ;
    rect->top = rect->bottom - pvnc->borderHeight ;
  }
}

BOOL NEAR PASCAL pvncInTopLeftRect (pvncStruct *pvnc, RECT *winrc, POINT *pt)
{
  if (pt->x < winrc->left || pt->x > winrc->left + pvnc->sizing)
    return (FALSE) ;
  if (pt->y < winrc->top || pt->y > winrc->top + pvnc->sizing)
    return (FALSE) ;
  return (TRUE) ;
}

BOOL NEAR PASCAL pvncInTopRightRect (pvncStruct *pvnc, RECT *winrc, POINT *pt)
{
  if (pt->x < winrc->right - pvnc->sizing || pt->x > winrc->right)
    return (FALSE) ;
  if (pt->y < winrc->top || pt->y > winrc->top + pvnc->sizing)
    return (FALSE) ;
  return (TRUE) ;
}

BOOL NEAR PASCAL pvncInBottomLeftRect (pvncStruct *pvnc, RECT *winrc, POINT *pt)
{
  if (pt->x < winrc->left || pt->x > winrc->left + pvnc->sizing)
    return (FALSE) ;
  if (pt->y < winrc->bottom - pvnc->sizing || pt->y > winrc->bottom)
    return (FALSE) ;
  return (TRUE) ;
}

BOOL NEAR PASCAL pvncInBottomRightRect (pvncStruct *pvnc, RECT *winrc, POINT *pt)
{
  if (pt->x < winrc->right - pvnc->sizing || pt->x > winrc->right)
    return (FALSE) ;
  if (pt->y < winrc->bottom - pvnc->sizing || pt->y > winrc->bottom)
    return (FALSE) ;
  return (TRUE) ;
}

///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//  UINT WINAPI pvncIsEnabled (HWND hWnd)
//
//  Description:
//
//    Tells us if non-client effects are enabled for this window
//
//  Comments:
//
///////////////////////////////////////////////////////////////
UINT WINAPI pvncIsEnabled (HWND hWnd )
{
  pvncStruct  pvnc ;

  pvncGetStruct (hWnd, &pvnc) ;
  return (pvnc.ncEnabled) ;
} // pvncIsEnabled()

/////////////////////////////////////////////////////////////////
//  UINT WINAPI pvncGetCaptionSize (HWND hWnd)
//
//  Description:
//
//    Gets the size of the caption (height if horz, width if
//    vertical).
//
//  Comments:
//
///////////////////////////////////////////////////////////////
UINT WINAPI pvncGetCaptionSize (HWND hWnd )
{
  pvncStruct  pvnc ;

  pvncGetStruct (hWnd, &pvnc) ;
  return (pvnc.captionTotal) ;
} // pvncGetCaptionSize()

/////////////////////////////////////////////////////////////////
//  UINT WINAPI pvncSetCaptionSize (HWND hWnd, UINT nSize)
//
//  Description:
//
//    Changes the size of the caption (height if horz, width if
//    vertical).
//
//  Comments:
//
//////////////////////////////////////////////////////////////////
UINT WINAPI pvncSetCaptionSize (HWND hWnd, UINT nSize)
{
  pvncStruct  pvnc ;

  if (nSize <= 0)
    return 0 ;
  nSize-- ;

  pvncGetStruct (hWnd, &pvnc) ;
  pvnc.captionTotal = nSize ;
  pvncSetStruct (hWnd, &pvnc) ;
  return (TRUE) ;
} // pvncSetCaptionSize()

/////////////////////////////////////////////////////////////////
//  LRESULT WINAPI pvncDefWindowProcLocal (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, defWP (*defProc))
//
//  Description:
//
//    This function should be called instead of DefWindowProc() for
//    windows that want to have itsybitsy captions.
//
//  Comments:
//
//////////////////////////////////////////////////////////////////
LRESULT WINAPI pvncDefWindowProcLocal (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, defWP defProc)
{
  RECT        rc ;
  RECT        winrect ;
  BOOL        fActive ;
  DWORD       style = GetWindowLong (hWnd, GWL_STYLE) ;
  LPRECT      lpr ;
  LRESULT     lRet ;
  pvncStruct  pvnc ;

  // the window may have our structure handle (or may not, depending on
  // whether non-client effects were enabled when it was created.) so we
  // call our destroy function which takes care of this if so.
  if (uiMsg == WM_NCDESTROY)
  {
    pvncDestroyStruct (hWnd) ;
    return (defProc (hWnd, uiMsg, wParam, lParam)) ;
  }

  pvncGetStruct (hWnd, &pvnc) ;

  // if we have switched off non-client effects then just return
  if (!non_client || !pvnc.ncEnabled)
    return (defProc (hWnd, uiMsg, wParam, lParam)) ;

  switch (uiMsg)
  {
    case WM_COMMAND:
         if (wParam >= 0xF000)
         {
           if (wParam == SC_RESTORE && pvnc.isMaxiMinimized)
           {
             pvnc.isMaxiMinimized = FALSE ;
             pvncSetStruct (hWnd, &pvnc) ;
             SendMessage (hWnd, WM_SIZE, PVNC_MAXIMINIMIZE, FALSE) ;
             return (0) ;
           }
           SendMessage (hWnd, WM_SYSCOMMAND, wParam, lParam) ;
         }
         return (defProc (hWnd, uiMsg, wParam, lParam)) ;

    case WM_GETMINMAXINFO:
         if (pvnc.isMaxiMinimized)
         {
           LPPOINT lppt = (LPPOINT) lParam ;

           lppt [1].x = 72 ;
           lppt [1].y = 72 ;
           lppt [3].x = 72 ;
           lppt [3].y = 72 ;
           lppt [4].x = 72 ;
           lppt [4].y = 72 ;
           return (0) ;
         }
         if (pvnc.hasCaption || pvnc.hasBorder)
         {
           int     nX ;
           int     cx, cy ;
           RECT    rcMenu ;
           RECT    rcNew ;
           RECT    rcMin ;
           RECT    rcMax ;
           RECT    rcClose ;
           LPPOINT lppt = (LPPOINT) lParam ;

           lRet = defProc (hWnd, uiMsg, wParam, lParam) ;
           cx = pvnc.borderWidth ;
           cy = pvnc.borderHeight ;
           pvncGetSysIconRect (&pvnc, &rcMenu, FALSE) ;
           pvncGetNewButtonRect (&pvnc, &rcNew) ;
           pvncGetMinButtonRect (&pvnc, &rcMin) ;
           pvncGetMaxButtonRect (&pvnc, &rcMax) ;
           pvncGetCloseButtonRect (&pvnc, &rcClose) ;

           // nX is the total pixels our bitmaps take up in the X direction
           nX = (rcMenu.right - rcMenu.left) +
                (rcNew.right - rcNew.left)   +
                (rcMin.right - rcMin.left)   +
                (rcMax.right - rcMax.left)    +
                (rcClose.right - rcClose.left) ;

           // lppt [3] is the minimum tracking size
           lppt [3].x = nX + pvnc.captionTotal * 2 + cx * 2 ;
           lppt [3].y = pvnc.captionTotal + cy * 2 - 1 ;
           return (lRet) ;
         }
         else
           return (defProc (hWnd, uiMsg, wParam, lParam)) ;

    /////////////////////////////////////////////////////////////////////
    // Non-client area messages.
    //
#if defined WM_SETTINGCHANGE && WM_SETTINGCHANGE != WM_WININICHANGE
    case WM_SETTINGCHANGE :
#endif
    case WM_WININICHANGE :
    case WM_NCCREATE:
         if (uiMsg != WM_NCCREATE)
           pvncDestroyStruct (hWnd) ;
         memset (&pvnc, 0, sizeof (pvnc)) ;
         pvnc.ncEnabled = TRUE ;
         pvnc.hWnd = hWnd ;

         if (HAS_BORDER (hWnd))
         {
           HDC          hdc ;
           HFONT        hfontOld ;
           LOGFONT      lf ;
           TEXTMETRIC   tm ;

           pvnc.hasBorder = TRUE ;
           pvnc.hasSizeableBorder = TestWinStyle (hWnd, WS_THICKFRAME) ;
           if (IsW95UserInterface)
           {
             pvnc.borderWidth = GetSystemMetrics (pvnc.hasSizeableBorder ? SM_CXSIZEFRAME : SM_CXBORDER) ;
             pvnc.borderHeight = GetSystemMetrics (pvnc.hasSizeableBorder ? SM_CYSIZEFRAME : SM_CYBORDER) ;
           }
           else
           {
             pvnc.borderWidth = GetSystemMetrics (pvnc.hasSizeableBorder ? SM_CXFRAME : SM_CXBORDER) + 1 ;
             pvnc.borderHeight = GetSystemMetrics (pvnc.hasSizeableBorder ? SM_CYFRAME : SM_CYBORDER) + 1 ;
           }

           pvnc.sizing = pvnc.borderWidth * 3 ;
           pvnc.hasStatusBar = TRUE ;
           pvnc.statusBarBorder = pvnc.borderHeight ;

           hdc = GetDC (hWnd) ;
           memset (&lf, '\0', sizeof (LOGFONT)) ;
           lf.lfHeight = -MulDiv (8, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
           lf.lfCharSet = ANSI_CHARSET ;
           lf.lfQuality = DEFAULT_QUALITY ;
           lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS ;
           lstrcpy (lf.lfFaceName, "Helv") ;
           lf.lfPitchAndFamily = FF_SWISS ;
           pvnc.hStatusBarFont = CreateFontIndirect (&lf) ;
           hfontOld = SelectObject (hdc, pvnc.hStatusBarFont) ;
           GetTextMetrics (hdc, &tm) ;
           pvnc.statusBarTotal = tm.tmHeight + tm.tmExternalLeading + pvnc.statusBarBorder + 3 ;
           SelectObject (hdc, hfontOld) ;
           ReleaseDC (hWnd, hdc) ;
         }

         if (HAS_CAPTION (hWnd))
         {
           HDC          hdc ;
           LOGFONT      lf ;

           pvnc.hasCaption = TRUE ;
           pvnc.captionInternal = GetSystemMetrics (IsW95UserInterface ? SM_CYSIZE : SM_CYCAPTION) ;
           pvnc.captionTotal = pvnc.captionInternal ;
           pvnc.sizing += pvnc.captionInternal / 2 - pvnc.borderWidth ;

           if (IsW95UserInterface)
           {
             pvnc.buttonWidth = GetSystemMetrics (SM_CXSIZE) - 2 ;
             pvnc.buttonHeight = GetSystemMetrics (SM_CYSIZE) - 4 ;
           }
           else
           {
             pvnc.buttonWidth = GetSystemMetrics (SM_CXSIZE) - 1 ;
             pvnc.buttonHeight = GetSystemMetrics (SM_CYSIZE) - 2 ;
           }

           if (IsW95UserInterface)
           {
             memset (&lf, 0, sizeof (LOGFONT)) ;
             hdc = GetDC (hWnd) ;
             lf.lfHeight = -MulDiv (8, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
             ReleaseDC (hWnd, hdc) ;
             lf.lfCharSet = ANSI_CHARSET ;
             lf.lfQuality = DEFAULT_QUALITY ;
             lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS ;
             lf.lfWeight = FW_BOLD ;
             lstrcpy (lf.lfFaceName, "Ms Sans Serif") ;
             lf.lfPitchAndFamily = FF_SWISS ;
             pvnc.hSystemFont = CreateFontIndirect (&lf) ;
           }
           else
             pvnc.hSystemFont = GetStockObject (SYSTEM_FONT) ;
         }

         pvncCreateStruct (hWnd, &pvnc) ;
         return (defProc (hWnd, uiMsg, wParam, lParam)) ;

    case WM_NCCALCSIZE:
         // This is sent when the window manager wants to find out
         // how big our client area is to be.
         //

         lpr = (LPRECT) lParam ;
         if (!IsIconic (hWnd))
         {
           if (pvnc.isMaxiMinimized)
           {
             InflateRect (lpr, -3, -3) ;
             return (0) ;
           }
           defProc (hWnd, uiMsg, wParam, lParam) ;
           if (pvnc.hasStatusBar)
             lpr->bottom -= pvnc.statusBarTotal ;
           if (!IsW95UserInterface)
           {
             lpr->left++ ;
//           lpr->bottom-- ;
           }
           return (0) ;
         }
         else
           return (defProc (hWnd, uiMsg, wParam, lParam)) ;

    case WM_NCHITTEST:
         if (!pvnc.isMaxiMinimized)
         {
           lRet = defProc (hWnd, uiMsg, wParam, lParam) ;
           switch (lRet)
           {
             case HTSYSMENU :
             case HTMINBUTTON :
             case HTMAXBUTTON :
                  if (!IsW95UserInterface)
                    break ;
                  // fall through
             case HTHSCROLL :
             case HTVSCROLL :
             case HTMENU :
             case HTTOPLEFT :
             case HTTOPRIGHT :
             case HTBOTTOMLEFT :
             case HTBOTTOMRIGHT :
             case HTLEFT :
             case HTRIGHT :
             case HTTOP :
             case HTBOTTOM :
                  return (lRet) ;
           }
         }
         else
           lRet = HTCLIENT ;

         if (!IsIconic (hWnd))
         {
           RECT  rect ;
           POINT pt ;

           lRet = HTCLIENT ;
           pt.x = LOWORD (lParam) ;
           pt.y = HIWORD (lParam) ;
           GetWindowRect (hWnd, &winrect) ;

           // rely on the fact that Get???Rect() return an invalid
           // (empty) rectangle if the menu/buttons don't exist
           //
           pvncGetSysIconRect (&pvnc, &rect, FALSE) ;
           if (PtInRect (&rect, pt))
             return (HTSYSMENU) ;

           // if we're in our special MaxiMinimized mode, we always return the caption.
           // this means the user can drag the window by clicking on any part of it.
           if (pvnc.isMaxiMinimized)
             return (HTCAPTION) ;

           pvncGetNewButtonRect (&pvnc, &rect) ;
           if (PtInRect (&rect, pt))
             return (HTNEWBUTTON) ;

           pvncGetMinButtonRect (&pvnc, &rect) ;
           if (PtInRect (&rect, pt))
             return (HTMINBUTTON) ;

           pvncGetMaxButtonRect (&pvnc, &rect) ;
           if (PtInRect (&rect, pt))
             return (HTMAXBUTTON) ;

           pvncGetCloseButtonRect (&pvnc, &rect) ;
           if (PtInRect (&rect, pt))
             return (HTCLOSE) ;

           pvncGetCaptionRect (&pvnc, &rect, FALSE) ;
           if (PtInRect (&rect, pt))
             return (HTCAPTION) ;

           pvncGetStatusBarRect (&pvnc, &rect, FALSE) ;
           if (PtInRect (&rect, pt))
             return (HTCAPTION) ;
         }
         return (lRet) ;

    case WM_NCLBUTTONDBLCLK:
         // Windows recieve WM_NC?BUTTONDBLCLK messages whether they
         // have CS_DBLCLKS or not.  We watch for one of these
         // to see if the user double clicked on the system menu (to
         // close the window) or on the caption (to maximize the window).
         //
         if (!IsIconic (hWnd))
         {
           if (pvnc.hasCaption && wParam == HTSYSMENU)
           {
             SendMessage (hWnd, WM_CLOSE, 0, 0L) ;
             break ;
           }
           if (pvnc.isMaxiMinimized)
           {
             pvnc.isMaxiMinimized = FALSE ;
             pvncSetStruct (hWnd, &pvnc) ;

             // changed this from PostMessage to SendMessage to combat possible problem
             // with window being double-clicked twice before GUI thread has had a chance
             // to process the first message. this is possible with high render priority
             // relative to the GUI priority. NOTE : this comment also applies to other
             // locations in the code where PostMessage was being used for this purpose.
             SendMessage (hWnd, WM_SIZE, PVNC_MAXIMINIMIZE, FALSE) ;
             break ;
           }
         }
         return (defProc (hWnd, uiMsg, wParam, lParam)) ;

    case WM_NCLBUTTONDOWN:
         if (IsIconic (hWnd))
           return (defProc (hWnd, uiMsg, wParam, lParam)) ;

         if (IsW95UserInterface && wParam != HTNEWBUTTON)
           return (defProc (hWnd, uiMsg, wParam, lParam)) ;

         switch (wParam)
         {
           case HTNEWBUTTON:
                pvncGetNewButtonRect (&pvnc, &rc) ;
                // Note that pvncDepressButton() goes into
                // a PeekMessage() loop waiting for the mouse
                // to come back up.
                //
                if (pvncDepressButton (&pvnc, HTNEWBUTTON, &rc))
                {
                  pvnc.isMaxiMinimized = TRUE ;
                  pvncSetStruct (hWnd, &pvnc) ;
                  SendMessage (hWnd, WM_SIZE, PVNC_MAXIMINIMIZE, TRUE) ;
                }
                break ;

           case HTMINBUTTON:
                pvncGetMinButtonRect (&pvnc, &rc) ;
                if (pvncDepressButton (&pvnc, HTMINBUTTON, &rc))
                  SendMessage (hWnd, WM_SYSCOMMAND, SC_MINIMIZE, lParam) ;
                break ;

           case HTMAXBUTTON:
                pvncGetMaxButtonRect (&pvnc, &rc) ;
                if (pvncDepressButton (&pvnc, HTMAXBUTTON, &rc))
                {
                  if (IsZoomed (hWnd))
                    SendMessage (hWnd, WM_SYSCOMMAND, SC_RESTORE, lParam) ;
                  else
                    SendMessage (hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, lParam) ;
                }
                break ;

           case HTCLOSE:
                pvncGetCloseButtonRect (&pvnc, &rc) ;
                if (pvncDepressButton (&pvnc, HTCLOSE, &rc))
                  SendMessage (hWnd, WM_CLOSE, 0, 0L) ;
                break ;

           case HTSYSMENU:
                pvncInvertSysMenu (NULL, &pvnc) ;
                defProc (hWnd, uiMsg, wParam, lParam) ;
                pvncInvertSysMenu (NULL, &pvnc) ;
                break ;

           default:
               return (defProc (hWnd, uiMsg, wParam, lParam)) ;
         }
         break ;

    case WM_SETTEXT:
    case WM_NCACTIVATE:
         if (uiMsg == WM_SETTEXT)
         {
           SetWindowLong (hWnd, GWL_STYLE, style & ~WS_CAPTION) ;
           lRet = defProc (hWnd, uiMsg, wParam, lParam) ;
           SetWindowLong (hWnd, GWL_STYLE, style) ;
           fActive = GetForegroundWindow () == hWnd ;
         }
         else
         {
           if (pvnc.isMaxiMinimized)
             return (TRUE) ;

           // if W95 user interface, redraw area around buttons as well to change colour
           if (IsW95UserInterface)
             defProc (hWnd, uiMsg, wParam, lParam) ;
           fActive = (BOOL) wParam ;
         }
         if (!IsIconic (hWnd) && pvnc.hasCaption && !pvnc.isMaxiMinimized)
         {
           RECT  rcCap ;
           RECT  rc ;
           HDC   hDC = GetWindowDC (hWnd) ;

           GetWindowRect (hWnd, &rc) ;
           pvncGetCaptionRect (&pvnc, &rcCap, TRUE) ;
           OffsetRect (&rcCap, -rc.left, -rc.top) ;
           pvncDrawCaption (hDC, &pvnc, &rcCap,
                            (TestWinStyle (hWnd, WS_SYSMENU) || pvnc.sysMenuOverride),
                            TestWinStyle (hWnd, WS_MINIMIZEBOX),
                            TestWinStyle (hWnd, WS_MINIMIZEBOX),
                            TestWinStyle (hWnd, WS_MAXIMIZEBOX),
                            fActive, uiMsg == WM_SETTEXT) ;
           ReleaseDC (hWnd, hDC) ;
         }
         return (uiMsg == WM_NCACTIVATE ? TRUE : lRet) ;

    case WM_NCPAINT:
         if (IsIconic (hWnd))
           return (defProc (hWnd, uiMsg, wParam, lParam)) ;

         if (!pvnc.isMaxiMinimized)
         {
           if (!IsW95UserInterface)
           {
             SetWindowLong (hWnd, GWL_STYLE, style & ~WS_SYSMENU) ;
             defProc (hWnd, uiMsg, wParam, lParam) ;
             SetWindowLong (hWnd, GWL_STYLE, style) ;
           }
           else
             defProc (hWnd, uiMsg, wParam, lParam) ;
         }

         if (pvnc.hasCaption)
         {
           RECT  rcCap ;
           RECT  rc ;
           HDC   hDC = GetWindowDC (hWnd) ;

           fActive = GetForegroundWindow () == hWnd ;
           pvncDrawBorder (hDC, &pvnc, fActive) ;
           if (pvnc.isMaxiMinimized)
           {
             ReleaseDC (hWnd, hDC) ;
             return (0) ;
           }
           GetWindowRect (hWnd, &rc) ;
           pvncGetCaptionRect (&pvnc, &rcCap, TRUE) ;
           OffsetRect (&rcCap, -rc.left, -rc.top) ;
           pvncDrawCaption (hDC, &pvnc, &rcCap,
                            (TestWinStyle (hWnd, WS_SYSMENU) || pvnc.sysMenuOverride),
                            TestWinStyle (hWnd, WS_MINIMIZEBOX),
                            TestWinStyle (hWnd, WS_MINIMIZEBOX),
                            TestWinStyle (hWnd, WS_MAXIMIZEBOX),
                            fActive, FALSE) ;
           ReleaseDC (hWnd, hDC) ;
         }
         return (0) ;

    default:
         return (defProc (hWnd, uiMsg, wParam, lParam)) ;
  }
  return (0L) ;
} // pvncDefWindowProcLocal()

LRESULT WINAPI ncDefWindowProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
//return (IsW95UserInterface ? pvnc95DefWindowProc (hWnd, uiMsg, wParam, lParam) :
//                             pvncDefWindowProcLocal (hWnd, uiMsg, wParam, lParam, DefWindowProc)) ;
  return (pvncDefWindowProcLocal (hWnd, uiMsg, wParam, lParam, DefWindowProc)) ;
}

LRESULT WINAPI pvncDefDlgProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  return (pvncDefWindowProcLocal (hWnd, uiMsg, wParam, lParam, DefDlgProc)) ;
}

//  pvncAdjustWindowRectHWND  (HWND hWnd, LPRECT lprc)
//
//    Does the same thing as the USER function AdjustWindowRect(),
//    but knows about itsybitsy windows.  AdjustWindowRect() is
//    bogus for stuff like this.
//
VOID WINAPI pvncAdjustWindowRectHWND (HWND hWnd, LPRECT lprc)
{
  pvncStruct        pvnc ;

  pvncGetStruct (hWnd, &pvnc) ;
  if (pvnc.hasBorder)
    InflateRect (lprc, pvnc.borderWidth, pvnc.borderHeight) ;
  if (pvnc.hasCaption)
    lprc->top -= pvnc.captionTotal ;
} // pvncAdjustWindowRectHWND()

///////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////

// pvncDepressButton()
//
// This function is called when the user has pressed either the min or
// max button (i.e. WM_NCLBUTTONDOWN).  We go into a Peekmessage() loop,
// waiting for the mouse to come back up.  This allows us to make the
// button change up/down state like a real button does.
//
// lprc points to the rectangle that describes the button the
// user has clicked on.
//
BOOL NEAR PASCAL pvncDepressButton (pvncStruct *pvnc, UINT button, LPRECT lprc)
{
  BOOL    fDepressed = TRUE ;
  MSG     msg ;

  // Draw button in down state
  pvncDrawButton (NULL, pvnc, button, TRUE) ;
  SetCapture (pvnc->hWnd) ;

  while (TRUE)
  {
    if (PeekMessage ((LPMSG) &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
    {
      switch (msg.message)
      {
        case WM_LBUTTONUP:
             if (fDepressed)
               pvncDrawButton (NULL, pvnc, button, FALSE) ;
             ReleaseCapture () ;
             return (PtInRect (lprc, msg.pt)) ;

        case WM_MOUSEMOVE:
             if (PtInRect (lprc, msg.pt))
             {
               if (!fDepressed)
                 pvncDrawButton (NULL, pvnc, button, fDepressed = TRUE) ;
             }
             else
             {
               if (fDepressed)
                 pvncDrawButton (NULL, pvnc, button, fDepressed = FALSE) ;
             }
             break ;
      }
    }
  }
} // pvncDepressButton()

// pvncDrawCaption (HDC hDC, HWND hWnd, LPRECT lprc,
//                   BOOL fVert, BOOL fSysMenu, BOOL fActive, BOOL updateTextOnly)
BOOL NEAR PASCAL pvncDrawCaption (HDC hDC, pvncStruct *pvnc, LPRECT lprc,
                                  BOOL fSysMenu, BOOL fNew, BOOL fMin,
                                  BOOL fMax, BOOL fActive, BOOL updateTextOnly)
{
  int         cy ;
  RECT        rcCap ;
  UINT        ui ;
  SIZE        Size ;
  HFONT       hOldFont ;
  LPSTR       lpsz ;
  HBRUSH      hbrCaption ;
  COLORREF    rgbCaptionBG ;
  COLORREF    oldColour ;

  if (IsW95UserInterface)
    rgbCaptionBG = GetSysColor (fActive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION) ;
  else
    rgbCaptionBG = fActive ? RGB (0, 0, 128) : RGB (128, 128, 128) ;
  rcCap = *lprc ;
  if (updateTextOnly || IsW95UserInterface)
  {
    if (fSysMenu)
      rcCap.left += pvnc->buttonWidth + 1 ;

    if (fMax)
      rcCap.right -= pvnc->buttonWidth + 1 ;

    if (fMin)
      rcCap.right -= pvnc->buttonWidth + 1 ;

    if (fNew)
      rcCap.right -= pvnc->buttonWidth + 1 ;

    // always allow for close button & 3-pixel offset
    rcCap.right -= pvnc->buttonWidth + 3 ;
  }

  SetBkMode (hDC, TRANSPARENT) ;
  hbrCaption = CreateSolidBrush (rgbCaptionBG) ;
  hbrCaption = SelectObject (hDC, hbrCaption) ;
  SelectObject (hDC, GetStockObject (NULL_PEN)) ;
  Rectangle (hDC, rcCap.left, rcCap.top, rcCap.right, rcCap.bottom) ;
  hbrCaption = SelectObject (hDC, hbrCaption) ;
  DeleteObject (hbrCaption) ;

  if (!updateTextOnly && !IsW95UserInterface)
  {
    if (fSysMenu)
      rcCap.left += pvnc->buttonWidth + 1 ;

    if (fMax)
      rcCap.right -= pvnc->buttonWidth + 1 ;

    if (fMin)
      rcCap.right -= pvnc->buttonWidth + 1 ;

    if (fNew)
      rcCap.right -= pvnc->buttonWidth + 1 ;

    // always allow for close button & 3-pixel offset
    rcCap.right -= pvnc->buttonWidth + 3 ;
  }

  ui = GetWindowTextLength (pvnc->hWnd) ;
  if ((lpsz = GlobalAllocPtr (GHND, ui + 2)) != NULL)
  {
    UINT    nBkMode ;

    GetWindowText (pvnc->hWnd, lpsz, ui + 1) ;
    nBkMode = SetBkMode (hDC, TRANSPARENT) ;
    hOldFont = SelectObject (hDC, pvnc->hSystemFont) ;

    GetTextExtentPoint (hDC, lpsz, ui, &Size) ;
    cy = rcCap.top + ((rcCap.bottom - rcCap.top - Size.cy) / 2) ;

    rcCap.bottom-- ;
    InflateRect (&rcCap, -1, -1) ;
    oldColour = SetTextColor (hDC, fActive ? RGBWHITE : RGBLTGRAY) ;
    ExtTextOut (hDC,
                rcCap.left,
                cy,
                ETO_CLIPPED, &rcCap,
                lpsz, ui, NULL) ;
    SetTextColor (hDC, oldColour) ;
    SelectObject (hDC, hOldFont) ;
    SetBkMode (hDC, nBkMode) ;

    GlobalUnlockPtr (lpsz) ;
    GlobalFree (GlobalPtrHandle (lpsz)) ;
  }

  if (!updateTextOnly)
  {
    if (!IsW95UserInterface)
    {
      if (fSysMenu)
        pvncDrawSysMenu (hDC, pvnc) ;
      pvncDrawButton (hDC, pvnc, HTCLOSE, FALSE) ;
      if (fMax)
        pvncDrawButton (hDC, pvnc, HTMAXBUTTON, FALSE) ;
      if (fMin)
        pvncDrawButton (hDC, pvnc, HTMINBUTTON, FALSE) ;
    }
    if (fNew)
      pvncDrawButton (hDC, pvnc, HTNEWBUTTON, FALSE) ;
  }

  return (TRUE) ;
} // pvncDrawCaption()

VOID NEAR PASCAL pvncInvertSysMenu (HDC hDC, pvncStruct *pvnc)
{
  RECT            rcButton ;
  RECT            rc ;
  BOOL            fDC ;

  if (!hDC)
  {
    fDC = TRUE ;
    hDC = GetWindowDC (pvnc->hWnd) ;
  }
  else
    fDC = FALSE ;

  if (hDC)
  {
    pvncGetSysIconRect (pvnc, &rcButton, TRUE) ;
    GetWindowRect (pvnc->hWnd, &rc) ;
    OffsetRect (&rcButton, -rc.left, -rc.top) ;
    InvertRect (hDC, &rcButton) ;
    if (fDC)
      ReleaseDC (pvnc->hWnd, hDC) ;
  }
}

VOID NEAR PASCAL pvncDrawSysMenu (HDC hDC, pvncStruct *pvnc)
{
  RECT            rcButton ;
  RECT            rc ;
  BOOL            fDC ;

  if (!hDC)
  {
    fDC = TRUE ;
    hDC = GetWindowDC (pvnc->hWnd) ;
  }
  else
    fDC = FALSE ;

  if (hDC)
  {
    pvncGetSysIconRect (pvnc, &rcButton, FALSE) ;
    GetWindowRect (pvnc->hWnd, &rc) ;
    OffsetRect (&rcButton, -rc.left, -rc.top) ;
    pvncDrawArrow (hDC, &rcButton, ARROW_SYSTEM) ;

    if (fDC)
      ReleaseDC (pvnc->hWnd, hDC) ;
  }
} // pvncDrawSysMenu()

BOOL NEAR PASCAL pvncGetCaptionRect (pvncStruct *pvnc, LPRECT lprc, BOOL isInternal)
{
  if (!pvnc->hasCaption)
  {
    SetRectEmpty (lprc) ;
    return (FALSE) ;
  }

  GetWindowRect (pvnc->hWnd, lprc) ;

  if (pvnc->isMaxiMinimized)
  {
    lprc->left += 3 ;
    lprc->top += 3 ;
    lprc->right -= 3 ;
    lprc->bottom = lprc->top + pvnc->captionTotal - 1 ;
    return (TRUE) ;
  }

  // the window might have other non-client components like
  // borders
  //
  if (pvnc->hasBorder)
  {
    lprc->left += pvnc->borderWidth ;
    lprc->top += pvnc->borderHeight ;
    lprc->right -= pvnc->borderWidth ;
  }

  lprc->bottom = lprc->top + pvnc->captionTotal - 1 ;

  if (!IsW95UserInterface)
    lprc->right += 2 ;

  return (TRUE) ;
} // pvncGetCaptionRect()

// pvncGetStatusBarRect (pvncStruct *pvnc, LPRECT lprc, BOOL isInternal)
//
//    calculates the rectangle of the status bar in screen coords.
//
BOOL NEAR PASCAL pvncGetStatusBarRect (pvncStruct *pvnc, LPRECT lprc, BOOL isInternal)
{
  RECT        captionRect ;

  if (!pvnc->hasStatusBar)
  {
    SetRectEmpty (lprc) ;
    return (FALSE) ;
  }

  GetWindowRect (pvnc->hWnd, lprc) ;

  if (pvnc->hasBorder)
  {
    lprc->left += pvnc->borderWidth ;
    lprc->right -= pvnc->borderWidth ;
    lprc->bottom -= pvnc->borderHeight ;
    if (!IsW95UserInterface)
      lprc->right++ ;
    else
      lprc->bottom-- ;
    if (isInternal)
    {
      lprc->left++ ;
      lprc->right-- ;
      lprc->bottom-- ;
    }
  }

  lprc->top = lprc->bottom - pvnc->statusBarTotal + 1 ;
  pvncGetCaptionRect (pvnc, &captionRect, FALSE) ;
  if (captionRect.bottom >= lprc->top)
    lprc->top = captionRect.bottom + 1 ;
  if ((lprc->top >= lprc->bottom) || (lprc->left >= lprc->right))
  {
    SetRectEmpty (lprc) ;
    return (FALSE) ;
  }
  if (isInternal)
    lprc->top += pvnc->statusBarBorder ;

  return (TRUE) ;
} // pvncGetStatusBarRect()

// pvncGetSysIconRect (HWND pvnc->hWnd, LPRECT lprc)
//
//    Calculates the rect of the icon in screen coordinates.
//
BOOL NEAR PASCAL pvncGetSysIconRect (pvncStruct *pvnc, LPRECT lprc, BOOL origonal)
{
  if (!TestWinStyle (pvnc->hWnd, WS_SYSMENU) && !pvnc->sysMenuOverride)
  {
    SetRectEmpty (lprc) ;
    return (FALSE) ;
  }

  if (!pvncGetCaptionRect (pvnc, lprc, FALSE))
    return (FALSE) ;

  if (pvnc->isMaxiMinimized)
  {
    lprc->right = lprc->left + 16 ;
    lprc->bottom = lprc->top + 16 ;
    return (TRUE) ;
  }

  if (!origonal)
  {
    lprc->left++ ;
    lprc->top++ ;
    lprc->right = lprc->left + pvnc->buttonWidth ;
    lprc->bottom -= 4 ;
  }
  else
  {
    lprc->left-- ;
    lprc->top-- ;
    lprc->right = lprc->left + pvnc->buttonWidth + 1 ;
    lprc->bottom -= 2 ;
  }

  return (TRUE) ;
} // pvncGetSysIconRect()

BOOL NEAR PASCAL pvncGetNewButtonRect (pvncStruct *pvnc, LPRECT lprc)
{
  BOOL        ret ;

  if (!TestWinStyle (pvnc->hWnd, WS_MINIMIZEBOX))
  {
    SetRectEmpty (lprc) ;
    return (FALSE) ;
  }

  if (TestWinStyle (pvnc->hWnd, WS_MAXIMIZEBOX))
    ret = pvncGetButtonRect (pvnc, 3, lprc)  ;
  else
    ret = pvncGetButtonRect (pvnc, 2, lprc) ;

  return (ret) ;
}

BOOL NEAR PASCAL pvncGetMinButtonRect (pvncStruct *pvnc, LPRECT lprc)
{
  if (!TestWinStyle (pvnc->hWnd, WS_MINIMIZEBOX))
  {
    SetRectEmpty (lprc) ;
    return (FALSE) ;
  }

  // The minimize button can be in either position 1 or 2.  If there
  // is a maximize button, it's in position 1.
  //
  if (TestWinStyle (pvnc->hWnd, WS_MAXIMIZEBOX))
    return (pvncGetButtonRect (pvnc, 2, lprc)) ;
  else
    return (pvncGetButtonRect (pvnc, 1, lprc)) ;
}

BOOL NEAR PASCAL pvncGetMaxButtonRect (pvncStruct *pvnc, LPRECT lprc)
{
  // The maximize button can only be in position 1.
  //
  if (!TestWinStyle (pvnc->hWnd, WS_MAXIMIZEBOX))
  {
    SetRectEmpty (lprc) ;
    return FALSE ;
  }
  else
    return (pvncGetButtonRect (pvnc, 1, lprc)) ;
}

BOOL NEAR PASCAL pvncGetCloseButtonRect (pvncStruct *pvnc, LPRECT lprc)
{
  return (pvncGetButtonRect (pvnc, 0, lprc)) ;
}

// Get the rect where a button would go.
//
// This function does not care if it's a min or max, just whether
// it is the first from the right/bottom or second from the right/bottom
// and so on..
//
BOOL NEAR PASCAL pvncGetButtonRect (pvncStruct *pvnc, UINT nPos, LPRECT lprc)
{
  if (!pvncGetCaptionRect (pvnc, lprc, TRUE))   // window coords
    return (FALSE) ;

//lprc->right -= (pvnc->captionInternal - 3) * nPos + 1 ;
//lprc->left = lprc->right - pvnc->captionInternal + 4 ;
//lprc->left = lprc->right - GetSystemMetrics (SM_CXSMSIZE) ;
//lprc->top += 1 ;
//lprc->bottom -= 4 ;

  if (!IsW95UserInterface)
    OffsetRect (lprc, -3, 0) ;

  lprc->right -= nPos * (pvnc->buttonWidth + 1) ;
  lprc->left = lprc->right - pvnc->buttonWidth + 1 ;
  lprc->top += IsW95UserInterface ? 2 : 1 ;
  lprc->bottom = lprc->top + pvnc->buttonHeight - 1 ;

  if (!IsW95UserInterface)
    lprc->left-- ;

  if (nPos)
    OffsetRect (lprc, -2, 0) ;

  return (TRUE) ;
} // pvncGetButtonRect()

// pvncDrawButton (HDC hDC, HWND pvnc->hWnd, BOOL fMin, BOOL fDepressed)
//
// Draws either the new min & min, max, or restore buttons. If fMin is FALSE
// then it will draw either the Max or Restore button.  If fDepressed is TRUE
// it will draw the button in a down state.
//
VOID NEAR PASCAL pvncDrawButton (HDC hDC, pvncStruct *pvnc, UINT button, BOOL fDepressed)
{
  RECT            rcButton ;
  RECT            rc ;
  BOOL            fDC ;
  COLORREF        colour1 ;
  COLORREF        colour2 ;
  COLORREF        colour3 ;
  COLORREF        colour4 ;
  COLORREF        oldColour ;

  if (!hDC)
  {
    fDC = TRUE ;
    hDC = GetWindowDC (pvnc->hWnd) ;
  }
  else
    fDC = FALSE ;

  if (hDC)
  {
    if (IsW95UserInterface)
    {
      colour1 = GetSysColor (fDepressed ? COLOR_3DSHADOW : COLOR_3DLIGHT) ;
      colour2 = GetSysColor (fDepressed ? COLOR_3DLIGHT : COLOR_3DSHADOW) ;
      colour3 = GetSysColor (COLOR_3DFACE) ;
      colour4 = GetSysColor (COLOR_3DHILIGHT) ;
    }
    else
    {
      colour1 = fDepressed ? RGBGRAY : RGB (223, 223, 223) ;
      colour2 = fDepressed ? RGB (223, 223, 223) : RGBGRAY ;
      colour3 = RGBLTGRAY ;
      colour4 = RGBWHITE ;
    }
    switch (button)
    {
      case HTCLOSE :
           pvncGetCloseButtonRect (pvnc, &rcButton) ;
           break ;

      case HTMAXBUTTON :
           pvncGetMaxButtonRect (pvnc, &rcButton) ;
           break ;

      case HTMINBUTTON :
           pvncGetMinButtonRect (pvnc, &rcButton) ;
           break ;

      case HTNEWBUTTON :
           pvncGetNewButtonRect (pvnc, &rcButton) ;
           break ;
    }

    if (IsRectEmpty (&rcButton))
    {
      if (fDC)
        ReleaseDC (pvnc->hWnd, hDC) ;
      return ;
    }

    GetWindowRect (pvnc->hWnd, &rc) ;
    OffsetRect (&rcButton, -rc.left, -rc.top) ;

    // Fill button
    oldColour = SetBkColor (hDC, colour3) ;
    DRAWFASTRECT (hDC, &rcButton) ;

    // Draw highlights around button
    SetBkColor (hDC, fDepressed ? RGBBLACK : colour4) ;
    rc = rcButton ;
    rc.right = rc.left + 1 ;
    DRAWFASTRECT (hDC, &rc) ;
    rc = rcButton ;
    rc.bottom = rc.top + 1 ;
    DRAWFASTRECT (hDC, &rc) ;

    SetBkColor (hDC, fDepressed ? colour4 : RGBBLACK) ;
    rc = rcButton ;
    rc.left = rc.right++ ;
    rc.bottom++ ;
    DRAWFASTRECT (hDC, &rc) ;
    rc = rcButton ;
    rc.top = rc.bottom++ ;
    DRAWFASTRECT (hDC, &rc) ;

    InflateRect (&rcButton, -1, -1) ;

    SetBkColor (hDC, colour1) ;
    // Draw left side
    rc = rcButton ;
    rc.right = rc.left + 1 ;
    DRAWFASTRECT (hDC, &rc) ;

    // Draw Top
    rc = rcButton ;
    rc.bottom = rc.top + 1 ;
    DRAWFASTRECT (hDC, &rc) ;

    SetBkColor (hDC, colour2) ;
    // Draw right side
    rc = rcButton ;
    rc.left = rc.right++ ;
    rc.bottom++ ;
    DRAWFASTRECT (hDC, &rc) ;

    // Draw bottom
    rc = rcButton ;
    rc.top = rc.bottom++ ;
    DRAWFASTRECT (hDC, &rc) ;

    rcButton.left++ ;
    rcButton.top++ ;
    rcButton.right-- ;
    rcButton.bottom-- ;

    if (fDepressed)
      OffsetRect (&rcButton, 1, 1) ;

    // rcButton now represents where we can place our arrows.
    switch (button)
    {
      case HTCLOSE :
           pvncDrawArrow (hDC, &rcButton, ARROW_CLOSE) ;
           break ;

      case HTMAXBUTTON :
           pvncDrawArrow (hDC, &rcButton, IsZoomed (pvnc->hWnd) ? ARROW_RESTORE : ARROW_UP) ;
           break ;

      case HTMINBUTTON :
           pvncDrawArrow (hDC, &rcButton, ARROW_DOWN) ;
           break ;

      case HTNEWBUTTON :
           pvncDrawArrow (hDC, &rcButton, ARROW_NEW) ;
           break ;
    }

    SetBkColor (hDC, oldColour) ;

    if (fDC)
      ReleaseDC (pvnc->hWnd, hDC) ;
  }
} // pvncDrawButton()

VOID NEAR PASCAL pvncDrawArrow (HDC hDC, LPRECT lprc, UINT uiStyle)
{
  int         i ;
  int         x ;
  int         y ;
  int         oldMode ;
  HDC         hMemDC ;
  RECT        rc ;
  RECT        rc1 ;
  HBRUSH      hbr ;
  HBITMAP     hOldBmp ;
  COLORREF    oldColour ;
  COLORREF    colour ;

  colour = GetSysColor (COLOR_BTNTEXT) ;
  oldColour = SetBkColor (hDC, colour) ;
  hbr = CreateSolidBrush (colour) ;
  x = (lprc->right - lprc->left + 1) >> 1 ;
  y = (lprc->bottom - lprc->top + 1) >> 1 ;

  switch (uiStyle)
  {
    case ARROW_CLOSE:
         rc = *lprc ;
         x-- ;
         y-- ;
         rc.left += (x + 1) / 2 ;
         rc.top += y / 2 ;
         rc.right = rc.left + 2 ;
         rc.bottom = rc.top + 2 ;
         rc1 = rc ;
         OffsetRect (&rc1, x, 0) ;
         for (i = 0 ; i <= x ; i++)
         {
           DRAWFASTRECT (hDC, &rc) ;
           DRAWFASTRECT (hDC, &rc1) ;
           OffsetRect (&rc, 1, 1) ;
           OffsetRect (&rc1, -1, 1) ;
         }
         break ;

    case ARROW_UP:
         rc = *lprc ;
         InflateRect (&rc, -2, -1) ;
         rc.right-- ;
         FrameRect (hDC, &rc, hbr) ;
         rc.bottom = ++rc.top + 1 ;
         DRAWFASTRECT (hDC, &rc) ;
         break ;

    case ARROW_DOWN:
         rc = *lprc ;
         InflateRect (&rc, -4, 0) ;
         rc.top = --rc.bottom - 2 ;
         OffsetRect (&rc, -1, 0) ;
         DRAWFASTRECT (hDC, &rc) ;
         break ;

    case ARROW_RESTORE:
         rc = *lprc ;
         rc.left++ ;
         rc.right = rc.left + x + 1 ;
         rc.top = rc.bottom - y ;
         rc1 = rc ;
         FrameRect (hDC, &rc, hbr) ;
         rc.bottom = ++rc.top + 1 ;
         DRAWFASTRECT (hDC, &rc) ;
         OffsetRect (&rc1, x / 2, -y + 2) ;
         FrameRect (hDC, &rc1, hbr) ;
         rc1.bottom = ++rc1.top + 1 ;
         DRAWFASTRECT (hDC, &rc1) ;
         break ;

    case ARROW_NEW :
         rc = *lprc ;
         InflateRect (&rc, -2, -1) ;
         rc.left += (rc.right - rc.left + 1) / 2 - 1 ;
         rc.bottom = rc.top + (rc.bottom - rc.top + 1) / 2 + 2 ;
         rc.left-- ;
         FrameRect (hDC, &rc, hbr) ;
         rc.bottom = ++rc.top + 1 ;
         DRAWFASTRECT (hDC, &rc) ;
         break ;

    case ARROW_SYSTEM :
         hMemDC = CreateCompatibleDC (hDC) ;
         hOldBmp = SelectObject (hMemDC, hBmpIcon) ;
         oldMode = SetStretchBltMode (hDC, STRETCH_DELETESCANS) ;
         StretchBlt (hDC, lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top, hMemDC, 0, 0, 17, 13, SRCCOPY) ;
         SetStretchBltMode (hDC, oldMode) ;
         SelectObject (hMemDC, hOldBmp) ;
         DeleteDC (hMemDC) ;
         break ;
  }
  DeleteObject (hbr) ;
  SetBkColor (hDC, oldColour) ;
}  // pvncDrawArrow()

VOID NEAR PASCAL pvncDraw3DFrame (HDC hDC, RECT *rect, BOOL inset, BOOL fActive)
{
  HPEN        hpen1 ;
  HPEN        hpen2 ;
  HPEN        hpenOld ;

  if (hDC)
  {
    if (fActive)
    {
      hpen1 = CreatePen (PS_SOLID, 1, GetSysColor (inset ? COLOR_INACTIVECAPTION : COLOR_ACTIVECAPTION)) ;
      hpen2 = CreatePen (PS_SOLID, 1, GetSysColor (inset ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION)) ;
    }
    else
    {
      hpen1 = CreatePen (PS_SOLID, 1, GetSysColor (inset ? COLOR_BTNSHADOW : COLOR_BTNHIGHLIGHT)) ;
      hpen2 = CreatePen (PS_SOLID, 1, GetSysColor (inset ? COLOR_BTNHIGHLIGHT : COLOR_BTNSHADOW)) ;
    }
    hpenOld = SelectObject (hDC, hpen1) ;

    MoveToEx (hDC, rect->left, rect->bottom - 1, NULL) ;
    SelectObject (hDC, hpen1) ;
    LineTo (hDC, rect->left, rect->top) ;
    LineTo (hDC, rect->right - 1, rect->top) ;
    SelectObject (hDC, hpen2) ;
    LineTo (hDC, rect->right - 1, rect->bottom - 1) ;
    LineTo (hDC, rect->left, rect->bottom - 1) ;

    SelectObject (hDC, hpenOld) ;
    DeleteObject (hpen1) ;
    DeleteObject (hpen2) ;
  }
}

VOID NEAR PASCAL pvncDraw3DRect (HDC hDC, RECT *rect, UINT width, UINT height, BOOL blackBorder, BOOL inset, BOOL fActive)
{
  RECT        rc1 ;
  RECT        rc2 ;
  COLORREF    oldColour ;

  if (hDC)
  {
    rc1 = *rect ;
    oldColour = SetBkColor (hDC, GetSysColor (fActive ? COLOR_ACTIVECAPTION : COLOR_BTNFACE)) ;

    if (blackBorder)
    {
      FrameRect (hDC, &rc1, GetStockObject (BLACK_BRUSH)) ;
      InflateRect (&rc1, -width + 1, -height + 1) ;
      FrameRect (hDC, &rc1, GetStockObject (BLACK_BRUSH)) ;
      InflateRect (&rc1, width - 2, height - 2) ;
      width -= 2 ;
      height -= 2 ;
    }

    if (width > 2)
    {
      rc2 = rc1 ;
      rc2.left++ ;
      rc2.right = rc2.left + width - 2 ;
      rc2.top++ ;
      rc2.bottom-- ;
      DRAWFASTRECT (hDC, &rc2) ;
      OffsetRect (&rc2, rc1.right - rc1.left - width, 0) ;
      DRAWFASTRECT (hDC, &rc2) ;
    }

    if (height > 2)
    {
      rc2 = rc1 ;
      rc2.left++ ;
      rc2.right-- ;
      rc2.top++ ;
      rc2.bottom = rc2.top + height - 2 ;
      DRAWFASTRECT (hDC, &rc2) ;
      OffsetRect (&rc2, 0, rc1.bottom - rc1.top - height) ;
      DRAWFASTRECT (hDC, &rc2) ;
    }

    pvncDraw3DFrame (hDC, &rc1, inset, fActive) ;
    InflateRect (&rc1, -width + 1, -height + 1) ;
    pvncDraw3DFrame (hDC, &rc1, !inset, fActive) ;

    SetBkColor (hDC, oldColour) ;
  }
}

VOID NEAR PASCAL pvncDrawRectangle (HDC hDC, RECT *rect, COLORREF colour)
{
  HBRUSH      hBrush, hBrushOld ;

  if (hDC)
  {
    hBrush = CreateSolidBrush (colour) ;
    hBrushOld = SelectObject (hDC, hBrush) ;
    FrameRect (hDC, rect, hBrush) ;
    SelectObject (hDC, hBrushOld) ;
    DeleteObject (hBrush) ;
  }
}

VOID NEAR PASCAL pvncDrawW95Border (HDC hDC, RECT *rect)
{
  RECT        rc ;
  RECT        rc1 ;
  COLORREF    oldColour ;
  COLORREF    colour1 = IsW95UserInterface ? RGB (223, 223, 223) : RGB (224, 224, 224) ;
  COLORREF    colour2 = IsW95UserInterface ? RGB (191, 191, 191) : RGB (192, 192, 192) ;
  COLORREF    colour3 = IsW95UserInterface ? RGB (127, 127, 127) : RGB (128, 128, 128) ;

  if (hDC)
  {
    rc1 = *rect ;
    rect = &rc1 ;

    if (!IsW95UserInterface)
    {
      // this will leave the black border on the left and top.
      rect->left++ ;
      rect->top++ ;
    }

    rc = *rect ;
    oldColour = SetBkColor (hDC, colour1) ;
    // draw left-hand side
    rc.right = rc.left + 1 ;
    rc.bottom-- ;
    DRAWFASTRECT (hDC, &rc) ;
    rc = *rect ;
    // draw top
    rc.bottom = rc.top + 1 ;
    rc.right-- ;
    DRAWFASTRECT (hDC, &rc) ;

    SetBkColor (hDC, RGBWHITE) ;
    OffsetRect (&rc, 0, 1) ;
    rc.left++ ;
    rc.right-- ;
    DRAWFASTRECT (hDC, &rc) ;
    rc = *rect ;
    rc.right = ++rc.left + 1 ;
    rc.top++ ;
    rc.bottom -= 2 ;
    DRAWFASTRECT (hDC, &rc) ;

    SetBkColor (hDC, colour3) ;
    rc = *rect ;
    // draw right-hand side
    rc.left = --rc.right - 1 ;
    rc.bottom-- ;
    DRAWFASTRECT (hDC, &rc) ;
    rc = *rect ;
    // draw bottom
    rc.top = --rc.bottom - 1 ;
    rc.right-- ;
    DRAWFASTRECT (hDC, &rc) ;
    SetBkColor (hDC, oldColour) ;

    rc = *rect ;
    InflateRect (&rc, -2, -2) ;
    pvncDrawRectangle (hDC, &rc, colour2) ;
    InflateRect (&rc, -1, -1) ;
    pvncDrawRectangle (hDC, &rc, colour2) ;
  }
}

VOID NEAR PASCAL pvncDrawBorder (HDC hDC, pvncStruct *pvnc, BOOL fActive)
{
  RECT            windowRect ;
  RECT            rc1 ;
  RECT            rc2 ;
  BOOL            fDC ;
  COLORREF        oldColour ;
  COLORREF        highlight = GetSysColor (COLOR_BTNHIGHLIGHT) ;
  COLORREF        shadow = GetSysColor (COLOR_BTNSHADOW) ;

  if (!hDC)
  {
    fDC = TRUE ;
    hDC = GetWindowDC (pvnc->hWnd) ;
  }
  else
    fDC = FALSE ;

  if (hDC)
  {
    GetWindowRect (pvnc->hWnd, &windowRect) ;
    OffsetRect (&windowRect, -windowRect.left, -windowRect.top) ;

    if (pvnc->isMaxiMinimized)
    {
      pvncDraw3DRect (hDC, &windowRect, 4, 4, TRUE, FALSE, FALSE) ;
      return ;
    }

    // this draws the actual border, sans decorations.
    if (!IsW95UserInterface)
      pvncDrawW95Border (hDC, &windowRect) ;

    if (pvnc->hasStatusBar)
    {
      oldColour = SetBkColor (hDC, GetSysColor (COLOR_BTNFACE)) ;
      GetWindowRect (pvnc->hWnd, &windowRect) ;
      pvncGetStatusBarRect (pvnc, &rc1, FALSE) ;
      OffsetRect (&rc1, -windowRect.left, -windowRect.top) ;

      // clear status bar (including border)
      rc1.bottom++ ;
      DRAWFASTRECT (hDC, &rc1) ;

      // draw top highlight of client-area separator
      SetBkColor (hDC, highlight) ;
      rc1.bottom = rc1.top + 1 ;
      DRAWFASTRECT (hDC, &rc1) ;

      // draw bottom highlight of status bar
      OffsetRect (&rc1, 0, pvnc->statusBarTotal - (IsW95UserInterface ? 2 : 1)) ;
      rc2.left = rc1.left ;
      rc2.bottom = rc1.bottom ;
      DRAWFASTRECT (hDC, &rc1) ;

      // draw bottom shadow of client-area separator
      OffsetRect (&rc1, 0, pvnc->statusBarBorder - pvnc->statusBarTotal + (IsW95UserInterface ? 1 : 0)) ;
      rc2.top = rc1.top ;
      SetBkColor (hDC, shadow) ;
      DRAWFASTRECT (hDC, &rc1) ;

      // draw black line above client-area separator
      OffsetRect (&rc1, 0, -pvnc->statusBarBorder) ;
      SetBkColor (hDC, RGBBLACK) ;
//    DRAWFASTRECT (hDC, &rc1) ;

      // draw left-hand shadow
      rc2.right = rc2.left + 1 ;
      SetBkColor (hDC, shadow) ;
      DRAWFASTRECT (hDC, &rc2) ;

      // draw right-hand highlight
      rc2.top++ ;
      rc2.left = rc1.right - 1 ;
      rc2.right = rc1.right ;
      SetBkColor (hDC, highlight) ;
      DRAWFASTRECT (hDC, &rc2) ;
      SetBkColor (hDC, oldColour) ;
    }

    if (fDC)
      ReleaseDC (pvnc->hWnd, hDC) ;
  }
}


