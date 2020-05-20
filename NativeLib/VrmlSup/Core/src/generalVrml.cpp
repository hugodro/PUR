/**************************************************
* File: generalVrml.cc.
* Desc: Implementation of the general counter-parts of VRML objects.
* Module: AkraLog : VrmlSupport.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vm/jmachine.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <native/nError.h>
#include "geometriesVrml.h"
#include "materialVrml.h"
#include "rendererVrml.h"
#include "generalVrml.h"

#define RADIANDEG	0.0174532922


/**************************************************
* Implementation: ViewPointVrml.
**************************************************/
JVVariable *ViewPointVrml::orientationField;
JVVariable *ViewPointVrml::positionField;
JVVariable *ViewPointVrml::fovField;
bool ViewPointVrml::vpMustFetchDict= true;

bool ViewPointVrml::vpFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (vpMustFetchDict) {
	orientationField= aMachine->getFieldOfClass(aClass, "orientation");
	positionField= aMachine->getFieldOfClass(aClass, "position");
	fovField= aMachine->getFieldOfClass(aClass, "fieldOfView");
	vpMustFetchDict= false;
    }

    return ((orientationField != NULL) && (positionField != NULL) && (fovField != NULL));
}


ViewPointVrml::ViewPointVrml(void)
{
    fieldOfView= 0.78752f;
    position.setXYZ(0.0f, 0.0f, 100.0f);
    orientation.setXYZR(0.0f, 1.0f, 0.0f, 0.0f);
}


ViewPointVrml::ViewPointVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
    if (!vpFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    JVBasicArray *jOrien, *jPos;
    unsigned int **gaga;

    gaga= (unsigned int **)&fieldOfView;
    *gaga= (unsigned int *)javaEquiv->getField(fovField);

    jOrien= (JVBasicArray *)javaEquiv->getField(orientationField);
    jPos= (JVBasicArray *)javaEquiv->getField(positionField);
	// ATTN: No check is performed to see if indeed the fields were correctly set with the
	//	right amount of data.
    jOrien->byteSet((unsigned char *)orientation.points, 4 * sizeof(float));
    jPos->byteSet((unsigned char *)position.points, 3 * sizeof(float));
}


ViewPointVrml::ViewPointVrml(AkCoord3D &aPos, AkCoord4D &anOrientation, float aFOV)
{
    fieldOfView= aFOV;
    position= aPos;
    orientation= anOrientation;
}


ViewPointVrml::~ViewPointVrml(void)
{

}


void ViewPointVrml::renderOn(RendererVrml *aContext)
{
    aContext->useViewPoint(this);
}


/**************************************************
* Implementation: ContainerVrml.
**************************************************/

ContainerVrml::ContainerVrml(void)
{
    nbrChildren= 0;
    maxChildren= 0;
    children= NULL;
}


ContainerVrml::ContainerVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
    nbrChildren= 0;
    maxChildren= 0;
    children= NULL;
}


bool ContainerVrml::addChild(NodeVrml *aNode)
{
    if (nbrChildren >= maxChildren) {
	if (maxChildren == 0) {
	    maxChildren= 16;
	    children= new NodeVrml*[maxChildren];
	}
	else {
	    unsigned int tmpChildCount= maxChildren;
	    maxChildren+= (maxChildren / 2) + 1;
	    NodeVrml **tmpChildren= new NodeVrml*[maxChildren];
	    for (unsigned int i= 0; i < tmpChildCount; i++) {
		tmpChildren[i]= children[i];
	    }
	    delete[] children;
	    children= tmpChildren;
	}
    }
    children[nbrChildren++]= aNode;
    return true;    // ATTN: For now, always succeed.
}


/**************************************************
* Implementation: AnchorVrml.
**************************************************/

AnchorVrml::AnchorVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ContainerVrml(anEquiv, aMachine)
{
    // Anything to do ?
}


/**************************************************
* Implementation: BillboardVrml.
**************************************************/

JVVariable *BillboardVrml::axisField;
JVVariable *BillboardVrml::childrenField;
bool BillboardVrml::bbMustFetchDict= true;


bool BillboardVrml::bbFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (bbMustFetchDict) {
	axisField= aMachine->getFieldOfClass(aClass, "axisOfRotation");
	childrenField= aMachine->getFieldOfClass(aClass, "children");
	bbMustFetchDict= false;
    }

    return ((axisField != NULL) && (childrenField == NULL));
}


BillboardVrml::BillboardVrml(void)
{
    axisOfRotation.setXYZ(0.0f, 1.0f, 0.0f);
}


BillboardVrml::BillboardVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ContainerVrml(anEquiv, aMachine)
{
    if (!bbFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }
    JVInstanceArray *jChildren;
    jChildren= (JVInstanceArray *)javaEquiv->getField(childrenField);
    if (jChildren != (JVInstanceArray *)JVAbstractInstance::globalNullObject()) {
	if ((maxChildren= jChildren->nbrElements) != 0) {
	    children= new NodeVrml*[maxChildren];
	}
    }
}


