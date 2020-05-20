/**************************************************
* File: cameraX11.cc.
* Desc: Implementation of the akralog.animation.CameraX11 native methods.
* Module: AkraLog : VrmlSupport.
* Rev: 9 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jmachine.h>
#include <NativeLib/nativity.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <vm/execFrame.h>
#include <vm/stacks.h>
#include <vm/stringSupport.h>

#include "utility.h"
#include "x113DWindow.h"
#include "nativeError.h"
#include "lowSFNode.h"
#include "generalVrml.h"
#include "geometriesVrml.h"
#include "materialVrml.h"
#include "lightingVrml.h"
#include "cameraX11.h"


JVVariable *akralog_animation_CameraX11::llowField;
JVVariable *akralog_animation_CameraX11::widthField;
JVVariable *akralog_animation_CameraX11::heightField;
JVVariable *akralog_animation_CameraX11::vpField;
boolean akralog_animation_CameraX11::mustFetchDict= true;


boolean akralog_animation_CameraX11::fetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (mustFetchDict) {
	llowField= aMachine->getFieldOfClass(aClass, "linkLow");
	widthField= aMachine->getFieldOfClass(aClass, "width");
	heightField= aMachine->getFieldOfClass(aClass, "height");
	vpField= aMachine->getFieldOfClass(aClass, "currentVP");
	mustFetchDict= false;
    }

    return ((llowField != NULL) && (widthField != NULL) && (heightField != NULL) && (vpField != NULL));
}


akralog_animation_CameraX11::akralog_animation_CameraX11(JVInstance *anInstance, JVMachine *aMachine)
{
    javaEquiv= anInstance;
    if (mustFetchDict) {
	if (!fetchDictionary(aMachine, javaEquiv->category())) {
	    throw new NativeError(1);
	}
    }
}


akralog_animation_CameraX11::~akralog_animation_CameraX11(void)
{
    // Anything to do ?
}


JVInstance *akralog_animation_CameraX11::get_this(void)
{
    return javaEquiv;
}


int akralog_animation_CameraX11::get_linkLow(void)
{
    return (int)javaEquiv->getField(llowField);
}


int akralog_animation_CameraX11::get_width(void)
{
    return (int)javaEquiv->getField(widthField);
}


int akralog_animation_CameraX11::get_height(void)
{
    return (int)javaEquiv->getField(heightField);
}


JVInstance *akralog_animation_CameraX11::get_currentVP(void)
{
    return (JVInstance *)javaEquiv->getField(vpField);
}


// Native implemented.
Define_Native(akralog_animation, CameraX11, createLowLevelWindow);
Define_Native(akralog_animation, CameraX11, stackContainer);
Define_Native(akralog_animation, CameraX11, unstackContainer);
Define_Native(akralog_animation, CameraX11, releaseScene0);
Define_Native(akralog_animation, CameraX11, renderScene0);
Define_Native(akralog_animation, CameraX11, orderFront0);
Define_Native(akralog_animation, CameraX11, loopInfinite);
Define_Native(akralog_animation, CameraX11, mlapContainer);
Define_Native(akralog_animation, CameraX11, mlapViewpoint);
Define_Native(akralog_animation, CameraX11, mlapLight);
Define_Native(akralog_animation, CameraX11, mlapAppearance);
Define_Native(akralog_animation, CameraX11, mlapShape);

Define_Native(akralog_animation, CameraX11, mlapV1Container);
Define_Native(akralog_animation, CameraX11, mlapV1Light);
Define_Native(akralog_animation, CameraX11, mlapV1Geometry);
Define_Native(akralog_animation, CameraX11, mlapV1Coordinates);
Define_Native(akralog_animation, CameraX11, mlapV1Transformation);


//    private native int createLowLevelWindow(String aName);
Implement_Native(akralog_animation, CameraX11, createLowLevelWindow)
{
    JVInstance *receiver, *aName, *viewPoint;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    X113DWindow *x11Window;
    char *winName;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
     else gotAProblem= 1;

	// Get <String aName>.
    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((aName= tmpValue->extractInstance()) != NULL) {
	    if (aName != JVAbstractInstance::globalNullObject()) {
		StringSupport::mapStringToNarrowArray(aName, &winName);
	    }
	    else {
		winName= new char[9];
		strcpy(winName, "UNTITLED");
	    }
	 }
	 else gotAProblem= 2;
     }
     else gotAProblem= 2;

    if (gotAProblem != 0) {
	if (gotAProblem > 1) delete cameraX11;
	callFrame->returnInteger(0);
	return 0;
    }

    AkRectangle cameraFrame(0.0f, 0.0f, (float)cameraX11->get_width(), (float)cameraX11->get_height());
    x11Window= new X113DWindow(winName, &cameraFrame);
    x11Window->setJavaEquiv(cameraX11->get_this());
    viewPoint= cameraX11->get_currentVP();
    if ((viewPoint != NULL) && (viewPoint != JVAbstractInstance::globalNullObject())) {
	ViewPointVrml *lowVP= new ViewPointVrml(viewPoint, machine);
	x11Window->useViewPoint(lowVP);
    }
    else {
	x11Window->useViewPoint(new ViewPointVrml());
    }

/* TMP: The deletion of the string cause the heap debug functions to trigger.
    delete[] winName;
*/
    delete cameraX11;
    callFrame->returnInteger((int)x11Window);
    return 0;	
}


