/**************************************************
* File: povEngine.cc.
* Desc: Implementation of the PovRayEngine class.
* Module: AkraLog : VrmlSupport.
* Rev: 23 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vrcore/containerStack.h>
#include <vrcore/transfoStack.h>
#include <vrcore/generalVrml.h>
#include <vrcore/geometriesVrml.h>
#include <vrcore/lightingVrml.h>
#include "pLink.h"
#include "povExtensions.h"
#include "povEngine.h"


PovRayEngine::PovRayEngine(char *format, char *prefix, int width, int height, int depth)
{
    povLink= LinkPovRay::createContext();
    viewPoint= NULL;
    nodeTree= NULL;
    treeStack= new ContainerStack();
    transfoStack= new TransfoStack();
    povLink->setImageFile(format, prefix);
}


void PovRayEngine::pushContainer(ContainerVrml *aContainer)
{
    treeStack->push(aContainer);
}


void PovRayEngine::popContainer(void)
{
    treeStack->pop();
}


void PovRayEngine::deleteSceneTree(void)
{
    if (nodeTree != NULL) {
	nodeTree->destroy();
    }
    nodeTree= NULL;
}


void PovRayEngine::renderTree(void)
{
    static unsigned int frameNbr= 0;
    static char tmpFrameName[20]= { "_test" };

    if (nodeTree != NULL) {
	nodeTree->renderOn(this);

         povLink->setCameraTo(&viewPoint->position, &viewPoint->orientation, viewPoint->fieldOfView);

	tmpFrameName[5]= (char)('0' + (frameNbr / 100));
	tmpFrameName[6]= (char)('0' + ((frameNbr / 10) % 10));
	tmpFrameName[7]= (char)('0' + (frameNbr % 10));
	tmpFrameName[8]= '\0';
	povLink->produceImage(tmpFrameName);
	frameNbr++;
    }
}


void PovRayEngine::addToScene(NodeVrml *aNode)
{
    if (nodeTree == NULL) nodeTree= aNode;
    else {
	if (treeStack->isUsed()) {
	    treeStack->topNode()->addChild(aNode);
	}
    }
}


void PovRayEngine::drawBox(BoxVrml *aBox, AppearanceVrml *appearance)
{
    PovObject *result;
    PovTexture *texture;

    ((ExtBoxVrml *)aBox)->setPovEquiv((result= povLink->createBox(transfoStack->getMatrix(), aBox->size[0], aBox->size[1], aBox->size[2])));

    if (appearance != NULL) {
	if (appearance->material != NULL) {
	    texture= povLink->createTexture(transfoStack->getMatrix(),
				&appearance->material->diffuseColor, &appearance->material->emissiveColor,
				&appearance->material->specularColor,
				appearance->material->shininess, appearance->material->transparency);
	}
	else {
	    texture= povLink->createDefaultTexture();
	}
    }
    else {
	texture= povLink->createDefaultTexture();
    }

    povLink->linkTextureToObject(result, texture);
    povLink->useObject(result);
}


void PovRayEngine::drawCone(ConeVrml *aCone, AppearanceVrml *appearance)
{
    PovObject *result;
    PovTexture *texture;

    ((ExtConeVrml *)aCone)->setPovEquiv((result= povLink->createCone(transfoStack->getMatrix(), aCone->bottomRadius, aCone->height)));
    if (appearance != NULL) {
	if (appearance->material != NULL) {
	    texture= povLink->createTexture(transfoStack->getMatrix(),
				&appearance->material->diffuseColor, &appearance->material->emissiveColor,
				&appearance->material->specularColor,
				appearance->material->shininess, appearance->material->transparency);
	}
	else {
	    texture= povLink->createDefaultTexture();
	}
    }
    else {
	texture= povLink->createDefaultTexture();
    }

    povLink->linkTextureToObject(result, texture);
    povLink->useObject(result);
}


void PovRayEngine::drawCylinder(CylinderVrml *aCylinder, AppearanceVrml *appearance)
{
    PovObject *result;
    PovTexture *texture;

    ((ExtCylinderVrml *)aCylinder)->setPovEquiv((result= povLink->createCylinder(transfoStack->getMatrix(), aCylinder->radius, aCylinder->height)));
    if (appearance != NULL) {
	if (appearance->material != NULL) {
	    texture= povLink->createTexture(transfoStack->getMatrix(),
				&appearance->material->diffuseColor, &appearance->material->emissiveColor,
				&appearance->material->specularColor,
				appearance->material->shininess, appearance->material->transparency);
	}
	else {
	    texture= povLink->createDefaultTexture();
	}
    }
    else {
	texture= povLink->createDefaultTexture();
    }

    povLink->linkTextureToObject(result, texture);
    povLink->useObject(result);
}


void PovRayEngine::drawElevationGrid(ElevationGridVrml *aGrid, AppearanceVrml *appearance)
{
// TODO.
}


void PovRayEngine::drawExtrusion(ExtrusionVrml *anExtrusion, AppearanceVrml *appearance)
{
// TODO.
}


void PovRayEngine::drawIndexedFaceSet(IndexedFaceSetVrml *aFaceSet, AppearanceVrml *appearance)
{
    PovObject *result;
    PovTexture *texture;
    unsigned int minSize, maxSize, *indexes, currentSize, nbrPolys;
    unsigned int nbrTriangles, nbrSquares;

    minSize= 0x0FFFFFFFF;
    nbrPolys= currentSize= maxSize= 0;
    nbrSquares= nbrTriangles= 0;

    for (unsigned int i= 0; i < aFaceSet->nbrIndexes; i++) {
	if (aFaceSet->indexes[i] == 0x0FFFFFFFF) {
	    if (currentSize == 3) nbrTriangles++;
	    else if (currentSize == 4) nbrSquares++;
	    if (minSize > currentSize) minSize= currentSize;
	    if (maxSize < currentSize) maxSize= currentSize;
	    nbrPolys++;
	    currentSize= 0;
	}
	else {
	    currentSize++;
	}
    }

    if (minSize == maxSize) {
	if (minSize == 3) {		// Triangles.
	    ((ExtIndexedFaceSetVrml *)aFaceSet)->setPovEquiv((result= povLink->createTriangleSet(transfoStack->getMatrix(), aFaceSet->coordinates, aFaceSet->indexes, aFaceSet->nbrPoints, nbrPolys)));
	}
	else {			// Regular polygons.
	    ((ExtIndexedFaceSetVrml *)aFaceSet)->setPovEquiv((result= povLink->createRegularPolygonSet(transfoStack->getMatrix(), aFaceSet->coordinates, aFaceSet->indexes, aFaceSet->nbrPoints, nbrPolys, minSize)));
	}
    }
    else {			// Mixed kinds of polygons.
	((ExtIndexedFaceSetVrml *)aFaceSet)->setPovEquiv((result= povLink->createPolygonSet(transfoStack->getMatrix(), aFaceSet->coordinates, aFaceSet->indexes, aFaceSet->nbrPoints, aFaceSet->nbrIndexes, nbrPolys, nbrTriangles, nbrSquares)));
    }

    if (appearance != NULL) {
	if (appearance->material != NULL) {
	    texture= povLink->createTexture(transfoStack->getMatrix(),
				&appearance->material->diffuseColor, &appearance->material->emissiveColor,
				&appearance->material->specularColor,
				appearance->material->shininess, appearance->material->transparency);
	}
	else {
	    texture= povLink->createDefaultTexture();
	}
    }
    else {
	texture= povLink->createDefaultTexture();
    }

    povLink->linkTextureToObject(result, texture);
    povLink->useObject(result);
}


void PovRayEngine::drawIndexedLineSet(IndexedLineSetVrml *aLineSet, AppearanceVrml *appearance)
{
// TODO.
}


void PovRayEngine::drawPointSet(PointSetVrml *aPointSet, AppearanceVrml *appearance)
{
// TODO.
}


void PovRayEngine::drawSphere(SphereVrml *aSphere, AppearanceVrml *appearance)
{
    PovObject *result;
    PovTexture *texture;

    ((ExtSphereVrml *)aSphere)->setPovEquiv((result= povLink->createSphere(transfoStack->getMatrix(), aSphere->radius)));
    if (appearance != NULL) {
	if (appearance->material != NULL) {
	    texture= povLink->createTexture(transfoStack->getMatrix(),
				&appearance->material->diffuseColor, &appearance->material->emissiveColor,
				&appearance->material->specularColor,
				appearance->material->shininess, appearance->material->transparency);
	}
	else {
	    texture= povLink->createDefaultTexture();
	}
    }
    else {
	texture= povLink->createDefaultTexture();
    }

    povLink->linkTextureToObject(result, texture);
    povLink->useObject(result);
}


void PovRayEngine::drawText(TextVrml *aText, AppearanceVrml *appearance)
{
// TODO.
}


void PovRayEngine::openTransform(TransformVrml *aTransform)
{
    transfoStack= new TransfoStack(&aTransform->translation, &aTransform->rotation,
							&aTransform->scale, &aTransform->center,
							&aTransform->scaleOrient, transfoStack);
}


void PovRayEngine::closeTransform(TransformVrml *aTransform)
{
    TransfoStack *tmpElement= transfoStack;

    transfoStack= transfoStack->getPrevious();
    delete tmpElement;
}


void PovRayEngine::addLighting(LightVrml *aLight)
{
    ExtDirectionalLightVrml *dirLight;
    ExtPointLightVrml *pointLight;
    ExtSpotLightVrml *spotLight;
    PovObject *result;

    switch(aLight->getKind()) {
	case LightVrml::directional:
	    dirLight= (ExtDirectionalLightVrml *)aLight;
	    dirLight->setPovEquiv((result= povLink->createDirLight(transfoStack->getMatrix(), dirLight->ambientIntensity, dirLight->intensity, &dirLight->color, &dirLight->direction)));
	    break;
	case LightVrml::point:
	    pointLight= (ExtPointLightVrml *)aLight;
	    pointLight->setPovEquiv((result= povLink->createPointLight(transfoStack->getMatrix(), pointLight->ambientIntensity, pointLight->intensity, &pointLight->color, pointLight->radius, &pointLight->attenuation, &pointLight->location)));
	    break;
	case LightVrml::spot:
	    spotLight= (ExtSpotLightVrml *)aLight;
	    spotLight->setPovEquiv((result= povLink->createSpotLight(transfoStack->getMatrix(), spotLight->ambientIntensity, spotLight->intensity, &spotLight->color, spotLight->beamWidth, spotLight->cutOffAngle, spotLight->radius, &spotLight->attenuation, &spotLight->direction, &spotLight->location)));
	    break;
	default:
		// TODO: Catch the internal error.
	    result= NULL;
	    break;
    }

    if (result != NULL) povLink->useObject(result);
}


void PovRayEngine::useViewPoint(ViewPointVrml *aVP)
{
    if (viewPoint != NULL) {
   	delete viewPoint;
    }
    viewPoint= aVP;
}


