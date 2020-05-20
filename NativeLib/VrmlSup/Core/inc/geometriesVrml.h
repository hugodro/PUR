#ifndef _VRMLOBJECTS_H_
#define _VRMLOBJECTS_H_
/**************************************************
* File: geometriesVrml.h.
* Desc: Definition of the VRML geometric classes.
* Module: Plzen : VrmlSup.
* Rev: 25 novembre 1997 : REV 0 : Hugo DesRosiers : Creation.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers.
*	Note: updated the hierarchy for a better fitting in Java and vrml capabilities.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>
#include "geometry3D.h"
#include "baseVrml.h"


class AppearanceVrml;


class GeometryVrml : public NodeVrml {
  public:
    enum Kind {
      box, cone, cylinder, elevationGrid, extrusion,
      indexedFaceSet, indexedLineSet, pointSet,
      sphere, text, transformation, endTransformation
    };

  protected:	// Instance variables.

  public:   // Instance methods.
    GeometryVrml(void);
    GeometryVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual ~GeometryVrml(void);
    virtual Kind getKind(void)= 0;
    virtual void renderOn(RendererVrml *aContext);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance)= 0;
};


class BoxVrml : public GeometryVrml {
  protected:	// Class variables.
    static JVVariable *sizeField;
    static bool boxMustFetchDict;

  protected:	// Class methods.
    static bool boxFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    float size[3];

  public:   // Instance methods.
    BoxVrml(float *theParams);
    BoxVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~BoxVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class ConeVrml : public GeometryVrml {
  protected:	// Class variables.
    static JVVariable *bottomRadiusField, *heightField, *sideField, *bottomField;
    static bool coneMustFetchDict;

  protected:	// Class methods.
    static bool coneFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    float bottomRadius;
    float height;
    bool side, bottom;

  public:   // Instance methods.
    ConeVrml(float aRadius, float aHeight, bool sideFlag, bool bottomFlag);
    ConeVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~ConeVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class CylinderVrml : public GeometryVrml {
  protected:	// Class variables.
    static JVVariable *heightField, *radiusField, *bottomField, *sideField, *topField;
    static bool cylMustFetchDict;

  protected:	// Class methods.
    static bool cylFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    float radius;
    float height;
    bool side, bottom, top;

  public:   // Instance methods.
    CylinderVrml(float aRadius, float aHeight, bool sideFlag, bool bottomFlag, bool topFlag);
    CylinderVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~CylinderVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class ElevationGridVrml : public GeometryVrml {
  protected:	// Instance variables.

  public:   // Instance methods.
    ElevationGridVrml(void);
    ElevationGridVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~ElevationGridVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class ExtrusionVrml : public GeometryVrml {
  protected:	// Instance variables.

  public:   // Instance methods.
    ExtrusionVrml(void);
    ExtrusionVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~ExtrusionVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class IndexedFaceSetVrml : public GeometryVrml {
  protected:	// Class variables.
    static JVVariable *colorField, *coordField, *normalField, *texCoordField, *ccwField;
    static JVVariable *colorIndexField, *coordIndexField;
    static bool ifsMustFetchDict;

  protected:	// Class methods.
    static bool ifsFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    AkCoord3D *coordinates;
    int *indexes;
    unsigned int nbrPoints;
    unsigned int nbrIndexes;

  public:   // Instance methods.
    IndexedFaceSetVrml(unsigned int ptCount, int *someIndexes, int indexCount);
    IndexedFaceSetVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~IndexedFaceSetVrml(void);
    virtual Kind getKind(void);
    virtual void loadCoord(float *aCoordPtr, unsigned int offset);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class IndexedLineSetVrml : public GeometryVrml {
  protected:	// Instance variables.

  public:   // Instance methods.
    IndexedLineSetVrml(void);
    IndexedLineSetVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~IndexedLineSetVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class PointSetVrml : public GeometryVrml {
  protected:	// Instance variables.

  public:   // Instance methods.
    PointSetVrml(void);
    PointSetVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~PointSetVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class SphereVrml : public GeometryVrml {
  protected:	// Class variables.
    static JVVariable *radiusField;
    static bool sphMustFetchDict;

  protected:	// Class methods.
    static bool sphFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    float radius;

  public:   // Instance methods.
    SphereVrml(float aRadius);
    SphereVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~SphereVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


class TextVrml : public GeometryVrml {
  protected:	// Instance variables.

  public:   // Instance methods.
    TextVrml(void);
    TextVrml(JVInstance *anEquiv, JVMachine *aMachine);
    ~TextVrml(void);
    virtual Kind getKind(void);
    virtual void renderOn(RendererVrml *aContext, AppearanceVrml *appearance);
};


#endif	    /* _VRMLOBJECTS_H_ */
