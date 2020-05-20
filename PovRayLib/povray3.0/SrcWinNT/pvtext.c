/****************************************************************************
*                pvtext.c
*
*  This module implements message and message display routines for Windows.
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

// There is a nasty bug somewhere in this code that causes GPF's when we
// try to clear the buffer. This is intermittent. I suspect the cause is
// freeing an already freed pointer. I really want to rewrite the whole
// message buffer handling code. Volunteers ? (:-).

#define POVWIN_FILE

#include <math.h>
#include <setjmp.h>
#include <string.h>
#include <windows.h>
#include "frame.h"
#include "colour.h"
#include "povray.h"
#include "optout.h"
#pragma hdrstop

#include <commctrl.h>
#include "pvengine.h"
#include "pvengine.rh"
#include "pvclient.h"
#include "pvguiext.h"

#define MAX_SEEK_INDEX            1024

typedef struct
{
  int         offset ;
  int         dividerOffset ;
  char        text [128] ;
  BOOL        visible ;
  BOOL        hasText ;
  BOOL        isCentered ;
  BOOL        useWidth ;
  UINT        textLen ;
  UINT        chars ;
  UINT        width ;
  UINT        bitmapOffsetX ;
  UINT        bitmapOffsetY ;
  UINT        bitBltWidth ;
  UINT        bitBltHeight ;
  HBITMAP     hBitmap ;
} statusBarStruct ;

typedef struct
{
  unsigned              Line ;
  unsigned short        Offset ;
} seekStruct ;

int                     message_xchar ;
int                     message_ychar ;
int                     top_message_row ;
int                     message_scroll_pos_x ;
int                     message_scroll_pos_y ;
int                     message_count ;
int                     message_cols ;
int                     message_rows ;
char                    *message_buffer [MAX_MESSAGE] ;
char                    **first_message ;
char                    **next_message ;
char                    **last_message ;
char                    str_buffer [512] ;
char                    message_font_name [128] ;
char                    line_buffer [2048] ;
unsigned                message_font_size ;
unsigned                message_font_weight ;
unsigned                editor_line_number ;
unsigned                editor_char_pos ;
unsigned                editor_offset ;
unsigned                seek_entry_count ;
RECT                    current_rect ;
HFONT                   message_font ;
HFONT                   tab_font ;
BOOL                    keep_messages ;
seekStruct              seek_entries [MAX_SEEK_INDEX] ;
statusBarStruct         statusBar [StatusLast] ;
extern int              statistics_count ;
extern char             ini_path [_MAX_PATH] ;
extern char             EngineIniFileName [_MAX_PATH] ;
extern char             RerunIniPath [_MAX_PATH] ;
extern char             CurrentRerunFileName [_MAX_PATH] ;
extern char             tool_help [MAX_TOOLCMD] [MAX_TOOLHELPTEXT] ;
extern char             requested_render_file [] ;
extern void             *CurrentEditor ;
extern unsigned         background_width ;
extern unsigned         background_height ;
extern unsigned         background_shade ;
extern unsigned         window_count ;
extern HWND             main_window ;
extern HWND             message_window ;
extern HWND             hToolComboBox ;
extern HWND             window_list [MAX_WINDOWS] ;
extern HWND             current_window ;
extern HWND             tabbed_window ;
extern HWND             editor_window ;
extern HWND             toolbar_combobox ;
extern BOOL             IsWin32 ;
extern BOOL             fast_scroll ;
extern BOOL             tile_background ;
extern BOOL             isMaxiMinimized ;
extern BOOL             IsW95UserInterface ;
extern BOOL             use_toolbar ;
extern BOOL             resizing ;
extern BOOL             quit ;
extern BOOL             exit_after_render ;
extern BOOL             demo_mode ;
extern BOOL             debugging ;
extern BOOL             render_requested ;
extern BOOL             noexec ;
extern jmp_buf          jump_buffer ;
extern HBITMAP          hBmpBackground ;
extern HBITMAP          hBmpRendering ;
extern COLORREF         background_colour ;
extern COLORREF         text_colour ;
extern HPALETTE         hPalApp ;
extern HINSTANCE        hInstance ;
extern EditStruct       Edit ;
extern CRITICAL_SECTION critical_section ;

#define MAX_TOOLS       sizeof (toolbar_ids) / sizeof (int)

int                     toolbar_ids [] = {
                                          0,
                                          CM_FILENEW,
                                          CM_FILEOPEN,
                                          CM_FILESAVE,
                                          0,
                                          CM_FILEQUEUE,
                                          CM_RERUNDIALOG,
                                          0,
                                          CM_RENDERSHOW,
                                          CM_RENDERCLOSE,
                                          0,
                                          CM_COMMANDLINE,
                                          CM_SOURCEFILE,
                                          CM_FILERENDER,
                                          0,
                                          CM_HELPCONTENTS,
                                          CM_HELPPOVRAY,
                                          0,
                                          CM_RENDERSLEEP | 0x8000,
                                          0,
                                          0
                                         } ;

BOOL is_horz_line (char *s)
{
  if (strlen (s) < 70)
    return (FALSE) ;

  while (*s)
    if (*s++ != '-')
      return (FALSE) ;

  return (TRUE) ;
}

char *buffer_str (char *s, lftype *lf)
{
  char                  *ptr ;
  static int            x = -1 ;

  if (s == NULL)
  {
    x = -1 ;
    return (NULL) ;
  }
  *lf = None ;
  if (x == -1)
  {
    x = 0 ;
    memset (str_buffer, 0, sizeof (str_buffer)) ;
  }
  ptr = str_buffer + x ;
  while (*s)
  {
    switch (*s)
    {
      case '\r' : x = 0 ;
                  ptr = str_buffer ;
                  *lf = CR ;
                  break ;
      case '\n' : x = -1 ;
                  *lf = LF ;
                  return (++s) ;
      case '\b' : if (x)
                  {
                    --x ;
                    *ptr-- = '\0' ;
                  }
                  break ;
      default   : if (isprint (*s) && x < sizeof (str_buffer) - 1)
                  {
                    x++ ;
                    *ptr++ = *s ;
                  }
                  break ;
    }
    s++ ;
  }
  return (s) ;
}

int update_message_display (lftype lf)
{
  RECT        rect ;

  if (message_window == NULL)
    return (0) ;
  GetClientRect (message_window, &rect) ;
  message_cols = rect.right / message_xchar ;
  message_rows = rect.bottom / message_ychar ;
  if (lf == None || lf == LF)
  {
    EnterCriticalSection (&critical_section) ;
    top_message_row = message_count > message_rows ? message_count - message_rows : 0 ;
    LeaveCriticalSection (&critical_section) ;
    SetScrollRange (message_window, SB_HORZ, 0, need_hscroll (), FALSE) ;
    SetScrollPos (message_window, SB_HORZ, message_scroll_pos_x, TRUE) ;
    SetScrollRange (message_window, SB_VERT, 0, top_message_row, FALSE) ;
    SetScrollPos (message_window, SB_VERT, message_scroll_pos_y, TRUE) ;
  }
  if (lf == None)
    return (message_rows) ;
  if (lf == LF)
  {
    EnterCriticalSection (&critical_section) ;
    // is there room for another row ?
    if (current_rect.bottom + message_ychar <= rect.bottom)
    {
      // yes there is
      current_rect.top += message_ychar ;
      current_rect.bottom += message_ychar ;
    }
    else
      ScrollWindow (message_window, 0, -message_ychar, NULL, &rect) ;
    message_scroll_pos_y = top_message_row ;
    SetScrollPos (message_window, SB_VERT, message_scroll_pos_y, TRUE) ;
    LeaveCriticalSection (&critical_section) ;
  }
  PovInvalidateRect (message_window, &current_rect, TRUE) ;
  if (!fast_scroll)
    UpdateWindow (message_window) ;
  return (message_rows) ;
}

void buffer_message (msgtype message_type, char *s)
{
  char                  *s1 ;
  char                  str1 [32] ;
  char                  str2 [128] ;
  lftype                lf ;
  static msgtype        _message_type = All ;

  ExternalBufferMessage (message_type, s) ;

  if (message_type != _message_type)
    _message_type = message_type ;

  while (*s)
  {
    s1 = s ;
    EnterCriticalSection (&critical_section) ;
    s = buffer_str (s, &lf) ;
    LeaveCriticalSection (&critical_section) ;
    if (lf == None)
      continue ;
    if (message_type == mStatistics)
    {
      if (statistics_count != -1)
      {
        sprintf (str1, "StatLn%02d", statistics_count++) ;
        sprintf (str2, "\"%.125s\"", str_buffer) ;
        WritePrivateProfileString ("Statistics", str1, str2, CurrentRerunFileName) ;
        /*
        ** We do this to make sure the total statistics don't get written as well,
        ** since we really don't want them for now. 'Total Time' is the last line.
        */
        if (strncmp (s1, "          Total Time:", 21) == 0)
          statistics_count = -1 ;
      }
    }

    if ((s1 = malloc (strlen (str_buffer) + 2)) == NULL)
    {
      PovMessageBox ("Failed to allocate memory for message string", "Fatal Error") ;
      longjmp (jump_buffer, -1) ;
    }

    if (is_horz_line (str_buffer))
      message_type = mHorzLine ;

    strcpy (s1 + 1, str_buffer) ;
    *s1 = (uchar) message_type ;

    if (lf == CR)
    {
      EnterCriticalSection (&critical_section) ;
      if (*last_message)
        free (*last_message) ;
      *last_message = s1 ;
      LeaveCriticalSection (&critical_section) ;
      update_message_display (CR) ;
    }
    else
    {
      EnterCriticalSection (&critical_section) ;
      if (*next_message)
      {
        free (*next_message) ;
        *next_message = NULL ;
        if (++first_message == message_buffer + MAX_MESSAGE)
          first_message = message_buffer ;
      }
      else
        message_count++ ;
      *next_message = s1 ;
      last_message = next_message ;
      if (++next_message == message_buffer + MAX_MESSAGE)
        next_message = message_buffer ;
      LeaveCriticalSection (&critical_section) ;
      update_message_display (LF) ;
    }
  }
}

