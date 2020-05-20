#ifndef _POVSCENE_H_
#define _POVSCENE_H_
/**************************************************
* File: povScene.h.
* Desc: Definition of the PovScene class.
* Module: AkraLog : PovRayExt.
* Rev: 10 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vrParser/values.h>
#include <vrParser/metaObjects.h>

struct Frame_Struct;
class VRNode;
class RenderContext;


#define callMemberFunction(anObject, ptrToMember)    \
			((anObject).*(ptrToMember))


/********************************************************
* Class: PovScene.
* Desc: Vrml scene that maps itself to a PovRay frame.
* Note: At the moment, PovScene implements the Vrml V2 mapping by
*	itself.  That should be done instead by a mapper object, that can
*	be changed if V1, V2, Inventor or some other mapping is desired.
* Class variables:
* Instance variables:
* Super-Class: VRScene.
* Module: AkraLog : PovRayExt.
********************************************************/

class PovScene : public VRScene {
  protected:
    typedef void (PovScene::*mapOperation)(VRNode *aNode, RenderContext *aCtxt);
    typedef mapOperation *mapArrayPtr;

  protected:	// Class variables.
    static mapArrayPtr mapArray;

  protected:	// Class methods.
    static void loadMapOperators(void);

  protected:
    RenderContext *contexts;

  public:
    PovScene();
    void useVrml2Map(void);
    RenderContext *createContext(void);
    bool buildContext(RenderContext *aContext);
    void mapNode(VRNode *aNode, RenderContext *aContext);
    void cacaOperation(unsigned int anIndex, VRNode *aNode, RenderContext *aCtxt);

  protected:
    void mp_v2Anchor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Appearance(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2AudioClip(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Background(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Billboard(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Box(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Collision(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Color(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2ColorInterpolator(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Cone(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Coordinate(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2CoordinateInterpolator(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Cylinder(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2CylinderSensor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2DirectionalLight(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2ElevationGrid(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Extrusion(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Fog(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2FontStyle(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Group(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2ImageTexture(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2IndexedFaceSet(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2IndexedLineSet(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Inline(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2LOD(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Material(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2MovieTexture(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2NavigationInfo(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Normal(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2NormalInterpolator(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2OrientationInterpolator(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2PixelTexture(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2PlaneSensor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2PointLight(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2PointSet(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2PositionInterpolator(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2ProximitySensor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2ScalarInterpolator(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Script(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Shape(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Sound(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Sphere(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2SphereSensor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2SpotLight(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Switch(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Text(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2TextureCoordinate(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2TextureTransform(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2TimeSensor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2TouchSensor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Transform(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2Viewpoint(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2VisibilitySensor(VRNode *aNode, RenderContext *aCtxt);
    void mp_v2WorldInfo(VRNode *aNode, RenderContext *aCtxt);
    void mp_Unknown(VRNode *aNode, RenderContext *aCtxt);
};


#endif		/*  _POVSCENE_H_ */

