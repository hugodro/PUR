#ifndef _POVRAYOBJECT_H_
#define _POVRAYOBJECT_H_
/**************************************************
* File: pObject.h.
* Desc: Definition of the PovObject class.
* Module: AkraLog : PovRayExt.
* Rev: 22 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class TrMatrix;
class PovTexture;

class PovContainer : public AkObject {
  protected:
    void *lowObject;

  public:
    PovContainer(void *anObject);
    void *getLowObject(void);
};


class PovObject : public PovContainer {
  public:
    enum Type {
	box, cone, cylinder, heightField, lathe, mesh, polygon, sphere
	, light
	, camera
	, polygonArray
    };

  protected:
    Type type;
    PovTexture *texture;

  public:
    PovObject(void *anObject, Type aType);
    ~PovObject(void);
    Type getType(void);
    void addTexture(PovTexture *aTexture);
};


class PovArrayObject : public PovObject {
  public:
    enum ContentType {
	pure, mixed
    };

  protected:
    unsigned int count;
    ContentType contentType;

  public:
    PovArrayObject(void *content, unsigned int aCount);
    PovArrayObject(void *content, void *prefix, unsigned int aCount);
    ~PovArrayObject(void);
    ContentType getContentType(void);
    unsigned int getCount(void);
};


class PovTexture : public PovContainer {
  public:
    PovTexture(void *aTexture);
};

#endif	/* _POVRAYOBJECT_H_ */