//    private native void stackContainer();
Implement_Native(akralog_animation, CameraX11, stackContainer)
{
    JVInstance *receiver, *containerNode;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    vrml_basic_SFNode *container;
    X113DWindow *x11Window;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
	else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((containerNode= tmpValue->extractInstance()) != NULL) {
	    container= new vrml_basic_SFNode(containerNode, machine);
	}
	else gotAProblem= 2;
     }
     else gotAProblem= 2;


    if (gotAProblem != 0) {
	if (gotAProblem == 2) delete cameraX11;
	return 0;
    }

    x11Window= (X113DWindow *)cameraX11->get_linkLow();

    x11Window->pushContainer((ContainerVrml *)container->get_linkLow());

    delete container;
    delete cameraX11;
    return 0;
}


//    private native void unstackContainer();
Implement_Native(akralog_animation, CameraX11, unstackContainer)
{
    JVInstance *receiver;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    X113DWindow *x11Window;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
	else gotAProblem= 1;

    if (gotAProblem != 0) {
	return 0;
    }

    x11Window= (X113DWindow *)cameraX11->get_linkLow();

    x11Window->popContainer();

    delete cameraX11;
    return 0;
}


//    private native void releaseScene0();
Implement_Native(akralog_animation, CameraX11, releaseScene0)
{
    JVInstance *receiver;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    X113DWindow *x11Window;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
	else gotAProblem= 1;

    if (gotAProblem != 0) {
	return 0;
    }
    x11Window= (X113DWindow *)cameraX11->get_linkLow();

    x11Window->deleteSceneTree();
    x11Window->renderTree();

    delete cameraX11;
    return 0;
}


//    private native void renderScene0();
Implement_Native(akralog_animation, CameraX11, renderScene0)
{
    JVInstance *receiver;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    X113DWindow *x11Window;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
	else gotAProblem= 1;

    if (gotAProblem != 0) {
	return 0;
    }
    x11Window= (X113DWindow *)cameraX11->get_linkLow();
    x11Window->renderTree();

    return 0;
}


//    private native void orderFront0();	// Show a window on the screen.
Implement_Native(akralog_animation, CameraX11, orderFront0)
{
    JVInstance *receiver;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    X113DWindow *x11Window;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
	else gotAProblem= 1;

    if (gotAProblem != 0) {
	return 0;
    }

    x11Window= (X113DWindow *)cameraX11->get_linkLow();

    x11Window->orderFrontAndMakeKey();

    delete cameraX11;
    return 0;
}


//    private static native void loopInfinite();
Implement_Native(akralog_animation, CameraX11, loopInfinite)
{
    X113DWindow::globalModalLooper();
    return 0;
}