void message_printf (char *format, ...)
{
  char                  str [2048] ;
  va_list               arg_ptr ;

  if (strlen (format) > sizeof (str) - 256)
    return ;
  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  buffer_message (None, str) ;
}

void status_printf (int nSection, char *format, ...)
{
  char                  str [256] ;
  va_list               arg_ptr ;

  va_start (arg_ptr, format) ;
  vsprintf (str, format, arg_ptr) ;
  va_end (arg_ptr) ;
  say_status_message (nSection, str) ;
}

char *clean_str (char *s)
{
  char        *s1 ;
  static char str [512] ;

  EnterCriticalSection (&critical_section) ;
  for (s1 = str ; *s ; s++)
    if (*s >= ' ')
      *s1++ = *s ;
  *s1 = '\0' ;
  LeaveCriticalSection (&critical_section) ;
  return  (str) ;
}

void erase_display_window (HDC hdc, int xoffset, int yoffset)
{
  int         x, y ;
  HDC         hdcMemory ;
  RECT        rect ;
  HBRUSH      hbr ;
  HBITMAP     oldBmp ;

  if (message_window == NULL)
    return ;
  GetClientRect (message_window, &rect) ;
  rect.right++ ;
  rect.bottom++ ;
  if (tile_background)
  {
    hdcMemory = CreateCompatibleDC (hdc) ;
    oldBmp = SelectObject (hdcMemory, hBmpBackground) ;
    xoffset %= background_width ;
    yoffset %= background_height ;
    for (y = -yoffset ; y < rect.bottom ; y += background_height)
      for (x = -xoffset ; x < rect.right ; x += background_width)
        BitBlt (hdc, x, y, background_width, background_height, hdcMemory, 0, 0, SRCCOPY) ;
    SelectObject (hdcMemory, oldBmp) ;
    DeleteDC (hdcMemory) ;
  }
  else
  {
    hbr = CreateSolidBrush (background_colour) ;
    FillRect (hdc, &rect, hbr) ;
    DeleteObject (hbr) ;
  }
}

