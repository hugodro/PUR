#ifndef _GENERALVRML_H_
#define _GENERALVRML_H_
/**************************************************
* File: generalVrml.h.
* Desc: Definition of the general counter-parts of VRML objects.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>
#include "geometry3D.h"
#include "baseVrml.h"


class RendererVrml;
class JVInstance;
class JVMachine;
class ClassFile;
class JVVariable;
class AppearanceVrml;
class GeometryVrml;


class ViewPointVrml : public NodeVrml {
  protected:	// Class variables.
    static JVVariable *orientationField, *positionField, *fovField;
    static bool vpMustFetchDict;

  protected:	// Class methods.
    static bool vpFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    float fieldOfView;
    AkCoord3D position;
    AkCoord4D orientation;

  public:		// Instance methods.
    ViewPointVrml(void);
    ViewPointVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ViewPointVrml(AkCoord3D &aPos, AkCoord4D &anOrientation, float aFOV);
    virtual ~ViewPointVrml(void);
    virtual void renderOn(RendererVrml *aContext);
};


class ContainerVrml : public NodeVrml {
  protected:	// Instance variables.
    NodeVrml **children;
    unsigned int nbrChildren;
    unsigned int maxChildren;

  public:		// Instance methods.
    ContainerVrml(void);
    ContainerVrml(JVInstance *anEquiv, JVMachine *aMachine);
    bool addChild(NodeVrml *aNode);
};


class AnchorVrml : public ContainerVrml {
  public:		// Instance methods.
    AnchorVrml(JVInstance *anEquiv, JVMachine *aMachine);
};


class BillboardVrml : public ContainerVrml {
  protected:	// Class variables.
    static JVVariable *axisField, *childrenField;
    static bool bbMustFetchDict;

  protected:	// Class methods.
    static bool bbFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  protected:	// Instance variables.
    AkCoord3D axisOfRotation;

  public:		// Instance methods.
    BillboardVrml(void);
    BillboardVrml(JVInstance *anEquiv, JVMachine *aMachine);
};


class CollisionVrml : public ContainerVrml {
  protected:	// Class variables.
    static JVVariable *childrenField, *proxyField;
    static bool colMustFetchDict;

  protected:	// Class methods.
    static bool colFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:		// Instance methods.
    CollisionVrml(void);
    CollisionVrml(JVInstance *anEquiv, JVMachine *aMachine);
};


class GroupVrml : public ContainerVrml {
  protected:	// Class variables.
    static JVVariable *childrenField;
    static bool grpMustFetchDict;

  protected:	// Class methods.
    static bool grpFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:		// Instance methods.
    GroupVrml(void);
    GroupVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual void renderOn(RendererVrml *aContext);
};


class LodVrml : public ContainerVrml {
  protected:	// Class variables.
    static JVVariable *childrenField, *centerField, *rangeField;
    static bool lodMustFetchDict;

  protected:	// Class methods.
    static bool lodFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:		// Instance methods.
    LodVrml(void);
    LodVrml(JVInstance *anEquiv, JVMachine *aMachine);
};


class SwitchVrml : public ContainerVrml {
  protected:	// Class variables.
    static JVVariable *childrenField, *choiceField;
    static bool swiMustFetchDict;

  protected:	// Class methods.
    static bool swiFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  protected:	// Instance variables.
    unsigned int choice;

  public:		// Instance methods.
    SwitchVrml(void);
    SwitchVrml(JVInstance *anEquiv, JVMachine *aMachine);
};


class TransformVrml : public ContainerVrml {
  protected:	// Class variables.
    static JVVariable *childrenField, *rotationField, *scaleField, *scaleOrientField, *translationField, *centerField;
    static bool trsMustFetchDict;

  protected:	// Class methods.
    static bool trsFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    AkCoord4D rotation;
    AkCoord3D scale;
    AkCoord4D scaleOrient;
    AkCoord3D translation;
    AkCoord3D center;

  public:   // Instance methods.
    TransformVrml(void);
    TransformVrml(float *rotSpec, float *scaleSpec, float *scaleOrientSpec, float *transfoSpec, float *centerSpec);
    TransformVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~TransformVrml(void);
    virtual void renderOn(RendererVrml *aContext);
};


class ShapeVrml : public NodeVrml {
  protected:	// Class variables.
    static JVVariable *appearanceField, *geometryField;
    static bool shpMustFetchDict;

  protected:	// Class methods.
    static bool shpFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  protected:	// Instance variables.
    AppearanceVrml *appearance;
    GeometryVrml *geometry;

  public:		// Instance methods.
    ShapeVrml(void);
    ShapeVrml(JVInstance *anEquiv, JVMachine *aMachine);
    void setAppearance(AppearanceVrml *anAppear);
    void setGeometry(GeometryVrml *aGeometry);
    virtual void renderOn(RendererVrml *aContext);
};


#endif		/* _GENERALVRML_H_ */
