#ifndef _MATERIALVRML_H_
#define _MATERIALVRML_H_
/**************************************************
* File: materialVrml.h.
* Desc: Definition of the basic counter-parts of VRML material objects.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>
#include "baseVrml.h"
#include "geometry3D.h"


class MaterialVrml;
class TextureVrml;
class TextureTransformVrml;
class JVInstance;
class JVMachine;

class AppearanceVrml : public NodeVrml {
  public:	// Instance variables.
    MaterialVrml *material;
    TextureVrml *texture;
    TextureTransformVrml *texTransform;

  public:		// Instance methods.
    AppearanceVrml(void);
    AppearanceVrml(JVInstance *anEquiv, JVMachine *aMachine);
    void setMaterial(MaterialVrml *aMaterial);
    void setTexture(TextureVrml *aTexture);
    void setTextureTransform(TextureTransformVrml *aTexTransform);
    MaterialVrml *getMaterial(void);
    TextureVrml *getTexture(void);
    TextureTransformVrml *getTexTransform(void);
};


class MaterialVrml : public NodeVrml {
  protected:	// Class variables.
    static JVVariable *ambientField, *diffuseField, *emissiveField, *shininessField,
    	*specularField, *transparencyField;
    static bool matMustFetchDict;

  protected:	// Class methods.
    static bool matFetchDictionary(JVMachine *aMachine, ClassFile *aClass);

  public:	// Instance variables.
    AkColor diffuseColor;
    AkColor emissiveColor;
    AkColor specularColor;
    float ambientIntensity;
    float shininess;
    float transparency;

  public:		// Instance methods.
    MaterialVrml(void);
    MaterialVrml(JVInstance *anEquiv, JVMachine *aMachine);
    void getColors(AkColor *diffuse, AkColor *emissive, AkColor *specular);
    void getParams(float *ambient, float *shine, float *transpar);
};


class FogVrml : public NodeVrml {
};


// Base for all textures.
class TextureVrml : public NodeVrml {
  public:		// Instance methods.
    TextureVrml(void);
    TextureVrml(JVInstance *anEquiv, JVMachine *aMachine);
};


class ImageTexture : public TextureVrml {
};


class MovieTexture : public TextureVrml {
};


class PixelTexture : public TextureVrml {
};


class TextureTransformVrml : public NodeVrml {
  public:		// Instance methods.
    TextureTransformVrml(void);
    TextureTransformVrml(JVInstance *anEquiv, JVMachine *aMachine);
};



#endif		/* _MATERIALVRML_H_ */
