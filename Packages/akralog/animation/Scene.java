/**************************************************
* File: Scene.java.
* Desc: Implementation of the Scene class.
* Module: AkraLog : Animation.
* Rev: 31 juillet 1997 : REV 0 : Hugo DesRosiers : Creation.
* Rev: 5 decembre 1997 : REV 0.1 : Hugo DesRosiers.
*	Note: Update and revision for a better animation/vrml support.
* Rev: 24 septembre 1998 : REV 0.2 : Hugo DesRosiers.
*	Note: Becomes a real 'scene', not a geometry anymore.
**************************************************/

package akralog.animation;

import vrml.basic.SFNode;
import vrml.nodes.Group;
import java.util.Vector;


public class Scene {
    RenderingDevice renderDevice;	// TODO: Update to support multiple rendering devices.
    Vector actors;			// Array of Actor instances.
    Vector props;			// Array of Property instances.
    Vector lights;			// Array of Light instances.


    public Scene() {
	actors= new Vector(5);
	props= new Vector(5);
	lights= new Vector(5);
    }


    public void addActor(Actor anActor) {
	actors.addElement(anActor);
    }


    public void addProp(Property aProperty) {
	props.addElement(aProperty);
    }


    public void addLight(Light aLight) {
	lights.addElement(aLight);
    }


    public boolean renderOn(RenderingDevice aDevice) {
	if (aDevice == renderDevice) return true;	// Do nothing, the device is already there.

	// First, tell the old device to stop doing the rendering.
	if (renderDevice != null) {
	    renderDevice.relenquishRenderingFor(this);
	}

	// Then, put the new device as the renderer.
	renderDevice= aDevice;
	renderDevice.prepareRenderingFor(this);

	// If the renderer optimizes the scene internally, give it.
	if (renderDevice.wantsScene()) {
	    int nbrActors, nbrProps, nbrLights;

	    nbrActors= actors.size();
	    nbrProps= props.size();
	    nbrLights= lights.size();

	    if ((nbrActors + nbrProps + nbrLights) > 0) {
		Group sceneNodes;
		int i;

		sceneNodes= new Group();
		sceneNodes.children= new SFNode[nbrActors + nbrProps + nbrLights];
		
		for (i= 0; i < nbrActors; i++) {
		    sceneNodes.children[i]= (SFNode)actors.elementAt(i).getGeometry().getRoot();
		}
		for (i= 0; i < nbrProps; i++) {
		    sceneNodes.children[nbrActors + i]= (SFNode)props.elementAt(i).getGeometry().getRoot();
		}
		for (i= 0; i < nbrLights; i++) {
		    sceneNodes.children[nbrActors + nbrProps + i]= (SFNode)lights.elementAt(i).getGeometry().getRoot();
		}

		sceneNodes.uploadTo(renderDevice);
	    }
	}

	return true;
    }


    public int getNodesCount() {
	return actors.size() + props.size() + lights.size();
    }

}

