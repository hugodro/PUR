/**************************************************
* File: mapOperators.cc.
* Desc: Implementation of the PovScene class, section of V2 map operators.
* NOTE: Most internal values are assumed to be of a typical type, which will mean that
*	it will panic in case of non-standard values (eg: scalar as for 1-elemnt array).
* Module: AkraLog : PovRayExt.
* Rev: 14 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <VrmlParser/v2OptimumDefs.h>
#include "renderContext.h"
#include "glue.h"
#include "povScene.h"
#include "pObject.h"


void PovScene::mp_v2Anchor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Appearance(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fMaterial= V2OptiDef::getFieldOf(V2OptiDef::v2Appearance, "material");
    static VRField *fTexture= V2OptiDef::getFieldOf(V2OptiDef::v2Appearance, "texture");
    static VRField *fTextureTransform= V2OptiDef::getFieldOf(V2OptiDef::v2Appearance, "textureTransform");
    VRValue *vMaterial, *vTexture, *vTextureTransform;
    VRNode *sfNode;
    PovTexture *newTexture;
    int valFlag[3];

    vMaterial= aNode->getFieldContent(fMaterial, valFlag[0]);
    vTexture= aNode->getFieldContent(fTexture, valFlag[1]);
    vTextureTransform= aNode->getFieldContent(fTextureTransform, valFlag[2]);

    newTexture= PovTexture::createTexture();
    aCtxt->setCurrentTexture(newTexture);

    if ((vTexture != NULL) && (valFlag[1] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vTexture)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }
    if ((vTextureTransform != NULL) && (valFlag[2] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vTextureTransform)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }

    if ((vMaterial != NULL) && (valFlag[0] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vMaterial)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }
    if (aCtxt->getCurrentObject() != NULL) aCtxt->getCurrentObject()->addTexture(newTexture);
}


void PovScene::mp_v2AudioClip(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Background(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Billboard(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Box(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fSize= V2OptiDef::getFieldOf(V2OptiDef::v2Box, "size");
    VRValue *vSize;
    Vector3 tSizes;
    void *params[1]= { NULL };
    int valFlag;
    unsigned int nbrFound;
    PovObject *newObject;

    vSize= aNode->getFieldContent(fSize, valFlag);
    if ((vSize != NULL) && (valFlag != 0)) {
	if (((VRNumeric *)vSize)->getDoubleArray(tSizes.Vc, 3) == 3) {
	    params[0]= &tSizes;
	}
    }


    if ((newObject= PovObject::createBox(aCtxt->getCurrentMatrix(), params)) != NULL) {
	aCtxt->setCurrentObject(newObject);
    }
}


void PovScene::mp_v2Collision(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fChildren= V2OptiDef::getFieldOf(V2OptiDef::v2Collision, "children");
    static VRField *fBboxCenter= V2OptiDef::getFieldOf(V2OptiDef::v2Collision, "bboxCenter");
    static VRField *fBboxSize= V2OptiDef::getFieldOf(V2OptiDef::v2Collision, "bboxSize");
    VRValue *vChildren, *vBboxCenter, *vBboxSize;
    Vector3 tBboxCenter, tBboxSize;
    int valFlag[3];

    vChildren= aNode->getFieldContent(fChildren, valFlag[0]);
    vBboxCenter= aNode->getFieldContent(fBboxCenter, valFlag[1]);
    vBboxSize= aNode->getFieldContent(fBboxSize, valFlag[2]);

    if ((vChildren != NULL) && (valFlag[0] == 1)) {
	VRNode *mfNode;

	switch(((VRComplexValue *)vChildren)->getType()) {
	    case VRComplexValue::ntNode:
		if ((mfNode= (VRNode *)((VRComplexValue *)vChildren)->getValue()) != NULL) {
		    mapNode(mfNode, aCtxt);
		}
		break;
	    case VRComplexValue::ntMfNode:
		if ((mfNode= (VRNode *)((VRComplexValue *)vChildren)->getValue()) != NULL) {
		    do {
			mapNode(mfNode, aCtxt);
			mfNode= (VRNode *)mfNode->getNext();
		    } while (mfNode != NULL);
		}
		break;
	    default:
		// TODO: Take care of other kinds.
	        break;
	}
    }
    // TODO: Take care of bounding box.
}


void PovScene::mp_v2Color(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2ColorInterpolator(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Cone(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fBottomRadius= V2OptiDef::getFieldOf(V2OptiDef::v2Cone, "bottomRadius");
    static VRField *fHeight= V2OptiDef::getFieldOf(V2OptiDef::v2Cone, "height");
    static VRField *fSide= V2OptiDef::getFieldOf(V2OptiDef::v2Cone, "side");
    static VRField *fBottom= V2OptiDef::getFieldOf(V2OptiDef::v2Cone, "bottom");
    VRValue *vBottomRadius, *vHeight, *vSide, *vBottom;
    double tBottomRadius, tHeight;
    void *params[2]= { NULL, NULL };
    int valFlag[2];
    PovObject *newObject;

    vBottomRadius= aNode->getFieldContent(fBottomRadius, valFlag[0]);
    vHeight= aNode->getFieldContent(fHeight, valFlag[1]);
    if ((vBottomRadius != NULL) && (valFlag[0] != 0)) {
	tBottomRadius= ((VRNumeric *)vBottomRadius)->getDoubleValue();
	params[0]= &tBottomRadius;
    }
    if ((vHeight != NULL) && (valFlag[1] != 0)) {
	tHeight= ((VRNumeric *)vHeight)->getDoubleValue();
	params[1]= &tHeight;
    }

    if ((newObject= PovObject::createCone(aCtxt->getCurrentMatrix(), params)) != NULL) {
	aCtxt->setCurrentObject(newObject);
    }
}


void PovScene::mp_v2Coordinate(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fPoint= V2OptiDef::getFieldOf(V2OptiDef::v2Coordinate, "point");
    VRValue *vPoint;
    float (*coordinates3D)[3];
    unsigned int coordSize= 0;
    int valFlag;

    coordinates3D= NULL;
    vPoint= aNode->getFieldContent(fPoint, valFlag);
    if ((vPoint != NULL) && (valFlag != 0)) {
	VRValue *elements;

	coordSize= ((VRVector *)vPoint)->getNbrElements();
	if (coordSize > 0) {
	    elements= ((VRVector *)vPoint)->getValues();
	    if ((coordSize % 3)  != 0) coordSize= coordSize/3 + 1;
	    else coordSize/= 3;
	    coordinates3D= new float[coordSize][3];
	    ((VRNumeric *)elements)->getFloatArray((float *)coordinates3D, coordSize * 3);
	}
    }
    aCtxt->setCurrentArray(coordSize, coordinates3D);
}


void PovScene::mp_v2CoordinateInterpolator(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Cylinder(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fBottom= V2OptiDef::getFieldOf(V2OptiDef::v2Cylinder, "bottom");
    static VRField *fHeight= V2OptiDef::getFieldOf(V2OptiDef::v2Cylinder, "height");
    static VRField *fRadius= V2OptiDef::getFieldOf(V2OptiDef::v2Cylinder, "radius");
    static VRField *fSide= V2OptiDef::getFieldOf(V2OptiDef::v2Cylinder, "side");
    static VRField *fTop= V2OptiDef::getFieldOf(V2OptiDef::v2Cylinder, "top");
    VRValue *vHeight, *vRadius;
    double tRadius, tHeight;
    void *params[2]= { NULL, NULL };
    int valFlag[2];
    PovObject *newObject;

    vHeight= aNode->getFieldContent(fHeight, valFlag[0]);
    vRadius= aNode->getFieldContent(fRadius, valFlag[1]);
    if ((vHeight != NULL) && (valFlag[0] != 0)) {
	tHeight= ((VRNumeric *)vHeight)->getDoubleValue();
	params[1]= &tHeight;
    }
    if ((vRadius != NULL) && (valFlag[1] != 0)) {
	tRadius= ((VRNumeric *)vRadius)->getDoubleValue();
	params[0]= &tRadius;
    }

    if ((newObject= PovObject::createCylinder(aCtxt->getCurrentMatrix(), params)) != NULL) {
	aCtxt->setCurrentObject(newObject);
    }
}


void PovScene::mp_v2CylinderSensor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2DirectionalLight(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fAmbientIntensity= V2OptiDef::getFieldOf(V2OptiDef::v2DirectionalLight, "ambientIntensity");
    static VRField *fColor= V2OptiDef::getFieldOf(V2OptiDef::v2DirectionalLight, "color");
    static VRField *fDirection= V2OptiDef::getFieldOf(V2OptiDef::v2DirectionalLight, "direction");
    static VRField *fIntensity= V2OptiDef::getFieldOf(V2OptiDef::v2DirectionalLight, "intensity");
    static VRField *fOn= V2OptiDef::getFieldOf(V2OptiDef::v2DirectionalLight, "on");
    VRValue *vAmbientIntensity, *vColor, *vDirection, *vIntensity, *vOn;
    double tAmbient, tIntensity;
    Vector3 tColor, tDirection;
    void *params[5]= { NULL, NULL, NULL, NULL, NULL };
    int valFlag[5];
    PovObject *newObject;

    vAmbientIntensity= aNode->getFieldContent(fAmbientIntensity, valFlag[0]);
    vColor= aNode->getFieldContent(fColor, valFlag[1]);
    vDirection= aNode->getFieldContent(fDirection, valFlag[2]);
    vIntensity= aNode->getFieldContent(fIntensity, valFlag[3]);
    vOn= aNode->getFieldContent(fOn, valFlag[4]);

    if ((vAmbientIntensity != NULL) && (valFlag[0] != 0)) {
	tAmbient= ((VRNumeric *)vAmbientIntensity)->getDoubleValue();
	params[0]= &tAmbient;
    }
    if ((vColor != NULL) && (valFlag[1] != 0)) {
	if (((VRNumeric *)vColor)->getDoubleArray(tColor.Vc, 3) == 3) {
	    params[1]= &tColor;
	}
    }
    if ((vDirection != NULL) && (valFlag[2] != 0)) {
	if (((VRNumeric *)vDirection)->getDoubleArray(tDirection.Vc, 3) == 3) {
	    params[2]= &tDirection;
	}
    }
    if ((vIntensity != NULL) && (valFlag[3] != 0)) {
	tIntensity= ((VRNumeric *)vIntensity)->getDoubleValue();
	params[3]= &tIntensity;
    }
    if ((vOn != NULL) && (valFlag[4] != 0)) {
	params[4]= (void *)((VRNumeric *)vOn)->getIntValue();
    }

    if ((newObject= PovObject::createDirLight(aCtxt->getCurrentMatrix(), params)) != NULL) {
	PovRayGlue::frameObject(newObject);
    }
}


void PovScene::mp_v2ElevationGrid(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fColor= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "color");
    static VRField *fNormal= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "normal");
    static VRField *fTexCoord= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "texCoord");
    static VRField *fHeight= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "height");
    static VRField *fCcw= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "ccw");
    static VRField *fColorPerVertex= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "colorPerVertex");
    static VRField *fCreaseAngle= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "creaseAngle");
    static VRField *fNormalPerVertex= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "normalPerVertex");
    static VRField *fSolid= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "solid");
    static VRField *fXDimension= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "xDimension");
    static VRField *fXSpacing= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "xSpacing");
    static VRField *fZDimension= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "zDimension");
    static VRField *fZSpacing= V2OptiDef::getFieldOf(V2OptiDef::v2ElevationGrid, "zSpacing");
    VRValue *vColor, *vNormal, *vTexCoord, *vHeight, *vCcw, *vColorPerVertex, *vCreaseAngle, *vNormalPerVertex, *vSolid, *vXDimension, *vXSpacing, *vZDimension, *vZSpacing;
    VRNode *sfNode;
    double tCreaseAngle, tHeight, tXSpacing, tZSpacing;
    int tXDimension, tZDimension, tCcw, tColorPerVertex, tNormalPerVertex, tSolid;
    void *params[13]= { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int valFlag[13];
    PovObject *newObject;

    vColor= aNode->getFieldContent(fColor, valFlag[0]);
    vNormal= aNode->getFieldContent(fNormal, valFlag[1]);
    vTexCoord= aNode->getFieldContent(fTexCoord, valFlag[2]);
    vHeight= aNode->getFieldContent(fHeight, valFlag[3]);
    vCcw= aNode->getFieldContent(fCcw, valFlag[4]);
    vColorPerVertex= aNode->getFieldContent(fColorPerVertex, valFlag[5]);
    vCreaseAngle= aNode->getFieldContent(fCreaseAngle, valFlag[6]);
    vNormalPerVertex= aNode->getFieldContent(fNormalPerVertex, valFlag[7]);
    vSolid= aNode->getFieldContent(fSolid, valFlag[8]);
    vXDimension= aNode->getFieldContent(fXDimension, valFlag[9]);
    vXSpacing= aNode->getFieldContent(fXSpacing, valFlag[10]);
    vZDimension= aNode->getFieldContent(fZDimension, valFlag[11]);
    vZSpacing= aNode->getFieldContent(fZSpacing, valFlag[12]);

    if ((vColor != NULL) && (valFlag[0] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vColor)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }
	// TODO: The rest.
    if ((newObject= PovObject::createHeightField(aCtxt->getCurrentMatrix(), params)) != NULL) {
	aCtxt->setCurrentObject(newObject);
    }
}


void PovScene::mp_v2Extrusion(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fBeginCap= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "beginCap");
    static VRField *fCcw= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "ccw");
    static VRField *fConvex= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "convex");
    static VRField *fCreaseAngle= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "creaseAngle");
    static VRField *fCrossSection= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "crossSection");
    static VRField *fEndCap= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "endCap");
    static VRField *fOrientation= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "orientation");
    static VRField *fScale= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "scale");
    static VRField *fSolid= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "solid");
    static VRField *fSpine= V2OptiDef::getFieldOf(V2OptiDef::v2Extrusion, "spine");
    VRValue *vBeginCap;
    void *params[10]= { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int valFlag[10];
    PovObject *newObject;

    vBeginCap= aNode->getFieldContent(fBeginCap, valFlag[0]);
    if ((vBeginCap != NULL) && (valFlag[0] != 0)) {
	params[0]= (void *)((VRNumeric *)vBeginCap)->getIntValue();
    }
	// TODO: The rest.
	// ATTN: Is the lathe the right map for an Extrusion ?
    if ((newObject= PovObject::createLathe(aCtxt->getCurrentMatrix(), params)) != NULL) {
	aCtxt->setCurrentObject(newObject);
    }
}


void PovScene::mp_v2Fog(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2FontStyle(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Group(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fChildren= V2OptiDef::getFieldOf(V2OptiDef::v2Group, "children");
    static VRField *fBboxCenter= V2OptiDef::getFieldOf(V2OptiDef::v2Group, "bboxCenter");
    static VRField *fBboxSize= V2OptiDef::getFieldOf(V2OptiDef::v2Group, "bboxSize");
    VRValue *vChildren, *vBboxCenter, *vBboxSize;
    Vector3 tBboxCenter, tBboxSize;
    int valFlag[3];

    vChildren= aNode->getFieldContent(fChildren, valFlag[0]);
    vBboxCenter= aNode->getFieldContent(fBboxCenter, valFlag[1]);
    vBboxSize= aNode->getFieldContent(fBboxSize, valFlag[2]);

    if ((vChildren != NULL) && (valFlag[0] == 1)) {
	VRNode *mfNode;

	switch(((VRComplexValue *)vChildren)->getType()) {
	    case VRComplexValue::ntNode:
		if ((mfNode= (VRNode *)((VRComplexValue *)vChildren)->getValue()) != NULL) {
		    mapNode(mfNode, aCtxt);
		}
		break;
	    case VRComplexValue::ntMfNode:
		if ((mfNode= (VRNode *)((VRComplexValue *)vChildren)->getValue()) != NULL) {
		    do {
			mapNode(mfNode, aCtxt);
			mfNode= (VRNode *)mfNode->getNext();
		    } while (mfNode != NULL);
		}
		break;
	    default:
		// TODO: Take care of other kinds.
	        break;
	}
    }
    // TODO: Take care of bounding box.
}


void PovScene::mp_v2ImageTexture(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2IndexedFaceSet(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fColor= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "color");
    static VRField *fCoord= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "coord");
    static VRField *fNormal= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "normal");
    static VRField *fTexCoord= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "texCoord");
    static VRField *fCcw= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "ccw");
    static VRField *fColorIndex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "colorIndex");
    static VRField *fColorPerVertex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "colorPerVextex");
    static VRField *fConvex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "convex");
    static VRField *fCoordIndex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "coordIndex");
    static VRField *fCreaseAngle= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "creaseAngle");
    static VRField *fNormalIndex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "normalIndex");
    static VRField *fNormalPerVertex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "normalPerVertex");
    static VRField *fSolid= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "solid");
    static VRField *fTexCoordIndex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedFaceSet, "texCoordIndex");
    VRValue *vColor, *vCoord, *vNormal, *vCoordIndex;
    VRNode *sfNode;
    void *params[22];
    float (*coords)[3];
    int *indexes;
    int valFlag[14];
    PovObject *newObject= NULL;

    vColor= aNode->getFieldContent(fColor, valFlag[0]);
    vCoord= aNode->getFieldContent(fCoord, valFlag[1]);
    vNormal= aNode->getFieldContent(fNormal, valFlag[2]);
    vCoordIndex= aNode->getFieldContent(fCoordIndex, valFlag[8]);

    if ((vColor != NULL) && (valFlag[0] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vColor)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }
    if ((vCoord != NULL) && (valFlag[1] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vCoord)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	    params[2]= (void *)aCtxt->getCurrentArray((void **)&coords);
	    params[3]= coords;
	}
    }
    if ((vNormal != NULL) && (valFlag[2] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vNormal)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }
    if ((vCoordIndex != NULL) && (valFlag[8] == 1)) {
	VRValue *elements;

	params[13]= (void *)((VRVector *)vCoordIndex)->getNbrElements();
	if (((unsigned int)params[13]) > 0) {
	    elements= ((VRVector *)vCoordIndex)->getValues();
	    indexes= new int[((unsigned int)params[13])];
	    ((VRNumeric *)elements)->getIntArray(indexes, ((unsigned int)params[13]));
	    params[14]= indexes;
	}
    }
	// ATTN: Code moved from parseVrml, not optimal.
    // First, scan the indices to see the size of the polynomials.
    // If all are size 3, then create a mesh.
    if (((unsigned int)params[13]) != 0) {
	unsigned int minSize, maxSize, nbrIndexes, *indexes, currentSize, nbrPolys;
	nbrIndexes= (unsigned int)params[13];
	indexes= (unsigned int *)params[14];
	nbrPolys= maxSize= currentSize= 0;
	minSize= 0x0FFFFFFFF;
	
	for (; nbrIndexes != 0; indexes++) {
	    if (*indexes == 0x0FFFFFFFF) {
		if (minSize > currentSize) minSize= currentSize;
		if (maxSize < currentSize) maxSize= currentSize;
		nbrPolys++;
		currentSize= 0;
	    }
	    else {
		currentSize++;
	    }
	    nbrIndexes--;
	}
	if (minSize == maxSize) {
	    if (minSize == 3) {
		newObject= PovObject::createMeshSet(aCtxt->getCurrentMatrix(), params, nbrPolys);
	    }
	    else {
		// Create Regular Polygons.
#if defined(USE_POLYGONS)
		newObject= PovObject::createRegularPolygonSet(aCtxt->getCurrentMatrix(), params, nbrPolys, minSize);
#else
		if (minSize == 4) {
		    newObject= PovObject::createSquareSet(aCtxt->getCurrentMatrix(), params, nbrPolys);
		}
		else {
			// Would need to do serious tessalation.
		}
#endif
	    }
	}
	else {
		// Must create unregular polygons.
	}
    }
    if (params[3] != NULL) delete[] (float *)params[3];
    if (params[14] != NULL) delete[] (int *)params[14];

    if (newObject != NULL) {
	aCtxt->setCurrentObject(newObject);
    }
}


void PovScene::mp_v2IndexedLineSet(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fColor= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedLineSet, "color");
    static VRField *fCoord= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedLineSet, "coord");
    static VRField *fColorIndex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedLineSet, "colorIndex");
    static VRField *fColorPerVertex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedLineSet, "colorPerVertex");
    static VRField *fCoordIndex= V2OptiDef::getFieldOf(V2OptiDef::v2IndexedLineSet, "coordIndex");
    VRValue *vColor, *vCoord, *vColorIndex, *vColorPerVertex, *vCoordIndex;
    VRNode *sfNode;
    double (*coords)[3];
    unsigned int nbrCoords;
    void *params[5]= { NULL, NULL, NULL, NULL, NULL };
    int valFlag[5];
    PovObject *newObject;

    vColor= aNode->getFieldContent(fColor, valFlag[0]);
    if ((vColor != NULL) && (valFlag[0] != 0)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vColor)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	    
	}
    }
    vCoord= aNode->getFieldContent(fCoord, valFlag[1]);
    if ((vCoord != NULL) && (valFlag[1] != 0)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vCoord)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	    nbrCoords= aCtxt->getCurrentArray((void **)&coords);
	}
    }

	// TODO: The rest.
    // ATTN: What PovRay object will represent a line set ?
}


void PovScene::mp_v2Inline(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2LOD(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Material(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fAmbientIntensity= V2OptiDef::getFieldOf(V2OptiDef::v2Material, "ambientIntensity");
    static VRField *fDiffuseColor= V2OptiDef::getFieldOf(V2OptiDef::v2Material, "diffuseColor");
    static VRField *fEmissiveColor= V2OptiDef::getFieldOf(V2OptiDef::v2Material, "emissiveColor");
    static VRField *fShininess= V2OptiDef::getFieldOf(V2OptiDef::v2Material, "shininess");
    static VRField *fSpecularColor= V2OptiDef::getFieldOf(V2OptiDef::v2Material, "specularColor");
    static VRField *fTransparency= V2OptiDef::getFieldOf(V2OptiDef::v2Material, "transparency");
    VRValue *vAmbientIntensity, *vDiffuseColor, *vEmissiveColor, *vShininess, *vSpecularColor, *vTransparency;
    double tAmbientIntensity, tShininess, tTransparency;
    Vector3 tDiffuseColor, tEmissiveColor, tSpecularColor;
    void *params[6]= { NULL, NULL, NULL, NULL, NULL, NULL };
    int valFlag[6];

    vAmbientIntensity= aNode->getFieldContent(fAmbientIntensity, valFlag[0]);
    vDiffuseColor= aNode->getFieldContent(fDiffuseColor, valFlag[1]);
    vEmissiveColor= aNode->getFieldContent(fEmissiveColor, valFlag[2]);
    vShininess= aNode->getFieldContent(fEmissiveColor, valFlag[3]);
    vSpecularColor= aNode->getFieldContent(fEmissiveColor, valFlag[4]);
    vTransparency= aNode->getFieldContent(fEmissiveColor, valFlag[5]);

    vAmbientIntensity= aNode->getFieldContent(fAmbientIntensity, valFlag[0]);
    if ((vAmbientIntensity != NULL) && (valFlag[0] != 0)) {
	tAmbientIntensity= ((VRNumeric *)vAmbientIntensity)->getDoubleValue();
	params[0]= &tAmbientIntensity;
    }
    vDiffuseColor= aNode->getFieldContent(fDiffuseColor, valFlag[1]);
    if ((vDiffuseColor != NULL) && (valFlag[1] != 0)) {
	if (((VRNumeric *)vDiffuseColor)->getDoubleArray(tDiffuseColor.Vc, 3) == 3) {
	    params[1]= &tDiffuseColor;
	}
    }
    vEmissiveColor= aNode->getFieldContent(fEmissiveColor, valFlag[2]);
    if ((vEmissiveColor != NULL) && (valFlag[2] != 0)) {
	if (((VRNumeric *)vEmissiveColor)->getDoubleArray(tEmissiveColor.Vc, 3) == 3) {
	    params[2]= &tEmissiveColor;
	}
    }
    vShininess= aNode->getFieldContent(fShininess, valFlag[3]);
    if ((vShininess != NULL) && (valFlag[3] != 0)) {
	tShininess= ((VRNumeric *)vShininess)->getDoubleValue();
	params[3]= &tShininess;
    }
    vSpecularColor= aNode->getFieldContent(fSpecularColor, valFlag[4]);
    if ((vSpecularColor != NULL) && (valFlag[4] != 0)) {
	if (((VRNumeric *)vSpecularColor)->getDoubleArray(tSpecularColor.Vc, 3) == 3) {
	    params[4]= &tSpecularColor;
	}
    }
    vTransparency= aNode->getFieldContent(fTransparency, valFlag[5]);
    if ((vTransparency != NULL) && (valFlag[5] != 0)) {
	tTransparency= ((VRNumeric *)vTransparency)->getDoubleValue();
	params[5]= &tTransparency;
    }

    if (aCtxt->getCurrentTexture() != NULL) aCtxt->getCurrentTexture()->modify(params);
}


void PovScene::mp_v2MovieTexture(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2NavigationInfo(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Normal(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2NormalInterpolator(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2OrientationInterpolator(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2PixelTexture(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2PlaneSensor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2PointLight(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fAmbientIntensity= V2OptiDef::getFieldOf(V2OptiDef::v2PointLight, "ambientIntensity");
    static VRField *fAttenuation= V2OptiDef::getFieldOf(V2OptiDef::v2PointLight, "attenuation");
    static VRField *fColor= V2OptiDef::getFieldOf(V2OptiDef::v2PointLight, "color");
    static VRField *fIntensity= V2OptiDef::getFieldOf(V2OptiDef::v2PointLight, "intensity");
    static VRField *fLocation= V2OptiDef::getFieldOf(V2OptiDef::v2PointLight, "location");
    static VRField *fOn= V2OptiDef::getFieldOf(V2OptiDef::v2PointLight, "on");
    static VRField *fRadius= V2OptiDef::getFieldOf(V2OptiDef::v2PointLight, "radius");
    VRValue *vAmbientIntensity, *vAttenuation, *vColor, *vIntensity, *vLocation, *vOn, *vRadius;
    double tAmbientIntensity, tIntensity, tRadius;
    Vector3 tAttenuation, tColor, tLocation;
    void *params[7]= { NULL, NULL, NULL, NULL, NULL };
    int valFlag[7];
    PovObject *newObject;

    vAmbientIntensity= aNode->getFieldContent(fAmbientIntensity, valFlag[0]);
    vAttenuation= aNode->getFieldContent(fAttenuation, valFlag[1]);
    vColor= aNode->getFieldContent(fColor, valFlag[2]);
    vIntensity= aNode->getFieldContent(fIntensity, valFlag[3]);
    vLocation= aNode->getFieldContent(fLocation, valFlag[4]);
    vOn= aNode->getFieldContent(fOn, valFlag[5]);
    vRadius= aNode->getFieldContent(fRadius, valFlag[6]);

    if ((vAmbientIntensity != NULL) && (valFlag[0] != 0)) {
	tAmbientIntensity= ((VRNumeric *)vAmbientIntensity)->getDoubleValue();
	params[0]= &tAmbientIntensity;
    }
    if ((vAttenuation != NULL) && (valFlag[1] != 0)) {
	if (((VRNumeric *)vAttenuation)->getDoubleArray(tAttenuation.Vc, 3) == 3) {
	    params[1]= &tAttenuation;
	}
    }
    if ((vColor != NULL) && (valFlag[2] != 0)) {
	if (((VRNumeric *)vColor)->getDoubleArray(tColor.Vc, 3) == 3) {
	    params[2]= &tColor;
	}
    }
    if ((vIntensity != NULL) && (valFlag[3] != 0)) {
	tIntensity= ((VRNumeric *)vIntensity)->getDoubleValue();
	params[3]= &tIntensity;
    }
    if ((vLocation != NULL) && (valFlag[4] != 0)) {
	if (((VRNumeric *)vLocation)->getDoubleArray(tLocation.Vc, 3) == 3) {
	    params[4]= &tLocation;
	}
    }
    if ((vOn != NULL) && (valFlag[5] != 0)) {
	params[5]= (void *)((VRNumeric *)vOn)->getIntValue();
    }
    if ((vRadius != NULL) && (valFlag[6] != 0)) {
	tRadius= ((VRNumeric *)vRadius)->getDoubleValue();
	params[6]= &tRadius;
    }

    if ((newObject= PovObject::createPointLight(aCtxt->getCurrentMatrix(), params)) != NULL) {
	PovRayGlue::frameObject(newObject);
    }
}


void PovScene::mp_v2PointSet(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fColor= V2OptiDef::getFieldOf(V2OptiDef::v2PointSet, "color");
    static VRField *fCoord= V2OptiDef::getFieldOf(V2OptiDef::v2PointSet, "coord");
    VRValue *vColor, *vCoord;
    VRNode *sfNode;
    void *params[2]= { NULL, NULL};
    int valFlag[2];
    PovObject *newObject;

    vColor= aNode->getFieldContent(fColor, valFlag[0]);
    vCoord= aNode->getFieldContent(fCoord, valFlag[1]);

    if ((vColor != NULL) && (valFlag[0] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vColor)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }
    if ((vCoord != NULL) && (valFlag[1] == 1)) {
	if ((sfNode= (VRNode *)((VRComplexValue *)vCoord)->getValue()) != NULL) {
	    mapNode(sfNode, aCtxt);
	}
    }
	// TODO: The rest.
    // ATTN: What is the right PovRay object to represent a PointSet.
}


void PovScene::mp_v2PositionInterpolator(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2ProximitySensor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2ScalarInterpolator(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Script(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Shape(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fAppear= V2OptiDef::getFieldOf(V2OptiDef::v2Shape, "appearance");
    static VRField *fGeometry= V2OptiDef::getFieldOf(V2OptiDef::v2Shape, "geometry");
    VRValue *appearance, *geometry;
    VRNode *mfNode;
    PovObject *anObject;
    int valFlag[2];

    appearance= aNode->getFieldContent(fAppear, valFlag[0]);
    geometry= aNode->getFieldContent(fGeometry, valFlag[1]);

    if ((geometry != NULL) && (valFlag[1] == 1)) {
	if ((mfNode= (VRNode *)((VRComplexValue *)geometry)->getValue()) != NULL) {
	    mapNode(mfNode, aCtxt);	// This will install a current object in the context.
	}
    }

    if ((appearance != NULL) && (valFlag[0] == 1)) {
	if ((mfNode= (VRNode *)((VRComplexValue *)appearance)->getValue()) != NULL) {
	    mapNode(mfNode, aCtxt);
	}
    }

    if ((anObject= aCtxt->getCurrentObject()) != NULL) {
	PovRayGlue::frameObject(anObject);
	aCtxt->clearCurrentObject();
    }
}


void PovScene::mp_v2Sound(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Sphere(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fRadius= V2OptiDef::getFieldOf(V2OptiDef::v2Sphere, "radius");
    VRValue *vRadius;
    double tRadius;
    void *params[1]= { NULL };
    int valFlag;
    PovObject *newObject;

    vRadius= aNode->getFieldContent(fRadius, valFlag);
    if ((vRadius != NULL) && (valFlag != 0)) {
	tRadius= ((VRNumeric *)vRadius)->getDoubleValue();
	params[0]= &tRadius;
    }

    if ((newObject= PovObject::createSphere(aCtxt->getCurrentMatrix(), params)) != NULL) {
	aCtxt->setCurrentObject(newObject);
    }
}


void PovScene::mp_v2SphereSensor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2SpotLight(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fAmbientIntensity= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "ambientIntensity");
    static VRField *fAttenuation= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "attenuation");
    static VRField *fBeamWidth= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "beamWidth");
    static VRField *fColor= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "color");
    static VRField *fCutOffAngle= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "cutOffAngle");
    static VRField *fDirection= V2OptiDef::getFieldOf(V2OptiDef::v2DirectionalLight, "direction");
    static VRField *fIntensity= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "intensity");
    static VRField *fLocation= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "location");
    static VRField *fOn= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "on");
    static VRField *fRadius= V2OptiDef::getFieldOf(V2OptiDef::v2SpotLight, "radius");
    VRValue *vAmbientIntensity, *vAttenuation, *vBeamWidth, *vColor, *vCutOffAngle, *vDirection,
    			*vIntensity, *vLocation, *vOn, *vRadius;
    double tAmbientIntensity, tBeamWidth, tCutOffAngle,
		tIntensity, tRadius;
    Vector3 tAttenuation, tColor, tDirection, tLocation;
    void *params[10]= { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int valFlag[10];
    PovObject *newObject;

    vAmbientIntensity= aNode->getFieldContent(fAmbientIntensity, valFlag[0]);
    vAttenuation= aNode->getFieldContent(fAttenuation, valFlag[1]);
    vBeamWidth= aNode->getFieldContent(fBeamWidth, valFlag[2]);
    vColor= aNode->getFieldContent(fColor, valFlag[3]);
    vCutOffAngle= aNode->getFieldContent(fCutOffAngle, valFlag[4]);
    vDirection= aNode->getFieldContent(fDirection, valFlag[5]);
    vIntensity= aNode->getFieldContent(fIntensity, valFlag[6]);
    vLocation= aNode->getFieldContent(fLocation, valFlag[7]);
    vOn= aNode->getFieldContent(fOn, valFlag[8]);
    vRadius= aNode->getFieldContent(fRadius, valFlag[9]);

    if ((vAmbientIntensity != NULL) && (valFlag[0] != 0)) {
	tAmbientIntensity= ((VRNumeric *)vAmbientIntensity)->getDoubleValue();
	params[0]= &tAmbientIntensity;
    }
    if ((vAttenuation != NULL) && (valFlag[1] != 0)) {
	if (((VRNumeric *)vAttenuation)->getDoubleArray(tAttenuation.Vc, 3) == 3) {
	    params[1]= &tAttenuation;
	}
    }
    if ((vBeamWidth != NULL) && (valFlag[2] != 0)) {
	tBeamWidth= ((VRNumeric *)vBeamWidth)->getDoubleValue();
	params[2]= &tBeamWidth;
    }
    if ((vColor != NULL) && (valFlag[3] != 0)) {
	if (((VRNumeric *)vColor)->getDoubleArray(tColor.Vc, 3) == 3) {
	    params[3]= &tColor;
	}
    }
    if ((vCutOffAngle != NULL) && (valFlag[4] != 0)) {
	tCutOffAngle= ((VRNumeric *)vCutOffAngle)->getDoubleValue();
	params[4]= &tCutOffAngle;
    }
    if ((vDirection != NULL) && (valFlag[5] != 0)) {
	if (((VRNumeric *)vDirection)->getDoubleArray(tDirection.Vc, 3) == 3) {
	    params[5]= &tDirection;
	}
    }
    if ((vIntensity != NULL) && (valFlag[6] != 0)) {
	tIntensity= ((VRNumeric *)vIntensity)->getDoubleValue();
	params[6]= &tIntensity;
    }
    if ((vLocation != NULL) && (valFlag[7] != 0)) {
	if (((VRNumeric *)vLocation)->getDoubleArray(tLocation.Vc, 3) == 3) {
	    params[7]= &tLocation;
	}
    }
    if ((vOn != NULL) && (valFlag[8] != 0)) {
	params[8]= (void *)((VRNumeric *)vOn)->getIntValue();
    }
    if ((vRadius != NULL) && (valFlag[9] != 0)) {
	tRadius= ((VRNumeric *)vRadius)->getDoubleValue();
	params[9]= &tRadius;
    }

    if ((newObject= PovObject::createSpotLight(aCtxt->getCurrentMatrix(), params)) != NULL) {
	PovRayGlue::frameObject(newObject);
    }
}


void PovScene::mp_v2Switch(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Text(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2TextureCoordinate(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2TextureTransform(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2TimeSensor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2TouchSensor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2Transform(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fCenter= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "center");
    static VRField *fChildren= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "children");
    static VRField *fRotation= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "rotation");
    static VRField *fScale= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "scale");
    static VRField *fScaleOrientation= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "scaleOrientation");
    static VRField *fTranslation= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "translation");
    static VRField *fBboxCenter= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "bboxCenter");
    static VRField *fBboxSize= V2OptiDef::getFieldOf(V2OptiDef::v2Transform, "bboxSize");
    VRValue *vCenter, *vChildren, *vRotation, *vScale, *vScaleOrientation, *vTranslation,
    	*vBboxCenter, *vBboxSize;
    Vector3 tCenter, tScale, tTranslation, tBboxCenter, tBboxSize;
    Vector4 tRotation, tScaleOrientation;
    Vector3 defCenter(0.0, 0.0, 0.0);
    Vector4 defRotation(0.0, 0.0, 1.0, 0.0);
    Vector3 defScale(1.0, 1.0, 1.0);
    Vector4 defScaleOrientation(0.0, 0.0, 1.0, 0.0);
    Vector3 defTranslation(0.0, 0.0, 0.0);
    void *params[7]= { &defCenter, &defRotation, &defScale, &defScaleOrientation, &defTranslation, NULL, NULL};
    int valFlag[8];

    vCenter= aNode->getFieldContent(fCenter, valFlag[0]);
    vChildren= aNode->getFieldContent(fChildren, valFlag[1]);
    vRotation= aNode->getFieldContent(fRotation, valFlag[2]);
    vScale= aNode->getFieldContent(fScale, valFlag[3]);
    vScaleOrientation= aNode->getFieldContent(fScaleOrientation, valFlag[4]);
    vTranslation= aNode->getFieldContent(fTranslation, valFlag[5]);
    vBboxCenter= aNode->getFieldContent(fBboxCenter, valFlag[6]);
    vBboxSize= aNode->getFieldContent(fBboxSize, valFlag[7]);


    if ((vCenter != NULL) && (valFlag[0] != 0)) {
	if (((VRNumeric *)vCenter)->getDoubleArray(tCenter.Vc, 3) == 3) {
	    params[0]= &tCenter;
	}
    }
    if ((vRotation != NULL) && (valFlag[2] != 0)) {
	if (((VRNumeric *)vRotation)->getDoubleArray(tRotation.Vc, 4) == 4) {
	    params[1]= &tRotation;
	}
    }
    if ((vScale != NULL) && (valFlag[3] != 0)) {
	if (((VRNumeric *)vScale)->getDoubleArray(tScale.Vc, 3) == 3) {
	    params[2]= &tScale;
	}
    }
    if ((vScaleOrientation != NULL) && (valFlag[4] != 0)) {
	if (((VRNumeric *)vScaleOrientation)->getDoubleArray(tScaleOrientation.Vc, 4) == 4) {
	    params[3]= &tScaleOrientation;
	}
    }
    if ((vTranslation != NULL) && (valFlag[5] != 0)) {
	if (((VRNumeric *)vTranslation)->getDoubleArray(tTranslation.Vc, 3) == 3) {
	    params[4]= &tTranslation;
	}
    }

// TODO: Take care of BBox parameters.

    aCtxt->pushTransfo((Vector3 *)params[4], (Vector4 *)params[1], (Vector3 *)params[2], (Vector3 *)params[0], (Vector4 *)params[3]);

    if ((vChildren != NULL) && (valFlag[1] == 1)) {
	VRNode *mfNode;

	switch(((VRComplexValue *)vChildren)->getType()) {
	    case VRComplexValue::ntNode:
		if ((mfNode= (VRNode *)((VRComplexValue *)vChildren)->getValue()) != NULL) {
		    mapNode(mfNode, aCtxt);
		}
		break;
	    case VRComplexValue::ntMfNode:
		if ((mfNode= (VRNode *)((VRComplexValue *)vChildren)->getValue()) != NULL) {
		    do {
			mapNode(mfNode, aCtxt);
			mfNode= (VRNode *)mfNode->getNext();
		    } while (mfNode != NULL);
		}
		break;
	    default:
		// TODO: Take care of other kinds.
	        break;
	}
    }

    aCtxt->popTransfo();
}


void PovScene::mp_v2Viewpoint(VRNode *aNode, RenderContext *aCtxt)
{
    static VRField *fFieldOfView= V2OptiDef::getFieldOf(V2OptiDef::v2Viewpoint, "fieldOfView");
    static VRField *fOrientation= V2OptiDef::getFieldOf(V2OptiDef::v2Viewpoint, "orientation");
    static VRField *fPosition= V2OptiDef::getFieldOf(V2OptiDef::v2Viewpoint, "position");

    if (!aCtxt->hasFlag(RenderContext::hasViewpoint)) {
	VRValue *vFov, *vOrientation, *vPosition;
	int valFlag[3];
	double tFov, tOrientation[4], tPos[3];
	void *params[3]= { NULL, NULL, NULL };

	vFov= aNode->getFieldContent(fFieldOfView, valFlag[0]);
	vOrientation= aNode->getFieldContent(fOrientation, valFlag[1]);
	vPosition= aNode->getFieldContent(fPosition, valFlag[2]);

	if ((vFov != NULL) && (valFlag[0] != 0)) {
	    tFov= ((VRNumeric *)vFov)->getDoubleValue();
	    params[0]= &tFov;
	}
	if ((vOrientation != NULL) && (valFlag[1] != 0)) {
	    if (((VRNumeric *)vOrientation)->getDoubleArray(tOrientation, 4) == 4) {
		params[1]= tOrientation;
	    }
	}
	if ((vPosition != NULL) && (valFlag[2] != 0)) {
	    if (((VRNumeric *)vPosition)->getDoubleArray(tPos, 3) == 3) {
		params[2]= tPos;
	    }
	}
	PovRayGlue::defineCamera(aCtxt->getFrame(), params);
	aCtxt->setFlag(RenderContext::hasViewpoint);
    }
}


void PovScene::mp_v2VisibilitySensor(VRNode *aNode, RenderContext *aCtxt)
{
}


void PovScene::mp_v2WorldInfo(VRNode *aNode, RenderContext *aCtxt)
{
}



