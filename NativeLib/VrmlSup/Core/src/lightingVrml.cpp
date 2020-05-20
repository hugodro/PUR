/**************************************************
* File: lightingVrml.cc.
* Desc: Implementation of the basic counter-parts of VRML lighting objects.
* Module: AkraLog : VrmlSupport.
* Rev: 23 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jmachine.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <native/nError.h>
#include "rendererVrml.h"
#include "lightingVrml.h"

/**************************************************
* Implementation: LightVrml.
**************************************************/

JVVariable *LightVrml::ambientField;
JVVariable *LightVrml::intensityField;
JVVariable *LightVrml::colorField;
bool LightVrml::lightMustFetchDict= true;


bool LightVrml::lightFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (lightMustFetchDict) {
	ambientField= aMachine->getFieldOfClass(aClass, "ambientIntensity");
	intensityField= aMachine->getFieldOfClass(aClass, "intensity");
	colorField= aMachine->getFieldOfClass(aClass, "color");
	lightMustFetchDict= false;
    }

    return ((ambientField != NULL) && (intensityField != NULL) && (colorField != NULL));
}


LightVrml::LightVrml(void)
{
    ambientIntensity= 0.0f;
    intensity= 1.0f;
    color.setRGB(1.0f, 1.0f, 1.0f);
}


LightVrml::LightVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
    if (!lightFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    JVBasicArray *jColor;
    unsigned int **gaga;

    gaga= (unsigned int **)&ambientIntensity;
    *gaga= (unsigned int *)javaEquiv->getField(ambientField);
    gaga= (unsigned int **)&intensity;
    *gaga= (unsigned int *)javaEquiv->getField(intensityField);

    jColor= (JVBasicArray *)javaEquiv->getField(colorField);
	// ATTN: No check is performed to see if indeed the fields were correctly set with the
	//	right amount of data.
    jColor->byteSet((unsigned char *)color.indexes, 3 * sizeof(float));
}


void LightVrml::renderOn(RendererVrml *aContext)
{
    aContext->addLighting(this);
}


/**************************************************
* Implementation: DirectionalLightVrml.
**************************************************/

JVVariable *DirectionalLightVrml::directionField;
bool DirectionalLightVrml::dirLMustFetchDict= true;


bool DirectionalLightVrml::dirLFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (dirLMustFetchDict) {
	directionField= aMachine->getFieldOfClass(aClass, "direction");
	dirLMustFetchDict= false;
    }

    return (directionField != NULL);
}


DirectionalLightVrml::DirectionalLightVrml(void)
{
    direction.setXYZ(0.0f, 0.0f, -1.0f);
}


DirectionalLightVrml::DirectionalLightVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : LightVrml(anEquiv, aMachine)
{
    if (!dirLFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    JVBasicArray *jDirection;
    jDirection= (JVBasicArray *)javaEquiv->getField(directionField);
	// ATTN: No check is performed to see if indeed the fields were correctly set with the
	//	right amount of data.
    jDirection->byteSet((unsigned char *)direction.points, 3 * sizeof(float));
}


LightVrml::Kind DirectionalLightVrml::getKind(void)
{
    return directional;
}


/**************************************************
* Implementation: PointLightVrml.
**************************************************/

JVVariable *PointLightVrml::radiusField;
JVVariable *PointLightVrml::attenuationField;
JVVariable *PointLightVrml::locationField;
bool PointLightVrml::ptLMustFetchDict= true;


bool PointLightVrml::ptLFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (ptLMustFetchDict) {
	radiusField= aMachine->getFieldOfClass(aClass, "radius");
	attenuationField= aMachine->getFieldOfClass(aClass, "attenuation");
	locationField= aMachine->getFieldOfClass(aClass, "location");
	ptLMustFetchDict= false;
    }

    return ((radiusField != NULL) && (attenuationField != NULL) && (locationField != NULL));
}


