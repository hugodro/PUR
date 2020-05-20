/**************************************************
* File: camPovRay.cc.
* Desc: Implementation of the akralog.animation.PovCam native methods.
* Module: AkraLog : VrmlSupport.
* Rev: 22 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jmachine.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <vm/execFrame.h>
#include <vm/stacks.h>
#include <vm/stringSupport.h>
#include <native/nativity.h>
#include <native/nError.h>
#include <vrcore/optiVrml.h>
#include "povExtensions.h"
#include "povEngine.h"
#include "camPovRay.h"


JVVariable *akralog_animation_PovCam::llowField;
JVVariable *akralog_animation_PovCam::formatField;
JVVariable *akralog_animation_PovCam::prefixField;
JVVariable *akralog_animation_PovCam::resoField;
JVVariable *akralog_animation_PovCam::vpField;
bool akralog_animation_PovCam::mustFetchDict= true;


bool akralog_animation_PovCam::fetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (mustFetchDict) {
	llowField= aMachine->getFieldOfClass(aClass, "linkLow");
	formatField= aMachine->getFieldOfClass(aClass, "formatOutput");
	prefixField= aMachine->getFieldOfClass(aClass, "prefixOutput");
	resoField= aMachine->getFieldOfClass(aClass, "resolution");
	vpField= aMachine->getFieldOfClass(aClass, "currentVP");
	mustFetchDict= false;
    }

    return ((llowField != NULL) && (formatField != NULL) && (prefixField != NULL) && (resoField != NULL) && (vpField != NULL));
}


akralog_animation_PovCam::akralog_animation_PovCam(JVInstance *anInstance, JVMachine *aMachine)
{
    javaEquiv= anInstance;
    if (mustFetchDict) {
	if (!fetchDictionary(aMachine, javaEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }
}


akralog_animation_PovCam::~akralog_animation_PovCam(void)
{
    // Anything to do ?
}


JVInstance *akralog_animation_PovCam::get_this(void)
{
    return javaEquiv;
}


int akralog_animation_PovCam::get_linkLow(void)
{
    return (int)javaEquiv->getField(llowField);
}


char *akralog_animation_PovCam::get_formatOutput(void)
{
    JVInstance *tmpString;
    char *result= NULL;

    if ((tmpString= (JVInstance *)javaEquiv->getField(formatField)) != NULL) {
	if (tmpString != JVAbstractInstance::globalNullObject()) {
	    StringSupport::mapStringToNarrowArray(tmpString, &result);
	}
    }

    return result;
}


char *akralog_animation_PovCam::get_prefixOutput(void)
{
    JVInstance *tmpString;
    char *result= NULL;

    if ((tmpString= (JVInstance *)javaEquiv->getField(prefixField)) != NULL) {
	StringSupport::mapStringToNarrowArray(tmpString, &result);
    }

    return result;
}


int akralog_animation_PovCam::get_resolution(unsigned int index)
{
    JVBasicArray *tmpArray;
    int result= 0;

    if ((tmpArray= (JVBasicArray *)javaEquiv->getField(resoField)) != NULL) {
	if (index < tmpArray->nbrElements) {
	    result= (int)tmpArray->getElement(index);
	}
    }
    return result;
}


int akralog_animation_PovCam::get_resolutionLength(void)
{
    JVBasicArray *tmpArray;
    int result= 0;

    if ((tmpArray= (JVBasicArray *)javaEquiv->getField(resoField)) != NULL) {
	result= tmpArray->nbrElements;
    }
    return result;
}


JVInstance * akralog_animation_PovCam::get_currentVP(void)
{
    return (JVInstance *)javaEquiv->getField(vpField);
}


// Native implemented.
Define_Native(akralog_animation, PovCam, createLowLevelDual);
Define_Native(akralog_animation, PovCam, stackContainer);
Define_Native(akralog_animation, PovCam, unstackContainer);
Define_Native(akralog_animation, PovCam, releaseScene0);
Define_Native(akralog_animation, PovCam, renderScene0);
Define_Native(akralog_animation, PovCam, mlapContainer);
Define_Native(akralog_animation, PovCam, mlapViewpoint);
Define_Native(akralog_animation, PovCam, mlapLight);
Define_Native(akralog_animation, PovCam, mlapAppearance);
Define_Native(akralog_animation, PovCam, mlapShape);


//    private native int createLowLevelDual();
Implement_Native(akralog_animation, PovCam, createLowLevelDual)
{
    JVInstance *receiver, *currentVP;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    PovRayEngine *povEngine;
    unsigned int gotAProblem= 0;
    char *format, *prefix;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
	}
	else gotAProblem= 2;
    }
    else gotAProblem= 1;

    if (gotAProblem != 0) {
	if (gotAProblem > 2) delete povCam;
	callFrame->returnInteger(0);
	return 0;
    }

    format= povCam->get_formatOutput();
    prefix= povCam->get_prefixOutput();
    povEngine= new PovRayEngine(format, prefix, povCam->get_resolution(0), povCam->get_resolution(1), povCam->get_resolution(2));
    delete[] prefix;
    delete[] format;

    currentVP= povCam->get_currentVP();
    if ((currentVP != NULL) && (currentVP != JVAbstractInstance::globalNullObject())) {
	povEngine->useViewPoint(new ViewPointVrml(currentVP, machine));
    }
    else {
	povEngine->useViewPoint(new ViewPointVrml());
    }

    delete povCam;
    callFrame->returnInteger((int)povEngine);
    return 0;
}


//    private native void stackContainer();
Implement_Native(akralog_animation, PovCam, stackContainer)
{
    JVInstance *receiver, *containerNode;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    vrml_basic_SFNode *container;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
	}
	else gotAProblem= 2;
     }
    else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((containerNode= tmpValue->extractInstance()) != NULL) {
	    container= new vrml_basic_SFNode(containerNode, machine);
	}
	else gotAProblem= 4;
     }
     else gotAProblem= 3;


    if (gotAProblem != 0) {
	if (gotAProblem > 2) delete povCam;
	return 0;
    }

    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();
    povEngine->pushContainer((ContainerVrml *)container->get_linkLow());

    delete container;
    delete povCam;
    return 0;
}


//    private native void unstackContainer();
Implement_Native(akralog_animation, PovCam, unstackContainer)
{
    JVInstance *receiver;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
	}
	else gotAProblem= 2;
     }
    else gotAProblem= 1;

    if (gotAProblem != 0) {
	return 0;
    }

    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();
    povEngine->popContainer();

    delete povCam;
    return 0;
}


//    private native void releaseScene0();
Implement_Native(akralog_animation, PovCam, releaseScene0)
{
    JVInstance *receiver;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
	}
	else gotAProblem= 2;
     }
    else gotAProblem= 1;

    if (gotAProblem != 0) {
	return 0;
    }

    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();
    povEngine->deleteSceneTree();

    delete povCam;
    return 0;
}


//    private native void renderScene0();
Implement_Native(akralog_animation, PovCam, renderScene0)
{
    JVInstance *receiver;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
	}
	else gotAProblem= 2;
     }
    else gotAProblem= 1;

    if (gotAProblem != 0) {
	return 0;
    }

    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();
    povEngine->renderTree();

    return 0;
}


//    private native void mlapContainer(SFNode aNode, int type);
Implement_Native(akralog_animation, PovCam, mlapContainer)
{
    JVInstance *receiver, *containerNode;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    int nodeType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
	}
	else gotAProblem= 2;
     }
    else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(1);
    if (tmpValue != NULL) {
	if ((containerNode= tmpValue->extractInstance()) == NULL) {
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
	if (gotAProblem > 2) delete povCam;
	return 0;
    }

    ContainerVrml *lowContainer;
    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();

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
	    // TODO: Inline.
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
	default:
	    lowContainer= NULL;
	    break;
    }
    if (lowContainer != NULL) povEngine->addToScene(lowContainer);

    delete povCam;
    return 0;
}


//    private native void mlapViewpoint(SFNode aNode);
Implement_Native(akralog_animation, PovCam, mlapViewpoint)
{
    JVInstance *receiver, *vpNode;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    int nodeType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
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
	if (gotAProblem > 2) delete povCam;
	return 0;
    }

    ViewPointVrml *viewPoint= NULL;
    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();

    viewPoint= new ViewPointVrml(vpNode, machine);

    if (viewPoint != NULL) {
	povEngine->useViewPoint(viewPoint);
    }

    delete vpNode;
    delete povCam;
    return 0;
}


//    private native void mlapLight(SFNode aNode, int type);
Implement_Native(akralog_animation, PovCam, mlapLight)
{
    JVInstance *receiver, *lightNode;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    int nodeType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
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
	if (gotAProblem > 2) delete povCam;
	return 0;
    }

    LightVrml *lowLight= NULL;
    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();

/* Note: the following is defined in vrml.basic.SFNode, and must be up-to-date:
//	3- Lighting types.
    public final static int directionalLightType= 19;
    public final static int pointLightType= 20;
    public final static int spotLightType= 21;
*/

    switch(nodeType) {
	case 19:
	    lowLight= new ExtDirectionalLightVrml(lightNode, machine);
	    break;
	case 20:
	    lowLight= new ExtPointLightVrml(lightNode, machine);
	    break;
	case 21:
	    lowLight= new ExtSpotLightVrml(lightNode, machine);
	    break;
	default:
	    lowLight= NULL;
	    break;
    }

    if (lowLight != NULL) povEngine->addToScene(lowLight);

    delete povCam;
    return 0;
}