/**************************************************
* Implementation: CollisionVrml.
**************************************************/

JVVariable *CollisionVrml::childrenField;
JVVariable *CollisionVrml::proxyField;
bool CollisionVrml::colMustFetchDict= true;


bool CollisionVrml::colFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (colMustFetchDict) {
	childrenField= aMachine->getFieldOfClass(aClass, "children");
	proxyField= aMachine->getFieldOfClass(aClass, "proxy");
	colMustFetchDict= false;
    }

    return ((proxyField != NULL) && (childrenField != NULL));
}


CollisionVrml::CollisionVrml(void)
{
}


CollisionVrml::CollisionVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ContainerVrml(anEquiv, aMachine)
{
    if (!colFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }
    JVInstanceArray *jChildren;
    jChildren= (JVInstanceArray *)javaEquiv->getField(childrenField);
    if (jChildren != (JVInstanceArray *)JVAbstractInstance::globalNullObject()) {
	if ((maxChildren= jChildren->nbrElements) != 0) {
	    maxChildren++;		// Add a spot for the proxy node.
	    children= new NodeVrml*[maxChildren];
	}
    }
}


/**************************************************
* Implementation: GroupVrml.
**************************************************/

JVVariable *GroupVrml::childrenField;
bool GroupVrml::grpMustFetchDict= true;


bool GroupVrml::grpFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (grpMustFetchDict) {
	childrenField= aMachine->getFieldOfClass(aClass, "children");
	grpMustFetchDict= false;
    }

    return (childrenField != NULL);
}


GroupVrml::GroupVrml(void)
{
}


GroupVrml::GroupVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ContainerVrml(anEquiv, aMachine)
{
    if (!grpFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    JVInstanceArray *jChildren;

    jChildren= (JVInstanceArray *)javaEquiv->getField(childrenField);
    if (jChildren != (JVInstanceArray *)JVAbstractInstance::globalNullObject()) {
	if ((maxChildren= jChildren->nbrElements) != 0) {
	    children= new NodeVrml*[maxChildren];
	}
    }
}


void GroupVrml::renderOn(RendererVrml *aContext)
{
    if (nbrChildren > 0) {
        for (unsigned int i= 0; i < nbrChildren; i++) {
	    children[i]->renderOn(aContext);
	}
    }
}


/**************************************************
* Implementation: LodVrml.
**************************************************/

JVVariable *LodVrml::childrenField;
JVVariable *LodVrml::centerField;
JVVariable *LodVrml::rangeField;
bool LodVrml::lodMustFetchDict= true;


bool LodVrml::lodFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (lodMustFetchDict) {
	childrenField= aMachine->getFieldOfClass(aClass, "level");
	centerField= aMachine->getFieldOfClass(aClass, "center");
	rangeField= aMachine->getFieldOfClass(aClass, "range");
	lodMustFetchDict= false;
    }

    return ((childrenField != NULL) && (centerField != NULL) && (rangeField != NULL));
}


LodVrml::LodVrml(void)
{
}


LodVrml::LodVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ContainerVrml(anEquiv, aMachine)
{
    if (!lodFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    JVInstanceArray *jChildren;

    jChildren= (JVInstanceArray *)javaEquiv->getField(childrenField);
    if (jChildren != (JVInstanceArray *)JVAbstractInstance::globalNullObject()) {
	if ((maxChildren= jChildren->nbrElements) != 0) {
	    children= new NodeVrml*[maxChildren];
	}
    }
}


/**************************************************
* Implementation: SwitchVrml.
**************************************************/

JVVariable *SwitchVrml::childrenField;
JVVariable *SwitchVrml::choiceField;
bool SwitchVrml::swiMustFetchDict= true;


bool SwitchVrml::swiFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (swiMustFetchDict) {
	childrenField= aMachine->getFieldOfClass(aClass, "choice");
	choiceField= aMachine->getFieldOfClass(aClass, "whichChoice");
	swiMustFetchDict= false;
    }

    return ((childrenField != NULL) && (choiceField != NULL));
}


SwitchVrml::SwitchVrml(void)
{
}


