/**************************************************
* File: PovClark.java.
* Desc: Test of the animation/vrml support with PovRay rendering, using Superman.
* Module: AkraLog : Animation.
* Rev: 28 avril 1998 : REV 0 : Hugo DesRosiers.
**************************************************/

import akralog.animation.*;
import akralog.animation.demos.Superman;

public class PovClark extends Producer {

    Superman clark;

    public PovClark()
    {
	camera= new PovCam();
	clark= new Superman();
	scene= clark.geometry;
    }

    public void animate()
    {
	GenericScene aScene= scene;	// TMP: contourne un 'TODO'.
	aScene.renderOn(camera);
//	camera.renderScene();

	clark.action();
    }

    public static void main()
    {
	PovClark aDemo= new PovClark();
	aDemo.animate();
    }
}
