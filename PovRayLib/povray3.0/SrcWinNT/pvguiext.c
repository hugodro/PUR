/****************************************************************************
*                pvguiext.c
*
*  This file contains POV-ray for Windows GUI Extension support code.
*
*  Copyright © POV-Team 1996-1997. All Rights Reserved.
*  This windows version of POV-Ray is Copyright 1996-1997 Christopher J. Cason.
*  Author : Christopher J. Cason
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
#include "userio.h"
#pragma hdrstop

#include <commctrl.h>
#include "optin.h"
#include "pvengine.h"
#include "pvengine.rh"
#include "pvclient.h"
#include "pvdialog.h"
#include "pvguiext.h"

typedef struct _RGPB
{
  DWORD                 Signature ;
  DWORD                 InstanceID ;
  BOOL                  (WINAPI *Init) (DWORD InstanceID, int RecSize, GuiExtInitStruct *InitStruct) ;
  void                  (WINAPI *Destroy) (IDataStruct *InstanceData) ;
  DWORD                 (WINAPI *MenuSelect) (IDataStruct *InstanceData, WPARAM Code) ;
  LPSTR                 (WINAPI *MenuTip) (IDataStruct *InstanceData, WPARAM Code) ;
  DWORD                 (WINAPI *Event) (IDataStruct *InstanceData, ExternalEvents Event, DWORD EventVal) ;
  void                  (WINAPI *DisplayPlot) (IDataStruct *InstanceData, int x, int y, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *DisplayPlotRect) (IDataStruct *InstanceData, int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
  void                  (WINAPI *WinPrePixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  void                  (WINAPI *WinPostPixel) (IDataStruct *InstanceData, int x, int y, COLOUR colour) ;
  BOOL                  (WINAPI *WinSystem) (IDataStruct *InstanceData, LPSTR command, int *returnval) ;
  void                  (WINAPI *CleanupAll) (IDataStruct *InstanceData) ;
  void                  (WINAPI *BufferMessage) (IDataStruct *InstanceData, msgtype message_type, LPSTR message) ;
  LPSTR                 (WINAPI *ParseToolCommand) (IDataStruct *InstanceData, char command [512]) ;
  BOOL                  (WINAPI *DragFunction) (IDataStruct *InstanceData, LPSTR szFile, ExternalDropType DropType) ;
  struct _RGPB          *Next ;
  int                   IniID ;
  IDataStruct           InstanceData ;
  char                  Name [256] ;
  char                  Author [256] ;
  char                  AuthorEmail [256] ;
  char                  FileName [_MAX_PATH] ;
} RealGuiPointerBlock ;

RealGuiPointerBlock               *GuiPointerBlocks = NULL ;

DWORD (WINAPI *GuiExtGetPointers) (int RecSize, GuiPointerBlock *PointerBlock) ;

extern char                       EngineIniFileName [_MAX_PATH] ;
extern char                       source_file_name [_MAX_PATH] ;
extern FILE                       *debugFile ;
extern BOOL                       ExtensionsEnabled ;
extern BOOL                       rendering ;
extern BOOL                       stop_rendering ;
extern BOOL                       IsWin32 ;
extern HWND                       main_window ;
extern HMENU                      hPluginsMenu ;
extern HMENU                      hPluginsMenuNE ;
extern HINSTANCE                  hInstance ;

void getvars (ExternalVarStruct *v) ;
void setvars (ExternalVarStruct *v) ;

DWORD ExternalEvent (ExternalEvents Event, DWORD EventVal)
{
  DWORD                 rval = 0 ;
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (0) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->Event != NULL)
      rval |= (*p->Event ) (&p->InstanceData, Event, EventVal) ;
    p = p->Next ;
  }
  return (rval) ;
}

void ExternalDisplayPlot (int x, int y, int Red, int Green, int Blue, int Alpha)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->DisplayPlot != NULL)
      (*p->DisplayPlot) (&p->InstanceData, x, y, Red, Green, Blue, Alpha) ;
    p = p->Next ;
  }
}

void ExternalDisplayPlotRect (int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->DisplayPlotRect != NULL)
      (*p->DisplayPlotRect) (&p->InstanceData, x1, y1, x2, y2, Red, Green, Blue, Alpha) ;
    p = p->Next ;
  }
}

void ExternalWinPrePixel (int x, int y, COLOUR colour)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->WinPrePixel != NULL)
      (*p->WinPrePixel) (&p->InstanceData, x, y, colour) ;
    p = p->Next ;
  }
}

void ExternalWinPostPixel (int x, int y, COLOUR colour)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->WinPostPixel != NULL)
      (*p->WinPostPixel) (&p->InstanceData, x, y, colour) ;
    p = p->Next ;
  }
}

BOOL ExternalWinSystem (LPSTR command, int *returnval)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (FALSE) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->WinSystem != NULL)
      if ((*p->WinSystem) (&p->InstanceData, command, returnval))
        return (TRUE) ;
    p = p->Next ;
  }
  return (FALSE) ;
}

void ExternalCleanupAll (void)
{
  // always enabled
  while (GuiPointerBlocks)
  {
    if (GuiPointerBlocks->Destroy != NULL)
      (*GuiPointerBlocks->Destroy) (&GuiPointerBlocks->InstanceData) ;
    GuiPointerBlocks = GuiPointerBlocks->Next ;
  }
}

void ExternalBufferMessage (msgtype message_type, LPSTR message)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->BufferMessage != NULL)
      (*p->BufferMessage) (&p->InstanceData, message_type, message) ;
    p = p->Next ;
  }
}

void ExternalParseToolCommand (char command [512])
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->ParseToolCommand != NULL)
      (*p->ParseToolCommand) (&p->InstanceData, command) ;
    p = p->Next ;
  }
}

BOOL ExternalDragFunction (LPSTR szFile, ExternalDropType DropType)
{
  int                   n ;
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (FALSE) ;
  n = get_file_type (szFile) ;
  if (n == filePOV || n == fileINI)
    return (FALSE) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (p->DragFunction != NULL)
      if ((*p->DragFunction) (&p->InstanceData, szFile, DropType))
        return (TRUE) ;
    p = p->Next ;
  }
  return (FALSE) ;
}

ExternalRequestResult WINAPI ExternalRequest (ExternalRequests Request, void *RequestBlock)
{
  if (!ExtensionsEnabled)
    return (ExRequestDisabled) ;
  switch (Request)
  {
    case RequestGetVars :
         if (((ExternalVarStruct *) RequestBlock)->RecSize != sizeof (ExternalVarStruct))
           return (ExRequestBadRecSize) ;
         getvars ((ExternalVarStruct *) RequestBlock) ;
         return (ExRequestOK) ;

    case RequestSetVars :
         if (((ExternalVarStruct *) RequestBlock)->RecSize != sizeof (ExternalVarStruct))
           return (ExRequestBadRecSize) ;
         setvars ((ExternalVarStruct *) RequestBlock) ;
         return (ExRequestOK) ;

    case RequestStartRendering :
         if (rendering)
           return (ExRequestFailedRendering) ;
         message_printf ("Rendering started by GUI extension\n") ;
         start_rendering (FALSE, FALSE) ;
         return (ExRequestOK) ;

    case RequestStopRendering :
         if (!rendering)
           return (ExRequestFailedNotRendering) ;
         stop_rendering = TRUE ;
         message_printf ("Rendering stopped by GUI extension\n") ;
         return (ExRequestOK) ;

    case RequestExit :
         if (rendering)
           return (ExRequestFailedRendering) ;
         message_printf ("Exit requested by GUI extension\n") ;
         if (IsWin32)
           Sleep (1000) ;
         SendMessage (main_window, WM_COMMAND, CM_FILEEXIT, 0L) ;
         return (ExRequestOK) ;
  }
  return (ExRequestUnknown) ;
}

DWORD ExternalMenuSelect (WPARAM Code)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return (1) ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (Code >= p->InstanceData.FirstMenuItem && Code <= p->InstanceData.FirstMenuItem + 59)
      return ((*p->MenuSelect) (&p->InstanceData, Code - p->InstanceData.FirstMenuItem)) ;
    p = p->Next ;
  }
  return (1) ;
}

char *ExternalMenuTip (WPARAM wParam)
{
  RealGuiPointerBlock   *p ;

  if (!ExtensionsEnabled)
    return ("Extensions Disabled") ;
  p = GuiPointerBlocks ;
  while (p)
  {
    if (wParam >= p->InstanceData.FirstMenuItem && wParam <= p->InstanceData.FirstMenuItem + 59)
      return ((*p->MenuTip) (&p->InstanceData, wParam - p->InstanceData.FirstMenuItem)) ;
    p = p->Next ;
  }
  return ("") ;
}

BOOL ValidateMenu (HMENU hMenu, int first, int last)
{
  int         count ;
  int         i ;
  int         id ;
  HMENU       hSubMenu ;

  count = GetMenuItemCount (hMenu) ;
  for (i = 0 ; i < count ; i++)
  {
    if ((id = GetMenuItemID (hMenu, i)) == 0xffffffff)
    {
      // could be a popup menu
      if ((hSubMenu = GetSubMenu (hMenu, i)) == NULL)
        continue ;
      if (!ValidateMenu (hSubMenu, first, last))
        return (FALSE) ;
      continue ;
    }
    if (id == 0xffff)
      continue ;
    if (id < first || id > last)
      return (FALSE) ;
  }
  return (TRUE) ;
}

void LoadGUIExtensions (void)
{
  int                   i ;
  int                   FirstMenuItem = CM_FIRSTGUIEXT ;
  char                  str [64] ;
  char                  filename [_MAX_PATH] ;
  char                  name [_MAX_PATH] ;
  BOOL                  first = TRUE ;
  BOOL                  loaded = FALSE ;
  HINSTANCE             hLib ;
  GuiPointerBlock       PointerBlock ;
  GuiExtInitStruct      InitStruct ;
  RealGuiPointerBlock   *RealPointerBlock ;
  RealGuiPointerBlock   *LastPointerBlock = NULL ;

  if (!ExtensionsEnabled)
    return ;
  for (i = 0 ; i < MAX_GUI_EXT ; i++)
  {
    sprintf (str, "ExtDll%02d", i) ;
    GetPrivateProfileString ("GuiExtensions", str, "", filename, sizeof (filename) - 1, EngineIniFileName) ;
    if (filename [0] != '\0')
    {
      if (first)
      {
        first = FALSE ;
        buffer_message (mDivider, "\n") ;
        message_printf ("DISCLAIMER : The POV-Team takes no responsibility for the operation of third-party GUI Extensions.\n") ;
        message_printf ("             DO NOT send us bug reports if they cause things to go haywire.\n\n") ;
      }

      splitpath (filename, NULL, name) ;
      if ((hLib = LoadLibrary (filename)) == NULL)
      {
        if (debugFile)
          fprintf (debugFile, "Could not load GUI Extension DLL '%s', error code is %08lx\n", filename, GetLastError ()) ;
        message_printf ("Could not load GUI Extension DLL '%s'\n", name) ;
        continue ;
      }
      GuiExtGetPointers = (void *) GetProcAddress (hLib, "PovGuiExtensionGetPointers") ;
      if (GuiExtGetPointers == NULL)
      {
        if (debugFile)
          fprintf (debugFile, "Could not get Init process address for GUI extension '%s', error code is %08lx\n", filename, GetLastError ()) ;
        message_printf ("Could not get Init address for GUI Extension DLL '%s'\n", name) ;
        message_printf ("  Contact author of '%s'\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      memset (&PointerBlock, 0, sizeof (PointerBlock)) ;
      if (GuiExtGetPointers (sizeof (PointerBlock), &PointerBlock) == 0)
      {
        if (debugFile)
          fprintf (debugFile, "Could not get pointers for GUI extension '%s'\n", filename) ;
        message_printf ("Could not get pointers for GUI extension '%s'\n", name) ;
        message_printf ("  Contact author of '%s'\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (PointerBlock.Init == NULL ||
          PointerBlock.MenuSelect == NULL ||
          PointerBlock.MenuTip == NULL ||
          (PointerBlock.Signature != 'CJC!' && PointerBlock.Signature != '!CJC'))
      {
        if (debugFile)
          fprintf (debugFile, "PointerBlock not correctly initialised by GUI extension '%s'\n", filename) ;
        message_printf ("PointerBlock not correctly initialised by GUI extension '%s'\n", name) ;
        message_printf ("  Contact author of '%s'\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      memset (&InitStruct, 0, sizeof (InitStruct)) ;
      InitStruct.PovVersion = "" ;
      InitStruct.GuiVersion = "" ;
      InitStruct.GuiInterfaceVersion = GUI_INTERFACE_VERSION ;
      InitStruct.hInst = hInstance ;
      InitStruct.MainWindow = main_window ;
      InitStruct.ExternalRequest = ExternalRequest ;
      InitStruct.FirstMenuItem = FirstMenuItem ;
      if ((*PointerBlock.Init) (PointerBlock.InstanceID, sizeof (InitStruct), &InitStruct) == 0)
      {
        if (debugFile)
          fprintf (debugFile, "Init () failed (return code == 0) for GUI extension '%s'\n", filename) ;
        message_printf ("Init () failed (return code == 0) for GUI extension '%s'\n", name) ;
        message_printf ("  Contact %s <%s>\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (InitStruct.Name == NULL || strlen (InitStruct.Name) < 8 ||
          InitStruct.Author == NULL || strlen (InitStruct.Author) < 8 ||
          InitStruct.AuthorEmail == NULL || strlen (InitStruct.AuthorEmail) < 8 ||
          strncmp (InitStruct.Author, "Place ", 6) == 0 ||
          strncmp (InitStruct.AuthorEmail, "Place ", 6) == 0)
      {
        if (debugFile)
          fprintf (debugFile, "GUI extension '%s' has invalid name/author/email\n", filename) ;
        message_printf ("GUI extension '%s' has invalid name/author/email. Contact author of add-in.\n", name) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (InitStruct.DLLInterfaceVersion / 100 != GUI_INTERFACE_VERSION / 100)
      {
        if (debugFile)
          fprintf (debugFile, "GUI extension '%s' ('%s') is wrong version (%d)\n", filename, InitStruct.Name, InitStruct.DLLInterfaceVersion) ;
        message_printf ("GUI extension '%s' ('%s') is wrong version (%d)\n", name, InitStruct.Name, InitStruct.DLLInterfaceVersion) ;
        message_printf ("  Contact %s <%s>\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (strcmp (InitStruct.Agreement, "The author of this POV-Ray(tm) GUI Extension DLL certifies that, at the time of "
                                        "its production or distribution, it complied with the POV-Ray Team's then current "
                                        "requirements for GUI Extensions to POV-Ray for Windows, and acknowledges that it "
                                        "is a violation of copyright to fail to do so. This text is copyright (c) the "
                                        "POV-Team 1996. Used by permission.") != 0)
      {
        if (debugFile)
          fprintf (debugFile, "GUI extension '%s' ('%s') has invalid agreement\n  '%s'", filename, InitStruct.Name, InitStruct.Agreement) ;
        message_printf ("GUI extension '%s' ('%s') has invalid agreement\n", name, InitStruct.Name) ;
        message_printf ("  Contact %s at %s\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (InitStruct.hMenu == NULL || !IsMenu (InitStruct.hMenu))
      {
        if (debugFile)
          fprintf (debugFile, "GUI extension '%s' ('%s') has no menu\n", filename, InitStruct.Name) ;
        message_printf ("GUI extension '%s' ('%s') has no menu\n", name, InitStruct.Name) ;
        message_printf ("  Contact %s at %s\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if (!ValidateMenu (InitStruct.hMenu, FirstMenuItem, FirstMenuItem + 59))
      {
        if (debugFile)
          fprintf (debugFile, "GUI extension '%s' ('%s') has invalid menu ID's\n", filename, InitStruct.Name) ;
        message_printf ("GUI extension '%s' ('%s') has invalid menu ID's\n", name, InitStruct.Name) ;
        message_printf ("  Contact %s at %s\n", InitStruct.Author, InitStruct.AuthorEmail) ;
        FreeLibrary (hLib) ;
        continue ;
      }
      if ((RealPointerBlock = calloc (sizeof (RealGuiPointerBlock), 1)) == NULL)
      {
        message_printf ("Memory allocation error\n") ;
        FreeLibrary (hLib) ;
        continue ;
      }
      memcpy (RealPointerBlock, &PointerBlock, sizeof (PointerBlock) - sizeof (PointerBlock.Reserved)) ;
      strncpy (RealPointerBlock->Name, InitStruct.Name, sizeof (RealPointerBlock->Name)) ;
      strncpy (RealPointerBlock->Author, InitStruct.Author, sizeof (RealPointerBlock->Author)) ;
      strncpy (RealPointerBlock->AuthorEmail, InitStruct.AuthorEmail, sizeof (RealPointerBlock->AuthorEmail)) ;
      strncpy (RealPointerBlock->FileName, filename, sizeof (RealPointerBlock->FileName)) ;
      RealPointerBlock->InstanceData.FirstMenuItem = FirstMenuItem ;
      RealPointerBlock->InstanceData.InstanceID = PointerBlock.InstanceID ;
      RealPointerBlock->InstanceData.hMenu = InitStruct.hMenu ;
      RealPointerBlock->IniID = i ;
      if (!loaded)
      {
        DeleteMenu (hPluginsMenu, 1, MF_BYPOSITION) ;
        DeleteMenu (hPluginsMenuNE, 1, MF_BYPOSITION) ;
      }
      AppendMenu (hPluginsMenu, MF_POPUP, (UINT) InitStruct.hMenu, InitStruct.Name) ;
      AppendMenu (hPluginsMenuNE, MF_POPUP, (UINT) InitStruct.hMenu, InitStruct.Name) ;
      FirstMenuItem += 64 ;
      message_printf ("GUI Extension '%s' loaded from '%s'\n", InitStruct.Name, name) ;
      message_printf ("  Report any problems to %s <%s>\n", InitStruct.Author, InitStruct.AuthorEmail) ;
      if (LastPointerBlock != NULL)
      {
        LastPointerBlock->Next = RealPointerBlock ;
        LastPointerBlock = RealPointerBlock ;
      }
      else
        LastPointerBlock = GuiPointerBlocks = RealPointerBlock ;
      loaded = TRUE ;
    }
  }
  if (!first)
    buffer_message (mDivider, "\n") ;
  if (debugFile && loaded)
  {
    RealPointerBlock = GuiPointerBlocks ;
    fprintf (debugFile, "\nGUI extension summary:\n") ;
    while (RealPointerBlock)
    {
      fprintf (debugFile, "\nExtension DLL '%s'\n\n", RealPointerBlock->FileName) ;
      fprintf (debugFile, "  Name             : '%s'\n", RealPointerBlock->Name) ;
      fprintf (debugFile, "  Author           : '%s'\n", RealPointerBlock->Author) ;
      fprintf (debugFile, "  AuthorEmail      : '%s'\n", RealPointerBlock->AuthorEmail) ;
      fprintf (debugFile, "  IniID            : %d\n", RealPointerBlock->IniID) ;
      fprintf (debugFile, "  Init             : %08lX\n", RealPointerBlock->Init) ;
      fprintf (debugFile, "  Destroy          : %08lX\n", RealPointerBlock->Destroy) ;
      fprintf (debugFile, "  MenuSelect       : %08lX\n", RealPointerBlock->MenuSelect) ;
      fprintf (debugFile, "  Event            : %08lX\n", RealPointerBlock->Event) ;
      fprintf (debugFile, "  DisplayPlot      : %08lX\n", RealPointerBlock->DisplayPlot) ;
      fprintf (debugFile, "  DisplayPlotRect  : %08lX\n", RealPointerBlock->DisplayPlotRect) ;
      fprintf (debugFile, "  WinPrePixel      : %08lX\n", RealPointerBlock->WinPrePixel) ;
      fprintf (debugFile, "  WinPostPixel     : %08lX\n", RealPointerBlock->WinPostPixel) ;
      fprintf (debugFile, "  WinSystem        : %08lX\n", RealPointerBlock->WinSystem) ;
      fprintf (debugFile, "  CleanupAll       : %08lX\n", RealPointerBlock->CleanupAll) ;
      fprintf (debugFile, "  BufferMessage    : %08lX\n", RealPointerBlock->BufferMessage) ;
      fprintf (debugFile, "  ParseToolCommand : %08lX\n", RealPointerBlock->ParseToolCommand) ;
      fprintf (debugFile, "  DragFunction     : %08lX\n", RealPointerBlock->DragFunction) ;
      fprintf (debugFile, "  FirstMenuItem    : %08lX\n", RealPointerBlock->InstanceData.FirstMenuItem) ;
      fprintf (debugFile, "  InstanceID       : %08lX\n", RealPointerBlock->InstanceData.InstanceID) ;
      fprintf (debugFile, "  hMenu            : %08lX\n", RealPointerBlock->InstanceData.hMenu) ;
      RealPointerBlock = RealPointerBlock->Next ;
    }
    fprintf (debugFile, "\n") ;
  }
}
