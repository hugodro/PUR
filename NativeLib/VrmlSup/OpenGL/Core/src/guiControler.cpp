/**************************************************
* File: guiControler.cc.
* Desc: Definition of the GuiControler class.
* Module: AkraLog : VrmlSupport.
* Rev: 15 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <math.h>
#if defined(DEBUG_VP)
#include <stdio.h>
#endif

#include <vrcore/generalVrml.h>
#include "guiControler.h"

#define min(a, b)	(((a) < (b)) ? (a) : (b))

GuiControler::GuiControler(int width, int height)
{
    wCenter= (float)width/2.0f;
    hCenter= (float)height/2.0f;
    globeRadius= (float)min(width, height)/2.0f;
    globeRadius*= globeRadius;
    ballMode= pan;
    movingStep= 1.0f;
    camTarget.points[2]= -1.0f;
}


void GuiControler::resize(int width, int height)
{
    wCenter= (float)width/2.0f;
    hCenter= (float)height/2.0f;
    globeRadius= (float)min(width, height)/2.0f;
    globeRadius*= globeRadius;
    ballMode= pan;
    movingStep= 1.0f;
    camTarget.points[2]= -1.0f;
}


void GuiControler::setBallMode(GuiControler::BallMode aMode)
{
    ballMode= aMode;
}


GuiControler::BallMode GuiControler::getBallMode(void)
{
    return ballMode;
}


bool GuiControler::setMouseAt(int posX, int posY)
{
    if ((ballMode != rotate) && (ballMode != orbit)) {
	mouseX= (float)posX;
	mouseY= (float)posY;
	return true;	// Warning: quick exit.
    }
    else if ((mouseX*mouseX + mouseY*mouseY) <= globeRadius) {
	mouseX= (float)posX;
	mouseY= (float)posY;
        return true;
    }
    return false;
}


void GuiControler::moveMouseTo(int posX, int posY, ViewPointVrml *vp)
{
    float distX, distY;
    
    distX= ((float)posX - mouseX) * movingStep;
    distY= ((float)posY - mouseY) * movingStep;

#if defined(DEBUG_VP)
printf("mmt: Dist: <%f, %f>, ballMode: %d, ", distX, distY, ballMode);
printf("\n\t\tvp: p=[%f, %f, %f] ; o=[%f, %f, %f, %f]\n", vp->position.points[0], vp->position.points[1], vp->position.points[2],
	    vp->orientation.points[0], vp->orientation.points[1], vp->orientation.points[2], vp->orientation.points[3]);
printf("\n\t\tvp: cam=[%f, %f, %f]\n", camera.M[0][3], camera.M[1][3], camera.M[2][3]);
#endif
    switch(ballMode) {
	case pan:
		// Move on plane created by x-y axis of vision orientation.
	    if ((distX != 0) || (distY != 0)) {
		AkCoord3D aPt(distX, distY, 0.0f);

		camera.transformPoint(&aPt);
		aPt.points[0]-= (float)camera.M[0][3];
		aPt.points[1]-= (float)camera.M[1][3];
		aPt.points[2]-= (float)camera.M[2][3];
		camera.translateBy(&aPt);
		vp->position.points[0]= (float)camera.M[0][3];
		vp->position.points[1]= (float)camera.M[1][3];
		vp->position.points[2]= (float)camera.M[2][3];
	    }
	    break;
	case travelling:
		// Move back/forth on z axis of vision orientation.
	    if (distY != 0) {
		AkCoord3D aPt(0.0f, 0.0f, -distY);

		camera.transformPoint(&aPt);
		aPt.points[0]-= (float)camera.M[0][3];
		aPt.points[1]-= (float)camera.M[1][3];
		aPt.points[2]-= (float)camera.M[2][3];
		camera.translateBy(&aPt);
		vp->position.points[0]= (float)camera.M[0][3];
		vp->position.points[1]= (float)camera.M[1][3];
		vp->position.points[2]= (float)camera.M[2][3];
	    }
	    break;
	case rotate:
	    // Change vision orientation angle, according to ball radius.
	    if ((distX != 0) || (distY != 0)) {
		Vector4 rotX, rotY;
		Vector3 offLate(camera.M[0][3], camera.M[1][3], camera.M[2][3]);
		Vector3 initVP(0.0, 0.0, -1.0);

		distX= distX / globeRadius * 180.0f / 3.141592654f;
		distY= distY / globeRadius * 180.0f / 3.141592654f;
		rotX.setTo(1.0, 0.0, 0.0, distX);
		rotY.setTo(0.0, 1.0, 0.0, distY);

		camera.M[0][3]= 0.0; camera.M[1][3]= 0.0; camera.M[2][3]= 0.0;
		camera.rotateBy(&rotX);
		camera.rotateBy(&rotY);

		camera.transformPoint(&initVP);
		double length= sqrt(initVP.Vc[0] * initVP.Vc[0] + initVP.Vc[1]*initVP.Vc[1] + initVP.Vc[2] * initVP.Vc[2]);
		vp->orientation.points[0]= (float)(initVP.Vc[1] / length);
		vp->orientation.points[1]= (float)(-initVP.Vc[0] / length);
		vp->orientation.points[2]= 0.0f;
		vp->orientation.points[3]= (float)acos(-initVP.Vc[2] / length);
		camera.translateBy(&offLate);
	    }
	    break;
	case orbit:
		// Change position and vision orientation angle, according to ball radius, keeping same vision target.
	    if ((distX != 0) || (distY != 0)) {
		Vector4 rotX, rotY;
		Vector3 initVP(0.0, 0.0, -1.0);

		distX= distX / globeRadius * 180.0f / 3.141592654f;
		distY= distY / globeRadius * 180.0f / 3.141592654f;
		rotX.setTo(1.0, 0.0, 0.0, distX);
		rotY.setTo(0.0, 1.0, 0.0, distY);

		camera.rotateBy(&rotX);
		camera.rotateBy(&rotY);

		camera.transformPoint(&initVP);
		double length= sqrt(initVP.Vc[0] * initVP.Vc[0] + initVP.Vc[1]*initVP.Vc[1] + initVP.Vc[2] * initVP.Vc[2]);
		vp->orientation.points[0]= (float)(initVP.Vc[1] / length);
		vp->orientation.points[1]= (float)(-initVP.Vc[0] / length);
		vp->orientation.points[2]= 0.0f;
		vp->orientation.points[3]= (float)acos(-initVP.Vc[2] / length);
	    }
	    break;
    };

#if defined(DEBUG_VP)
printf("\t\tvp': p=[%f, %f, %f] ; o=[%f, %f, %f, %f]\n", vp->position.points[0], vp->position.points[1], vp->position.points[2],
	    vp->orientation.points[0], vp->orientation.points[1], vp->orientation.points[2], vp->orientation.points[3]);
printf("\n\t\tvp: cam=[%f, %f, %f]\n", camera.M[0][3], camera.M[1][3], camera.M[2][3]);
#endif
    mouseX= (float)posX;
    mouseY= (float)posY;
}


void GuiControler::locateAt(ViewPointVrml *aVP)
{
    for (unsigned int i= 0; i < 4; i++) {
        camera.M[i][0]= camera.M[i][1]= camera.M[i][2]= camera.M[i][3]= 0.0;
    }
    camera.M[0][0]= camera.M[1][1]= camera.M[2][2]= camera.M[3][3]= 1.0;
    camera.rotateBy(&aVP->orientation);
    camera.translateBy(&aVP->position);
}