//    private native void mlapAppearance(Appearance anAppearance, SFNode aMaterial,  SFNode aTexture, SFNode aTexTransform);
Implement_Native(akralog_animation, PovCam, mlapAppearance)
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
	    if (anAppear != JVAbstractInstance::globalNullObject()) {
		appearance= new vrml_basic_SFNode(anAppear, machine);
	    }
	}
	else gotAProblem= 2;
     }
     else gotAProblem= 1;

    tmpValue= callFrame->getLocalValue(2);
    if (tmpValue != NULL) {
	if ((aMaterial= tmpValue->extractInstance()) != NULL) {
	    if (aMaterial != JVAbstractInstance::globalNullObject()) {
		material= new vrml_basic_SFNode(aMaterial, machine);
	    }
	}
	else gotAProblem= 4;
     }
     else gotAProblem= 3;

    tmpValue= callFrame->getLocalValue(3);
    if (tmpValue != NULL) {
	if ((aTexture= tmpValue->extractInstance()) != NULL) {
	    if (aTexture != JVAbstractInstance::globalNullObject()) {
		texture= new vrml_basic_SFNode(aTexture, machine);
	    }
	}
	else gotAProblem= 6;
     }
     else gotAProblem= 5;

    tmpValue= callFrame->getLocalValue(4);
    if (tmpValue != NULL) {
	if ((aTexTransform= tmpValue->extractInstance()) != NULL) {
	    if (aTexTransform != JVAbstractInstance::globalNullObject()) {
		texTransform= new vrml_basic_SFNode(aTexTransform, machine);
	    }
	}
	else gotAProblem= 8;
     }
     else gotAProblem= 7;

    if (gotAProblem != 0) {
	if (texTransform != NULL) delete texTransform;
	if (texture != NULL) delete texture;
	if (material != NULL) delete material;
	if (appearance != NULL) delete appearance;
	return 0;
    }

    if (appearance != NULL) {
	if ((lAppearance= (AppearanceVrml *)appearance->get_linkLow()) == NULL) {
	    lAppearance= new ExtAppearanceVrml(anAppear, machine);
	}

	if (material != NULL) {
	    if ((lMaterial= (MaterialVrml *)material->get_linkLow()) == NULL) {
		lMaterial= new MaterialVrml(aMaterial, machine);
	    }
	}
    
	if (texture != NULL) {
	    if ((lTexture= (TextureVrml *)texture->get_linkLow()) == NULL) {
		lTexture= new TextureVrml(aTexture, machine);
	    }
	}
    
	if (texTransform != NULL) {
	    if ((lTexTransform= (TextureTransformVrml *)texTransform->get_linkLow()) == NULL) {
		lTexTransform= new TextureTransformVrml(aTexTransform, machine);
	    }
	}
    
	lAppearance->setMaterial(lMaterial);
	lAppearance->setTexture(lTexture);
	lAppearance->setTextureTransform(lTexTransform);
    }

    if (texTransform != NULL) delete texTransform;
    if (texture != NULL) delete texture;
    if (material != NULL) delete material;
    if (appearance != NULL) delete appearance;
    return 0;
}


