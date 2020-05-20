#ifndef _RENDERCONTEXT_H_
#define _RENDERCONTEXT_H_
/**************************************************
* File: renderContext.h.
* Desc: Definition of the RenderContext class.
* Module: AkraLog : PovRayExt.
* Rev: 14 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>

#include "maths.h"


extern "C" {
#if defined(_WIN32)
#undef __cplusplus
#endif
#include "frame.h"
#if defined(_WIN32)
#define __cplusplus
#endif
}


class PovScene;
class PovObject;
class PovTexture;
class VRNode;


class StackedTransfo : public AkObject {
  friend class RenderContext;
  protected:
    StackedTransfo *previous;
    TrMatrix matrix;

  public:
    StackedTransfo(void);
    StackedTransfo(Vector3 *translation, Vector4 *rotation, Vector3 *scaling, Vector3 *center, Vector4 *scaleRotation, StackedTransfo *link);
};


class RenderContext : public AkObject {
  public:
    enum Flags {
	hasViewpoint= 1, hasLight= 2
    };

  protected:
    RenderContext *next;
    PovScene *owner;
    StackedTransfo *transformations;
    PovObject *currentObject;
    PovTexture *currentTexture;
    void *frame;
    float xMinBound, yMinBound, zMinBound, xMaxBound, yMaxBound, zMaxBound;
    void *currentArray;
    unsigned int arraySize;
    unsigned int flags;

  public:

    RenderContext(PovScene *aParent, RenderContext *aContext);
    void setFrame(void *aFrame);
    void *getFrame(void);
    int render(void);
    void cleanTransformation(void);
    void pushTransfo(Vector3 *translation, Vector4 *rotation, Vector3 *scaling, Vector3 *center, Vector4 *scaleRotation);
    void popTransfo(void);
    TrMatrix *getCurrentMatrix(void);
    bool hasFlag(Flags aFlag);
    void setFlag(Flags aFlag);
    void setCurrentObject(PovObject *anObject);
    PovObject *getCurrentObject(void);
    void setCurrentTexture(PovTexture *aTexture);
    PovTexture *getCurrentTexture(void);
    void clearCurrentObject(void);
    void setCurrentArray(unsigned int size, void *elements);
    unsigned int getCurrentArray(void **elements);
};


#endif		/* _RENDERCONTEXT_H_ */

