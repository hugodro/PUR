/**************************************************
* File: CameraNT.java.
* Desc: Implementation of the CameraNT class.
* Module: AkraLog : Animation.
* Rev: 5 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

package akralog.animation;

import vrml.basic.SFNode;
import vrml.nodes.*;


public class CameraNT extends RenderingDevice {
    private int linkLow;	// Link to low-level equivalent.

    int rootCount;
    int width, height;		// Size of the window frame.

    Viewpoint currentVP;		// Viewpoint of the camera.

	// Class initialization.
    static {
	try {
	    System.loadLibrary( "vrmlsup" );
	} catch ( UnsatisfiedLinkError e) {
	    System.out.println( "CameraNT: can't get the necessary native library.");
	}
    }


    public static boolean showApplicationContext()
    {
	// TODO: Create an icon, an application menu.
	return true;
    }


    public static int runModalLoop()
    {
	// TODO: Start a modal loop session, until the 'terminate' is received.
	loopInfinite();
	return 0;		// TODO: return some meaningfull return code.
    }


    static void terminate()
    {
	// TODO: Indicate that the model loop session is over, must quit.
    }


    public CameraNT(String aName, int aWidth, int aHeight)
    {
	category= live;
	width= aWidth;
	height= aHeight;
	currentVP= new Viewpoint();

	linkLow= createLowLevelWindow(aName);
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

	// Support for V1 and Inventor nodes.
		// Grouping nodes.
	    case SFNode.groupV1Type:
	    case SFNode.transformSeparatorType:
	    case SFNode.separatorType:
	    case SFNode.switchV1Type:
	    case SFNode.wwwAnchorType:
		mlapV1Container(aNode, nodeType);
		break;
	    case SFNode.lodV1Type:
		    // TODO.
		break;

		// Lighting types.
	    case SFNode.directionalLightV1Type:
	    case SFNode.pointLightV1Type:
	    case SFNode.spotLightV1Type:
		mlapV1Light(aNode, nodeType);
		break;

		// Geometries.
	    case SFNode.asciiTextType:
	    case SFNode.coneV1Type:
	    case SFNode.cubeType:
	    case SFNode.cylinderV1Type:
	    case SFNode.indexedFaceSetV1Type:
	    case SFNode.indexedLineSetV1Type:
	    case SFNode.pointSetV1Type:
	    case SFNode.sphereV1Type:
		mlapV1Geometry(aNode, nodeType);
		break;

		// Coordinate dictionary.
	    case SFNode.coordinate3Type:
		mlapV1Geometry(aNode, nodeType);
		break;

		// Space transformations.
	    case SFNode.matrixTransformType:
	    case SFNode.rotationType:
	    case SFNode.scaleType:
	    case SFNode.transformV1Type:
	    case SFNode.translationType:
		mlapV1Transformation(aNode, nodeType);
		break;
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

    public void orderFront()
    {
	orderFront0();
    }

    public void renderScene()
    {
	renderScene0();
    }


// Low level support.
    private native int createLowLevelWindow(String aName);    // Create the low-level window, return 0 on failure.
    private native void stackContainer(SFNode aContainer);
    private native void unstackContainer();
    private native void releaseScene0();
    private native void renderScene0();
    private native void orderFront0();	// Show a window on the screen.
    private static native void loopInfinite();	    // Do an 'infinite' gui loop.

	// Optimized nodes mappers.
    private native void mlapContainer(SFNode aNode, int type);
    private native void mlapViewpoint(SFNode aNode);
    private native void mlapLight(SFNode aNode, int type);
    private native void mlapAppearance(Appearance anAppearance, SFNode aMaterial,  SFNode aTexture, SFNode aTexTransform);
    private native void mlapShape(SFNode aNode, Appearance anAppearance, SFNode aGeometry, int geoType);

	// Optimized nodes mappers, V1 support.
    private native void mlapV1Container(SFNode aNode, int type);
    private native void mlapV1Light(SFNode aNode, int type);
    private native void mlapV1Geometry(SFNode aNode, int type);
    private native void mlapV1Coordinates(SFNode aNode, int type);
    private native void mlapV1Transformation(SFNode aNode, int type);
}

