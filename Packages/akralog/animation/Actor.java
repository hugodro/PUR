/**************************************************
* File: Actor.java.
* Desc: Implementation of the Actor class.
* Module: AkraLog : Animation.
* Rev: 5 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
* Rev: 25 septembre 1998 : REV 0.1 : Hugo DesRosiers.
*	Note: Updated for new geometry hierarchy.
**************************************************/

package akralog.animation;


public class Actor extends java.lang.Object {
    Geometry geometry;		// Actor visual data.

    public Geometry getGeometry() {
	return geometry;
    }


	// Method called to get the actor to do its stuff.
    public void act() {
    }

	// Freezes action.
    public void holdAction() {
    }

	// Cancel acting.
    public void stop() {
    }

}
