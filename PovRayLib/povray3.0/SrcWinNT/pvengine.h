/****************************************************************************
*                pvengine.h
*
*  This file contains PVENGINE specific defines.
*  POV-Ray defines are in CONFIG.H.
*
*  Copyright © POV-Team 1996-1997. All Rights Reserved.
*  This windows version of POV-Ray is Copyright 1996-1997 Christopher J. Cason.
*  Authors : Christopher J. Cason and Kendell Bennett.
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

#ifndef PVENGINE_H_INCLUDED
#define PVENGINE_H_INCLUDED

// put your name and email address here. this is a REQUIREMENT if you want to
// distribute your compile in accordance with POVLEGAL.DOC (i.e. legally.)
// there are other requirements, too ; make sure you read ALL of povlegal.doc.
#define UNOFFICIALCOMPILE        "someone who has not read README.NOW"

#include <time.h>

#define __USECTL3D__
#define OPTIMISATION              "Pentium"
#define MAX_MESSAGE               1024
#define PVENGINE_VER              "unofficial-win32"
#define MAX_ARGV                  256
#define STOP_BEING_NICE           4
#define INIFILENAME               "PVENGINE.INI"
#define RERUNFILENAME             "PVRERUN.INI"
#define TOOLFILENAME              "PVTOOLS.INI"
#define MAX_QUEUE                 128
#define MAX_TOOLCMD               32
#define MAX_TOOLCMDTEXT           128
#define MAX_TOOLHELPTEXT          128
#define MIN_EDITOR_VERSION        100
#define MAX_EDITOR_VERSION        199
#define MAX_WINDOWS               16
#define POV_INTERNAL_STREAM       ((FILE *) 1L)

// if this is more than 16 then add_rerun_to_menu () needs to be modified.
#define MAX_RERUN       16

#define RENDER_MESSAGE            WM_USER + 1000
#define CREATE_RENDERWIN_MESSAGE  WM_USER + 1005
#define CLOSE_EDITOR_MESSAGE      WM_USER + 1010
#define CREATE_EDITOR_MESSAGE     WM_USER + 1015
#define EDITOR_RENDER_MESSAGE     WM_USER + 1020
#define SHOW_MESSAGES_MESSAGE     WM_USER + 1025
#define TASKBAR_NOTIFY_MESSAGE    WM_USER + 1100

#define NUM_BUTTONS     16

#define HDIB            HANDLE
#define SEPARATOR       '\\'

#ifndef OPTIMISATION
#define OPTIMISATION    "486/Pentium"
#endif

#define RGBBLACK     RGB(0,0,0)
#define RGBRED       RGB(128,0,0)
#define RGBGREEN     RGB(0,128,0)
#define RGBBLUE      RGB(0,0,128)

#define RGBBROWN     RGB(128,128,0)
#define RGBMAGENTA   RGB(128,0,128)
#define RGBCYAN      RGB(0,128,128)
#define RGBLTGRAY    RGB(192,192,192)

#define RGBGRAY      RGB(128,128,128)
#define RGBLTRED     RGB(255,0,0)
#define RGBLTGREEN   RGB(0,255,0)
#define RGBLTBLUE    RGB(0,0,255)

#define RGBYELLOW    RGB(255,255,0)
#define RGBLTMAGENTA RGB(255,0,255)
#define RGBLTCYAN    RGB(0,255,255)
#define RGBWHITE     RGB(255,255,255)

typedef unsigned char uchar ;

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

typedef enum
{
  None,
  CR,
  LF
} lftype ;

typedef enum
{
  filePOV,
  fileINC,
  fileINI,
  fileTGA,
  filePPM,
  filePGM,
  filePBM,
  filePNG,
  fileGIF,
  fileBMP,
  fileUnknown
} filetypes ;

typedef enum
{
  StatusMessage,
  StatusPPM,
  StatusRendertime,
  StatusLast
} StatusBarSection ;

// Bitmap header info with palette included

typedef struct
{
  BITMAPINFOHEADER      header ;
  RGBQUAD               colors [256] ;
} BitmapInfo ;

// Windows LOGPALETTE palette structure

typedef struct
{
  WORD                  version ;
  WORD                  entries ;
  PALETTEENTRY          pe [256] ;
} LogPal ;

typedef struct
{
  HMENU       FileMenu ;
  HMENU       EditMenu ;
  HMENU       SearchMenu ;
  HMENU       InsertMenu ;
  HMENU       OptionsMenu ;
  HMENU       AppearanceMenu ;
  HWND        StatusWindow ;
} HandleStruct ;

typedef struct
{
  void (WINAPI *Init) (HANDLE hInstance) ;
  void (WINAPI *Destroy) (void) ;
  void (WINAPI *Show) (int State) ;
  BOOL (WINAPI *SelectEditor) (void *Editor) ;
  HWND (WINAPI *CreateEditor) (HWND hWndMain, HWND hWndTabbed, char *home) ;
  void (WINAPI *GetHandles) (HandleStruct *handles) ;
  void (WINAPI *UpdateMenus) (void *Editor) ;
  BOOL (WINAPI *Dispatch) (int Command) ;
  void *(WINAPI *GetEditor) (void) ;
  BOOL (WINAPI *ReleaseEditor) (void *Editor) ;
  char *(WINAPI *GetTitle) (void *Editor, int *TopLine, int *Line, int *Col, BOOL *Modified) ;
  BOOL (WINAPI *CanClose) (void) ;
  void (WINAPI *SetActive) (BOOL Active) ;
  BOOL (WINAPI *OpenFile) (void *Editor, char *FileName, int TopLine, int Line, int Col, BOOL AddToHistory) ;
  BOOL (WINAPI *GetLine) (void *Editor, int LineNumber, char *Buffer, int BufLen) ;
  int (WINAPI *Seek) (void *Editor, int *Offset) ;
  void (WINAPI *SaveFile) (void) ;
  void (WINAPI *OpenInNew) (void) ;
  void (WINAPI *SaveSettings) (void) ;
} EditStruct ;

typedef struct
{
  BOOL        ncEnabled ;
  BOOL        menuWasUp ;
  BOOL        hasCaption ;
  BOOL        hasBorder ;
  BOOL        hasSizeableBorder ;
  BOOL        hasStatusBar ;
  BOOL        hasMenuBar ;
  BOOL        sysMenuOverride ;
  BOOL        isMaxiMinimized ;
  HWND        hWnd ;
  HFONT       hMenuBarFont ;
  HFONT       hStatusBarFont ;
  HFONT       hSystemFont ;
  ushort      captionTotal ;
  ushort      captionInternal ;
  ushort      captionBorderLeft ;
  ushort      captionBorderRight ;
  ushort      captionBorderTop ;
  ushort      captionBorderBottom ;
  ushort      borderWidth ;
  ushort      borderHeight ;
  ushort      buttonWidth ;
  ushort      buttonHeight ;
  ushort      sizing ;
  ushort      statusBarTotal ;
  ushort      statusBarBorder ;
  ushort      menuBarTotal ;
  ushort      menuBarBorder ;
} pvncStruct ;

void PovMessageBox (char *message, char *title) ;
int initialise_message_display (void) ;
void erase_display_window (HDC hdc, int xoffset, int yoffset) ;
void paint_display_window (HDC hdc) ;
void buffer_message (msgtype message_type, char *s) ;
void clear_messages (void) ;
int update_message_display (lftype lf) ;
void message_printf (char *format, ...) ;
void dump_pane_to_clipboard (void) ;
BOOL PutPrivateProfileInt (LPCSTR lpszSection, LPCSTR lpszEntry, UINT uiValue, LPCSTR lpszFilename) ;
void WIN_Display_Init (int x, int y) ;
void WIN_Display_Finished (void) ;
void WIN_Display_Close (void) ;
void WIN_Display_Plot (int x, int y, int Red, int Green, int Blue, int Alpha) ;
void WIN_Display_Plot_Rect (int x1, int x2, int y1, int y2, int Red, int Green, int Blue, int Alpha) ;
void WIN_Banner(char *s) ;
void WIN_Warning(char *s) ;
void WIN_Render_Info(char *s) ;
void WIN_Status_Info(char *s) ;
void WIN_Debug_Info(char *s) ;
void WIN_Fatal(char *s) ;
void WIN_Statistics(char *s) ;
void WIN_Startup(void) ;
void WIN_Finish(int n) ;
void WIN_Cooperate(int n) ;
int WIN_Povray (int argc, char **argv) ;
int WIN_System (char *s) ;
void get_logfont (HDC hdc, LOGFONT *lf) ;
int create_message_font (HDC hdc, LOGFONT *lf) ;
void status_printf (int nSection, char *format, ...) ;
void SetupExplorerDialog (HWND win) ;
void validatePath (char *s) ;
int joinPath (char *out, char *path, char *name) ;
void UpdateTabbedWindow (int current, BOOL force) ;
void CalculateClientWindows (BOOL redraw) ;
void start_rendering (BOOL is_auto_render, BOOL ignore_source_file) ;
BOOL HaveWin95 (void) ;
void create_palette (RGBQUAD *rgb, int count) ;

// file PVMISC.C

int get_file_type (char *filename) ;
void rotate_rerun_entries (void) ;
void write_rerun_information (void) ;
void read_INI_settings (char *iniFilename) ;
void write_INI_settings (char *iniFilename) ;
void add_rerun_to_menu (void) ;
void update_menu_for_render (int rendering) ;
void update_queue_status (BOOL write_files) ;
void fill_rerun_listbox (HWND hlb, char *idList) ;
void draw_rerun_listbox (DRAWITEMSTRUCT *d) ;
void draw_ordinary_listbox (DRAWITEMSTRUCT *d) ;
void fill_statistics_listbox (HWND hlb, int id) ;
void resize_listbox_dialog (HWND hDlg, int idLb, int chars) ;
void CenterWindowRelative (HWND hRelativeTo, HWND hTarget, BOOL bRepaint) ;
void FitWindowInWindow (HWND hRelativeTo, HWND hTarget) ;
int splitfn (char *filename, char *path, char *name, char *ext) ;
void splitpath (char *filename, char *path, char *name) ;
BOOL process_toggles (WPARAM wParam) ;
void set_toggles (void) ;
void load_tool_menu (char *iniFilename) ;
char *parse_tool_command (char *command) ;
char *get_elapsed_time (time_t start, time_t finish) ;
void initialise_statusbar (BOOL isMaxiMiniMode) ;
void calculate_statusbar (void) ;
void paint_statusbar (int nSection) ;
void extract_ini_sections (char *filename, HWND hwnd) ;
void paint_rendering_signal (int which_one) ;
char *get_full_name (char *s) ;
BOOL PovInvalidateRect (HWND hWnd, CONST RECT *lpRect, BOOL bErase) ;
int load_editors (char *iniFilename) ;
BOOL TaskBarAddIcon (HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip) ;
BOOL TaskBarDeleteIcon (HWND hwnd, UINT uID) ;

// file PVFILES.C

BOOL FAR PASCAL PovLegalDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
char *save_demo_file (char *s1, char *s2) ;
void save_povlegal (void) ;

// file PVMENU.C

BOOL PVEnableMenuItem (UINT idItem, UINT state) ;
BOOL PVCheckMenuItem (UINT idItem, UINT state) ;
BOOL PVModifyMenu (UINT idItem, UINT flags, UINT idNewItem, LPCSTR lpNewItem) ;
BOOL PVDeleteMenuItem (UINT idItem) ;
void init_menus (void) ;
void setup_menus (BOOL have_editor) ;
void clear_menu (HMENU hMenu) ;
void build_main_menu (HMENU hMenu, BOOL have_editor) ;
void build_editor_menu (HMENU hMenu) ;

// file PVTEXT.C

void write_wrapped_text (HDC hdc, RECT *rect, char *text) ;
void tip_of_the_day (HDC hdc, RECT *rect, char *text) ;
void paint_statusbar (int nSection) ;
void say_status_message (int section, char *message) ;
void handle_menu_select (WPARAM wParam, LPARAM lParam) ;
char *clean_str (char *s) ;
HWND create_toolbar (HWND hwndParent) ;
HWND create_tabbed_window (HWND hwndParent) ;
void initialise_tabbed_window (HWND hwnd) ;
unsigned add_window_to_tab (HWND hwnd, void *editor, char *s) ;
void resize_windows (unsigned left, unsigned top, unsigned width, unsigned height) ;
unsigned get_tab_index (HWND hwnd, void *editor) ;
char *preparse_commandline (char *s) ;
int need_hscroll (void) ;

// file PVCLIENT.C

void NEAR PASCAL pvncGetStruct (HWND hWnd, pvncStruct *pvnc) ;
void NEAR PASCAL pvncSetStruct (HWND hWnd, pvncStruct *pvnc) ;

// file PVBITMAP.C

HDIB      FAR  BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
HDIB      FAR  ChangeBitmapFormat (HBITMAP  hBitmap,
                                   WORD     wBitCount,
                                   DWORD    dwCompression,
                                   HPALETTE hPal);
HDIB      FAR  ChangeDIBFormat (HDIB hDIB, WORD wBitCount,
                                DWORD dwCompression);
HBITMAP   FAR  CopyScreenToBitmap (LPRECT);
HDIB      FAR  CopyScreenToDIB (LPRECT);
HBITMAP   FAR  CopyWindowToBitmap (HWND, WORD);
HDIB      FAR  CopyWindowToDIB (HWND, WORD);
HPALETTE  FAR  CreateDIBPalette (HDIB hDIB);
HDIB      FAR  CreateDIB(DWORD, DWORD, WORD);
WORD      FAR  DestroyDIB (HDIB);
void      FAR  DIBError (int ErrNo);
DWORD     FAR  DIBHeight (LPSTR lpDIB);
WORD      FAR  DIBNumColors (LPSTR lpDIB);
HBITMAP   FAR  DIBToBitmap (HDIB hDIB, HPALETTE hPal);
DWORD     FAR  DIBWidth (LPSTR lpDIB);
LPSTR     FAR  FindDIBBits (LPSTR lpDIB);
HPALETTE  FAR  GetSystemPalette (void);
HDIB      FAR  LoadDIB (LPSTR);
BOOL      FAR  PaintBitmap (HDC, LPRECT, HBITMAP, LPRECT, HPALETTE);
BOOL      FAR  PaintDIB (HDC, LPRECT, HDIB, LPRECT, HPALETTE);
int       FAR  PalEntriesOnDevice (HDC hDC);
WORD      FAR  PaletteSize (LPSTR lpDIB);
WORD      FAR  PrintDIB (HDIB, WORD, WORD, WORD, LPSTR);
WORD      FAR  PrintScreen (LPRECT, WORD, WORD, WORD, LPSTR);
WORD      FAR  PrintWindow (HWND, WORD, WORD, WORD, WORD, LPSTR);
WORD      FAR  SaveDIB (HDIB, LPSTR);
HANDLE         AllocRoomForDIB(BITMAPINFOHEADER bi, HBITMAP hBitmap);
HBITMAP        lpDIBToBitmap(void *lpDIBHdr, HPALETTE hPal);
HBITMAP        lpDIBToBitmapAndPalette(void *lpDIBHdr);

#ifdef DECLARE_TABLES

// Default windows compatible halftone palette. This includes the default
// Windows system colors in the first 10 and last 10 entries in the
// palette.

RGBQUAD halftonePal [256] =
{
  0x00,0x00,0x00,0, 0xA8,0x00,0x00,0, 0x00,0xA8,0x00,0, 0xA8,0xA8,0x00,0,
  0x00,0x00,0xA8,0, 0xA8,0x00,0xA8,0, 0x00,0x54,0xA8,0, 0xA8,0xA8,0xA8,0,
  0x54,0x54,0x54,0, 0xFC,0x54,0x54,0, 0x54,0xFC,0x54,0, 0xFC,0xFC,0x54,0,
  0x54,0x54,0xFC,0, 0xFC,0x54,0xFC,0, 0x54,0xFC,0xFC,0, 0xFC,0xFC,0xFC,0,
  0x00,0x00,0x00,0, 0x14,0x14,0x14,0, 0x20,0x20,0x20,0, 0x2C,0x2C,0x2C,0,
  0x00,0x00,0x00,0, 0x00,0x00,0x33,0, 0x00,0x00,0x66,0, 0x00,0x00,0x99,0,
  0x00,0x00,0xCC,0, 0x00,0x00,0xFF,0, 0x00,0x33,0x00,0, 0x00,0x33,0x33,0,
  0x00,0x33,0x66,0, 0x00,0x33,0x99,0, 0x00,0x33,0xCC,0, 0x00,0x33,0xFF,0,
  0x00,0x66,0x00,0, 0x00,0x66,0x33,0, 0x00,0x66,0x66,0, 0x00,0x66,0x99,0,
  0x00,0x66,0xCC,0, 0x00,0x66,0xFF,0, 0x00,0x99,0x00,0, 0x00,0x99,0x33,0,
  0x00,0x99,0x66,0, 0x00,0x99,0x99,0, 0x00,0x99,0xCC,0, 0x00,0x99,0xFF,0,
  0x00,0xCC,0x00,0, 0x00,0xCC,0x33,0, 0x00,0xCC,0x66,0, 0x00,0xCC,0x99,0,
  0x00,0xCC,0xCC,0, 0x00,0xCC,0xFF,0, 0x00,0xFF,0x00,0, 0x00,0xFF,0x00,0,
  0x00,0xFF,0x66,0, 0x00,0xFF,0x99,0, 0x00,0xFF,0xCC,0, 0x00,0xFF,0xFF,0,
  0x33,0x00,0x00,0, 0x33,0x00,0x33,0, 0x33,0x00,0x66,0, 0x33,0x00,0x99,0,
  0x33,0x00,0xCC,0, 0x33,0x00,0xFF,0, 0x33,0x33,0x00,0, 0x33,0x33,0x33,0,
  0x33,0x33,0x66,0, 0x33,0x33,0x99,0, 0x33,0x33,0xCC,0, 0x33,0x33,0xFF,0,
  0x33,0x66,0x00,0, 0x33,0x66,0x33,0, 0x33,0x66,0x66,0, 0x33,0x66,0x99,0,
  0x33,0x66,0xCC,0, 0x33,0x66,0xFF,0, 0x33,0x99,0x00,0, 0x33,0x99,0x33,0,
  0x33,0x99,0x66,0, 0x33,0x99,0x99,0, 0x33,0x99,0xCC,0, 0x33,0x99,0xFF,0,
  0x33,0xCC,0x00,0, 0x33,0xCC,0x33,0, 0x33,0xCC,0x66,0, 0x33,0xCC,0x99,0,
  0x33,0xCC,0xCC,0, 0x33,0xCC,0xFF,0, 0x00,0xFF,0x00,0, 0x33,0xFF,0x33,0,
  0x33,0xFF,0x66,0, 0x33,0xFF,0x99,0, 0x33,0xFF,0xCC,0, 0x33,0xFF,0xFF,0,
  0x66,0x00,0x00,0, 0x66,0x00,0x33,0, 0x66,0x00,0x66,0, 0x66,0x00,0x99,0,
  0x66,0x00,0xCC,0, 0x66,0x00,0xFF,0, 0x66,0x33,0x00,0, 0x66,0x33,0x33,0,
  0x66,0x33,0x66,0, 0x66,0x33,0x99,0, 0x66,0x33,0xCC,0, 0x66,0x33,0xFF,0,
  0x66,0x66,0x00,0, 0x66,0x66,0x33,0, 0x66,0x66,0x66,0, 0x66,0x66,0x99,0,
  0x66,0x66,0xCC,0, 0x66,0x66,0xFF,0, 0x66,0x99,0x00,0, 0x66,0x99,0x33,0,
  0x66,0x99,0x66,0, 0x66,0x99,0x99,0, 0x66,0x99,0xCC,0, 0x66,0x99,0xFF,0,
  0x66,0xCC,0x00,0, 0x66,0xCC,0x33,0, 0x66,0xCC,0x66,0, 0x66,0xCC,0x99,0,
  0x66,0xCC,0xCC,0, 0x66,0xCC,0xFF,0, 0x66,0xFF,0x00,0, 0x66,0xFF,0x33,0,
  0x66,0xFF,0x66,0, 0x66,0xFF,0x99,0, 0x66,0xFF,0xCC,0, 0x66,0xFF,0xFF,0,
  0x99,0x00,0x00,0, 0x99,0x00,0x33,0, 0x99,0x00,0x66,0, 0x99,0x00,0x99,0,
  0x99,0x00,0xCC,0, 0x99,0x00,0xFF,0, 0x99,0x33,0x00,0, 0x99,0x33,0x33,0,
  0x99,0x33,0x66,0, 0x99,0x33,0x99,0, 0x99,0x33,0xCC,0, 0x99,0x33,0xFF,0,
  0x99,0x66,0x00,0, 0x99,0x66,0x33,0, 0x99,0x66,0x66,0, 0x99,0x66,0x99,0,
  0x99,0x66,0xCC,0, 0x99,0x66,0xFF,0, 0x99,0x99,0x00,0, 0x99,0x99,0x33,0,
  0x99,0x99,0x66,0, 0x99,0x99,0x99,0, 0x99,0x99,0xCC,0, 0x99,0x99,0xFF,0,
  0x99,0xCC,0x00,0, 0x99,0xCC,0x33,0, 0x99,0xCC,0x66,0, 0x99,0xCC,0x99,0,
  0x99,0xCC,0xCC,0, 0x99,0xCC,0xFF,0, 0x99,0xFF,0x00,0, 0x99,0xFF,0x33,0,
  0x99,0xFF,0x66,0, 0x99,0xFF,0x99,0, 0x99,0xFF,0xCC,0, 0x99,0xFF,0xFF,0,
  0xCC,0x00,0x00,0, 0xCC,0x00,0x33,0, 0xCC,0x00,0x66,0, 0xCC,0x00,0x99,0,
  0xCC,0x00,0xCC,0, 0xCC,0x00,0xFF,0, 0xCC,0x33,0x00,0, 0xCC,0x33,0x33,0,
  0xCC,0x33,0x66,0, 0xCC,0x33,0x99,0, 0xCC,0x33,0xCC,0, 0xCC,0x33,0xFF,0,
  0xCC,0x66,0x00,0, 0xCC,0x66,0x33,0, 0xCC,0x66,0x66,0, 0xCC,0x66,0x99,0,
  0xCC,0x66,0xCC,0, 0xCC,0x66,0xFF,0, 0xCC,0x99,0x00,0, 0xCC,0x99,0x33,0,
  0xCC,0x99,0x66,0, 0xCC,0x99,0x99,0, 0xCC,0x99,0xCC,0, 0xCC,0x99,0xFF,0,
  0xCC,0xCC,0x00,0, 0xCC,0xCC,0x33,0, 0xCC,0xCC,0x66,0, 0xCC,0xCC,0x99,0,
  0xCC,0xCC,0xCC,0, 0xCC,0xCC,0xFF,0, 0xCC,0xFF,0x00,0, 0xCC,0xFF,0x33,0,
  0xCC,0xFF,0x66,0, 0xCC,0xFF,0x99,0, 0xCC,0xFF,0xCC,0, 0xCC,0xFF,0xFF,0,
  0xFF,0x00,0x00,0, 0xFF,0x00,0x00,0, 0xFF,0x00,0x66,0, 0xFF,0x00,0x99,0,
  0xFF,0x00,0xCC,0, 0xFF,0x00,0xFF,0, 0xFF,0x00,0x00,0, 0xFF,0x33,0x33,0,
  0xFF,0x33,0x66,0, 0xFF,0x33,0x99,0, 0xFF,0x33,0xCC,0, 0xFF,0x33,0xFF,0,
  0xFF,0x66,0x00,0, 0xFF,0x66,0x33,0, 0xFF,0x66,0x66,0, 0xFF,0x66,0x99,0,
  0xFF,0x66,0xCC,0, 0xFF,0x66,0xFF,0, 0xFF,0x99,0x00,0, 0xFF,0x99,0x33,0,
  0xFF,0x99,0x66,0, 0xFF,0x99,0x99,0, 0xFF,0x99,0xCC,0, 0xFF,0x99,0xFF,0,
  0xFF,0xCC,0x00,0, 0xFF,0xCC,0x33,0, 0xFF,0xCC,0x66,0, 0xFF,0xCC,0x99,0,
  0xFF,0xCC,0xCC,0, 0xFF,0xCC,0xFF,0, 0xFF,0xFF,0x00,0, 0xFF,0xFF,0x33,0,
  0xFF,0xFF,0x66,0, 0xFF,0xFF,0x99,0, 0xFF,0xFF,0xCC,0, 0xFF,0xFF,0xFF,0,
  0x2C,0x40,0x40,0, 0x2C,0x40,0x3C,0, 0x2C,0x40,0x34,0, 0x2C,0x40,0x30,0,
  0x2C,0x40,0x2C,0, 0x30,0x40,0x2C,0, 0x34,0x40,0x2C,0, 0x3C,0x40,0x2C,0,
  0x40,0x40,0x2C,0, 0x40,0x3C,0x2C,0, 0x40,0x34,0x2C,0, 0x40,0x30,0x2C,0,
  0x54,0x54,0x54,0, 0xFC,0x54,0x54,0, 0x54,0xFC,0x54,0, 0xFC,0xFC,0x54,0,
  0x54,0x54,0xFC,0, 0xFC,0x54,0xFC,0, 0x54,0xFC,0xFC,0, 0xFC,0xFC,0xFC,0
} ;

// Division lookup tables.  These tables compute 0-255 divided by 51 and
// modulo 51.  These tables could approximate gamma correction.

uchar div51 [256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5
} ;

uchar mod51 [256] =
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6,
  7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3,
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0
} ;

// Multiplication lookup tables. These compute 0-5 times 6 and 36.

uchar mul6 [6] = {0, 6, 12, 18, 24, 30} ;
uchar mul36 [6] = {0, 36, 72, 108, 144, 180} ;

// Ordered 8x8 dither matrix for 8 bit to 2.6 bit halftones.

uchar dither8x8 [64] =
{
   0, 38,  9, 47,  2, 40, 11, 50,
  25, 12, 35, 22, 27, 15, 37, 24,
   6, 44,  3, 41,  8, 47,  5, 43,
  31, 19, 28, 15, 34, 21, 31, 18,
   1, 39, 11, 49,  0, 39, 10, 48,
  27, 14, 36, 23, 26, 13, 35, 23,
   7, 46,  4, 43,  7, 45,  3, 42,
  33, 20, 30, 17, 32, 19, 29, 16,
} ;

#endif // #if DECLARE_TABLES

#endif // PVENGINE_H_INCLUDED
