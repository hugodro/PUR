/**************************************************
* File: nt3DWindow.cc.
* Desc: Definition of the NT3DWindow class.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <ogl/guiControler.h>
#include "nt3DWindow.h"


/*** Fonctions from other modules. ***/

extern void CreateRGBPalette(HDC);


// Class variables.

HINSTANCE NT3DWindow::dllInstance= NULL;
char *NT3DWindow::windowClassName=  "AkNT3DWindow";
WNDCLASS *NT3DWindow::windowClass= NULL;
NT3DWindow *NT3DWindow::windows[maxWindowCount];
unsigned int NT3DWindow::nbrWindows;


/**************************************************
* Implementation: NT3DWindow.
**************************************************/

void NT3DWindow::setDLLHandle(HINSTANCE aHandle)
{
    dllInstance= aHandle;
    windowClass= (WNDCLASS *)calloc(1, sizeof(WNDCLASS));

    windowClass->style= CS_HREDRAW | CS_VREDRAW;
    windowClass->lpfnWndProc= (WNDPROC)baseEventHandler;
    windowClass->cbClsExtra= 0;
    windowClass->cbWndExtra= 0;
    windowClass->hInstance= dllInstance;
    windowClass->hIcon= LoadIcon(NULL, IDI_APPLICATION);
    windowClass->hCursor= LoadCursor(NULL, IDC_ARROW);
    windowClass->hbrBackground= GetStockObject(BLACK_BRUSH);
    windowClass->lpszMenuName= NULL;
    windowClass->lpszClassName= (LPSTR)windowClassName;

    if (!RegisterClass(windowClass)) {
	free(windowClass);
	windowClass= NULL;
    }
    for (unsigned int i=0; i < maxWindowCount; i++) {
	windows[i]= NULL;
    }
    nbrWindows= 0;
}


