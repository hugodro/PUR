#ifndef _LINKPOVRAY_H_
#define _LINKPOVRAY_H_
/**************************************************
* File: pLink.h.
* Desc: Definition of the LinkPovRay class.
* Module: AkraLog : VrmlSupport.
* Rev: 24 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>

class PovObject;
class PovTexture;
class TrMatrix;
class AkCoord3D;
class AkCoord4D;
class AkColor;

class LinkPovRay : public AkObject {
  protected:	// Class variables.
    static LinkPovRay *contexts;
    static void *referenceTexture;
    static void *referenceCamera;
    static bool coldInitFlag;

  public:		// Class methods.
    static LinkPovRay *createContext(void);
    static void terminateRenderer(void);

  protected:		// Instance variables.
    LinkPovRay *next;	// Linked list anchor.

  protected:			// Instance methods.
    LinkPovRay(LinkPovRay *link);

  public:
    void produceImage(char *aName);
    void setImageFile(char *aFormat, char *aPath);

	// Geometry creators.
    virtual PovObject *createBox(TrMatrix *transfo, double sizeX, double sizeY, double sizeZ);
    virtual PovObject *createCone(TrMatrix *transfo, double bottomRadius, double height);
    virtual PovObject *createCylinder(TrMatrix *transfo, double radius, double height);
    virtual PovObject *createTriangleSet(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrTriangles);
    virtual PovObject *createRegularPolygonSet(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrPolygons, unsigned int polySize);
    virtual PovObject *createPolygonSet(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrIndexes, unsigned int nbrPolygons, unsigned int nbrTriangles, unsigned int nbrSquares);
    virtual PovObject *createSphere(TrMatrix *transfo, double radius);

	// Light creators.
    virtual PovObject *createDirLight(TrMatrix *transfo, double ambient, double intensity, AkColor *color, AkCoord3D *direction);
    virtual PovObject *createPointLight(TrMatrix *transfo, double ambient, double intensity, AkColor *color, float radius, AkCoord3D *attenuation, AkCoord3D *location);
    virtual PovObject *createSpotLight(TrMatrix *transfo, double ambient, double intensity, AkColor *color, float width, float cutAngle, float radius, AkCoord3D *attenuation, AkCoord3D *location, AkCoord3D *direction);

	// Texture creators.
    virtual PovTexture *createTexture(TrMatrix *transfo, AkColor *diffuse, AkColor *emissive, AkColor *specular, float shininess, float transparency);
    virtual PovTexture *createDefaultTexture(void);

/* TODO:
    virtual PovObject *createHeightField(TrMatrix *transfo);
    virtual PovObject *createLathe(TrMatrix *transfo);
*/

    virtual PovObject *createTriaSquares(TrMatrix *transfo, AkCoord3D *vertices, int *indexes, unsigned int nbrPoints, unsigned int nbrPolygons);

	// Object manipulation.
    virtual void linkTextureToObject(PovObject *anObject, PovTexture *aTexture);
    virtual void useObject(PovObject *anObject);

	// Camera Orientation.
    virtual void setCameraTo(AkCoord3D *pos, AkCoord4D *orient, float fov);
};


#endif		/* _LINKPOVRAY_H_ */
