/**************************************************
* File: PovPhilemon.java.
* Desc: Test of the animation/vrml support with PovRay rendering, using Superman.
* Module: AkraLog : Animation.
* Rev: 28 avril 1998 : REV 0 : Hugo DesRosiers.
**************************************************/

import akralog.animation.*;
import pur.actors.Philemon;

public class PovPhilemon extends Producer {

    Philemon philemon;

    public PovPhilemon()
    {
	camera= new PovCam();
	philemon= new Philemon();
	scene= philemon.geometry;
    }

    public void animate()
    {
	GenericScene aScene= scene;	// TMP: contourne un 'TODO'.
	aScene.renderOn(camera);
//	camera.renderScene();

	philemon.action();
    }

    public static void main()
    {
	PovPhilemon aDemo= new PovPhilemon();
	aDemo.animate();
    }
}