//    private native void mlapShape(SFNode aNode, Appearance anAppearance, SFNode aGeometry, int geoType);
Implement_Native(akralog_animation, PovCam, mlapShape)
{
    JVInstance *receiver, *aNode, *anAppear, *aGeometry;
    JVRunValue *tmpValue;
    akralog_animation_PovCam *povCam;
    vrml_basic_SFNode *shape, *appearance, *geometry;
    ShapeVrml *lShape;
    AppearanceVrml *lAppearance;
    GeometryVrml *lGeometry;
    int geoType;
    unsigned int gotAProblem= 0;

    tmpValue= callFrame->getLocalValue(0);
    if (tmpValue != NULL) {
	if ((receiver= tmpValue->extractInstance()) != NULL) {
	    povCam= new akralog_animation_PovCam(receiver, machine);
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
	if (gotAProblem > 1) delete povCam;
	return 0;
    }

    PovRayEngine *povEngine= (PovRayEngine *)povCam->get_linkLow();
    if ((lShape= (ShapeVrml *)shape->get_linkLow()) == NULL) {
	lShape= new ShapeVrml(aNode, machine);
	povEngine->addToScene(lShape);
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
		lGeometry= new ExtBoxVrml(aGeometry, machine);
		break;
	    case 2:
		lGeometry= new ExtConeVrml(aGeometry, machine);
		break;
	    case 3:
		lGeometry= new ExtCylinderVrml(aGeometry, machine);
		break;
	    case 4:
		lGeometry= new ExtElevationGridVrml(aGeometry, machine);
		break;
	    case 5:
		lGeometry= new ExtExtrusionVrml(aGeometry, machine);
		break;
	    case 6:
		lGeometry= new ExtIndexedFaceSetVrml(aGeometry, machine);
		break;
	    case 7:
		lGeometry= new ExtIndexedLineSetVrml(aGeometry, machine);
		break;
	    case 8:
		lGeometry= new ExtPointSetVrml(aGeometry, machine);
		break;
	    case 9:
		lGeometry= new ExtSphereVrml(aGeometry, machine);
		break;
	    case 10:
		lGeometry= new ExtTextVrml(aGeometry, machine);
		break;
	    default:
		lGeometry= NULL;
		break;
	}
    }
    lShape->setAppearance(lAppearance);
    lShape->setGeometry(lGeometry);

    delete geometry;
    delete appearance;
    delete shape;
    delete povCam;
    return 0;
}


