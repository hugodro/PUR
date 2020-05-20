/**************************************************
* File: geometriesVrml.cc.
* Desc: Implementation of the VRML geometric classes.
* Module: Plzen : VrmlSup.
* Rev: 25 novembre 1997 : REV 0 : Hugo DesRosiers : Creation.
* Rev: 4 decembre 1997 : REV 0 : Hugo DesRosiers.
*	Note: updated the hierarchy for a better fitting in Java and vrml capabilities.
**************************************************/

#if defined(_WIN32)
#include <memory.h>
#elif defined(__linux__)
#include <string.h>
#endif

#include <math.h>
#include <vm/jmachine.h>
#include <vm/classFile.h>
#include <vm/jvInstance.h>
#include <native/nError.h>
#include "lowSFNode.h"
#include "rendererVrml.h"
#include "geometriesVrml.h"


/**************************************************
* Implementation: GeometryVrml.
**************************************************/

GeometryVrml::GeometryVrml(void)
{
}


GeometryVrml::GeometryVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : NodeVrml(anEquiv, aMachine)
{
}

GeometryVrml::~GeometryVrml(void)
{
    // Anything to do ?
}


void GeometryVrml::renderOn(RendererVrml *aContext)
{
    renderOn(aContext, NULL);
}


/**************************************************
* Implementation: BoxVrml.
**************************************************/

JVVariable *BoxVrml::sizeField;
bool BoxVrml::boxMustFetchDict= true;

bool BoxVrml::boxFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (boxMustFetchDict) {
	sizeField= aMachine->getFieldOfClass(aClass, "size");
	boxMustFetchDict= false;
    }
    return (sizeField != NULL);
}


BoxVrml::BoxVrml(float *theParams)
{
    size[0]= theParams[0];
    size[1]= theParams[1];
    size[2]= theParams[2];
}


BoxVrml::BoxVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
    if (boxMustFetchDict) {
	if (!boxFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }

    JVBasicArray *jSize;
    jSize= (JVBasicArray *)javaEquiv->getField(sizeField);
    jSize->byteSet((unsigned char *)size, 3 * sizeof(float));
}



BoxVrml::~BoxVrml(void)
{
    // ATTN: Anything to do ?
}


GeometryVrml::Kind BoxVrml::getKind(void)
{
    return box;
}


void BoxVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawBox(this, appearance);
}


/**************************************************
* Implementation: ConeVrml.
**************************************************/

JVVariable *ConeVrml::bottomRadiusField;
JVVariable *ConeVrml::heightField;
JVVariable *ConeVrml::sideField;
JVVariable *ConeVrml::bottomField;
bool ConeVrml::coneMustFetchDict= true;


bool ConeVrml::coneFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (coneMustFetchDict) {
	bottomRadiusField= aMachine->getFieldOfClass(aClass, "bottomRadius");
	heightField= aMachine->getFieldOfClass(aClass, "height");
	sideField= aMachine->getFieldOfClass(aClass, "side");
	bottomField= aMachine->getFieldOfClass(aClass, "bottom");
	coneMustFetchDict= false;
    }
    return ((bottomRadiusField != NULL) || (heightField != NULL) || (sideField != NULL) || (bottomField != NULL));
}


ConeVrml::ConeVrml(float aRadius, float aHeight, bool sideFlag, bool bottomFlag)
{
    bottomRadius= aRadius;
    height= aHeight;
    side= sideFlag;
    bottom= bottomFlag;
}


ConeVrml::ConeVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
    if (coneMustFetchDict) {
	if (!coneFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }

    unsigned int **gaga;

    gaga= (unsigned int **)&bottomRadius;
    *gaga= (unsigned int *)javaEquiv->getField(bottomRadiusField);
    gaga= (unsigned int **)&height;
    *gaga= (unsigned int *)javaEquiv->getField(heightField);
    side= ((int)javaEquiv->getField(sideField) != 0);
    bottom= ((int)javaEquiv->getField(bottomField) != 0);
}


ConeVrml::~ConeVrml(void)
{
    // ATTN: Anything to do ?
}


GeometryVrml::Kind ConeVrml::getKind(void)
{
    return cone;
}


void ConeVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawCone(this, appearance);
}



/**************************************************
* Implementation: CylinderVrml.
**************************************************/

JVVariable *CylinderVrml::radiusField;
JVVariable *CylinderVrml::heightField;
JVVariable *CylinderVrml::sideField;
JVVariable *CylinderVrml::topField;
JVVariable *CylinderVrml::bottomField;
bool CylinderVrml::cylMustFetchDict= true;


