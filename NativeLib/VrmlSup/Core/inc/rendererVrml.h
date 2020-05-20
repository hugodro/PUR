#ifndef _RENDERERVRML_H_
#define _RENDERERVRML_H_

/**************************************************
* File: rendererVrml.h.
* Desc: Definition of the RendererVrml class.
* Module: AkraLog : Animation.
* Rev: 9 decembre 1997 : REV 0 : Hugo DesRosiers : Creation.
**************************************************/

#include <akra/AkObject.h>
#include <akra/portableDefs.h>


class BoxVrml;
class ConeVrml;
class CylinderVrml;
class ElevationGridVrml;
class ExtrusionVrml;
class IndexedFaceSetVrml;
class IndexedLineSetVrml;
class PointSetVrml;
class SphereVrml;
class TextVrml;
class TransformVrml;
class AkColor;
class AppearanceVrml;
class LightVrml;
class ViewPointVrml;
class NodeVrml;


class ClassExport RendererVrml : public AkObject {
  protected:    // Instance variables.
    ViewPointVrml *viewPoint;	// Viewpoint of the camera.
    NodeVrml *nodeTree;		// Hierarchy of objects to draw.

  public:    // Instance methods.
    RendererVrml(void);
    virtual void drawBox(BoxVrml *aBox, AppearanceVrml *appearance)= 0;
    virtual void drawCone(ConeVrml *aCone, AppearanceVrml *appearance)= 0;
    virtual void drawCylinder(CylinderVrml *aCylinder, AppearanceVrml *appearance)= 0;
    virtual void drawElevationGrid(ElevationGridVrml *aGrid, AppearanceVrml *appearance)= 0;
    virtual void drawExtrusion(ExtrusionVrml *anExtrusion, AppearanceVrml *appearance)= 0;
    virtual void drawIndexedFaceSet(IndexedFaceSetVrml *aFaceSet, AppearanceVrml *appearance)= 0;
    virtual void drawIndexedLineSet(IndexedLineSetVrml *aLineSet, AppearanceVrml *appearance)= 0;
    virtual void drawPointSet(PointSetVrml *aPointSet, AppearanceVrml *appearance)= 0;
    virtual void drawSphere(SphereVrml *aSphere, AppearanceVrml *appearance)= 0;
    virtual void drawText(TextVrml *aText, AppearanceVrml *appearance)= 0;
    virtual void openTransform(TransformVrml *aTransform)= 0;
    virtual void closeTransform(TransformVrml *aTransform)= 0;
    virtual void addLighting(LightVrml *aLight)= 0;

    virtual void useViewPoint(ViewPointVrml *aVP)= 0;
};


#endif		/* _RENDERERVRML_H_ */