//    private native void mlapContainer(SFNode aNode, int type);
Implement_Native(akralog_animation, CameraX11, mlapContainer)
{
    JVInstance *receiver, *containerNode;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    X113DWindow *x11Window;
    ContainerVrml *lowContainer;
    int nodeType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
	else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((containerNode= tmpValue->extractInstance()) == NULL) {
	    gotAProblem= 2;
	}
     }
     else gotAProblem= 2;

    tmpValue= callFrame->getLocalValue(2);
    if (tmpValue != NULL) {
	nodeType= tmpValue->extractInteger();
    }
     else gotAProblem= 3;

    if (gotAProblem != 0) {
	if (gotAProblem > 1) delete cameraX11;
	return 0;
    }

    x11Window= (X113DWindow *)cameraX11->get_linkLow();

/* Note: the following is defined in vrml.basic.SFNode, and must be up-to-date:
//	2- Collection types.
    public final int anchorType= 11;
    public final int billboardType= 12;
    public final int collisionType= 13;
    public final int groupType= 14;
    public final int inlineType= 15;
    public final int lodType= 16;
    public final int switchType= 17;
    public final int transformType= 18;
*/
    switch(nodeType) {
	case 11:
	    lowContainer= new AnchorVrml(containerNode, machine);
	    break;
	case 12:
	    lowContainer= new BillboardVrml(containerNode, machine);
	    break;
	case 13:
	    lowContainer= new CollisionVrml(containerNode, machine);
	    break;
	case 14:
	    lowContainer= new GroupVrml(containerNode, machine);
	    break;
	case 15:
	    // TODO.
	    break;
	case 16:
	    lowContainer= new LodVrml(containerNode, machine);
	    break;
	case 17:
	    lowContainer= new SwitchVrml(containerNode, machine);
	    break;
	case 18:
	    lowContainer= new TransformVrml(containerNode, machine);
	    break;
    }
    x11Window->addToScene(lowContainer);

    delete cameraX11;
    return 0;
}


//    private native void mlapViewpoint(SFNode aNode);
Implement_Native(akralog_animation, CameraX11, mlapViewpoint)
{
    JVInstance *receiver, *vpNode;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    int nodeType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 2;
     }
    else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((vpNode= tmpValue->extractInstance()) == NULL) {
	    gotAProblem= 4;
	}
     }
     else gotAProblem= 3;

    if (gotAProblem != 0) {
	if (gotAProblem > 2) delete cameraX11;
	return 0;
    }

    ViewPointVrml *viewPoint= NULL;
    X113DWindow *x11Window= (X113DWindow *)cameraX11->get_linkLow();

    viewPoint= new ViewPointVrml(vpNode, machine);

    if (viewPoint != NULL) {
	x11Window->useViewPoint(viewPoint);
    }

    delete cameraX11;
    return 0;
}


//    private native void mlapLight(SFNode aNode, int type);
Implement_Native(akralog_animation, CameraX11, mlapLight)
{
    JVInstance *receiver, *lightNode;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    int nodeType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 2;
     }
    else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((lightNode= tmpValue->extractInstance()) == NULL) {
	    gotAProblem= 4;
	}
     }
     else gotAProblem= 3;

    tmpValue= callFrame->getLocalValue(2);
    if (tmpValue != NULL) {
	nodeType= tmpValue->extractInteger();
    }
    else gotAProblem= 5;

    if (gotAProblem != 0) {
	if (gotAProblem > 2) delete cameraX11;
	return 0;
    }

    LightVrml *lowLight= NULL;
    X113DWindow *x11Window= (X113DWindow *)cameraX11->get_linkLow();

/* Note: the following is defined in vrml.basic.SFNode, and must be up-to-date:
//	3- Lighting types.
    public final static int directionalLightType= 19;
    public final static int pointLightType= 20;
    public final static int spotLightType= 21;
*/

    switch(nodeType) {
	case 19:
	    lowLight= new DirectionalLightVrml(lightNode, machine);
	    break;
	case 20:
	    lowLight= new PointLightVrml(lightNode, machine);
	    break;
	case 21:
	    lowLight= new SpotLightVrml(lightNode, machine);
	    break;
	default:
	    lowLight= NULL;
	    break;
    }

    if (lowLight != NULL) x11Window->addToScene(lowLight);

    delete cameraX11;
    return 0;
}