LRESULT CALLBACK NT3DWindow::baseEventHandler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    NT3DWindow *winResponder= NULL;

    for (unsigned int i= 0; i < nbrWindows; i++) {
	if (windows[i]->handle= handle) { 
	    winResponder= windows[i];
	    break;
	}
    }

    if (winResponder != NULL) {
	switch(message) {
	    case WM_PAINT:
		winResponder->updateCamera();
		break;
	    case WM_SIZE:
		winResponder->windowDidResize();
		break;
	    case WM_LBUTTONDOWN:
		winResponder->mouseDown((int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	    case WM_MOUSEMOVE:
		winResponder->mouseMove((int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	    case WM_LBUTTONUP:
		winResponder->mouseUp((int)LOWORD(lParam), (int)HIWORD(lParam));
		break;
	    case WM_KEYDOWN:
		winResponder->keyDown(wParam);
		break;
	    case WM_CLOSE:
		winResponder->performClose((int)wParam);
		delete winResponder;
		if (nbrWindows == 0) {
		    PostQuitMessage((int)wParam);
		}
		break;
	    default:
		return DefWindowProc(handle, message, wParam, lParam);
		break;
	}
    }
    else {
	return DefWindowProc(handle, message, wParam, lParam);
    }

    return 0;
}


void NT3DWindow::globalModalLooper(void)
{
    MSG msg;

    while (GetMessage (&msg, NULL, 0, 0) != FALSE) {
      	TranslateMessage (&msg);
      	DispatchMessage (&msg);
    }
}


NT3DWindow::NT3DWindow(char *winTitle, AkRectangle *frameRect)
    : OglRenderer()
{
    int winWidth, winHeight;

    if (windowClass == NULL) {
	return;
    }

    if (winTitle == NULL) winTitle= "CameraNT";
    if (frameRect == NULL) {
	winWidth= 640;
	winHeight= 480;
    }
    else {
	winWidth= (int)frameRect->getWidth();
	winHeight= (int)frameRect->getHeight();
    }

    handle= CreateWindow(windowClassName, winTitle,
	    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	    CW_USEDEFAULT, CW_USEDEFAULT,
	    winWidth, winHeight,
	    NULL, NULL, dllInstance, NULL);

    if (handle != NULL) {
	javaEquiv= NULL;
	mouseFollow= false;

	drawContext= GetDC(handle);
	initOGL();
	windows[nbrWindows++]= this;
    }
    else {
	// TODO: Handle the error.
    }
}


NT3DWindow::~NT3DWindow(void)
{
    if (handle != NULL) {
	unsigned int i;

	for (i= 0; i < nbrWindows; i++) {
	    if (windows[i]->handle == handle) {
		break;
	    }
	}
	if (i < nbrWindows) {
	    for (i++; i < nbrWindows; i++) {
		windows[i]= windows[i-1];
	    }
	    nbrWindows--;
	}

	if (renderContext != NULL) {
	    wglMakeCurrent(NULL, NULL);
	    wglDeleteContext (renderContext);
	}
	ReleaseDC(handle, drawContext);
	DestroyWindow(handle);
    }
}


void NT3DWindow::initOGL(void)
{
    PIXELFORMATDESCRIPTOR pfd = {
	    sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd
	    1,                     // version number
	    PFD_DRAW_TO_WINDOW		// support window,
		| PFD_SUPPORT_OPENGL		// support OpenGL,
		| PFD_DOUBLEBUFFER,		// double buffered.
	    PFD_TYPE_RGBA,         // RGBA type
	    24,				// bits color depth
	    0, 0, 0, 0, 0, 0,		// color bits ignored
	    0,					// no alpha buffer
	    0,					// shift bit ignored
	    0,					// no accumulation buffer
	    0, 0, 0, 0,			// accum bits ignored
	    32,				// 32-bit z-buffer
	    0,					// no stencil buffer
	    0,					// no auxiliary buffer
	    PFD_MAIN_PLANE,        // main layer
	    0,					// reserved
	    0, 0, 0				// layer masks ignored
    };

    RECT frameRect;
    int  iPixelFormat;

    iPixelFormat = ChoosePixelFormat(drawContext, &pfd);
    SetPixelFormat(drawContext, iPixelFormat, &pfd);
    CreateRGBPalette(drawContext);

    renderContext= wglCreateContext(drawContext);
    wglMakeCurrent(drawContext, renderContext);

    GetClientRect(handle, &frameRect);
    OglRenderer::initOGL(frameRect.right, frameRect.bottom);
}


HWND NT3DWindow::getWinHandle(void)
{
    return handle;
}


void NT3DWindow::orderFrontAndMakeKey(void)
{
    if (handle) {
	ShowWindow(handle, SW_SHOW);
    }
}


void NT3DWindow::updateCamera(void)
{
    HDC localDC;
    PAINTSTRUCT ps;

    localDC= BeginPaint(handle, &ps);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    renderTree();
    glFlush();
    SwapBuffers(drawContext);
    glFinish();
    EndPaint(handle, &ps);
}


void NT3DWindow::setJavaEquiv(JVInstance *anInstance)
{
    javaEquiv= anInstance;
}


void NT3DWindow::windowDidResize()
{
    RECT newRect;

    GetClientRect(handle, &newRect);
    OglRenderer::windowDidResize(newRect.right, newRect.bottom);
}


void NT3DWindow::mouseDown(int posX, int posY)
{
    SetCapture(handle);
    mouseFollow= true;
    guiControler->setMouseAt(posX, posY);
    flags= (flags & ~drawMode) | drawWire;
    InvalidateRect(handle, NULL, FALSE);
}


void NT3DWindow::mouseUp(int posX, int posY)
{
    mouseFollow= false;
    guiControler->setMouseAt(posX, posY);
    ReleaseCapture();
    flags= (flags & ~drawMode) | drawSolid;
    InvalidateRect(handle, NULL, FALSE);
}


void NT3DWindow::mouseMove(int posX, int posY)
{
    if (mouseFollow) {
	Vector3 disp, initVP(0.0, 0.0, -1.0);
	TrMatrix transformer;

	guiControler->moveMouseTo(posX, posY, viewPoint);
	transformer.translateBy(&viewPoint->position);
	transformer.translateBy(&initVP);
	transformer.rotateBy(&viewPoint->orientation);
	transformer.transformPoint(&disp);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(viewPoint->position.points[0], viewPoint->position.points[1], viewPoint->position.points[2],
			disp.Vc[0],
			disp.Vc[1],
			disp.Vc[2],
			0.0f, 1.0f, 0.0f);
	InvalidateRect(handle, NULL, FALSE);
    }
}


void NT3DWindow::keyDown(WPARAM keyCode)
{
    static float posZ= 200.0f;
    static float posX= 0.0f;
    static float posY= 25.0f;

    switch (keyCode) {
	case VK_UP:
	    posZ+= 10.0f;
	    glMatrixMode( GL_MODELVIEW );
	    glLoadIdentity();
	    gluLookAt(posX, posY, posZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	    InvalidateRect(handle, NULL, FALSE);
	    break;
	case VK_DOWN:
	    posZ-= 10.0f;
	    glMatrixMode( GL_MODELVIEW );
	    glLoadIdentity();
	    gluLookAt(posX, posY, posZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	    InvalidateRect(handle, NULL, FALSE);
	    break;
	case VK_LEFT:
	    posX-= 10.0f;
	    glMatrixMode( GL_MODELVIEW );
	    glLoadIdentity();
	    gluLookAt(posX, posY, posZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	    InvalidateRect(handle, NULL, FALSE);
	    break;
	case VK_RIGHT:
	    posX+= 10.0f;
	    glMatrixMode( GL_MODELVIEW );
	    glLoadIdentity();
	    gluLookAt(posX, posY, posZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	    InvalidateRect(handle, NULL, FALSE);
	    break;
	case VK_END:
	    posY-= 10.0f;
	    glMatrixMode( GL_MODELVIEW );
	    glLoadIdentity();
	    gluLookAt(posX, posY, posZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	    InvalidateRect(handle, NULL, FALSE);
	    break;
	case VK_HOME:
	    posY+= 10.0f;
	    glMatrixMode( GL_MODELVIEW );
	    glLoadIdentity();
	    gluLookAt(posX, posY, posZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	    InvalidateRect(handle, NULL, FALSE);
	    break;
	case 'Z':
	    guiControler->setBallMode(GuiControler::travelling);
	    break;
	case 'P':
	    guiControler->setBallMode(GuiControler::pan);
	    break;
	case 'R':
	    guiControler->setBallMode(GuiControler::rotate);
	    break;
	case 'O':
	    guiControler->setBallMode(GuiControler::orbit);
	    break;
	case VK_SPACE:
	    viewPoint->position.points[0]= 0.0f;
	    viewPoint->position.points[1]= 250.0f;
	    viewPoint->position.points[2]= 500.0f;
	    viewPoint->orientation.points[0]= 0.0f;
	    viewPoint->orientation.points[1]= 0.0f;
	    viewPoint->orientation.points[2]= 1.0f;
	    viewPoint->orientation.points[3]= 0.0f;
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	    gluLookAt(viewPoint->position.points[0], viewPoint->position.points[1], viewPoint->position.points[2],
			    0,
			    0,
			    -1,
			    0.0f, 1.0f, 0.0f);
	    guiControler->locateAt(viewPoint);
	    InvalidateRect(handle, NULL, FALSE);
	    break;
    }
}


void NT3DWindow::performClose(int exitCode)
{
}