void paint_display_window (HDC hdc)
{
  int         x, y ;
  int         message_number ;
  int         oldMode ;
  int         dividerStep ;
  int         xoffset ;
  int         yoffset ;
  char        **message = first_message ;
  BOOL        erased = FALSE ;
  RECT        rect ;
  HPEN        hpen1 ;
  HPEN        hpen2 ;
  HPEN        hpenOld ;
  HFONT       oldFont ;
  COLORREF    oldColour ;
  COLORREF    oldBkColour ;
  static RECT oldRect ;

  EnterCriticalSection (&critical_section) ;
  GetClientRect (message_window, &rect) ;
  current_rect.left = 0 ;
  current_rect.right = rect.right ;
  current_rect.top = -message_ychar ;
  current_rect.bottom = 0 ;
  xoffset = (unsigned) message_scroll_pos_x * message_xchar ;
  yoffset = (unsigned) (first_message - message_buffer) * message_ychar ;

  if (*message == NULL || resizing)
  {
    erase_display_window (hdc, xoffset, yoffset) ;
    LeaveCriticalSection (&critical_section) ;
    oldRect = rect ;
    return ;
  }

//if (resizing)
//{
//  if ((oldRect.right != rect.right && oldRect.bottom == rect.bottom) || (oldRect.right == rect.right && oldRect.bottom != rect.bottom))
//    ExcludeClipRect (hdc, oldRect.left, oldRect.top, oldRect.right - message_xchar * 2, oldRect.bottom - message_ychar) ;
//  erase_display_window (hdc, offset) ;
//  erased = TRUE ;
//}

  oldRect = rect ;

  hpen1 = CreatePen (PS_SOLID, 1, RGB (192,192,192)) ;
  hpen2 = CreatePen (PS_SOLID, 1, RGB (64,64,64)) ;
  hpenOld = SelectObject (hdc, hpen2) ;
  oldFont = SelectObject (hdc, message_font) ;
  oldMode = SetBkMode (hdc, TRANSPARENT) ;
  oldColour = SetTextColor (hdc, text_colour) ;
  oldBkColour = SetBkColor (hdc, background_shade) ;
  x = message_scroll_pos_x * -message_xchar + message_xchar ;
//if (x == 0)
//  x = message_xchar ;
  for (message_number = y = 0 ; y < rect.bottom ; message_number++)
  {
    if (*message == NULL)
      break ;
    if (message_number >= message_scroll_pos_y)
    {
      if (!erased)
      {
        erase_display_window (hdc, xoffset, yoffset) ;
        erased++ ;
      }
      current_rect.top += message_ychar ;
      current_rect.bottom += message_ychar ;
      if (**message == mDivider || **message == mHorzLine)
      {
        if (background_shade != RGB (1, 1, 1) && tile_background)
          DRAWFASTRECT (hdc, &current_rect) ;
        dividerStep = **message == mDivider ? message_ychar / 3 : message_ychar / 2 - 1 ;
        MoveToEx (hdc, current_rect.left, y + message_ychar - dividerStep, NULL) ;
        SelectObject (hdc, hpen2) ;
        LineTo (hdc, current_rect.left, y + dividerStep) ;
        LineTo (hdc, current_rect.right - message_xchar, y + dividerStep) ;
        SelectObject (hdc, hpen1) ;
        LineTo (hdc, current_rect.right - message_xchar, y + message_ychar - dividerStep) ;
        LineTo (hdc, current_rect.left, y + message_ychar - dividerStep) ;
      }
      else
        ExtTextOut (hdc, x, y, ETO_CLIPPED, &current_rect, *message + 1, strlen (*message + 1), NULL) ;
      y += message_ychar ;
    }
    if (message == last_message)
      break ;
    if (++message == message_buffer + MAX_MESSAGE)
      message = message_buffer ;
    yoffset += message_ychar ;
  }
  if (!erased)
    erase_display_window (hdc, xoffset, yoffset) ;
  SetTextColor (hdc, oldColour) ;
  SetBkColor (hdc, oldBkColour) ;
  SetBkMode (hdc, oldMode) ;
  SelectObject (hdc, oldFont) ;
  SelectObject (hdc, hpenOld) ;
  DeleteObject (hpen1) ;
  DeleteObject (hpen2) ;
  LeaveCriticalSection (&critical_section) ;
}

