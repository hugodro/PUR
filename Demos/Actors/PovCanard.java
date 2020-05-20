/**************************************************
* File: PovCanard.java.
* Desc: Test of the animation/vrml support with PovRay rendering, using the canard.
* Module: AkraLog : Animation.
* Rev: 28 avril 1998 : REV 0 : Hugo DesRosiers.
**************************************************/

import akralog.animation.*;
import akralog.animation.demos.Arthur;

public class PovCanard extends Producer {

    Arthur arthur;

    public PovCanard()
    {
	camera= new PovCam();
	arthur= new Arthur();
	scene= arthur.geometry;
    }


    public static void main()
    {
	PovCanard aDemo= new PovCanard();
	GenericScene aScene= scene;	// TMP: contourne un 'TODO'.
	aScene.renderOn(camera);
//	camera.renderScene();

	arthur.action();
    }
}
