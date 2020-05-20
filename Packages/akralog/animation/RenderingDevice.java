/**************************************************
* File: RenderingDevice.java.
* Desc: Implementation of the RenderingDevice class.
* Module: AkraLog : Animation.
* Rev: 5 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

package akralog.animation;

import vrml.basic.SFNode;


public class RenderingDevice extends java.lang.Object {
    int category;		// Category of the device (live, imager, translator).
    int capabilities;		// Capabilities (category dependant).

	// Categories.
    public final int live= 1;	// Interactive rendering.
    public final int imager= 2;	// Off-line imager.
    public final int translator= 4;	// Rewriter into some other language/format.

    public RenderingDevice()
    {
    }

	// Indicates if the device does an internal optimization of the scene.
    public boolean wantsScene()
    {
	return false;
    }

    public boolean prepareRenderingFor(GenericScene aScene)
    {
	return true;
    }

    public boolean relenquishRenderingFor(GenericScene aScene)
    {
	return true;
    }

    public void load(SFNode aNode)
    {
	// Load a node in scene into the local context.
    }

	// Start/End a local group of children nodes.
    public void beginNodeSet(SFNode aContainer)
    {
    }

    public void endNodeSet(SFNode aContainer)
    {
    }

	// Includes user-defined extensions.
    public boolean loadExtension(String aName)
    {
	return false;	// Always fail, since nothing is done.
    }

    public boolean associateLowType(String aName, int aTypeID)
    {
	return false;	// Always fail, since nothing is done.
    }

    public void renderScene()
    {
    }
}