void get_logfont (HDC hdc, LOGFONT *lf)
{
  memset (lf, 0, sizeof (LOGFONT)) ;
  lf->lfHeight = -MulDiv (message_font_size, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf->lfWeight = message_font_weight ;
  lf->lfPitchAndFamily = FIXED_PITCH ;
  lf->lfCharSet = DEFAULT_CHARSET ;
  strncpy (lf->lfFaceName, message_font_name, sizeof (lf->lfFaceName) - 1) ;
}

int create_message_font (HDC hdc, LOGFONT *lf)
{
  HFONT       hfontOld ;
  TEXTMETRIC  tm ;

  if ((message_font = CreateFontIndirect (lf)) == NULL)
    return (1) ;
  hfontOld = SelectObject (hdc, message_font) ;
  GetTextMetrics (hdc, &tm) ;
  message_xchar = tm.tmAveCharWidth ;
  message_ychar = tm.tmHeight + tm.tmExternalLeading ;
  SelectObject (hdc, hfontOld) ;
  return (0) ;
}

int initialise_message_display (void)
{
  HDC         hdc ;
  LOGFONT     lf ;

  if (message_window == NULL)
  {
    PovMessageBox ("Message Window does not exist", "Initialise Message Display - Fatal Error") ;
    return (1) ;
  }
  hdc = GetDC (message_window) ;
  get_logfont (hdc, &lf) ;
  if (create_message_font (hdc, &lf) != 0)
  {
    PovMessageBox ("Failed to create message font", "Initialise Message Display - Fatal Error") ;
    ReleaseDC (message_window, hdc) ;
    return (1) ;
  }
  first_message = next_message = message_buffer ;
  last_message = NULL ;
  buffer_str (NULL, NULL) ;
  top_message_row = message_count = message_scroll_pos_x = message_scroll_pos_y = 0 ;
  current_rect.left = current_rect.bottom = current_rect.right = 0 ;
  current_rect.top = -message_ychar ;
  ReleaseDC (message_window, hdc) ;
  return (0) ;
}

void clear_messages (void)
{
  int         i ;
  char        **p ;

  EnterCriticalSection (&critical_section) ;
  buffer_str (NULL, NULL) ;
  // free any buffered lines still around from a previous run of the renderer
  for (p = message_buffer, i = 0 ; i < MAX_MESSAGE ; p++, i++)
  {
    if (*p)
      free (*p) ;
    *p = NULL ;
  }
  first_message = next_message = message_buffer ;
  last_message = NULL ;
  top_message_row = message_count = message_scroll_pos_x = message_scroll_pos_y = 0 ;
  current_rect.left = current_rect.bottom = current_rect.right = 0 ;
  current_rect.top = -message_ychar ;
  LeaveCriticalSection (&critical_section) ;
  message_printf ("Messages cleared.\n") ;
}

int need_hscroll (void)
{
  int         x ;
  int         xchars ;
  int         width = 0 ;
  char        **message = first_message ;
  RECT        rect ;

  /* modified to return the scroll range if ANY line is long enough */
  if (message_window == NULL || *message == NULL)
    return (0) ;
  GetClientRect (message_window, &rect) ;
  xchars = rect.right / message_xchar - 1 ;
  while (*message)
  {
    x = strlen (*message + 1) ;
    if (x >= xchars)
      if (x - xchars > width)
        width = x - xchars ;
    if (message == next_message)
      break ;
    if (++message == message_buffer + MAX_MESSAGE)
      message = message_buffer ;
  }
  return (width) ;
}

void dump_pane_to_clipboard (void)
{
  int         y ;
  int         message_number ;
  int         length = 0 ;
  char        **message = first_message ;
  char        *s ;
  RECT        rect ;
  HGLOBAL     hText ;

  if (message_window == NULL || *message == NULL)
    return ;
  if (OpenClipboard (message_window) == FALSE)
  {
    PovMessageBox ("Could not open clipboard", "Error") ;
    return ;
  }
  if ((hText = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, 33000)) == NULL)
    return ;
  if ((s = GlobalLock (hText)) == NULL)
    return ;
  GetClientRect (message_window, &rect) ;
  for (message_number = y = 0 ; y < rect.bottom ; message_number++)
  {
    if (*message == NULL)
      break ;
    if (message_number >= message_scroll_pos_y)
    {
      length += strlen (*message + 1) + 2 ;
      if (length >= 32767)
        break ;
      s += sprintf (s, "%s\r\n", *message + 1) ;
      y += message_ychar ;
    }
    if (message == next_message)
      break ;
    if (++message == message_buffer + MAX_MESSAGE)
      message = message_buffer ;
  }
  GlobalUnlock (hText) ;
  GlobalReAlloc (hText, length + 1, GMEM_MOVEABLE | GMEM_DDESHARE) ;
  SetClipboardData (CF_TEXT, hText) ;
  CloseClipboard () ;
}

void draw_rerun_listbox (DRAWITEMSTRUCT *d)
{
  int         oldMode ;
  char        str [256] ;
  char        *s ;
  RECT        rect ;
  HFONT       oldFont ;
  COLORREF    oldBackground ;
  COLORREF    oldForeground ;

  if (d->itemID == -1)
    return ;
  oldFont = SelectObject (d->hDC, message_font) ;
  oldMode = SetBkMode (d->hDC, TRANSPARENT) ;
  oldForeground = SetTextColor (d->hDC, GetSysColor (d->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT)) ;
  oldBackground = SetBkColor (d->hDC, GetSysColor (d->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT : COLOR_BTNFACE)) ;
  rect = d->rcItem ;
  ExtTextOut (d->hDC, rect.left, rect.top, ETO_OPAQUE, &rect, "", 0, NULL) ;
  SetBkColor (d->hDC, oldBackground) ;

  SendMessage (d->hwndItem, LB_GETTEXT, d->itemID, (LPARAM) str) ;

  s = strtok (str, "\t") ;
  rect = d->rcItem ;
  rect.left = message_xchar ;
  rect.right = d->rcItem.right - message_xchar * 46 ;
  ExtTextOut (d->hDC, rect.left, rect.top, ETO_CLIPPED, &rect, s, strlen (s), NULL) ;

  s = strtok (NULL, "\t") ;
  rect = d->rcItem ;
  rect.left = d->rcItem.right - message_xchar * 45 ;
  rect.right = d->rcItem.right - message_xchar * 26 ;
  ExtTextOut (d->hDC, rect.left, rect.top, ETO_CLIPPED, &rect, s, strlen (s), NULL) ;

  s = strtok (NULL, "\t") ;
  rect = d->rcItem ;
  rect.left = d->rcItem.right - message_xchar * 25 ;
  ExtTextOut (d->hDC, rect.left, rect.top, ETO_CLIPPED, &rect, s, strlen (s), NULL) ;

  SetTextColor (d->hDC, oldForeground) ;
  SetBkMode (d->hDC, oldMode) ;
  SelectObject (d->hDC, oldFont) ;
}

void draw_ordinary_listbox (DRAWITEMSTRUCT *d)
{
  int         oldMode ;
  int         dividerStep ;
  char        str [256] ;
  RECT        rect ;
  HPEN        hpen1 ;
  HPEN        hpen2 ;
  HPEN        hpenOld ;
  HFONT       oldFont ;
  COLORREF    oldBackground ;
  COLORREF    oldForeground ;

  if (d->itemID == -1)
    return ;
  hpen1 = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_BTNHIGHLIGHT)) ;
  hpen2 = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_BTNSHADOW)) ;
  hpenOld = SelectObject (d->hDC, hpen2) ;
  oldFont = SelectObject (d->hDC, message_font) ;
  oldMode = SetBkMode (d->hDC, TRANSPARENT) ;
  oldForeground = SetTextColor (d->hDC, GetSysColor (d->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT)) ;
  oldBackground = SetBkColor (d->hDC, GetSysColor (d->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT : COLOR_BTNFACE)) ;
  dividerStep = message_ychar / 2 - 1 ;

  rect = d->rcItem ;
  rect.left += message_xchar ;
  rect.right -= message_xchar ;
  SendMessage (d->hwndItem, LB_GETTEXT, d->itemID, (LPARAM) str) ;
  if (strncmp (str, "----------------", 16) == 0)
  {
    MoveToEx (d->hDC, rect.left, rect.top + message_ychar - dividerStep, NULL) ;
    SelectObject (d->hDC, hpen2) ;
    LineTo (d->hDC, rect.left, rect.top + dividerStep) ;
    LineTo (d->hDC, rect.right - message_xchar, rect.top + dividerStep) ;
    SelectObject (d->hDC, hpen1) ;
    LineTo (d->hDC, rect.right - message_xchar, rect.top + message_ychar - dividerStep) ;
    LineTo (d->hDC, rect.left, rect.top + message_ychar - dividerStep) ;
  }
  else
    ExtTextOut (d->hDC, rect.left, rect.top, ETO_CLIPPED | ETO_OPAQUE, &rect, str, strlen (str), NULL) ;

  SetBkColor (d->hDC, oldBackground) ;
  SetTextColor (d->hDC, oldForeground) ;
  SetBkMode (d->hDC, oldMode) ;
  SelectObject (d->hDC, oldFont) ;
  SelectObject (d->hDC, hpenOld) ;
  DeleteObject (hpen1) ;
  DeleteObject (hpen2) ;
}

