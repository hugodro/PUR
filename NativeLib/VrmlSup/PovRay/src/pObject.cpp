/**************************************************
* File: pObject.cc.
* Desc: Implementation of the PovObject class.
* Module: AkraLog : PovRayExt.
* Rev: 22 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#if defined(__linux__)
#include <string.h>
#endif

#if defined(_WIN32)
#include <string.h>
#endif
#include "pObject.h"


/**************************************************
* Implementation: PovContainer.
**************************************************/

PovContainer::PovContainer(void *anObject)
{
    lowObject= anObject;
}


void *PovContainer::getLowObject(void)
{
    return lowObject;
}


/**************************************************
* Implementation: PovObject.
**************************************************/

PovObject::PovObject(void *anObject, Type aType)
    : PovContainer(anObject)
{
    type= aType;
    texture= NULL;
}


PovObject::~PovObject(void)
{
    if (texture != NULL) delete texture;
}


PovObject::Type PovObject::getType(void)
{
    return type;
}


void PovObject::addTexture(PovTexture *aTexture)
{
    texture= aTexture;
}


/**************************************************
* Implementation: PovArrayObject.
**************************************************/

PovArrayObject::PovArrayObject(void *content, unsigned int aCount)
    : PovObject(NULL, PovObject::polygonArray)
{
    void **tmpValue;

    count= aCount;
    tmpValue= new void*[count];
    memcpy(tmpValue, (void **)content, count * sizeof(void *));
    lowObject= tmpValue;
    contentType= pure;
}


PovArrayObject::PovArrayObject(void *content, void *prefix, unsigned int aCount)
    : PovObject(NULL, PovObject::polygonArray)
{
    void **tmpValue;

    count= aCount;
    tmpValue= new void*[count+1];
    tmpValue[0]= prefix;
    memcpy(&tmpValue[1], (void **)content, count * sizeof(void *));
    lowObject= tmpValue;
    contentType= mixed;
}


PovArrayObject::~PovArrayObject(void)
{
    delete[] (void **)lowObject;
    if (texture != NULL) { delete texture; texture == NULL; }
}


PovArrayObject::ContentType PovArrayObject::getContentType(void)
{
    return contentType;
}


unsigned int PovArrayObject::getCount(void)
{
    return count;
}


/**************************************************
* Implementation: PovTexture.
**************************************************/

PovTexture::PovTexture(void *aTexture)
    : PovContainer(aTexture)
{
    // Things will get here.
}


