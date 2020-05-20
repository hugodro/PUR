/**************************************************
* File: materialVrml.h.
* Desc: Definition of the basic counter-parts of VRML material objects.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jmachine.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <native/nError.h>
#include "rendererVrml.h"
#include "materialVrml.h"


/**************************************************
* Implementation: AppearanceVrml.
**************************************************/

AppearanceVrml::AppearanceVrml(void)
{
// TODO.
}


AppearanceVrml::AppearanceVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
// TODO.
}


void AppearanceVrml::setMaterial(MaterialVrml *aMaterial)
{
    material= aMaterial;
}


void AppearanceVrml::setTexture(TextureVrml *aTexture)
{
    texture= aTexture;
}


void AppearanceVrml::setTextureTransform(TextureTransformVrml *aTexTransform)
{
    texTransform= aTexTransform;
}


MaterialVrml *AppearanceVrml::getMaterial(void)
{
    return material;
}


TextureVrml *AppearanceVrml::getTexture(void)
{
    return texture;
}


TextureTransformVrml *AppearanceVrml::getTexTransform(void)
{
    return texTransform;
}


/**************************************************
* Implementation: MaterialVrml.
**************************************************/

JVVariable *MaterialVrml::ambientField;
JVVariable *MaterialVrml::diffuseField;
JVVariable *MaterialVrml::emissiveField;
JVVariable *MaterialVrml::shininessField;
JVVariable *MaterialVrml::specularField;
JVVariable *MaterialVrml::transparencyField;
bool MaterialVrml::matMustFetchDict= true;

bool MaterialVrml::matFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (matMustFetchDict) {
	ambientField= aMachine->getFieldOfClass(aClass, "ambientIntensity");
	diffuseField= aMachine->getFieldOfClass(aClass, "diffuseColor");
	emissiveField= aMachine->getFieldOfClass(aClass, "emissiveColor");
	shininessField= aMachine->getFieldOfClass(aClass, "shininess");
	specularField= aMachine->getFieldOfClass(aClass, "specularColor");
	transparencyField= aMachine->getFieldOfClass(aClass, "transparency");
	matMustFetchDict= false;
    }
    return ((ambientField != NULL) && (diffuseField != NULL) && (emissiveField != NULL)
    	&& (shininessField != NULL) && (specularField != NULL) && (transparencyField != NULL));
}


MaterialVrml::MaterialVrml(void)
{
// TODO.
}


MaterialVrml::MaterialVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
    if (matMustFetchDict) {
	if (!matFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }

    JVBasicArray *jColor;
    jColor= (JVBasicArray *)javaEquiv->getField(diffuseField);
    jColor->byteSet((unsigned char *)diffuseColor.indexes, 3 * sizeof(float));
    jColor= (JVBasicArray *)javaEquiv->getField(emissiveField);
    jColor->byteSet((unsigned char *)emissiveColor.indexes, 3 * sizeof(float));
    jColor= (JVBasicArray *)javaEquiv->getField(specularField);
    jColor->byteSet((unsigned char *)specularColor.indexes, 3 * sizeof(float));

    unsigned int **gaga;

    gaga= (unsigned int **)&ambientIntensity;
    *gaga= (unsigned int *)javaEquiv->getField(ambientField);
    gaga= (unsigned int **)&shininess;
    *gaga= (unsigned int *)javaEquiv->getField(shininessField);
    gaga= (unsigned int **)&transparency;
    *gaga= (unsigned int *)javaEquiv->getField(transparencyField);
}


void MaterialVrml::getColors(AkColor *diffuse, AkColor *emissive, AkColor *specular)
{
    *diffuse= diffuseColor;
    *emissive= emissiveColor;
    *specular= specularColor;
}


void MaterialVrml::getParams(float *ambient, float *shine, float *transpar)
{
    *ambient= ambientIntensity;
    *shine= shininess;
    *transpar= transparency;
}


/**************************************************
* Implementation: TextureVrml.
**************************************************/

TextureVrml::TextureVrml(void)
{
// TODO.
}


TextureVrml::TextureVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
// TODO.
}


/**************************************************
* Implementation: TextureTransformVrml.
**************************************************/

TextureTransformVrml::TextureTransformVrml(void)
{
// TODO.
}


TextureTransformVrml::TextureTransformVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
// TODO.
}