SwitchVrml::SwitchVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ContainerVrml(anEquiv, aMachine)
{
    if (!swiFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    JVInstanceArray *jChildren;

    jChildren= (JVInstanceArray *)javaEquiv->getField(childrenField);
    if (jChildren != (JVInstanceArray *)JVAbstractInstance::globalNullObject()) {
	if ((maxChildren= jChildren->nbrElements) != 0) {
	    children= new NodeVrml*[maxChildren];
	}
    }
    choice= (int)javaEquiv->getField(choiceField);
}


/**************************************************
* Implementation: TransformVrml.
**************************************************/

JVVariable *TransformVrml::childrenField;
JVVariable *TransformVrml::rotationField;
JVVariable *TransformVrml::scaleField;
JVVariable *TransformVrml::scaleOrientField;
JVVariable *TransformVrml::translationField;
JVVariable *TransformVrml::centerField;
bool TransformVrml::trsMustFetchDict= true;


bool TransformVrml::trsFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (trsMustFetchDict) {
	childrenField= aMachine->getFieldOfClass(aClass, "children");
	rotationField= aMachine->getFieldOfClass(aClass, "rotation");
	scaleField= aMachine->getFieldOfClass(aClass, "scale");
	scaleOrientField= aMachine->getFieldOfClass(aClass, "scaleOrientation");
	translationField= aMachine->getFieldOfClass(aClass, "translation");
	centerField= aMachine->getFieldOfClass(aClass, "center");
	trsMustFetchDict= false;
    }

    return ((childrenField != NULL) && (rotationField != NULL)
    	&& (scaleField != NULL) && (scaleOrientField != NULL)
	&& (translationField != NULL) && (centerField != NULL));
}


TransformVrml::TransformVrml(void)
{
}


TransformVrml::TransformVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : ContainerVrml(anEquiv, aMachine)
{
    if (!trsFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }

    JVInstanceArray *jChildren;

    jChildren= (JVInstanceArray *)javaEquiv->getField(childrenField);
    if (jChildren != (JVInstanceArray *)JVAbstractInstance::globalNullObject()) {
	if ((maxChildren= jChildren->nbrElements) != 0) {
	    children= new NodeVrml*[maxChildren];
	}
    }

    JVBasicArray *jRotation, *jScale, *jScaleOrient, *jTranslation, *jCenter;

    jRotation= (JVBasicArray *)javaEquiv->getField(rotationField);
    jScale= (JVBasicArray *)javaEquiv->getField(scaleField);
    jScaleOrient= (JVBasicArray *)javaEquiv->getField(scaleOrientField);
    jTranslation= (JVBasicArray *)javaEquiv->getField(translationField);
    jCenter= (JVBasicArray *)javaEquiv->getField(centerField);

	// ATTN: No check is performed to see if indeed the fields were correctly set with the
	//	right amount of data.
    jRotation->byteSet((unsigned char *)rotation.points, 4 * sizeof(float));
    jScale->byteSet((unsigned char *)scale.points, 3 * sizeof(float));
    jScaleOrient->byteSet((unsigned char *)scaleOrient.points, 4 * sizeof(float));
    jTranslation->byteSet((unsigned char *)translation.points, 3 * sizeof(float));
    jCenter->byteSet((unsigned char *)center.points, 3 * sizeof(float));
}


TransformVrml::~TransformVrml(void)
{
// ATTN: Anything to do ?
}


void TransformVrml::renderOn(RendererVrml *aContext)
{
// TMP: Fake the future 'dirty flag' access through a continuous update of the translation/rotation.
    JVBasicArray *jRotation, *jTranslation, *jCenter;
    jRotation= (JVBasicArray *)javaEquiv->getField(rotationField);
    jTranslation= (JVBasicArray *)javaEquiv->getField(translationField);
    jCenter= (JVBasicArray *)javaEquiv->getField(centerField);

    jRotation->byteSet((unsigned char *)rotation.points, 4 * sizeof(float));
    jTranslation->byteSet((unsigned char *)translation.points, 3 * sizeof(float));
    jCenter->byteSet((unsigned char *)center.points, 3 * sizeof(float));

    aContext->openTransform(this);
    if (nbrChildren > 0) {
        for (unsigned int i= 0; i < nbrChildren; i++) {
	    children[i]->renderOn(aContext);
	}
    }
    aContext->closeTransform(this);
}


/**************************************************
* Implementation: ShapeVrml.
**************************************************/

JVVariable *ShapeVrml::appearanceField;
JVVariable *ShapeVrml::geometryField;
bool ShapeVrml::shpMustFetchDict= true;


bool ShapeVrml::shpFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (shpMustFetchDict) {
	appearanceField= aMachine->getFieldOfClass(aClass, "appearance");
	geometryField= aMachine->getFieldOfClass(aClass, "geometry");
	shpMustFetchDict= false;
    }

    return ((appearanceField != NULL) && (geometryField != NULL));
}


ShapeVrml::ShapeVrml(void)
{
}


ShapeVrml::ShapeVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
    if (!shpFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	throw new NativeError(1);
#endif
    }
}


void ShapeVrml::setAppearance(AppearanceVrml *anAppear)
{
    appearance= anAppear;
}


void ShapeVrml::setGeometry(GeometryVrml *aGeometry)
{
    geometry= aGeometry;
}


void ShapeVrml::renderOn(RendererVrml *aContext)
{
    geometry->renderOn(aContext, appearance);
}
