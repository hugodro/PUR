/**************************************************
* File: renderContext.cc.
* Desc: Implementation of the RenderContext class.
* Module: AkraLog : PovRayExt.
* Rev: 14 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include "maths.h"
#include "pObject.h"
#include "renderContext.h"


/**************************************************
* Implementation: StackedTransfo.
**************************************************/

StackedTransfo::StackedTransfo(void)
{
    previous= NULL;
}


StackedTransfo::StackedTransfo(Vector3 *translation, Vector4 *rotation, Vector3 *scaling, Vector3 *center, Vector4 *scaleRotation, StackedTransfo *link)
{
    if (link != NULL) {
	matrix.translationOf(&link->matrix, translation);
    }
    else {
	matrix.translateBy(translation);
    }

    matrix.translateBy(center);
    matrix.rotateBy(rotation);
    matrix.rotateBy(scaleRotation);
    matrix.scaleBy(scaling);
    scaleRotation->Vc[3]= -scaleRotation->Vc[3];
    matrix.rotateBy(scaleRotation);
    center->Vc[0]= -center->Vc[0]; center->Vc[1]= -center->Vc[1]; center->Vc[2]= -center->Vc[2];
    matrix.translateBy(center);
    previous= link;
}


/**************************************************
* Implementation: RenderContext.
**************************************************/

RenderContext::RenderContext(PovScene *aParent, RenderContext *aContext)
{
    next= aContext;
    owner= aParent;
    transformations= new StackedTransfo();
    currentObject= NULL;
    currentTexture= NULL;
    frame= NULL;
    xMinBound= yMinBound= zMinBound= xMaxBound= yMaxBound= zMaxBound= 0.0;
    flags= 0;
}


void RenderContext::setFrame(void *aFrame)
{
    frame= aFrame;
}


void *RenderContext::getFrame(void)
{
    return frame;
}


int RenderContext::render(void)
{
    return 0;
}


void RenderContext::cleanTransformation(void)
{
    StackedTransfo *tmpTransfo, *nextTransfo;

    tmpTransfo= transformations;
    while (tmpTransfo != NULL) {
	nextTransfo= tmpTransfo->previous;
	delete tmpTransfo;
	tmpTransfo= nextTransfo;
    }
    transformations= new StackedTransfo();
}


void RenderContext::pushTransfo(Vector3 *translation, Vector4 *rotation, Vector3 *scaling, Vector3 *center, Vector4 *scaleRotation)
{
    transformations= new StackedTransfo(translation, rotation, scaling, center, scaleRotation, transformations);
}


void RenderContext::popTransfo(void)
{
    if (transformations != NULL) {
	StackedTransfo *tmpTransfo;
	tmpTransfo= transformations;
	transformations= transformations->previous;
	delete tmpTransfo;
    }
}


TrMatrix *RenderContext::getCurrentMatrix(void)
{
    if (transformations != NULL) {
	return &transformations->matrix;
    }
    return NULL;
}


bool RenderContext::hasFlag(Flags aFlag)
{
    return ((flags & aFlag) != 0);
}


void RenderContext::setFlag(Flags aFlag)
{
    flags|= aFlag;
}


void RenderContext::setCurrentObject(PovObject *anObject)
{
    currentObject= anObject;
}


PovObject *RenderContext::getCurrentObject(void)
{
    return currentObject;
}


void RenderContext::setCurrentTexture(PovTexture *aTexture)
{
    currentTexture= aTexture;
}


PovTexture *RenderContext::getCurrentTexture(void)
{
    return currentTexture;
}

void RenderContext::clearCurrentObject(void)
{
    delete currentObject;
    if (currentTexture != NULL) delete currentTexture;
    currentObject= NULL;
    currentTexture= NULL;
    currentArray= NULL;
    arraySize= 0;
}


void RenderContext::setCurrentArray(unsigned int size, void *elements)
{
    currentArray= elements;
    arraySize= size;
}

unsigned int RenderContext::getCurrentArray(void **elements)
{
    *elements= currentArray;
    return arraySize;
}

