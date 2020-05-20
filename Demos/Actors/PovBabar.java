/**************************************************
* File: PovBabar.java.
* Desc: Test of the animation/vrml support with PovRay rendering, using Babar.
* Module: AkraLog : Animation.
* Rev: 23 septembre 1998 : REV 0 : Hugo DesRosiers.
**************************************************/

import akralog.animation.*;
import akralog.animation.demos.Babar;

public class PovBabar extends Producer {

    Babar babar;

    public PovBabar()
    {
	camera= new PovCam();
	babar= new Babar();
	scene= babar.geometry;
    }

    public void animate()
    {
	GenericScene aScene= scene;	// TMP: contourne un 'TODO'.
	aScene.renderOn(camera);
//	camera.renderScene();

	babar.action();
    }

    public static void main()
    {
	PovBabar aDemo= new PovBabar();
	aDemo.animate();
    }
}
