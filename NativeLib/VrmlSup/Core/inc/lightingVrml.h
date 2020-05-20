#ifndef _LIGHTINGVRML_H_
#define _LIGHTINGVRML_H_
/**************************************************
* File: lightingVrml.h.
* Desc: Definition of the basic counter-parts of VRML lighting objects.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>
#include "baseVrml.h"
#include "geometry3D.h"


class JVInstance;
class JVMachine;
class ClassFile;
class RendererVrml;


// Base for all lights.
class LightVrml : public NodeVrml {
  public:
    enum Kind {
	directional, point, spot
    };

  protected:	// Class variables.
    static JVVariable *ambientField, *intensityField, *colorField;
    static bool lightMustFetchDict;

  protected:	// Class methods.
    static bool lightFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:		// Instance variables.
    float ambientIntensity;
    float intensity;
    AkColor color;

  public:		// Instance methods.
    LightVrml(void);
    LightVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual Kind getKind(void)= 0;
    virtual void renderOn(RendererVrml *aContext);
};


class DirectionalLightVrml : public LightVrml {
  protected:	// Class variables.
    static JVVariable *directionField;
    static bool dirLMustFetchDict;

  protected:	// Class methods.
    static bool dirLFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:		// Instance variables.
    AkCoord3D direction;

  public:		// Instance methods.
    DirectionalLightVrml(void);
    DirectionalLightVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual Kind getKind(void);
};


class PointLightVrml : public LightVrml {
  protected:	// Class variables.
    static JVVariable *radiusField, *attenuationField, *locationField;
    static bool ptLMustFetchDict;

  protected:	// Class methods.
    static bool ptLFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:		// Instance variables.
    float radius;
    AkCoord3D attenuation;
    AkCoord3D location;

  public:		// Instance methods.
    PointLightVrml(void);
    PointLightVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual Kind getKind(void);
};


class SpotLightVrml : public LightVrml {
  protected:	// Class variables.
    static JVVariable *widthField, *cutOffField, *radiusField, *attenuationField, *directionField, *locationField;
    static bool spLMustFetchDict;

  protected:	// Class methods.
    static bool spLFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:		// Instance variables.
    float beamWidth;
    float cutOffAngle;
    float radius;
    AkCoord3D attenuation;
    AkCoord3D direction;
    AkCoord3D location;

  public:		// Instance methods.
    SpotLightVrml(void);
    SpotLightVrml(JVInstance *anEquiv, JVMachine *aMachine);
    virtual Kind getKind(void);
};


#endif		/* _LIGHTINGVRML_H_ */
