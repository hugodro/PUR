/**************************************************
* File: povScene.cc.
* Desc: Implementation of the PovScene class.
* Module: AkraLog : PovRayExt.
* Rev: 10 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <VrmlParser/v2OptimumDefs.h>
#include "renderContext.h"
#include "glue.h"
#include "povScene.h"


PovScene::mapArrayPtr PovScene::mapArray= NULL;


PovScene::PovScene()
{
    contexts= NULL;
}


void PovScene::loadMapOperators()
{
    mapArray[V2OptiDef::v2Anchor]= &PovScene::mp_v2Anchor;
    mapArray[V2OptiDef::v2Appearance]= &PovScene::mp_v2Appearance;
    mapArray[V2OptiDef::v2AudioClip]= &PovScene::mp_v2AudioClip;
    mapArray[V2OptiDef::v2Background]= &PovScene::mp_v2Background;
    mapArray[V2OptiDef::v2Billboard]= &PovScene::mp_v2Billboard;
    mapArray[V2OptiDef::v2Box]= &PovScene::mp_v2Box;
    mapArray[V2OptiDef::v2Collision]= &PovScene::mp_v2Collision;
    mapArray[V2OptiDef::v2Color]= &PovScene::mp_v2Color;
    mapArray[V2OptiDef::v2ColorInterpolator]= &PovScene::mp_v2ColorInterpolator;
    mapArray[V2OptiDef::v2Cone]= &PovScene::mp_v2Cone;
    mapArray[V2OptiDef::v2Coordinate]= &PovScene::mp_v2Coordinate;
    mapArray[V2OptiDef::v2CoordinateInterpolator]= &PovScene::mp_v2CoordinateInterpolator;
    mapArray[V2OptiDef::v2Cylinder]= &PovScene::mp_v2Cylinder;
    mapArray[V2OptiDef::v2CylinderSensor]= &PovScene::mp_v2CylinderSensor;
    mapArray[V2OptiDef::v2DirectionalLight]= &PovScene::mp_v2DirectionalLight;
    mapArray[V2OptiDef::v2ElevationGrid]= &PovScene::mp_v2ElevationGrid;
    mapArray[V2OptiDef::v2Extrusion]= &PovScene::mp_v2Extrusion;
    mapArray[V2OptiDef::v2Fog]= &PovScene::mp_v2Fog;
    mapArray[V2OptiDef::v2FontStyle]= &PovScene::mp_v2FontStyle;
    mapArray[V2OptiDef::v2Group]= &PovScene::mp_v2Group;
    mapArray[V2OptiDef::v2ImageTexture]= &PovScene::mp_v2ImageTexture;
    mapArray[V2OptiDef::v2IndexedFaceSet]= &PovScene::mp_v2IndexedFaceSet;
    mapArray[V2OptiDef::v2IndexedLineSet]= &PovScene::mp_v2IndexedLineSet;
    mapArray[V2OptiDef::v2Inline]= &PovScene::mp_v2Inline;
    mapArray[V2OptiDef::v2LOD]= &PovScene::mp_v2LOD;
    mapArray[V2OptiDef::v2Material]= &PovScene::mp_v2Material;
    mapArray[V2OptiDef::v2MovieTexture]= &PovScene::mp_v2MovieTexture;
    mapArray[V2OptiDef::v2NavigationInfo]= &PovScene::mp_v2NavigationInfo;
    mapArray[V2OptiDef::v2Normal]= &PovScene::mp_v2Normal;
    mapArray[V2OptiDef::v2NormalInterpolator]= &PovScene::mp_v2NormalInterpolator;
    mapArray[V2OptiDef::v2OrientationInterpolator]= &PovScene::mp_v2OrientationInterpolator;
    mapArray[V2OptiDef::v2PixelTexture]= &PovScene::mp_v2PixelTexture;
    mapArray[V2OptiDef::v2PlaneSensor]= &PovScene::mp_v2PlaneSensor;
    mapArray[V2OptiDef::v2PointLight]= &PovScene::mp_v2PointLight;
    mapArray[V2OptiDef::v2PointSet]= &PovScene::mp_v2PointSet;
    mapArray[V2OptiDef::v2PositionInterpolator]= &PovScene::mp_v2PositionInterpolator;
    mapArray[V2OptiDef::v2ProximitySensor]= &PovScene::mp_v2ProximitySensor;
    mapArray[V2OptiDef::v2ScalarInterpolator]= &PovScene::mp_v2ScalarInterpolator;
    mapArray[V2OptiDef::v2Script]= &PovScene::mp_v2Script;
    mapArray[V2OptiDef::v2Shape]= &PovScene::mp_v2Shape;
    mapArray[V2OptiDef::v2Sound]= &PovScene::mp_v2Sound;
    mapArray[V2OptiDef::v2Sphere]= &PovScene::mp_v2Sphere;
    mapArray[V2OptiDef::v2SphereSensor]= &PovScene::mp_v2SphereSensor;
    mapArray[V2OptiDef::v2SpotLight]= &PovScene::mp_v2SpotLight;
    mapArray[V2OptiDef::v2Switch]= &PovScene::mp_v2Switch;
    mapArray[V2OptiDef::v2Text]= &PovScene::mp_v2Text;
    mapArray[V2OptiDef::v2TextureCoordinate]= &PovScene::mp_v2TextureCoordinate;
    mapArray[V2OptiDef::v2TextureTransform]= &PovScene::mp_v2TextureTransform;
    mapArray[V2OptiDef::v2TimeSensor]= &PovScene::mp_v2TimeSensor;
    mapArray[V2OptiDef::v2TouchSensor]= &PovScene::mp_v2TouchSensor;
    mapArray[V2OptiDef::v2Transform]= &PovScene::mp_v2Transform;
    mapArray[V2OptiDef::v2Viewpoint]= &PovScene::mp_v2Viewpoint;
    mapArray[V2OptiDef::v2VisibilitySensor]= &PovScene::mp_v2VisibilitySensor;
    mapArray[V2OptiDef::v2WorldInfo]= &PovScene::mp_v2WorldInfo;

}


void PovScene::useVrml2Map(void)
{
    if (mapArray != NULL) delete[] mapArray;

// ATTN: The following makes an 'Internal compiler error' with nextstep-gnu-cc 2.5.8.
#if !defined(NeXT)
    mapArray= new mapOperation[V2OptiDef::numOfProtos];
#else
    mapArray= (mapArrayPtr)(new int*[V2OptiDef::numOfProtos]);
#endif

    for (unsigned int i= 0; i < V2OptiDef::numOfProtos; i++) {
	mapArray[i]= &PovScene::mp_Unknown;
    }
    loadMapOperators();
}


RenderContext *PovScene::createContext(void)
{

    contexts= new RenderContext(this, contexts);
    return contexts;
}


bool PovScene::buildContext(RenderContext *aContext)
{
    VRNode *tmpNode= nodes;
    bool result= true;	// Optimistic approach.

    aContext->cleanTransformation();

    do {
	if (tmpNode->getTypeID() == VRMetaObject::script) {
	    // Handle the logic somewhere else.
	}
	else {
	   mapNode(tmpNode, aContext);
	}
	tmpNode= (VRNode *)tmpNode->getNext();
    } while (tmpNode != NULL);

    if (!aContext->hasFlag(RenderContext::hasViewpoint)) {
	double tmpPosition[3]= { 0, 0, -20 };
	void *params[3]= { NULL, NULL, tmpPosition };
	PovRayGlue::defineCamera(aContext->getFrame(), params);
    }
    return result;
}


void PovScene::mapNode(VRNode *aNode, RenderContext *aContext)
{
    VRPrototype *nodeProto;
    int i;

    if ((nodeProto= aNode->getRefPrototype()) != NULL) {
	V2OptiDef::Protos nodeKind;
	nodeKind= V2OptiDef::getRootKind(nodeProto);
	if ((nodeKind >= V2OptiDef::v2Anchor) && (nodeKind < V2OptiDef::numOfProtos)) {
//	    callMemberFunction(*this, mapArray[(unsigned int)nodeKind])(aNode, aContext);
	    cacaOperation(nodeKind, aNode, aContext);
	}
	else {
	    // TODO: Signal internal error.
	}
    }
}


void PovScene::cacaOperation(unsigned int anIndex, VRNode *aNode, RenderContext *aCtxt)
{
    switch((V2OptiDef::Protos)anIndex) {
	case V2OptiDef::v2Anchor:
	    mp_v2Anchor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Appearance:
	    mp_v2Appearance(aNode, aCtxt);
	    break;
	case V2OptiDef::v2AudioClip:
	    mp_v2AudioClip(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Background:
	    mp_v2Background(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Billboard:
	    mp_v2Billboard(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Box:
	    mp_v2Box(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Collision:
	    mp_v2Collision(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Color:
	    mp_v2Color(aNode, aCtxt);
	    break;
	case V2OptiDef::v2ColorInterpolator:
	    mp_v2ColorInterpolator(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Cone:
	    mp_v2Cone(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Coordinate:
	    mp_v2Coordinate(aNode, aCtxt);
	    break;
	case V2OptiDef::v2CoordinateInterpolator:
	    mp_v2CoordinateInterpolator(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Cylinder:
	    mp_v2Cylinder(aNode, aCtxt);
	    break;
	case V2OptiDef::v2CylinderSensor:
	    mp_v2CylinderSensor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2DirectionalLight:
	    mp_v2DirectionalLight(aNode, aCtxt);
	    break;
	case V2OptiDef::v2ElevationGrid:
	    mp_v2ElevationGrid(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Extrusion:
	    mp_v2Extrusion(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Fog:
	    mp_v2Fog(aNode, aCtxt);
	    break;
	case V2OptiDef::v2FontStyle:
	    mp_v2FontStyle(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Group:
	    mp_v2Group(aNode, aCtxt);
	    break;
	case V2OptiDef::v2ImageTexture:
	    mp_v2ImageTexture(aNode, aCtxt);
	    break;
	case V2OptiDef::v2IndexedFaceSet:
	    mp_v2IndexedFaceSet(aNode, aCtxt);
	    break;
	case V2OptiDef::v2IndexedLineSet:
	    mp_v2IndexedLineSet(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Inline:
	    mp_v2Inline(aNode, aCtxt);
	    break;
	case V2OptiDef::v2LOD:
	    mp_v2LOD(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Material:
	    mp_v2Material(aNode, aCtxt);
	    break;
	case V2OptiDef::v2MovieTexture:
	    mp_v2MovieTexture(aNode, aCtxt);
	    break;
	case V2OptiDef::v2NavigationInfo:
	    mp_v2NavigationInfo(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Normal:
	    mp_v2Normal(aNode, aCtxt);
	    break;
	case V2OptiDef::v2NormalInterpolator:
	    mp_v2NormalInterpolator(aNode, aCtxt);
	    break;
	case V2OptiDef::v2OrientationInterpolator:
	    mp_v2OrientationInterpolator(aNode, aCtxt);
	    break;
	case V2OptiDef::v2PixelTexture:
	    mp_v2PixelTexture(aNode, aCtxt);
	    break;
	case V2OptiDef::v2PlaneSensor:
	    mp_v2PlaneSensor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2PointLight:
	    mp_v2PointLight(aNode, aCtxt);
	    break;
	case V2OptiDef::v2PointSet:
	    mp_v2PointSet(aNode, aCtxt);
	    break;
	case V2OptiDef::v2PositionInterpolator:
	    mp_v2PositionInterpolator(aNode, aCtxt);
	    break;
	case V2OptiDef::v2ProximitySensor:
	    mp_v2ProximitySensor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2ScalarInterpolator:
	    mp_v2ScalarInterpolator(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Script:
	    mp_v2Script(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Shape:
	    mp_v2Shape(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Sound:
	    mp_v2Sound(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Sphere:
	    mp_v2Sphere(aNode, aCtxt);
	    break;
	case V2OptiDef::v2SphereSensor:
	    mp_v2SphereSensor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2SpotLight:
	    mp_v2SpotLight(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Switch:
	    mp_v2Switch(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Text:
	    mp_v2Text(aNode, aCtxt);
	    break;
	case V2OptiDef::v2TextureCoordinate:
	    mp_v2TextureCoordinate(aNode, aCtxt);
	    break;
	case V2OptiDef::v2TextureTransform:
	    mp_v2TextureTransform(aNode, aCtxt);
	    break;
	case V2OptiDef::v2TimeSensor:
	    mp_v2TimeSensor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2TouchSensor:
	    mp_v2TouchSensor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Transform:
	    mp_v2Transform(aNode, aCtxt);
	    break;
	case V2OptiDef::v2Viewpoint:
	    mp_v2Viewpoint(aNode, aCtxt);
	    break;
	case V2OptiDef::v2VisibilitySensor:
	    mp_v2VisibilitySensor(aNode, aCtxt);
	    break;
	case V2OptiDef::v2WorldInfo:
	    mp_v2WorldInfo(aNode, aCtxt);
	    break;

    }
}


void PovScene::mp_Unknown(VRNode *aNode, RenderContext *aCtxt)
{
    // TODO: Signal error.
}