//    private native void mlapAppearance(Appearance anAppearance, SFNode aMaterial,  SFNode aTexture, SFNode aTexTransform);
Implement_Native(akralog_animation, CameraX11, mlapAppearance)
{
    JVRunValue *tmpValue;
    JVInstance *anAppear,*aMaterial, *aTexture, *aTexTransform;
    vrml_basic_SFNode  *appearance= NULL, *material= NULL, *texture= NULL, *texTransform= NULL;
    AppearanceVrml *lAppearance;
    MaterialVrml *lMaterial;
    TextureVrml *lTexture;
    TextureTransformVrml *lTexTransform;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((anAppear= tmpValue->extractInstance()) != NULL) {
	    appearance= new vrml_basic_SFNode(anAppear, machine);
	}
	else gotAProblem= 2;
     }
     else gotAProblem= 2;

    tmpValue= callFrame->getLocalValue(2);
    if (tmpValue != NULL) {
	if ((aMaterial= tmpValue->extractInstance()) != NULL) {
	    material= new vrml_basic_SFNode(aMaterial, machine);
	}
	else gotAProblem= 3;
     }
     else gotAProblem= 3;

    tmpValue= callFrame->getLocalValue(3);
    if (tmpValue != NULL) {
	if ((aTexture= tmpValue->extractInstance()) != NULL) {
	    texture= new vrml_basic_SFNode(aTexture, machine);
	}
	else gotAProblem= 4;
     }
     else gotAProblem= 4;

    tmpValue= callFrame->getLocalValue(4);
    if (tmpValue != NULL) {
	if ((aTexTransform= tmpValue->extractInstance()) != NULL) {
	    texTransform= new vrml_basic_SFNode(aTexTransform, machine);
	}
	else gotAProblem= 5;
     }
     else gotAProblem= 5;

    if (gotAProblem != 0) {
	if (texTransform != NULL) delete texTransform;
	if (texture != NULL) delete texture;
	if (material != NULL) delete material;
	if (appearance != NULL) delete appearance;
	return 0;
    }

    if ((lAppearance= (AppearanceVrml *)appearance->get_linkLow()) == NULL) {
	lAppearance= new AppearanceVrml(anAppear, machine);
    }

    if ((lMaterial= (MaterialVrml *)material->get_linkLow()) == NULL) {
	lMaterial= new MaterialVrml(aMaterial, machine);
    }

    if ((lTexture= (TextureVrml *)texture->get_linkLow()) == NULL) {
	lTexture= new TextureVrml(aTexture, machine);
    }

    if ((lTexTransform= (TextureTransformVrml *)texTransform->get_linkLow()) == NULL) {
	lTexTransform= new TextureTransformVrml(aTexTransform, machine);
    }

    lAppearance->setMaterial(lMaterial);
    lAppearance->setTexture(lTexture);
    lAppearance->setTextureTransform(lTexTransform);

    delete texture;
    delete material;
    delete appearance;
    return 0;
}


