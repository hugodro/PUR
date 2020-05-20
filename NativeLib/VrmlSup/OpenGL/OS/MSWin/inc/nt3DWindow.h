#ifndef _NT3DWINDOW_H_
#define _NT3DWINDOW_H_
/**************************************************
* File: nt3DWindow.h.
* Desc: Definition of the NT3DWindow class.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <ogl/oglRender.h>


#define RADTODEG	(180.0f / 3.141592654f)
#define DEGTORAD	(3.141592654f / 180.0f)

class JVArrayArray;
class JVInstance;
class ContainerStack;
class AkRectangle;
class ViewPointVrml;
class NodeVrml;
class ContainerVrml;
class GuiControler;


class NT3DWindow : public OglRenderer {
  private:
    enum Constants {
	maxWindowCount= 25
     };

  private:
    static HINSTANCE dllInstance;
    static char *windowClassName;
    static WNDCLASS *windowClass;
    static NT3DWindow *windows[maxWindowCount];
    static unsigned int nbrWindows;

  public:
    static void setDLLHandle(HINSTANCE aHandle);
    static LRESULT CALLBACK baseEventHandler(HWND, UINT, WPARAM, LPARAM);
    static void globalModalLooper(void);	// Blocking call.


  protected:    // Instance variables.
    HWND handle;
    HDC drawContext;
    HGLRC renderContext;
    HPALETTE colorPalette;
    JVInstance *javaEquiv;		// Our CameraNT equivalent.
    boolean mouseFollow;

  public:	  // Instance methods.
    NT3DWindow(char *winTitle, AkRectangle *frameRect);
    virtual ~NT3DWindow(void);
    virtual void initOpenGL(void);
    virtual HWND getWinHandle(void);
    virtual void orderFrontAndMakeKey(void);
    virtual void updateCamera(void);
    void setJavaEquiv(JVInstance *anInstance);

    virtual void windowDidResize();
    virtual void mouseDown(int posX, int posY);
    virtual void mouseUp(int posX, int posY);
    virtual void mouseMove(int posX, int posY);
    virtual void keyDown(WPARAM keyCode);
    virtual void performClose(int exitCode);
};

#endif		/* _NT3DWINDOW_H_ */