bool CylinderVrml::cylFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (cylMustFetchDict) {
	radiusField= aMachine->getFieldOfClass(aClass, "radius");
	heightField= aMachine->getFieldOfClass(aClass, "height");
	sideField= aMachine->getFieldOfClass(aClass, "side");
	bottomField= aMachine->getFieldOfClass(aClass, "bottom");
	topField= aMachine->getFieldOfClass(aClass, "top");
	cylMustFetchDict= false;
    }
    return ((radiusField != NULL) || (heightField != NULL) || (sideField != NULL) || (bottomField != NULL) || (topField != NULL));
}


CylinderVrml::CylinderVrml(float aRadius, float aHeight, bool sideFlag, bool bottomFlag, bool topFlag)
{
    radius= aRadius;
    height= aHeight;
    side= sideFlag;
    bottom= bottomFlag;
    top= topFlag;
}


CylinderVrml::CylinderVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
    if (cylMustFetchDict) {
	if (!cylFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }

    unsigned int **gaga;

    gaga= (unsigned int **)&radius;
    *gaga= (unsigned int *)javaEquiv->getField(radiusField);
    gaga= (unsigned int **)&height;
    *gaga= (unsigned int *)javaEquiv->getField(heightField);
    side= ((int)javaEquiv->getField(sideField) != 0);
    bottom= ((int)javaEquiv->getField(bottomField) != 0);
    top= ((int)javaEquiv->getField(topField) != 0);
}


CylinderVrml::~CylinderVrml(void)
{
    // ATTN: Anything to do ?
}


GeometryVrml::Kind CylinderVrml::getKind(void)
{
    return cylinder;
}


void CylinderVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawCylinder(this, appearance);
}


/**************************************************
* Implementation: ElevationGridVrml.
**************************************************/

ElevationGridVrml::ElevationGridVrml(void)
{
// TODO.
}

ElevationGridVrml::ElevationGridVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
// TODO.
}


ElevationGridVrml::~ElevationGridVrml(void)
{
}


GeometryVrml::Kind ElevationGridVrml::getKind(void)
{
    return elevationGrid;
}


void ElevationGridVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawElevationGrid(this, appearance);
}


/**************************************************
* Implementation: ExtrusionVrml.
**************************************************/

ExtrusionVrml::ExtrusionVrml(void)
{
// TODO.
}


ExtrusionVrml::ExtrusionVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
// TODO.
}


ExtrusionVrml::~ExtrusionVrml(void)
{
}


GeometryVrml::Kind ExtrusionVrml::getKind(void)
{
    return extrusion;
}


void ExtrusionVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawExtrusion(this, appearance);
}


/**************************************************
* Implementation: IndexedFaceSetVrml.
**************************************************/

JVVariable *IndexedFaceSetVrml::colorField;
JVVariable *IndexedFaceSetVrml::coordField;
JVVariable *IndexedFaceSetVrml::normalField;
JVVariable *IndexedFaceSetVrml::texCoordField;
JVVariable *IndexedFaceSetVrml::ccwField;
JVVariable *IndexedFaceSetVrml::colorIndexField;
JVVariable *IndexedFaceSetVrml::coordIndexField;
bool IndexedFaceSetVrml::ifsMustFetchDict= true;

/* Note: the fields of vrml.nodes.IndexedLineSet.
	0:  int[] set_colorIndex;		// Also in port.
	1:  int[] set_coordIndex;		// Also in port.
	2:  int[] set_normalIndex;		// Also in port.
	3:  int[] set_texCoordIndex;		// Also in port.
	4:  SFNode color;		// Also in/out port.
	5:  SFNode coord;		// Also in/out port.
	6:  SFNode normal;		// Also in/out port.
	7:  SFNode texCoord;		// Also in/out port.
	8:  boolean ccw;
	9:  int[] colorIndex;
	10: boolean colorPerVertex;
	11: boolean convex;
	12: int[] coordIndex;
	13: float creaseAngle;
	14: int[] normalIndex;
	15: boolean normalPerVertex;
	16: boolean solid;
	17: int[] texCoordIndex;
*/

bool IndexedFaceSetVrml::ifsFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (ifsMustFetchDict) {
	colorField= aMachine->getFieldOfClass(aClass, "color");
	coordField= aMachine->getFieldOfClass(aClass, "coord");
	normalField= aMachine->getFieldOfClass(aClass, "normal");
	texCoordField= aMachine->getFieldOfClass(aClass, "texCoord");
	ccwField= aMachine->getFieldOfClass(aClass, "ccw");
	colorIndexField= aMachine->getFieldOfClass(aClass, "colorIndex");
	coordIndexField= aMachine->getFieldOfClass(aClass, "coordIndex");
	ifsMustFetchDict= false;
    }
    return ((colorField != NULL) && (coordField != NULL) && (normalField != NULL) && (texCoordField != NULL) && (ccwField != NULL) && (colorIndexField != NULL) && (coordIndexField != NULL));
}


