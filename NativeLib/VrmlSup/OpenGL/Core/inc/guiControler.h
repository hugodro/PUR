#ifndef _GUICONTROLER_H_
#define _GUICONTROLER_H_
/**************************************************
* File: guiControler.h.
* Desc: Definition of the GuiControler class.
* Module: AkraLog : VrmlSupport.
* Rev: 15 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/portableDefs.h>
#include <vrcore/geometry3D.h>


class ViewPointVrml;


class GuiControler {
  public:
    enum BallMode {
	pan, travelling, rotate, orbit
    };

  protected:	    // Instance variables.
    float wCenter, hCenter;
    float mouseX, mouseY;	// To follow the mouse mouvements as delta.
    BallMode ballMode;	    	// Mode of tracking of the controler.
    float movingStep;
    float globeRadius;		// Radius of the globe that can represent angles.
    AkCoord3D camTarget;
    TrMatrix camera;

  public:	    // Instance methods.
    GuiControler(int width, int height);
    void resize(int width, int height);
    void setBallMode(BallMode aMode);
    BallMode getBallMode(void);
    bool setMouseAt(int posX, int posY);
    void moveMouseTo(int posX, int posY, ViewPointVrml *vp);
    void locateAt(ViewPointVrml *aVP);
};


#endif	    /* _GUICONTROLER_H_ */