PointLightVrml::PointLightVrml(void)
{
    radius= 100.0f;
    attenuation.setXYZ(1.0f, 0.0f, 0.0f);
    location.setXYZ(0.0f, 0.0f, 0.0f);
}


PointLightVrml::PointLightVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : LightVrml(anEquiv, aMachine)
{
    if (!ptLFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    unsigned int **gaga= (unsigned int **)&radius;
    *gaga= (unsigned int *)javaEquiv->getField(radiusField);

    JVBasicArray *jAttenuation, *jLocation;
    jAttenuation= (JVBasicArray *)javaEquiv->getField(attenuationField);
    jLocation= (JVBasicArray *)javaEquiv->getField(locationField);
	// ATTN: No check is performed to see if indeed the fields were correctly set with the
	//	right amount of data.
    jAttenuation->byteSet((unsigned char *)attenuation.points, 3 * sizeof(float));
    jLocation->byteSet((unsigned char *)location.points, 3 * sizeof(float));
}


LightVrml::Kind PointLightVrml::getKind(void)
{
    return point;
}


/**************************************************
* Implementation: SpotLightVrml.
**************************************************/

JVVariable *SpotLightVrml::widthField;
JVVariable *SpotLightVrml::cutOffField;
JVVariable *SpotLightVrml::radiusField;
JVVariable *SpotLightVrml::attenuationField;
JVVariable *SpotLightVrml::directionField;
JVVariable *SpotLightVrml::locationField;
bool SpotLightVrml::spLMustFetchDict= true;


bool SpotLightVrml::spLFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (spLMustFetchDict) {
	widthField= aMachine->getFieldOfClass(aClass, "beamWidth");
	cutOffField= aMachine->getFieldOfClass(aClass, "cutOffAngle");
	radiusField= aMachine->getFieldOfClass(aClass, "radius");
	attenuationField= aMachine->getFieldOfClass(aClass, "attenuation");
	directionField= aMachine->getFieldOfClass(aClass, "direction");
	locationField= aMachine->getFieldOfClass(aClass, "location");
	spLMustFetchDict= false;
    }

    return ((widthField != NULL) && (cutOffField != NULL) && (radiusField != NULL) && (attenuationField != NULL) && (directionField != NULL) && (locationField != NULL));
}


SpotLightVrml::SpotLightVrml(void)
{
    beamWidth= 1.570796f;
    cutOffAngle= 0.785398f;
    radius= 100.0f;
    attenuation.setXYZ(1.0f, 0.0f, 0.0f);
    direction.setXYZ(0.0f, 0.0f, -1.0f);
    location.setXYZ(0.0f, 0.0f, 0.0f);
}


SpotLightVrml::SpotLightVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : LightVrml(anEquiv, aMachine)
{
    if (!spLFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    unsigned int **gaga= (unsigned int **)&beamWidth;
    *gaga= (unsigned int *)javaEquiv->getField(widthField);
    gaga= (unsigned int **)&cutOffAngle;
    *gaga= (unsigned int *)javaEquiv->getField(cutOffField);
    gaga= (unsigned int **)&radius;
    *gaga= (unsigned int *)javaEquiv->getField(radiusField);

    JVBasicArray *jAttenuation, *jLocation, *jDirection;
    jAttenuation= (JVBasicArray *)javaEquiv->getField(attenuationField);
    jDirection= (JVBasicArray *)javaEquiv->getField(directionField);
    jLocation= (JVBasicArray *)javaEquiv->getField(locationField);
	// ATTN: No check is performed to see if indeed the fields were correctly set with the
	//	right amount of data.
    jAttenuation->byteSet((unsigned char *)attenuation.points, 3 * sizeof(float));
    jDirection->byteSet((unsigned char *)direction.points, 3 * sizeof(float));
    jLocation->byteSet((unsigned char *)location.points, 3 * sizeof(float));
}


LightVrml::Kind SpotLightVrml::getKind(void)
{
    return spot;
}


