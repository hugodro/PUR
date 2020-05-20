/**************************************************
* File: wrapDll.cc.
* Desc: Native lib wrapper for Window NT.
* Module: AkraLog : JavaKit.
* Rev: 20 mai 1997 : REV 0 : Hugo DesRosiers : Creation.
* Rev: 4 decembre 1997 : REV 0.1 : Hugo DesRosiers.
*	Note: update for the nt3DWindow class.
**************************************************/

#include <windows.h>
#include "nt3DWindow.h"


extern "C" {
    BOOL WINAPI DllMain(HINSTANCE hinstanceDll, DWORD fwdReason, LPVOID lpvReserved);
}


BOOL WINAPI DllMain(HINSTANCE hinstanceDll, DWORD fwdReason, LPVOID lpvReserved)
{
    switch (fwdReason) {
	case DLL_PROCESS_ATTACH:

	    NT3DWindow::setDLLHandle(hinstanceDll);
	    break;
	case DLL_THREAD_ATTACH:
	    break;
	case DLL_THREAD_DETACH:
	    break;
	case DLL_PROCESS_DETACH:
	    break;
    }
    return TRUE;
}