void write_wrapped_text (HDC hdc, RECT *rect, char *text)
{
  int         oldMode ;
  HFONT       hFont ;
  HFONT       hFontOld ;
  LOGFONT     lf ;
  COLORREF    oldForeground ;

  memset (&lf, 0, sizeof (LOGFONT)) ;
  lf.lfHeight = -MulDiv (9, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf.lfWeight = FW_NORMAL ;
  lf.lfPitchAndFamily = VARIABLE_PITCH ;
  lf.lfCharSet = DEFAULT_CHARSET ;
  strcpy (lf.lfFaceName, "MS Sans Serif") ;
  if ((hFont = CreateFontIndirect (&lf)) != NULL)
  {
    hFontOld = SelectObject (hdc, hFont) ;
    oldMode = SetBkMode (hdc, TRANSPARENT) ;
    oldForeground = SetTextColor (hdc, RGB (0, 0, 0)) ;
    DrawText (hdc, text, -1, rect, DT_WORDBREAK | DT_EXPANDTABS) ;
    SetTextColor (hdc, oldForeground) ;
    SetBkMode (hdc, oldMode) ;
    SelectObject (hdc, hFontOld) ;
    DeleteObject (hFont) ;
  }
}

void tip_of_the_day (HDC hdc, RECT *rect, char *text)
{
  int         oldMode ;
  RECT        rc ;
  HFONT       hFont ;
  HFONT       hFontOld ;
  LOGFONT     lf ;
  COLORREF    oldForeground ;

  rc = *rect ;
  memset (&lf, 0, sizeof (LOGFONT)) ;
  lf.lfHeight = -MulDiv (9, GetDeviceCaps (hdc, LOGPIXELSY), 72) ;
  lf.lfWeight = FW_BOLD ;
  lf.lfPitchAndFamily = VARIABLE_PITCH ;
  lf.lfCharSet = DEFAULT_CHARSET ;
  strcpy (lf.lfFaceName, "MS Sans Serif") ;
  if ((hFont = CreateFontIndirect (&lf)) != NULL)
  {
//  hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_BULB)) ;
//  DrawIcon (hdc, rc.left, rc.top, hIcon) ;
//  DestroyIcon (hIcon) ;
    hFontOld = SelectObject (hdc, hFont) ;
    oldMode = SetBkMode (hdc, TRANSPARENT) ;
    oldForeground = SetTextColor (hdc, RGB (0, 0, 0)) ;
    ExtTextOut (hdc, rc.left, rc.top + 11, ETO_CLIPPED, &rc, "Did you know ... ? ", 19, NULL) ;
    rc.top += 36 ;
    SetTextColor (hdc, oldForeground) ;
    SetBkMode (hdc, oldMode) ;
    SelectObject (hdc, hFontOld) ;
    DeleteObject (hFont) ;
    write_wrapped_text (hdc, &rc, text) ;
  }
}

/*
** StatusBar support code. These functions support our own Statusbar drawing and so forth.
** The status bar itself is actually part of the window frame (in the non-client area), and
** is initialised by the non-client drawing code. therefore it isn't available if non-client
** drawing is turned off. oh, and the non-client code has already cleared the background into
** COLOR_BTNFACE so we don't need to do that, except when we're erasing old sections of the
** status bar.
**
** PS - yes, I know I could have used the Win32[s] statusbar common control. But I wanted to
** do this myself, for some reason.
*/

BOOL getStatusbarClient (HWND hWnd, RECT *rect, HFONT *font)
{
  return (pvncGetStatusbarClient (hWnd, rect, font)) ;
}

void verticalDivider (HDC hdc, RECT *rect, UINT offset)
{
  RECT        rc ;
  COLORREF    oldColour ;

  rc = *rect ;
  rc.left += offset + 1 ;
  rc.right = rc.left + 3 ;
  InflateRect (&rc, 0, 1) ;
  oldColour = SetBkColor (hdc, GetSysColor (COLOR_BTNFACE)) ;
  DRAWFASTRECT (hdc, &rc) ;
  SetBkColor (hdc, GetSysColor (COLOR_BTNHIGHLIGHT)) ;
  rc.right = rc.left + 1 ;
  OffsetRect (&rc, -1, 0) ;
  DRAWFASTRECT (hdc, &rc) ;
  OffsetRect (&rc, 4, 0) ;
  SetBkColor (hdc, GetSysColor (COLOR_BTNSHADOW)) ;
  DRAWFASTRECT (hdc, &rc) ;
  SetBkColor (hdc, oldColour) ;
}

void paint_statusbar_section (HDC hdc, RECT *rect, HFONT hFont, int nSection, BOOL erase, BOOL divider, BOOL text)
{
  HDC                   hdcMemory ;
  RECT                  rc ;
  SIZE                  size ;
  HRGN                  hRgn ;
  HBITMAP               oldBmp ;
  statusBarStruct       *sb = statusBar + nSection ;

  rc = *rect ;
  rc.left += sb->offset ;
  rc.right = rc.left + sb->width ;
  if (rc.left < rect->left)
    rc.left = rect->left ;
  if (sb->dividerOffset)
    verticalDivider (hdc, rect, sb->dividerOffset) ;

  /*
  ** check to see if we're painting a bitmap or if it's text
  */
  if (!text)
  {
    hdcMemory = CreateCompatibleDC (hdc) ;
    oldBmp = SelectObject (hdcMemory, sb->hBitmap) ;
    hRgn = CreateRectRgnIndirect (&rc) ;
    SelectClipRgn (hdc, hRgn) ;
    if (rc.bottom - rc.top > sb->bitBltHeight)
      rc.top += (rc.bottom - rc.top - sb->bitBltHeight) / 2 ;
    BitBlt (hdc, rect->left + sb->offset, rc.top, sb->bitBltWidth, sb->bitBltHeight, hdcMemory, sb->bitmapOffsetX, sb->bitmapOffsetY, SRCCOPY) ;
    SelectClipRgn (hdc, NULL) ;
    SelectObject (hdcMemory, oldBmp) ;
    DeleteObject (hRgn) ;
    DeleteDC (hdcMemory) ;
  }
  else
  {
    if (erase)
      DRAWFASTRECT (hdc, &rc) ;
    if (sb->isCentered)
    {
      GetTextExtentPoint (hdc, sb->text, sb->textLen, &size) ;
      rc.left += (sb->width - size.cx) / 2 ;
    }
    ExtTextOut (hdc, rc.left, rc.top, ETO_CLIPPED, &rc, sb->text, sb->textLen, NULL) ;
  }
}

void paint_statusbar (int nSection)
{
  int         i ;
  int         oldMode ;
  HDC         hdc ;
  RECT        rect ;
  HFONT       hFont ;
  HFONT       hFontOld ;
  COLORREF    oldForeground ;
  COLORREF    oldBackground ;

  if (IsIconic (main_window) || isMaxiMinimized)
    return ;
  if (getStatusbarClient (main_window, &rect, &hFont) == FALSE)
    return ;
  hdc = GetWindowDC (main_window) ;
  hFontOld = SelectObject (hdc, hFont) ;
  oldMode = SetBkMode (hdc, TRANSPARENT) ;
  oldForeground = SetTextColor (hdc, RGB (0, 0, 0)) ;
  oldBackground = SetBkColor (hdc, GetSysColor (COLOR_BTNFACE)) ;
  rect.top++ ;
  if (nSection == -1)
    DRAWFASTRECT (hdc, &rect) ;

  if (nSection == -1)
  {
    for (i = 0 ; i < StatusLast ; i++)
      paint_statusbar_section (hdc, &rect, hFont, i, FALSE, i > 0, statusBar [i].hasText) ;
  }
  else
    paint_statusbar_section (hdc, &rect, hFont, nSection, TRUE, nSection > 0, statusBar [nSection].hasText) ;

  SetTextColor (hdc, oldForeground) ;
  SetBkColor (hdc, oldBackground) ;
  SetBkMode (hdc, oldMode) ;
  SelectObject (hdc, hFontOld) ;
  ReleaseDC (main_window, hdc) ;
}

void calculate_statusbar (void)
{
  int                   i ;
  int                   position ;
  int                   width ;
  HDC                   hdc ;
  RECT                  rect ;
  HFONT                 hFont ;
  HFONT                 hFontOld ;
  TEXTMETRIC            tm ;
  statusBarStruct       *sb ;

  if (getStatusbarClient (main_window, &rect, &hFont))
  {
    hdc = GetWindowDC (main_window) ;
    hFontOld = SelectObject (hdc, hFont) ;
    GetTextMetrics (hdc, &tm) ;
    SelectObject (hdc, hFontOld) ;
    ReleaseDC (main_window, hdc) ;

    width = tm.tmAveCharWidth ;
    position = rect.right - rect.left ;

    for (i = StatusLast - 1, sb = statusBar + i ; i >= 0 ; i--, sb--)
    {
      sb->visible = position > 0 ;
      if (!sb->useWidth)
      {
        if (sb->chars > 0)
        {
          sb->width = sb->chars * width ;
          position -= sb->width + 2 ;
          sb->offset = position ;
          sb->dividerOffset = position -= 7 ;
          if (sb->dividerOffset < 0)
            sb->dividerOffset = 0 ;
          position -= 2 ;
        }
        else
        {
          sb->width = position - 2 ;
          sb->offset = 0 ;
          sb->dividerOffset = 0 ;
          break ;
        }
      }
      else
      {
        // compensate for -=2 of previous (if any)
        if (sb != statusBar + StatusLast - 1)
          position += 2 ;
        position -= sb->width ;
        sb->offset = position ;
        sb->dividerOffset = position -= 5 ;
        if (sb->dividerOffset < 0)
          sb->dividerOffset = 0 ;
      }
    }
  }
}

void initialise_statusbar (BOOL isMaxiMiniMode)
{
  if (!isMaxiMiniMode)
  {
    memset (statusBar, 0, sizeof (statusBar)) ;
    statusBar [StatusMessage].chars = 0 ;
    statusBar [StatusMessage].useWidth = FALSE ;
    statusBar [StatusMessage].hasText = TRUE ;
    statusBar [StatusMessage].isCentered = FALSE ;
    statusBar [StatusMessage].chars = 0 ;

#if 0
    hBmpRendering = LoadBitmap (hInstance, MAKEINTRESOURCE (BMP_RENDERING)) ;
    statusBar [StatusRendering].width = 64 ;
    statusBar [StatusRendering].useWidth = TRUE ;
    statusBar [StatusRendering].hasText = FALSE ;
    statusBar [StatusRendering].isCentered = FALSE ;
    statusBar [StatusRendering].hBitmap = hBmpRendering ;
    statusBar [StatusRendering].bitBltWidth = 64 ;
    statusBar [StatusRendering].bitBltHeight = 16 ;
#endif

    statusBar [StatusPPM].chars = 11 ;
    statusBar [StatusPPM].useWidth = FALSE ;
    statusBar [StatusPPM].hasText = TRUE ;
    statusBar [StatusPPM].isCentered = TRUE ;

    statusBar [StatusRendertime].chars = 19 ;
    statusBar [StatusRendertime].useWidth = FALSE ;
    statusBar [StatusRendertime].hasText = TRUE ;
    statusBar [StatusRendertime].isCentered = TRUE ;
  }
}

void say_status_message (int section, char *message)
{
  if (section >= StatusLast)
    return ;
  EnterCriticalSection (&critical_section) ;
  strncpy (statusBar [section].text, message, sizeof (statusBar [0].text)) ;
  statusBar [section].textLen = strlen (message) ;
  LeaveCriticalSection (&critical_section) ;
  paint_statusbar (section) ;
}

void handle_menu_select (WPARAM wParam, LPARAM lParam)
{
  char        str [128] ;
  static int  nLastID = -1 ;

  wParam = LOWORD (wParam) ;
  if (wParam != nLastID)
  {
    nLastID = wParam ;
    if (wParam >= CM_FIRSTGUIEXT && wParam <= CM_LASTGUIEXT)
    {
      say_status_message (StatusMessage, ExternalMenuTip (wParam)) ;
      return ;
    }
    if (wParam < CM_FIRSTTOOL || wParam > CM_LASTTOOL)
    {
      if (LoadString (hInstance, wParam, str, sizeof (str)) != 0)
        say_status_message (StatusMessage, str) ;
      else
        say_status_message (StatusMessage, "") ;
    }
    else
      say_status_message (StatusMessage, tool_help [wParam - CM_FIRSTTOOL]) ;
  }
}

HWND create_toolbar (HWND hwndParent)
{
  int         count = 0 ;
  int         i ;
  HWND        hwnd ;
  RECT        rect ;
  TBBUTTON    tbb [MAX_TOOLS] ;
  TBBUTTON    *t ;

  for (i = 0, t = tbb ; i < MAX_TOOLS ; i++, t++)
  {
    if (toolbar_ids [i] != 0)
    {
      t->idCommand = toolbar_ids [i] & 0x7fff ;
      t->fsState = TBSTATE_ENABLED ;
      // bug in Watcom 10.6 causes this to make the tabbed control's font go screwy. I don't know why.
//    t->fsStyle = ((toolbar_ids [i] & 0x8000) != 0) ? TBSTYLE_CHECK : TBSTYLE_BUTTON ;
      if ((toolbar_ids [i] & 0x8000) != 0)
        t->fsStyle = TBSTYLE_CHECK ;
      else
        t->fsStyle = TBSTYLE_BUTTON ;
      t->dwData = 0 ;
      t->iString = 0 ;
      t->iBitmap = count++ ;
    }
    else
    {
      t->iBitmap = 0 ;
      t->idCommand = 0 ;
      t->fsState = 0 ;
      t->fsStyle = TBSTYLE_SEP ;
      t->dwData = 0 ;
      t->iString = 0 ;
    }
  }

  hwnd = CreateToolbarEx (hwndParent,
                          WS_CHILD | TBSTYLE_TOOLTIPS,
                          IDC_TOOLBAR,
                          20,
                          hInstance,
                          BMP_TOOLBAR,
                          tbb,
                          MAX_TOOLS,
                          24,
                          24,
                          0,
                          0,
                          sizeof (TBBUTTON)) ;

  if (hwnd == NULL)
    return (NULL) ;
  if (use_toolbar)
    ShowWindow (hwnd, SW_SHOW) ;

  SendMessage (hwnd, TB_GETITEMRECT, MAX_TOOLS - 1, (LPARAM) &rect) ;
  toolbar_combobox = CreateWindow ("COMBOBOX",
                                   "",
                                   WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                                   rect.left,
                                   rect.top,
                                   LOWORD (GetDialogBaseUnits ()) * 25,
                                   HIWORD (GetDialogBaseUnits ()) * 15,
                                   hwnd,
                                   NULL,
                                   hInstance,
                                   NULL) ;
  return (hwnd) ;
}

HWND create_tabbed_window (HWND hwndParent)
{
  HWND        hwnd ;

  hwnd = CreateWindow (WC_TABCONTROL,
                       "",
                       WS_CHILD | WS_CLIPSIBLINGS |
                       WS_VISIBLE | TCS_FOCUSNEVER | TCS_MULTILINE,
                       0,
                       0,
                       0,
                       0,
                       hwndParent,
                       NULL,
                       hInstance,
                       NULL) ;
  return (hwnd) ;
}

void initialise_tabbed_window (HWND hwnd)
{
  HDC         hDC ;
  LOGFONT     lf ;

  hDC = GetDC (hwnd) ;
  lf.lfHeight = -MulDiv (8, GetDeviceCaps (hDC, LOGPIXELSY), 72) ;
  ReleaseDC (hwnd, hDC) ;
  lf.lfWeight = FW_NORMAL ;
  lf.lfPitchAndFamily = VARIABLE_PITCH ;
  lf.lfCharSet = DEFAULT_CHARSET ;
  strcpy (lf.lfFaceName, "MS Sans Serif") ;
  if ((tab_font = CreateFontIndirect (&lf)) != NULL)
    SendMessage (hwnd, WM_SETFONT, (WPARAM) tab_font, FALSE) ;
}

unsigned add_window_to_tab (HWND hwnd, void *editor, char *s)
{
  TC_ITEM     tab ;

  if (window_count >= MAX_WINDOWS)
    return (1) ;
  window_list [window_count] = hwnd ;
  tab.mask = TCIF_TEXT | TCIF_PARAM ;
  tab.pszText = s ;
  tab.lParam = (LPARAM) editor ;
  TabCtrl_InsertItem (tabbed_window, window_count++, &tab) ;
  return (0) ;
}

unsigned get_tab_index (HWND hwnd, void *editor)
{
  int         i ;
  int         count ;
  TC_ITEM     tab ;

  count = TabCtrl_GetItemCount (hwnd) ;
  for (i = 0 ; i < count ; i++)
  {
    tab.mask = TCIF_PARAM ;
    TabCtrl_GetItem (tabbed_window, i, &tab) ;
    if (tab.lParam == (LPARAM) editor)
      return (i) ;
  }
  return (0) ;
}

void resize_windows (unsigned left, unsigned top, unsigned width, unsigned height)
{
  MoveWindow (message_window, left, top, width, height, FALSE) ;
  MoveWindow (editor_window, left, top, width, height, FALSE) ;
}

FILE *editor_stream_init (void)
{
  editor_line_number = 0 ;
  editor_char_pos = 0 ;
  editor_offset = 0 ;
  seek_entry_count = 0 ;
  line_buffer [0] = '\0' ;
  return (POV_INTERNAL_STREAM) ;
}

int editor_ftell (void)
{
  if (seek_entry_count >= MAX_SEEK_INDEX)
  {
    PovMessageBox ("Seek index overflow ; render scene from outside editor", "Error") ;
    quit = TRUE ;
    return (-1) ;
  }
  if (editor_char_pos)
  {
    seek_entries [seek_entry_count].Line = editor_line_number ;
    seek_entries [seek_entry_count++].Offset = editor_char_pos - 1 ;
  }
  seek_entries [seek_entry_count].Line = editor_line_number ;
  seek_entries [seek_entry_count].Offset = editor_char_pos ;
  return (seek_entry_count++) ;
}

int editor_fseek (long offset, int whence)
{
  if (whence != 0)
    return (-1) ;
  editor_line_number = seek_entries [offset].Line ;
  editor_char_pos = seek_entries [offset].Offset ;
  if (Edit.GetLine (CurrentEditor, editor_line_number, line_buffer, sizeof (line_buffer) - 1) == FALSE)
  {
    PovMessageBox ("Seek index error ; render scene from outside editor", "Error") ;
    quit = TRUE ;
    return (-1) ;
  }
  return (0) ;
}

int editor_getc (void)
{
  if (line_buffer [editor_char_pos] == '\0')
  {
    if (Edit.GetLine (CurrentEditor, ++editor_line_number, line_buffer, sizeof (line_buffer) - 2) == FALSE)
      return (EOF) ;
    editor_char_pos = 0 ;
    strcat (line_buffer, "\n") ;
  }
  return (line_buffer [editor_char_pos++]) ;
}

FILE *pov_fopen (const char *filename, const char *mode)
{
  if (filename == NULL)
    return (editor_stream_init ()) ;
  else
    return (fopen (filename, mode)) ;
}

int pov_fclose (FILE *stream)
{
  if (stream != POV_INTERNAL_STREAM)
    return (fclose (stream)) ;
  else
    return (0) ;
}

int pov_getc (FILE *stream)
{
  return (stream == POV_INTERNAL_STREAM ? editor_getc () : getc (stream)) ;
}

int pov_fseek (FILE *stream, long offset, int whence)
{
  return (stream == POV_INTERNAL_STREAM ? editor_fseek (offset, whence) : fseek (stream, offset, whence)) ;
}

int pov_ftell (FILE *stream)
{
  return (stream == POV_INTERNAL_STREAM ? editor_ftell () : ftell (stream)) ;
}

void add_edit_file (char *file)
{
  int         count ;
  char        str [16] ;
  static BOOL first = TRUE ;

  if (strlen (file) == 0)
  {
    PovMessageBox ("Empty filename after /EDIT", "Edit File") ;
    return ;
  }
  count = GetPrivateProfileInt ("Editor", "OpenCount", 0, EngineIniFileName) ;
  if (count == 16)
  {
    PovMessageBox ("Too many edit files would be open", file) ;
    return ;
  }
  sprintf (str, "Editor%d", ++count) ;
  WritePrivateProfileString (str, "FileName", file, EngineIniFileName) ;
  PutPrivateProfileInt (str, "TopLine", 1, EngineIniFileName) ;
  PutPrivateProfileInt (str, "Line", 1, EngineIniFileName) ;
  PutPrivateProfileInt (str, "Column", 1, EngineIniFileName) ;
  PutPrivateProfileInt ("Editor", "OpenCount", count, EngineIniFileName) ;
  if (first)
  {
    PutPrivateProfileInt ("MainWindow", "CurrentTab", count, EngineIniFileName) ;
    first = FALSE ;
  }
}

void add_render_file (char *file)
{
  static BOOL first = TRUE ;

  if (strlen (file) == 0)
  {
    PovMessageBox ("Empty filename after /EDIT", "Edit File") ;
    return ;
  }
  if (!first)
  {
    PovMessageBox ("Only one /RENDER file may be specified", "Render File") ;
    return ;
  }
  first = FALSE ;
  strcpy (requested_render_file, file) ;
  render_requested++ ;
}

char *extract_file (char *filename, char *s)
{
  BOOL        inQuote = FALSE ;

  while (*s == ' ' || *s == '\t')
    s++ ;
  while (*s)
  {
    switch (*s)
    {
      case '"' :
           if (inQuote)
           {
             *filename = '\0' ;
             return (++s) ;
           }
           inQuote = TRUE ;
           break ;

      case ' ' :
      case '\t' :
           if (!inQuote)
           {
             *filename  = '\0' ;
             return (s) ;
           }
           *filename++ = *s ;
           break ;

      default :
           *filename++ = *s ;
           break ;
    }
    s++ ;
  }
  *filename  = '\0' ;
  return (s) ;
}

char *preparse_commandline (char *s)
{
  char        *out ;
  char        *command ;
  char        last = ' ' ;
  char        commandstr [256] ;
  char        filename [_MAX_PATH] ;
  static char outstr [_MAX_PATH * 3] ;

  out = outstr ;
  while (*s)
  {
    if (*s == '/' && (last == ' ' || last == '\t'))
    {
      command = commandstr ;
      while (*++s)
      {
        if (*s == ' ' || *s == '\t')
          break ;
        *command++ = *s ;
      }
      *command = '\0' ;
      last = *s ;
      if (strlen (commandstr) == 0)
      {
        PovMessageBox ("Empty command on commandline", "Commandline processing error") ;
        return (NULL) ;
      }
      if (stricmp (commandstr, "EXIT") == 0)
      {
        exit_after_render++ ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (stricmp (commandstr, "DEMO") == 0)
      {
        demo_mode++ ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (stricmp (commandstr, "DEBUG") == 0)
      {
        debugging++ ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (stricmp (commandstr, "EDIT") == 0)
      {
        s = extract_file (filename, s) ;
        add_edit_file (filename) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (stricmp (commandstr, "RENDER") == 0)
      {
        s = extract_file (filename, s) ;
        add_render_file (filename) ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      if (stricmp (commandstr, "NOEXEC") == 0)
      {
        noexec++ ;
        while (*s == ' ')
          s++ ;
        continue ;
      }
      sprintf (outstr, "Unrecognised command '%s' on commandline", commandstr) ;
      PovMessageBox (outstr, "Commandline processing error") ;
      return (NULL) ;
    }
    last = *out++ = *s++ ;
  }
  return (outstr) ;
}


