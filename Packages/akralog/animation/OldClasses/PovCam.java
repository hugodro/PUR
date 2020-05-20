/**************************************************
* File: PovCam.java.
* Desc: Implementation of the PovCam class.
* Module: AkraLog : Animation.
* Rev: 22 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

package akralog.animation;

import vrml.basic.SFNode;
import vrml.nodes.*;


public class PovCam extends RenderingDevice {
    int linkLow;
    String formatOutput;		// Will decide on the output suffix.
    String prefixOutput;		// Start of the name of output data (for now, a file path).
    int resolution[];			// 0: x, 1: y, 2: pixel depth.
    Viewpoint currentVP;

	// Class initialization.
    static {
	try {
	    System.loadLibrary( "povsup" );
	} catch ( UnsatisfiedLinkError e) {
	    System.out.println( "PovCam: can't get the necessary native library.");
	}
    }


    public PovCam()
    {
	category= imager;
	formatOutput= "ppm";
	prefixOutput= "output";
	resolution= new int[3];
	resolution[0]= 320;
	resolution[1]= 200;
	resolution[2]= 24;
	linkLow= createLowLevelDual();
    }


    public boolean wantsScene()
    {
	return true;	// We keep an internal rep.
    }


    public boolean prepareRenderingFor(GenericScene aScene)
    {
	releaseScene0();
	return true;
    }


    public boolean relenquishRenderingFor(GenericScene aScene)
    {
	// TODO.
	return true;
    }


    public void load(SFNode aNode)
    {
	int nodeType;

	if (aNode.isLinkedLow()) return;		// Warning: quick exit.

	nodeType= aNode.nodeType();
	switch (nodeType) {
		// Collection types.
	    case SFNode.collisionType:
		    // TODO.
		break;
	    case SFNode.inlineType:
		    // TODO.
		break;
	    case SFNode.lodType:
		    // TODO.
		break;
	    case SFNode.anchorType:
	    case SFNode.billboardType:
	    case SFNode.groupType:
	    case SFNode.switchType:
	    case SFNode.transformType:
		mlapContainer(aNode, nodeType);
		break;

		// Lighting types.
	    case SFNode.directionalLightType:
	    case SFNode.pointLightType:
	    case SFNode.spotLightType:
		mlapLight(aNode, nodeType);
		break;

		// Shape:
	    case SFNode.shapeType:
		Appearance anAppearance= (Appearance)((Shape)aNode).appearance;
		mlapAppearance(anAppearance, anAppearance.material, anAppearance.texture, anAppearance.textureTransform);
		mlapShape(aNode, anAppearance, ((Shape)aNode).geometry, ((Shape)aNode).geometry.nodeType());
		break;

	    case SFNode.viewpointType:
		mlapViewpoint(aNode);
		break;

		// TODO: Take care of the rest of the nodes.

	}
    }


    public void beginNodeSet(SFNode aContainer)
    {
	stackContainer(aContainer);
    }

    public void endNodeSet(SFNode aContainer)
    {
	unstackContainer();
    }

    public void renderScene()
    {
	renderScene0();
    }

// Low level support.
    private native int createLowLevelDual();
    private native void stackContainer(SFNode aContainer);
    private native void unstackContainer();
    private native void releaseScene0();
    private native void renderScene0();

	// Optimized nodes mappers.
    private native void mlapContainer(SFNode aNode, int type);
    private native void mlapViewpoint(SFNode aNode);
    private native void mlapLight(SFNode aNode, int type);
    private native void mlapAppearance(Appearance anAppearance, SFNode aMaterial,  SFNode aTexture, SFNode aTexTransform);
    private native void mlapShape(SFNode aNode, Appearance anAppearance, SFNode aGeometry, int geoType);
}
