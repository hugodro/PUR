#ifndef _POVEXTENSIONS_H_
#define _POVEXTENSIONS_H_
/**************************************************
* File: povExtensions.h.
* Desc: Definition of the extended VRML geometric classes, for PovObjects follow-up.
* Module: Plzen : VrmlSup.
* Rev: 24 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/portableDefs.h>
#include <vrcore/geometriesVrml.h>
#include <vrcore/lightingVrml.h>
#include <vrcore/materialVrml.h>


class PovObject;
class PovTexture;


class ExtBoxVrml : public BoxVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtBoxVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtConeVrml : public ConeVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtConeVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtCylinderVrml : public CylinderVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtCylinderVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtElevationGridVrml : public ElevationGridVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtElevationGridVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtExtrusionVrml : public ExtrusionVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtExtrusionVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtIndexedFaceSetVrml : public IndexedFaceSetVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtIndexedFaceSetVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtIndexedLineSetVrml : public IndexedLineSetVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtIndexedLineSetVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtPointSetVrml : public PointSetVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtPointSetVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtSphereVrml : public SphereVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtSphereVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtTextVrml : public TextVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtTextVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtDirectionalLightVrml : public DirectionalLightVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtDirectionalLightVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtPointLightVrml : public PointLightVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtPointLightVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtSpotLightVrml : public SpotLightVrml {
  protected:	// Instance variables.
    PovObject *dual;

  public:   // Instance methods.
    ExtSpotLightVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovObject *getPovObject(void);
    virtual void setPovEquiv(PovObject *pObject);
};


class ExtAppearanceVrml : public AppearanceVrml {
  protected:	// Instance variables.
    PovTexture *dual;

  public:   // Instance methods.
    ExtAppearanceVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual PovTexture *getPovTexture(void);
    virtual void setPovTexture(PovTexture *pObject);
};


#endif	    /* _POVEXTENSIONS_H_ */
