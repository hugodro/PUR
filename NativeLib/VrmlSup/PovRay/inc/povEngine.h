#ifndef _POVENGINE_H_
#define _POVENGINE_H_
/**************************************************
* File: povEngine.h.
* Desc: Definition of the PovRayEngine class.
* Module: AkraLog : VrmlSupport.
* Rev: 23 avril 1998 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <vrcore/rendererVrml.h>


class ContainerVrml;
class NodeVrml;
class ViewPointVrml;
class LinkPovRay;
class TransfoStack;
class ContainerStack;


class PovRayEngine : public RendererVrml {
  protected:	// Instance variables.
    LinkPovRay *povLink;
    ContainerStack *treeStack;
    TransfoStack *transfoStack;

  public:		// Instance methods.
    PovRayEngine(char *format, char *prefix, int width, int height, int depth);
    virtual void pushContainer(ContainerVrml *aContainer);
    virtual void popContainer(void);
    virtual void deleteSceneTree(void);
    virtual void renderTree(void);
    virtual void addToScene(NodeVrml *aNode);

    virtual void drawBox(BoxVrml *aBox, AppearanceVrml *appearance);
    virtual void drawCone(ConeVrml *aCone, AppearanceVrml *appearance);
    virtual void drawCylinder(CylinderVrml *aCylinder, AppearanceVrml *appearance);
    virtual void drawElevationGrid(ElevationGridVrml *aGrid, AppearanceVrml *appearance);
    virtual void drawExtrusion(ExtrusionVrml *anExtrusion, AppearanceVrml *appearance);
    virtual void drawIndexedFaceSet(IndexedFaceSetVrml *aFaceSet, AppearanceVrml *appearance);
    virtual void drawIndexedLineSet(IndexedLineSetVrml *aLineSet, AppearanceVrml *appearance);
    virtual void drawPointSet(PointSetVrml *aPointSet, AppearanceVrml *appearance);
    virtual void drawSphere(SphereVrml *aSphere, AppearanceVrml *appearance);
    virtual void drawText(TextVrml *aText, AppearanceVrml *appearance);
    virtual void openTransform(TransformVrml *aTransform);
    virtual void closeTransform(TransformVrml *aTransform);

    virtual void addLighting(LightVrml *aLight);
    virtual void useViewPoint(ViewPointVrml *aVP);
};


#endif		/* _POVENGINE_H_ */