//    private native void mlapShape(SFNode aNode, Appearance anAppearance, SFNode aGeometry, int geoType);
Implement_Native(akralog_animation, CameraX11, mlapShape)
{
    JVInstance *receiver, *aNode, *anAppear, *aGeometry;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    vrml_basic_SFNode *shape, *appearance, *geometry;
    X113DWindow *x11Window;
    ShapeVrml *lShape;
    AppearanceVrml *lAppearance;
    GeometryVrml *lGeometry;
    int geoType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
    else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((aNode= tmpValue->extractInstance()) != NULL) {
	    shape= new vrml_basic_SFNode(aNode, machine);
	}
	else gotAProblem= 2;
     }
     else gotAProblem= 2;

    tmpValue= callFrame->getLocalValue(2);
    if (tmpValue != NULL) {
	if ((anAppear= tmpValue->extractInstance()) != NULL) {
	    appearance= new vrml_basic_SFNode(anAppear, machine);
	}
	else gotAProblem= 3;
     }
     else gotAProblem= 3;

    tmpValue= callFrame->getLocalValue(3);
    if (tmpValue != NULL) {
	if ((aGeometry= tmpValue->extractInstance()) != NULL) {
	    geometry= new vrml_basic_SFNode(aGeometry, machine);
	}
	else gotAProblem= 4;
     }
     else gotAProblem= 4;

    tmpValue= callFrame->getLocalValue(4);
    if (tmpValue != NULL) {
	geoType= tmpValue->extractInteger();
     }
     else gotAProblem= 5;

    if (gotAProblem != 0) {
	if (gotAProblem > 4) delete geometry;
	if (gotAProblem > 3) delete appearance;
	if (gotAProblem > 2) delete shape;
	if (gotAProblem > 1) delete cameraX11;
	return 0;
    }

    x11Window= (X113DWindow *)cameraX11->get_linkLow();
    if ((lShape= (ShapeVrml *)shape->get_linkLow()) == NULL) {
	lShape= new ShapeVrml(aNode, machine);
	x11Window->addToScene(lShape);
    }

    if ((lAppearance= (AppearanceVrml *)appearance->get_linkLow()) == NULL) {
	lAppearance= new AppearanceVrml(anAppear, machine);
    }

    if ((lGeometry= (GeometryVrml *)geometry->get_linkLow()) == NULL) {
/* Note: the following is defined in vrml.basic.SFNode, and must be up-to-date:
//	1- Geometric types.
    public final int boxType= 1;
    public final int coneType= 2;
    public final int cylinderType= 3;
    public final int elevationGridType= 4;
    public final int extrusionType= 5;
    public final int indexedFaceSetType= 6;
    public final int indexedLineSetType= 7;
    public final int pointSetType= 8;
    public final int sphereType= 9;
    public final int textType= 10;
*/
	switch(geoType) {
	    case 1:
		lGeometry= new BoxVrml(aGeometry, machine);
		break;
	    case 2:
		lGeometry= new ConeVrml(aGeometry, machine);
		break;
	    case 3:
		lGeometry= new CylinderVrml(aGeometry, machine);
		break;
	    case 4:
		lGeometry= new ElevationGridVrml(aGeometry, machine);
		break;
	    case 5:
		lGeometry= new ExtrusionVrml(aGeometry, machine);
		break;
	    case 6:
		lGeometry= new IndexedFaceSetVrml(aGeometry, machine);
		break;
	    case 7:
		lGeometry= new IndexedLineSetVrml(aGeometry, machine);
		break;
	    case 8:
		lGeometry= new PointSetVrml(aGeometry, machine);
		break;
	    case 9:
		lGeometry= new SphereVrml(aGeometry, machine);
		break;
	    case 10:
		lGeometry= new TextVrml(aGeometry, machine);
		break;
	}
    }
    lShape->setAppearance(lAppearance);
    lShape->setGeometry(lGeometry);

    delete geometry;
    delete appearance;
    delete shape;
    delete cameraX11;
    return 0;
}


//    private native void mlapV1Container(SFNode aNode, int type);
Implement_Native(akralog_animation, CameraX11, mlapV1Container)
{
    JVInstance *receiver, *containerNode;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    X113DWindow *x11Window;
    ContainerVrml *lowContainer;
    int nodeType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
	else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((containerNode= tmpValue->extractInstance()) == NULL) {
	    gotAProblem= 2;
	}
     }
     else gotAProblem= 2;

    tmpValue= callFrame->getLocalValue(2);
    if (tmpValue != NULL) {
	nodeType= tmpValue->extractInteger();
    }
     else gotAProblem= 3;

    if (gotAProblem != 0) {
	if (gotAProblem > 1) delete cameraX11;
	return 0;
    }

    x11Window= (X113DWindow *)cameraX11->get_linkLow();

/* Note: the following is defined in vrml.basic.SFNode, and must be up-to-date:
//	2- Collection types.
    public final static int groupV1Type= 109;
    public final static int separatorType= 110;
    public final static int switchV1Type= 111;
    public final static int transformSeparatorType= 112;
    public final static int wwwAnchorType= 113;
*/
#if defined(USE_VRML1)
    switch(nodeType) {
	case 109:
	    lowContainer= new GroupVrml1(containerNode, machine);
	    break;
	case 110:
	    lowContainer= new SeparatorVrml1(containerNode, machine);
	    break;
	case 111:
	    lowContainer= new SwitchVrml1(containerNode, machine);
	    break;
	case 112:
	    lowContainer= new TransformSeparatorVrml1(containerNode, machine);
	    break;
	case 113:
	    lowContainer= new WWWAnchorVrml1(containerNode, machine);
	    break;
    }
    x11Window->addToScene(lowContainer);
