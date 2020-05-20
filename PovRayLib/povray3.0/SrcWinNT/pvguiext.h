/****************************************************************************
*                pvguiext.h
*
*  This file contains POV-ray for Windows GUI Extension specific defines.
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

#ifndef PVGUIEXT_H_INCLUDED
#define PVGUIEXT_H_INCLUDED

#define MAX_GUI_EXT               32
#define GUI_INTERFACE_VERSION     100

#define CONTACT "Do not contact the POV-Team about this error. Contact the author of this extension - \n\n\t" AUTHOR " at " EMAIL

#ifndef POVWIN_FILE

#define MAX_QUEUE                 128

typedef enum
{
  All = 1,
  mBanner,
  mWarning,
  mRender,
  mStatus,
  mDebug,
  mFatal,
  mStatistics,
  mDivider,
  mHorzLine,
} msgtype ;

#endif

typedef struct
{
  int                   RecSize ;
  char                  command_line [_MAX_PATH * 3] ;
  char                  source_file_name [_MAX_PATH] ;
  char                  lastRenderName [_MAX_PATH] ;
  char                  lastRenderPath [_MAX_PATH] ;
  char                  lastQueuePath [_MAX_PATH] ;
  char                  lastSecondaryIniFilePath [_MAX_PATH] ;
  char                  DefaultRenderIniFileName [_MAX_PATH] ;
  char                  SecondaryRenderIniFileName [_MAX_PATH] ;
  char                  SecondaryRenderIniFileSection [64] ;
  char                  ourPath [_MAX_PATH] ;
  char                  engineHelpPath [_MAX_PATH] ;
  char                  rendererHelpPath [_MAX_PATH] ;
  char                  HomePath [_MAX_PATH] ;
  char                  EngineIniFileName [_MAX_PATH] ;
  char                  ToolIniFileName [_MAX_PATH] ;
  unsigned              loadRerun ;
  unsigned              continueRerun ;
  unsigned              povray_return_code ;
  BOOL                  rendering ;
  BOOL                  IsWin32 ;
  BOOL                  IsW95UserInterface ;
  BOOL                  running_demo ;
  BOOL                  debugging ;
  BOOL                  isMaxiMinimized ;
  BOOL                  newVersion ;
  BOOL                  use_threads ;
  BOOL                  use_toolbar ;
  BOOL                  use_tooltips ;
  BOOL                  use_editors ;
  BOOL                  drop_to_editor ;
  BOOL                  rendersleep ;
  BOOL                  ExtensionsEnabled ;
  char                  queued_files [MAX_QUEUE] [_MAX_PATH] ;
  unsigned              queued_file_count ;
  unsigned              auto_render ;
  DWORD                 Reserved [32] ;
} ExternalVarStruct ;

typedef enum
{
  EventFirst,
  EventStartRendering,
  EventStopRendering,
  EventDisplayInit,
  EventDisplayFinished,
  EventDisplayClose,
  EventWinStartup,
  EventWinFinish,
  EventWinCooperate,
  EventLoadToolMenu,
  EventTimer,
  EventSize,
  EventMove,
  EventClose,
  EventDestroy,
  EventLast
} ExternalEvents ;

typedef enum
{
  RequestFirst,
  RequestGetVars,
  RequestSetVars,
  RequestStartRendering,
  RequestStopRendering,
  RequestExit,
  RequestLast
} ExternalRequests ;

typedef enum
{
  ExRequestDisabled,
  ExRequestOK,
  ExRequestFailed,
  ExRequestFailedRendering,
  ExRequestFailedNotRendering,
  ExRequestBadRecSize,
  ExRequestUnknown
} ExternalRequestResult ;

typedef enum
{
  dfRealDrop,
  dfRenderEditor,
  dfRenderMessage,
  dfRenderCommandLine,
  dfRenderSourceFile,
  dfRenderFileQueue
} ExternalDropType ;

typedef struct
{
  // data POV passes to the DLL
  LPSTR                           PovVersion ;
  LPSTR                           GuiVersion ;
  DWORD                           GuiInterfaceVersion ;
  WPARAM                          FirstMenuItem ;
  HINSTANCE                       hInst ;
  HWND                            MainWindow ;
  ExternalRequestResult           (WINAPI *ExternalRequest) (ExternalRequests Request, void *RequestBlock) ;

  // data the DLL passes to POV
  LPSTR                 Name ;
  LPSTR                 Author ;
  LPSTR                 AuthorEmail ;
  HMENU                 hMenu ;
  DWORD                 DLLInterfaceVersion ;
  char                  Agreement [1024] ;
} GuiExtInitStruct ;

typedef struct
{
  DWORD                 InstanceID ;
  HMENU                 hMenu ;
  WPARAM                FirstMenuItem ;
} IDataStruct ;

typedef struct
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
  DWORD                 Reserved [128] ;
} GuiPointerBlock ;

DWORD ExternalEvent (ExternalEvents Event, DWORD EventVal) ;
void ExternalDisplayPlot (int x, int y, int Red, int Green, int Blue, int Alpha) ;
void ExternalDisplayPlotRect (int x1, int y1, int x2, int y2, int Red, int Green, int Blue, int Alpha) ;
void ExternalWinPrePixel (int x, int y, COLOUR colour) ;
void ExternalWinPostPixel (int x, int y, COLOUR colour) ;
BOOL ExternalWinSystem (LPSTR command, int *returnval) ;
void ExternalCleanupAll (void) ;
void ExternalBufferMessage (msgtype message_type, LPSTR message) ;
void ExternalParseToolCommand (char command [512]) ;
BOOL ExternalDragFunction (LPSTR szFile, ExternalDropType DropType) ;
void LoadGUIExtensions (void) ;
DWORD ExternalMenuSelect (WPARAM Code) ;
char *ExternalMenuTip (WPARAM wParam) ;

#endif // PVGUIEXT_H_INCLUDED


