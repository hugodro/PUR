/****************************************************************************
*                pvfiles.c
*
*  This module contains ASCII file data and related code.
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
#include "optout.h"
#pragma hdrstop

#include <io.h>
#include "pvengine.h"
#include "pvengine.rh"

#include "pvlegal.h"
#include "pvdemo.h"
#include "pvtips.h"

extern int              message_ychar ;
extern char             ourPath [] ;
extern char             HomePath [] ;

void fill_listbox (HWND hwnd)
{
  char        **p = povlegal_text ;

  while (*p)
    SendMessage (hwnd, LB_ADDSTRING, 0, (LPARAM) *p++) ;
}

void save_povlegal (void)
{
  char        **p = povlegal_text ;
  char        filename [_MAX_PATH + 64] ;
  FILE        *outF ;

  sprintf (filename, "%sPOVLEGAL.DOC", HomePath) ;
  if ((outF = fopen (filename, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create POVLEGAL.DOC", "Cannot save document") ;
    return ;
  }
  while (*p)
  {
    if (fprintf (outF, "%s\n", *p++) == EOF)
    {
      PovMessageBox ("Cannot write to POVLEGAL.DOC", "Cannot save document") ;
      fclose (outF) ;
      return ;
    }
  }
  fclose (outF) ;
  sprintf (filename, "POVLEGAL.DOC was written to the file %sPOVLEGAL.DOC", HomePath) ;
  PovMessageBox (filename, "POVLEGAL.DOC saved") ;
}

BOOL FAR PASCAL PovLegalDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  DRAWITEMSTRUCT        *d ;
  MEASUREITEMSTRUCT     *m ;
  static HBRUSH         hbr ;

  switch (message)
  {
    case WM_CTLCOLORLISTBOX :
         return ((BOOL) hbr) ;

    case WM_INITDIALOG :
         resize_listbox_dialog (hDlg, IDC_LISTBOX, 79) ;
         CenterWindowRelative ((HWND) lParam, hDlg, TRUE) ;
         SetWindowText (hDlg, "POV-Ray Legal Statement [POVLEGAL.DOC]") ;
         hbr = CreateSolidBrush (GetSysColor (COLOR_BTNFACE)) ;
         fill_listbox (GetDlgItem (hDlg, IDC_LISTBOX)) ;
         return (TRUE) ;

    case WM_COMMAND :
         switch (LOWORD (wParam))
         {
           case IDOK :
                DeleteObject (hbr) ;
                EndDialog (hDlg, TRUE) ;
                return (TRUE) ;

           default :
                return (TRUE) ;
         }

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

char *save_demo_file (char *s1, char *s2)
{
  char        **p = povdemo_scene ;
  FILE        *outF ;

  GetTempPath (_MAX_PATH - 16, s1) ;
  if (s1 [strlen (s1) - 1] != '\\')
    strcat (s1, "\\") ;
  strcpy (s2, s1) ;
  strcat (s1, "POVDEMO.$$1") ;
  strcat (s2, "POVDEMO.$$2") ;
  if ((outF = fopen (s1, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    return (NULL) ;
  }
  while (*p)
  {
    if (fprintf (outF, "%s\n", *p++) == EOF)
    {
      PovMessageBox ("Cannot write to temporary file", "Cannot run demo") ;
      fclose (outF) ;
      unlink (s1) ;
      return (NULL) ;
    }
  }
  fclose (outF) ;
  p = povdemo_ini ;
  if ((outF = fopen (s2, "wt")) == NULL)
  {
    PovMessageBox ("Cannot create temporary file", "Cannot run demo") ;
    unlink (s1) ;
    return (NULL) ;
  }
  while (*p)
  {
    if (fprintf (outF, "%s\n", *p++) == EOF)
    {
      PovMessageBox ("Cannot write to temporary file", "Cannot run demo") ;
      fclose (outF) ;
      unlink (s1) ;
      unlink (s2) ;
      return (NULL) ;
    }
  }
  fclose (outF) ;
  return (s1) ;
}