#endif

    delete cameraX11;
    return 0;
}

// private native void mlapV1Light(SFNode aNode, int type);
Implement_Native(akralog_animation, CameraX11, mlapV1Light)
{
// TODO.
    return 0;
}

//    private native void mlapV1Geometry(SFNode aNode, int type);
Implement_Native(akralog_animation, CameraX11, mlapV1Geometry)
{
    JVInstance *receiver, *aNode, *anAppear, *aGeometry;
    JVRunValue *tmpValue;
    akralog_animation_CameraX11 *cameraX11;
    vrml_basic_SFNode *shape, *appearance, *geometry;
    X113DWindow *x11Window;
    ShapeVrml *lShape;
    AppearanceVrml *lAppearance;
    GeometryVrml *lGeometry;
    int geoType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    cameraX11= new akralog_animation_CameraX11(receiver, machine);
	}
	else gotAProblem= 1;
     }
    else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((aGeometry= tmpValue->extractInstance()) != NULL) {
	    geometry= new vrml_basic_SFNode(aGeometry, machine);
	}
	else gotAProblem= 4;
     }
     else gotAProblem= 4;

    tmpValue= callFrame->getLocalValue(2);
    if (tmpValue != NULL) {
	geoType= tmpValue->extractInteger();
     }
     else gotAProblem= 5;

    if (gotAProblem != 0) {
	if (gotAProblem > 4) delete geometry;
	if (gotAProblem > 3) delete appearance;
	if (gotAProblem > 2) delete shape;
	if (gotAProblem > 1) delete cameraX11;
	return 0;
    }

    x11Window= (X113DWindow *)cameraX11->get_linkLow();

 #if defined(USE_VRML1)
   if ((lGeometry= (GeometryVrml *)geometry->get_linkLow()) == NULL) {
/* Note: the following is defined in vrml.basic.SFNode, and must be up-to-date:
//	1- Geometric types.
    public final static int asciiTextType= 101;
    public final static int coneV1Type= 102;
    public final static int cubeType= 103;
    public final static int cylinderV1Type= 104;
    public final static int indexedFaceSetV1Type= 105;
    public final static int indexedLineSetV1Type= 106;
    public final static int pointSetV1Type= 107;
    public final static int sphereV1Type= 8;
*/
// RENDU ICI.
	switch(geoType) {
	    case 101:
		lGeometry= new BoxVrml(aGeometry, machine);
		break;
	    case 102:
		lGeometry= new ConeVrml(aGeometry, machine);
		break;
	    case 103:
		lGeometry= new CylinderVrml(aGeometry, machine);
		break;
	    case 4:
		lGeometry= new ElevationGridVrml(aGeometry, machine);
		break;
	    case 5:
		lGeometry= new ExtrusionVrml(aGeometry, machine);
		break;
	    case 6:
		lGeometry= new IndexedFaceSetVrml(aGeometry, machine);
		break;
	    case 7:
		lGeometry= new IndexedLineSetVrml(aGeometry, machine);
		break;
	    case 8:
		lGeometry= new PointSetVrml(aGeometry, machine);
		break;
	    case 9:
		lGeometry= new SphereVrml(aGeometry, machine);
		break;
	    case 10:
		lGeometry= new TextVrml(aGeometry, machine);
		break;
	}
    }
    lShape->setAppearance(lAppearance);
    lShape->setGeometry(lGeometry);
#endif

    delete geometry;
    delete appearance;
    delete shape;
    delete cameraX11;
    return 0;
}

//    private native void mlapV1Coordinates(SFNode aNode, int type);
Implement_Native(akralog_animation, CameraX11, mlapV1Coordinates)
{
    return 0;
}

//    private native void mlapV1Transformation(SFNode aNode, int type);
Implement_Native(akralog_animation, CameraX11, mlapV1Transformation)
{
    return 0;
}