IndexedFaceSetVrml::IndexedFaceSetVrml(unsigned int ptCount, int *someIndexes, int indexCount)
{
    coordinates= new AkCoord3D[ptCount];
    nbrPoints= ptCount;
    indexes= someIndexes;
    nbrIndexes= indexCount;
}


IndexedFaceSetVrml::IndexedFaceSetVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
    if (ifsMustFetchDict) {
	if (!ifsFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }

    JVBasicArray *coordIndex;
    JVInstance *jCoord;

    coordIndex= (JVBasicArray *)javaEquiv->getField(coordIndexField);
    nbrIndexes= coordIndex->nbrElements;
    indexes= (int *)coordIndex->getElementPtrAt(0);
    jCoord= (JVInstance *)javaEquiv->getField(coordField);
    if ((JVAbstractInstance *)jCoord != JVAbstractInstance::globalNullObject()) {
	JVArrayArray *coordPoints;
	vrml_nodes_Coordinate vnCoord(jCoord, aMachine);

	coordPoints= vnCoord.get_points();
	nbrPoints= coordPoints->nbrElements;
	coordinates= new AkCoord3D[nbrPoints];
	for (unsigned int i= 0; i < nbrPoints; i++) {
	    ((JVBasicArray *)coordPoints->getElement(i))->byteSet((unsigned char *)coordinates[i].points, 3 * sizeof(float));
	}
    }
    else {
	nbrPoints= 0;
    }
}


IndexedFaceSetVrml::~IndexedFaceSetVrml(void)
{
    delete[] coordinates;
}


GeometryVrml::Kind IndexedFaceSetVrml::getKind(void)
{
    return indexedFaceSet;
}


void IndexedFaceSetVrml::loadCoord(float *aCoordPtr, unsigned int offset)
{
    memcpy(coordinates[offset].points, aCoordPtr, 3 * sizeof(float));
}


void IndexedFaceSetVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawIndexedFaceSet(this, appearance);
}



/**************************************************
* Implementation: IndexedLineSetVrml.
**************************************************/
IndexedLineSetVrml::IndexedLineSetVrml(void)
{
}


IndexedLineSetVrml::IndexedLineSetVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
// TODO.
}


IndexedLineSetVrml::~IndexedLineSetVrml(void)
{
}


GeometryVrml::Kind IndexedLineSetVrml::getKind(void)
{
    return indexedLineSet;
}


void IndexedLineSetVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawIndexedLineSet(this, appearance);
}



/**************************************************
* Implementation: PointSetVrml.
**************************************************/
PointSetVrml::PointSetVrml(void)
{
}


PointSetVrml::PointSetVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
// TODO.
}


PointSetVrml::~PointSetVrml(void)
{
}


GeometryVrml::Kind PointSetVrml::getKind(void)
{
    return pointSet;
}


void PointSetVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawPointSet(this, appearance);
}


/**************************************************
* Implementation: SphereVrml.
**************************************************/
JVVariable *SphereVrml::radiusField;
bool SphereVrml::sphMustFetchDict= true;

bool SphereVrml::sphFetchDictionary(JVMachine *aMachine, ClassFile *aClass)
{
    if (sphMustFetchDict) {
	radiusField= aMachine->getFieldOfClass(aClass, "radius");
	sphMustFetchDict= false;
    }
    return (radiusField != NULL);
}


SphereVrml::SphereVrml(float aRadius)
{
    radius= aRadius;
}


SphereVrml::SphereVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
    if (sphMustFetchDict) {
	if (!sphFetchDictionary(aMachine, anEquiv->category())) {
#if defined(USE_THROW)
	    throw new NativeError(1);
#endif
	}
    }

    unsigned int **gaga;

    gaga= (unsigned int **)&radius;
    *gaga= (unsigned int *)javaEquiv->getField(radiusField);
}



SphereVrml::~SphereVrml(void)
{
    // ATTN: Anything to do ?
}


GeometryVrml::Kind SphereVrml::getKind(void)
{
    return sphere;
}


void SphereVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawSphere(this, appearance);
}



/**************************************************
* Implementation: TextVrml.
**************************************************/

TextVrml::TextVrml(void)
{
}


TextVrml::TextVrml(JVInstance *anEquiv, JVMachine *aMachine)
    : GeometryVrml(anEquiv, aMachine)
{
// TODO.
}


TextVrml::~TextVrml(void)
{
}


GeometryVrml::Kind TextVrml::getKind(void)
{
    return text;
}


void TextVrml::renderOn(RendererVrml *aContext, AppearanceVrml *appearance)
{
    aContext->drawText(this, appearance);
}


