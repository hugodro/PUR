/**************************************************
* File: winLibSupport.cc.
* Desc: Implementation of additional functions for WinNT PovRay.
* Module: AkraLog : PovRay.
* Rev: 20 septembre 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <windows.h>

#define POVWIN_FILE
#include "frame.h"
#include "colour.h"
#include "povray.h"
#include "povproto.h"
#include "optout.h"
#include "userio.h"
#pragma hdrstop


char HomePath [_MAX_PATH];
char DefaultRenderIniFileName [_MAX_PATH];
BOOL IsWin32= FALSE;
OSVERSIONINFO version_info;

extern int parse_ini_file(char *);


FILE *pov_fopen (const char *filename, const char *mode)
{
  return (fopen(filename, mode));
}


int pov_fclose (FILE *stream)
{
  return (fclose(stream));
}


int pov_getc (FILE *stream)
{
  return (getc(stream));
}

int pov_fseek (FILE *stream, long offset, int whence)
{
  return (fseek(stream, offset, whence));
}

int pov_ftell (FILE *stream)
{
  return (ftell(stream));
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


void getHome (void)
{
  HKEY        key ;
  DWORD       len = sizeof (HomePath) ;

  version_info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  GetVersionEx (&version_info) ;
  IsWin32= HaveWin95() || HaveWinNT();

  HomePath [0] = '\0' ;
  if (IsWin32) {
    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE,"Software\\POV-Ray\\Windows",
                      0, KEY_READ, &key) == ERROR_SUCCESS) {
      RegQueryValueEx (key, "Home", 0, NULL, (unsigned char *)HomePath, &len);
      RegCloseKey(key);
    }
  }
  else {
    if (RegOpenKey (HKEY_CLASSES_ROOT, "POV-Ray\\Home", &key) == ERROR_SUCCESS)
    {
      RegQueryValue (key, NULL, HomePath, (LONG *) &len) ;
      RegCloseKey (key) ;
    }
  }
}


void WIN_ParseINI (void)
{

    getHome () ;
    if (HomePath [0] == '\0') {
        GetProfileString ("POV-Ray v3.0 for Windows", "Home", "", HomePath, sizeof (HomePath));
    }
    if (HomePath[0] != '\0') {
	sprintf (DefaultRenderIniFileName, "%sRENDERER\\POVRAY.INI", HomePath);
	parse_ini_file (DefaultRenderIniFileName);
    }
    else {
	if (getenv("POVINI") == NULL) {
	    parse_ini_file ("povray.ini");
	}
    }
}


void WIN_Display_Init (int x, int y)
{
}


void WIN_Display_Close (void)
{
}


int WIN_System (char *s)
{
  return (0);
}


void WIN_Banner (char *s)
{
  printf("Banner: %s.\n", s);
}


void WIN_Warning (char *s)
{
  printf("Warning: %s.\n", s);
}

void WIN_Render_Info (char *s)
{
  printf("Render_Info: %s.\n", s);
}

void WIN_Status_Info (char *s)
{
    printf("Status Info: %s.\n", s);
}

void WIN_Debug_Info (char *s)
{
  printf("Debug Info: %s.\n", s);
}

void WIN_Fatal (char *s)
{
  printf("Fatal: %s.\n", s);
}

void WIN_Statistics (char *s)
{
  printf("Statistics: %s.\n", s);
}


void WIN_Finish (int n)
{
    printf("\n") ;
}


// priority == 0 when called from a higher level (e.g. in Trace_Pixel()).
void WIN_Cooperate (int priority)
{
}


void WIN_Display_Plot (int x, int y, int Red, int Green, int Blue, int Alpha)
{
}

void WIN_Display_Plot_Rect (int x1, int x2, int y1, int y2, int Red, int Green, int Blue, int Alpha)
{
}

void WIN_Pre_Pixel (int x, int y, COLOUR colour)
{
}

void WIN_Post_Pixel (int x, int y, COLOUR colour)
{
}
