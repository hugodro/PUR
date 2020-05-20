/**************************************************
* File: Light.java.
* Desc: Implementation of the Light class.
* Module: AkraLog : Animation.
* Rev: 24 septembre 1998 : REV 0 : Hugo DesRosiers.
**************************************************/

package akralog.animation;


public class Light {

    SFNode vrmlObject;		// The object should be a valid 'Light' object in VRML.

    public Light(SFNode anObject) {
	vrmlObject= anObject;
    }
}
