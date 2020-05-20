/**************************************************
* File: GenericScene.java.
* Desc: Implementation of the GenericScene class.
* Module: AkraLog : Animation.
* Rev: 31 juillet 1997 : REV 0 : Hugo DesRosiers : Creation.
* Rev: 5 decembre 1997 : REV 1 : Hugo DesRosiers.
*	Note: Update and revision for a better animation/vrml support.
**************************************************/

package akralog.animation;

import vrml.basic.SFNode;
import vrml.nodes.Group;


public class GenericScene {
    RenderingDevice renderDevice;
    Actor[] actors;
    Property[] props;
    Group sceneNodes;
    SFNode[] rootNodes;	// TMP: The vrml node tree is put there.

    public GenericScene() {
    }

    public boolean renderOn(RenderingDevice aDevice)
    {
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
	    if (rootNodes != null) {
	        sceneNodes= new Group();
		sceneNodes.children= rootNodes;
		sceneNodes.uploadTo(renderDevice);
	    }
	}
	return true;
    }

    public int getRootCount()
    {
	return rootNodes.length;
    }


    public void archive() {
    }

}

