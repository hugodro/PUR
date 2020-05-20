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

  public:
    static PovObject *createBox(TrMatrix *aTransfo, void **params);
    static PovObject *createCone(TrMatrix *aTransfo, void **params);
    static PovObject *createCylinder(TrMatrix *aTransfo, void **params);
    static PovObject *createHeightField(TrMatrix *aTransfo, void **params);
    static PovObject *createLathe(TrMatrix *aTransfo, void **params);
    static PovObject *createMeshSet(TrMatrix *aTransfo, void **params, unsigned int nbrTriangles);
    static PovObject *createRegularPolygonSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys, int polySize);
    static PovObject *createPolygonSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys);
    static PovObject *createSquareSet(TrMatrix *aTransfo, void **params, unsigned int nbrPolys);
    static PovObject *createSphere(TrMatrix *aTransfo, void **params);
    static PovObject *createDirLight(TrMatrix *aTransfo, void **params);
    static PovObject *createPointLight(TrMatrix *aTransfo, void **params);
    static PovObject *createSpotLight(TrMatrix *aTransfo, void **params);

  protected:
    Type type;

  public:
    PovObject(void *anObject, Type aType);
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
    static void setReference(void *aValue);
    static PovTexture *createTexture(void);

  protected:
    static void *reference;

  public:
    PovTexture(void *aTexture);
    void modify(void **params);
};


#endif	/* _POVRAYOBJECT_H_ */
